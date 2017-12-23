// DpParse_ZJ_TL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DpParse_ZJ_TL.h"

#define DGI_NUMBER	4
#define DGI_LEN		2

bool HandleDp(const char* szFileName)
{
	ZJTLDpParse parse;
	return parse.HandleDp(szFileName);
}

bool ZJTLDpParse::HandleDp(const char* fileName)
{
	ifstream dpFile;
	if (!OpenDpFile(fileName, dpFile)) {
		return false;
	}
	long dpFileSize = GetFileSize(dpFile);

	char mic[8] = { 0 };
	GetBuffer(dpFile, mic, 7);
	int followedDataLen;
	GetLenTypeBuffer(dpFile, followedDataLen, 8);

	ReadDGIName(dpFile);		//读取DGI分组

	

	vector<CPS_ITEM> vecCpsItem;
	while (dpFile.tellg() < dpFileSize)		//遍历后续数据，每一次遍历解析一个卡片数据
	{
		string cardSeqNo;
		GetBCDBuffer(dpFile, cardSeqNo, 4);	//读取卡片序列号		
		int oneCardDataLen;
		GetLenTypeBuffer(dpFile, oneCardDataLen, 2); //读取该卡片个人化数据内容总长度

		CPS_ITEM cpsItem;
		for (unsigned int i = 0; i < m_vecDGI.size(); i++)	//解析每张卡片数据
		{
			DGI_ITEM dgiItem;
			dgiItem.dgi = m_vecDGI[i];
			if (dgiItem.dgi.substr(0, 3) == "PSE")	//对于DGIF001和PSE/PPSE数据，需要特殊处理
			{
				string dgiTag;
				GetBCDBuffer(dpFile, dgiTag, 1);
				if ("86" != dgiTag) {
					return 1;
				}
				int dgiLen;
				GetLenTypeBuffer(dpFile, dgiLen, 1);
				if (dgiLen == 0x81) {	//DGI数据为2字节
					GetLenTypeBuffer(dpFile, dgiLen, 2);
				}

				char* buffer = new char[dgiLen];
				memset(buffer, 0, dgiLen);
				dpFile.read(buffer, dgiLen);
				string value = StrToHex(buffer, dgiLen);	//直接保存，不用解析TLV结构
				dgiItem.value.InsertItem(dgiItem.dgi, value);
				cpsItem.items.push_back(dgiItem);
				continue;
			}

			string dgiTag;
			GetBCDBuffer(dpFile, dgiTag, 1);
			if ("86" != dgiTag) {
				return 1;
			}
			int dgiLen;
			int dgiDataLen;
			GetLenTypeBuffer(dpFile, dgiDataLen, 1);
			if (dgiDataLen == 0x81) {	//DGI数据为2字节
				GetLenTypeBuffer(dpFile, dgiDataLen, 1);
			}


			string dgi;
			//int dgiDataLen;
			GetBCDBuffer(dpFile, dgi, 2);
			GetLenTypeBuffer(dpFile, dgiLen, 1);
			//if (dgiDataLen - 3 > 0x81) {	//DGI数据为2字节
			//	GetLenTypeBuffer(dpFile, dgiLen, 1);
			//}
			unsigned short sDgi = stoi(dgi, 0, 16);

			if (sDgi < 0x0b01)		// 读取记录模板(该模板仅当DGI序号小于0x0B01时出现)
			{
				string templateTag;
				GetBCDBuffer(dpFile, templateTag, 1);
				if (templateTag != "70") {
					return false;
				}
				GetLenTypeBuffer(dpFile, dgiLen, 1);
				if (dgiLen == 0x81) {	//DGI数据为2字节
					GetLenTypeBuffer(dpFile, dgiLen, 1);
				}
			}


			char* buffer = new char[dgiLen];
			memset(buffer, 0, dgiLen);
			dpFile.read(buffer, dgiLen);
			vector<string> vecDGIs = { "8000","8201","8202","8203" ,"8204","8205","8020","8001","8402" };
			//判断是否仅包含数据内容，不是标准的TLV结构
			if (IsTlvStruct((unsigned char*)buffer, dgiLen)) {
				ParseTLVEx(buffer, dgiLen, dgiItem.value);
			}
			else {
				string dgiValue = StrToHex(buffer, dgiLen);
				//for (auto encryptDGI : vecDGIs)
				//{
				//	if (dgi == encryptDGI) {
				//		if (dgi == "8000" || dgi == "8020" || dgi == "8001") {
				//			dgiValue = DecryptDGI("9D705E435B267F2AA4AE62DCEFA13E07", dgiValue, false);
				//		}
				//		else {
				//			dgiValue = DecryptDGI("9D705E435B267F2AA4AE62DCEFA13E07", dgiValue, true);
				//		}

				//		break;
				//	}
				//}
				dgiItem.value.InsertItem(dgiItem.dgi, dgiValue);
			}
			cpsItem.items.push_back(dgiItem);
		}
		int pos = string(fileName).find_last_of('\\');
		string path = string(fileName).substr(0, pos + 1);
		cpsItem.fileName = path + "conv\\" + m_currentAccount;
		Save(cpsItem);
		vecCpsItem.push_back(cpsItem);
	}

	return true;
}

/********************************************************************
* 功能：读取DGI分组名称
*********************************************************************/
void ZJTLDpParse::ReadDGIName(ifstream &dpFile)
{
	int dgiNum;
	GetLenTypeBuffer(dpFile, dgiNum, 4);

	for (int i = 0; i < dgiNum; i++)
	{
		int dgiLen;
		GetLenTypeBuffer(dpFile, dgiLen, 2);
		char dgiName[12] = { 0 };
		GetBuffer(dpFile, dgiName, dgiLen);
		m_vecDGI.push_back(dgiName);
	}
}


/********************************************************************
* 功能：解析标准TLV结构，并保存到DGI数据结构中
*********************************************************************/
void ZJTLDpParse::ParseTLVEx(char* buffer, int nBufferLen, Dict& tlvs)
{
	TLVEntity entities[1024] = { 0 };
	unsigned int entitiesCount = 0;
	ParseTLV((unsigned char*)buffer, nBufferLen, entities, entitiesCount);
	unsigned char parseBuf[4096] = { 0 };
	for (unsigned int i = 0; i < entitiesCount; i++)
	{
		string strTag = StrToHex((char*)entities[i].tag, entities[i].tagSize);
		string strLen = StrToHex((char*)entities[i].length, entities[i].lengthSize);
		int nLen = std::stoi(strLen, 0, 16);
		string strValue = StrToHex((char*)entities[i].value, nLen);

		/****************小插曲，用于生成文件的文件名********************/
		string temp = DeleteSpace(strTag);
		if (temp.substr(0, 2).compare("57") == 0)
		{
			int index = strValue.find('D');
			if (index != string::npos)
			{
				m_currentAccount = strValue.substr(0, index);
			}
		}
		/*************************************************************/
		tlvs.InsertItem(strTag, strValue);
	}
}


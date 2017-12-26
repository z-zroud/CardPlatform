// DpParse_ZJ_TL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DpParse_ZJ_TL.h"

#define DGI_NUMBER	4
#define DGI_LEN		2

bool HandleDp(const char* szFileName, const char* ruleFile)
{
	ZJTLDpParse parse;
	return parse.HandleDp(szFileName, ruleFile);
}

bool ZJTLDpParse::HandleDp(const char* fileName, const char* ruleFile)
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

	ReadDGIName(dpFile);		//��ȡDGI����

	

	vector<CPS_ITEM> vecCpsItem;
	while (dpFile.tellg() < dpFileSize)		//�����������ݣ�ÿһ�α�������һ����Ƭ����
	{
		string cardSeqNo;
		GetBCDBuffer(dpFile, cardSeqNo, 4);	//��ȡ��Ƭ���к�		
		int oneCardDataLen;
		GetLenTypeBuffer(dpFile, oneCardDataLen, 2); //��ȡ�ÿ�Ƭ���˻����������ܳ���

		CPS_ITEM cpsItem;
		for (unsigned int i = 0; i < m_vecDGI.size(); i++)	//����ÿ�ſ�Ƭ����
		{
			DGI_ITEM dgiItem;
			dgiItem.dgi = m_vecDGI[i];
			if (dgiItem.dgi.substr(0, 3) == "PSE")	//����DGIF001��PSE/PPSE���ݣ���Ҫ���⴦��
			{
				string dgiTag;
				GetBCDBuffer(dpFile, dgiTag, 1);
				if ("86" != dgiTag) {
					return 1;
				}
				int dgiLen;
				GetLenTypeBuffer(dpFile, dgiLen, 1);
				if (dgiLen == 0x81) {	//DGI����Ϊ2�ֽ�
					GetLenTypeBuffer(dpFile, dgiLen, 2);
				}

				char* buffer = new char[dgiLen];
				memset(buffer, 0, dgiLen);
				dpFile.read(buffer, dgiLen);
				string value = StrToHex(buffer, dgiLen);	//ֱ�ӱ��棬���ý���TLV�ṹ
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
			if (dgiDataLen == 0x81) {	//DGI����Ϊ2�ֽ�
				GetLenTypeBuffer(dpFile, dgiDataLen, 1);
			}


			string dgi;
			//int dgiDataLen;
			GetBCDBuffer(dpFile, dgi, 2);
			GetLenTypeBuffer(dpFile, dgiLen, 1);
			//if (dgiDataLen - 3 > 0x81) {	//DGI����Ϊ2�ֽ�
			//	GetLenTypeBuffer(dpFile, dgiLen, 1);
			//}
			unsigned short sDgi = stoi(dgi, 0, 16);

			if (sDgi < 0x0b01)		// ��ȡ��¼ģ��(��ģ�����DGI���С��0x0B01ʱ����)
			{
				string templateTag;
				GetBCDBuffer(dpFile, templateTag, 1);
				if (templateTag != "70") {
					return false;
				}
				GetLenTypeBuffer(dpFile, dgiLen, 1);
				if (dgiLen == 0x81) {	//DGI����Ϊ2�ֽ�
					GetLenTypeBuffer(dpFile, dgiLen, 1);
				}
			}


			char* buffer = new char[dgiLen];
			memset(buffer, 0, dgiLen);
			dpFile.read(buffer, dgiLen);
			vector<string> vecDGIs = { "8000","8201","8202","8203" ,"8204","8205","8020","8001","8402" };
			//�ж��Ƿ�������������ݣ����Ǳ�׼��TLV�ṹ
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
* ���ܣ���ȡDGI��������
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
* ���ܣ�������׼TLV�ṹ�������浽DGI���ݽṹ��
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

		/****************С���������������ļ����ļ���********************/
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


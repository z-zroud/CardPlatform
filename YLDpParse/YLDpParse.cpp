#include "stdafx.h"
#include "YlDpParse.h"
#include "Des0.h"
#include <Windows.h>
#include <string>
#include <fstream>
using namespace std;

#define DGI_NUMBER	4
#define DGI_LEN		2


int YLDpParser::ParsePSE(ifstream &dpFile, DGI_ITEM &dgiItem)
{
	if (0x86 != (unsigned char)ReadDGIStartTag(dpFile)) {
		return 1;
	}
	int nFollowedDataLen = GetFollowedDataLen(dpFile);
	if (dgiItem.dgi == "DGIF001") {

		string dgi;
		GetBCDBuffer(dpFile, dgi, 2);	//读取该DGI序号

		int nFollowedDataLen = GetFollowedDataLen(dpFile);
		if (nFollowedDataLen > 0)
		{
			char* buffer = new char[nFollowedDataLen];
			memset(buffer, 0, nFollowedDataLen);
			dpFile.read(buffer, nFollowedDataLen);
			string value = StrToHex(buffer, nFollowedDataLen);
			dgiItem.value.InsertItem(dgiItem.dgi, value);
		}
	}
	else {
		char* buffer = new char[nFollowedDataLen];
		memset(buffer, 0, nFollowedDataLen);
		dpFile.read(buffer, nFollowedDataLen);
		string value = StrToHex(buffer, nFollowedDataLen);	//直接保存，不用解析TLV结构
		dgiItem.value.InsertItem(dgiItem.dgi, value);
	}

	return 0;
}

/********************************************************************
* 解析银联DP数据
********************************************************************/
bool YLDpParser::HandleDp(const char* fileName)
{
	ifstream dpFile;
	if (!OpenDpFile(fileName, dpFile)) {
		return false;
	}
	long dpFileSize = GetFileSize(dpFile);
	if (dpFileSize <= m_reserved){
		return false;
	}
	dpFile.seekg(m_reserved);
	ReadDGIName(dpFile);		//第一步： 读取DGI分组

	vector<CPS_ITEM> vecCpsItem;
	while (dpFile.tellg() < dpFileSize)		//遍历后续数据，每一次遍历解析一个卡片数据
	{
		string cardSeq;
		GetBCDBuffer(dpFile, cardSeq, 4);	//读取卡片序列号		
		int oneCardDataLen = GetOneCardDpDataLen(dpFile); //读取该卡片个人化数据内容总长度
		
		CPS_ITEM cpsItem;
		for (unsigned int i = 0; i < m_vecDGI.size(); i++)	//解析每张卡片数据
		{   
			DGI_ITEM dgiItem;
			dgiItem.dgi = m_vecDGI[i];			
			if (dgiItem.dgi == "DGIF001" || dgiItem.dgi.substr(0, 3) != "DGI")	//对于DGIF001和PSE/PPSE数据，需要特殊处理
			{
				ParsePSE(dpFile, dgiItem);
				cpsItem.items.push_back(dgiItem);
				continue;
			}
		
			if (0x86 != (unsigned char)ReadDGIStartTag(dpFile)) { //读取DGI起始标志
				return false;	
			}			
			int nFollowedDataLen = GetFollowedDataLen(dpFile);	//读取该DGI总数据长度(包含了该DGI序号)

			unsigned short sDgi;			
			string dgi;
			GetBCDBuffer(dpFile, dgi, 2);	//读取该DGI序号
			sDgi = stoi(dgi);
			
			nFollowedDataLen = GetFollowedDataLen(dpFile);	// 读取后续数据长度(该DGI数据部分)		
			if (sDgi < 0x0b01)		// 读取记录模板(该模板仅当DGI序号小于0x0B01时出现)
			{
				string templateTag;
				GetBCDBuffer(dpFile, templateTag, 1);
				if (templateTag != "70") {
					return false;
				}
				nFollowedDataLen = GetFollowedDataLen(dpFile);
			}

			char* buffer = new char[nFollowedDataLen];
			memset(buffer, 0, nFollowedDataLen);
			dpFile.read(buffer, nFollowedDataLen);

			//判断是否仅包含数据内容，不是标准的TLV结构
			if (IsTlvStruct((unsigned char*)buffer,nFollowedDataLen)){
				ParseTLVEx(buffer, nFollowedDataLen, dgiItem.value);
			}else{  
				char szTag[5] = { 0 };
				sprintf_s(szTag, "%X", sDgi);
				string value = StrToHex(buffer, nFollowedDataLen);
				dgiItem.value.InsertItem(szTag, value);
			}			
			cpsItem.items.push_back(dgiItem);			
		}
		//cpsItem.fileName = path + "yinlian\\" + m_currentAccount;
		Save(cpsItem);
		vecCpsItem.push_back(cpsItem);
	}

	return true;
}

//特殊DGI分组
YLDpParser::YLDpParser()
{
	//m_ValueOnlyDGI = { 0x8000,0x9000,0x8010,0x9010,0x8201,0x9103,
	//	0x8202,0x8203,0x8204,0x8205,0x9102, };
	//m_encryptTag = { "8000","8201","8202","8203" ,"8204","8205" };
}



/********************************************************************
* 功能：读取DGI起始标准符
*********************************************************************/
char YLDpParser::ReadDGIStartTag(ifstream &dpFile)
{
	char cDGIStartTag;

	dpFile.read(&cDGIStartTag, 1);

	return cDGIStartTag;
}

/********************************************************************
* 功能：读取DGI后续数据长度
*********************************************************************/
unsigned short YLDpParser::GetFollowedDataLen(ifstream &dpFile)
{
	unsigned char szBehindLen = '\0';
	dpFile.read((char*)&szBehindLen, 1);
	if (szBehindLen == 0x81)
		dpFile.read((char*)&szBehindLen, 1);

	return (unsigned short)szBehindLen;
}


/********************************************************************
* 功能：读取每一张卡片的个人化总数据长度
*********************************************************************/
int YLDpParser::GetOneCardDpDataLen(ifstream &dpFile)
{
	char szPersonlizedDataLen[3] = { 0 };
	dpFile.read(szPersonlizedDataLen, 2);
	unsigned short *pPersonlizedDataLen = (unsigned short*)(szPersonlizedDataLen);

	return *pPersonlizedDataLen;
}

/********************************************************************
* 功能：读取DGI分组名称
*********************************************************************/
void YLDpParser::ReadDGIName(ifstream &dpFile)
{
	//int dgiNum;
	//GetLenTypeBuffer(dpFile, dgiNum, 4);

	//for (int i = 0; i < dgiNum; i++)
	//{
	//	int dgiLen;
	//	GetLenTypeBuffer(dpFile, dgiLen, 2);
	//	char dgiName[12] = { 0 };
	//	GetBuffer(dpFile, dgiName, dgiLen);
	//	m_vecDGI.push_back(dgiName);
	//}

	char szDGICount[DGI_NUMBER] = { 0 };
	dpFile.read(szDGICount, DGI_NUMBER);
	int *nDGICount = (int*)szDGICount;

	for (int i = 0; i < *nDGICount; i++)
	{
		char szDGILen[DGI_LEN] = { 0 };
		dpFile.read(szDGILen, DGI_LEN);
		unsigned short *len = (unsigned short*)szDGILen;
		char* DGIName = new char[*len + 1];
		memset(DGIName, 0, *len + 1);
		dpFile.read(DGIName, *len);
		m_vecDGI.push_back(DGIName);
		delete DGIName;
	}
}

/********************************************************************
* 功能：解析标准TLV结构，并保存到DGI数据结构中
*********************************************************************/
void YLDpParser::ParseTLVEx(char* buffer, int nBufferLen, Dict& tlvs)
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



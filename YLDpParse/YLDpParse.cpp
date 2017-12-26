#include "stdafx.h"
#include "YlDpParse.h"
#include "Des0.h"
#include <Windows.h>
#include <string>
#include <fstream>
#include "../Util/Tool.h"
using namespace std;

#define DGI_NUMBER	4
#define DGI_LEN		2

YLDpParser::YLDpParser()
{
}


int YLDpParser::ParsePSE(ifstream &dpFile, DGI_ITEM &dgiItem)
{
	if (0x86 != (unsigned char)ReadDGIStartTag(dpFile)) {
		return 1;
	}
	int nFollowedDataLen = GetFollowedDataLen(dpFile);
	if (dgiItem.dgi == "DGIF001") {

		string dgi;
		GetBCDBuffer(dpFile, dgi, 2);	//读取该DGI序号
        dgiItem.dgi = "F001";
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

        if (dgiItem.dgi == "Store_PSE_1") {
            dgiItem.dgi = "PSE";
            dgiItem.value.InsertItem("0101", value);
        }
        else if (dgiItem.dgi == "Store_PSE_2") {
            dgiItem.dgi = "PSE";
            value = "880101" + value;
            char dataLen[3] = { 0 };
            Tool::GetBcdDataLen(value.c_str(), dataLen, 3);
            value = "A5" + string(dataLen) + value;
            dgiItem.value.InsertItem("9102", value);
        }
        else {
            dgiItem.dgi = "PPSE";
            dgiItem.value.InsertItem("9102", value);
        }
	}

	return 0;
}

/********************************************************************
* 解析银联DP数据
********************************************************************/
bool YLDpParser::HandleDp(const char* fileName,const char* ruleFile)
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
		GetBCDBufferLittleEnd(dpFile, cardSeq, 4);	//读取卡片序列号		
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
            streampos pos;
            pos = GetBCDBuffer(dpFile, dgi, 2);	//读取该DGI序号
            sDgi = stoi(dgi, 0, 16);
            if (dgi != dgiItem.dgi.substr(3)) {
                return false;   //对应的DGI有误
            }
            dgiItem.dgi = dgi;  //去掉"DGI"字符
			nFollowedDataLen = GetFollowedDataLen(dpFile);	// 读取后续数据长度(该DGI数据部分)		
			if (sDgi < 0x0b01)		// 读取记录模板(该模板仅当DGI序号小于0x0B01时出现)
			{
				string templateTag;
                GetBCDBufferLittleEnd(dpFile, templateTag, 1);
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

        //解析完成之后，调用规则
        if (ruleFile){
            HandleRule(ruleFile, cpsItem);
        }

        //保存数据
        int pos = string(fileName).find_last_of('\\');
        string path = string(fileName).substr(0, pos + 1);
		cpsItem.fileName = path + "conv\\" + m_currentAccount + ".txt";
		Save(cpsItem);
		vecCpsItem.push_back(cpsItem);
	}

	return true;
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
int YLDpParser::GetFollowedDataLen(ifstream &dpFile)
{
	int dataLen = 0;

    GetLenTypeBufferLittleEnd(dpFile, dataLen, 1);
	if (dataLen == 0x81)
        GetLenTypeBufferLittleEnd(dpFile, dataLen, 1);
    else if (dataLen == 0x82) {
        GetLenTypeBufferLittleEnd(dpFile, dataLen, 2);
    }

	return dataLen;
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

        if (nLen > 0xFF * 2) {
            char dataLen[5] = { 0 };
            Tool::GetBcdDataLen(strValue.c_str(), dataLen, 5);
            strLen = "82" + string(dataLen);
            
        }
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
        strValue = strTag + strLen + strValue;
		/*************************************************************/
		tlvs.InsertItem(strTag, strValue);
	}
}



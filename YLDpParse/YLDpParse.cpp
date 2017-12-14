#include "stdafx.h"
#include "YlDpParse.h"
#include "Des0.h"
#include <Windows.h>
#include <string>
#include <fstream>
using namespace std;

#define DGI_NUMBER	4
#define DGI_LEN		2



/********************************************************************
* 解析银联DP数据
********************************************************************/
bool YLDpParser::HandleDp(const char* szFileName)
{
	int pos = string(szFileName).find_last_of('\\');
	string path = string(szFileName).substr(0, pos + 1);
	ifstream dpFile;
	dpFile.open(szFileName, ios::in | ios::binary);
	if (!dpFile.is_open())
		return false;
	dpFile.seekg(0, ios::end);
	long dpFileSize = static_cast<long>(dpFile.tellg());
	if (dpFileSize <= m_reserved)   //检查文件是否异常
	{
		//Log->Error("银联DP文件大小异常! 检查DP文件大小[%l]", dpFileSize);
		return false;
	}
	dpFile.seekg(m_reserved, ios::beg);  //重置文件指针至保留数据末尾位置

	//第一步： 读取DGI分组
	ReadDGIName(dpFile, dpFile.tellg());


	vector<CPS_ITEM> vecCpsItem;
	//第二步： 遍历后续数据，每一次遍历解析一个卡片数据
	// 注意，一个DP文件可能包含几张卡片的数据
	while (dpFile.tellg() < dpFileSize)
	{
		//第三步： 读取卡片序列号
		ReadCardSequence(dpFile, dpFile.tellg());

		//第四步： 读取该卡片个人化数据内容总长度
		ReadCardPersonalizedTotelLen(dpFile, dpFile.tellg());

		//解析每张卡片数据
		CPS_ITEM cpsItem;
		for (unsigned int i = 0; i < m_vecDGI.size(); i++)
		{   //解析每一张卡片数据
			DGI_ITEM dgiItem;
			dgiItem.dgi = m_vecDGI[i];

			//对于DGIF001和PSE/PPSE数据，需要特殊处理
			if (dgiItem.dgi == "DGIF001" || dgiItem.dgi.substr(0, 3) != "DGI")
			{
				char tag = ReadDGIStartTag(dpFile, dpFile.tellg());
				if ((unsigned char)tag != 0x86)
				{
					//Log->Error("获取DGI分组标识[%X]错误", tag);
					return false;
				}
				int nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());
				if (dgiItem.dgi == "DGIF001") {

					ReadDGISequence(dpFile, dpFile.tellg()); //此时sDgi == 0xF001

					int nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());
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
					
					/*if (dgiItem.dgi == "Store_PSE_1") {	//不用管模板，直接写数据
						ReadRecordTemplate(dpFile, dpFile.tellg());
						nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());
					}*/
					char* buffer = new char[nFollowedDataLen];
					memset(buffer, 0, nFollowedDataLen);
					dpFile.read(buffer, nFollowedDataLen);
					string value = StrToHex(buffer, nFollowedDataLen);	//直接保存，不用解析TLV结构
					dgiItem.value.InsertItem(dgiItem.dgi, value);
				}
				cpsItem.items.push_back(dgiItem);
				continue;
			}

			//第五步： 读取DGI起始标志
			char tag = ReadDGIStartTag(dpFile, dpFile.tellg());
			if ((unsigned char)tag != 0x86)
			{
				//Log->Error("获取DGI分组标识[%X]错误", tag);
				return false;
			}

			//第六步： 读取该DGI总数据长度(包含了该DGI序号)
			int nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());

			unsigned short sDgi;

			//第七步： 读取该DGI序号
			sDgi = ReadDGISequence(dpFile, dpFile.tellg());

			//第八步： 读取后续数据长度(该DGI数据部分)
			nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());


			//第九步： 读取记录模板(该模板仅当DGI序号小于0x0B01时出现)
			if (sDgi < 0x0b01)
			{
				ReadRecordTemplate(dpFile, dpFile.tellg());
				nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());
			}

			char* buffer = new char[nFollowedDataLen];
			memset(buffer, 0, nFollowedDataLen);
			dpFile.read(buffer, nFollowedDataLen);

			//第十一步： 判断是否仅包含数据内容，不是标准的TLV结构
			if (IsTlvStruct((unsigned char*)buffer,nFollowedDataLen)){
				ParseTLV(buffer, nFollowedDataLen, dgiItem.value);
			}else{  
				char szTag[5] = { 0 };
				sprintf_s(szTag, "%X", sDgi);
				string value = StrToHex(buffer, nFollowedDataLen);
				dgiItem.value.InsertItem(szTag, value);
			}			
			cpsItem.items.push_back(dgiItem);			
		}
		cpsItem.fileName = path + "yinlian\\" + m_currentAccount;
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
char YLDpParser::ReadDGIStartTag(ifstream &dpFile, streamoff offset)
{
	char cDGIStartTag;

	dpFile.read(&cDGIStartTag, 1);

	return cDGIStartTag;
}

/********************************************************************
* 功能：读取DGI后续数据长度
*********************************************************************/
unsigned short YLDpParser::ReadFollowedDataLength(ifstream &dpFile, streamoff offset)
{
	unsigned char szBehindLen = '\0';
	dpFile.read((char*)&szBehindLen, 1);
	if (szBehindLen == 0x81)
		dpFile.read((char*)&szBehindLen, 1);

	return (unsigned short)szBehindLen;
}

/********************************************************************
* 功能：读取卡片序列号
*********************************************************************/
int YLDpParser::ReadCardSequence(ifstream &dpFile, streamoff offset)
{
	char szCardSeq[4] = { 0 };
	dpFile.read(szCardSeq, 4);
	int *pCardSeq = (int*)(szCardSeq);
	//m_oneCardDpData.nCardSequence = *pCardSeq;

	return *pCardSeq;
}

/********************************************************************
* 功能：读取每一张卡片的个人化总数据长度
*********************************************************************/
int YLDpParser::ReadCardPersonalizedTotelLen(ifstream &dpFile, streamoff offset)
{
	char szPersonlizedDataLen[3] = { 0 };
	dpFile.read(szPersonlizedDataLen, 2);
	unsigned short *pPersonlizedDataLen = (unsigned short*)(szPersonlizedDataLen);

	return *pPersonlizedDataLen;
}

/********************************************************************
* 功能：读取DGI分组名称
*********************************************************************/
void YLDpParser::ReadDGIName(ifstream &dpFile, streamoff offset)
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
* 功能：读取DGI需要，也就是DGI名称
*********************************************************************/
unsigned short YLDpParser::ReadDGISequence(ifstream &dpFile, streamoff offset)
{
	char szDGISeq[2] = { 0 };
	dpFile.read(szDGISeq, 2);

	char temp[2] = { 0 };
	temp[0] = szDGISeq[1];
	temp[1] = szDGISeq[0];
	unsigned short *sDGISeq = (unsigned short*)temp;

	return *sDGISeq;
}

/********************************************************************
* 功能：读取每一张卡片的记录模板("70")
*********************************************************************/
char YLDpParser::ReadRecordTemplate(ifstream &dpFile, streamoff offset)
{
	char cRecordTemplate;
	dpFile.read(&cRecordTemplate, 1);

	return cRecordTemplate;
}

/********************************************************************
* 功能：解析标准TLV结构，并保存到DGI数据结构中
*********************************************************************/
void YLDpParser::ParseTLV(char* buffer, int nBufferLen, Dict& tlvs)
{
	TLVEntity entities[1024] = { 0 };
	unsigned int entitiesCount = 0;
	TLVConstruct((unsigned char*)buffer, nBufferLen, entities, entitiesCount);
	unsigned char parseBuf[4096] = { 0 };
	unsigned int buf_count;
	//解析TLV   
	TLVParseAndFindError(entities, entitiesCount, parseBuf, buf_count);
	for (unsigned int i = 0; i < entitiesCount; i++)
	{
		string strTag = StrToHex((char*)entities[i].Tag, entities[i].TagSize);
		string strLen = StrToHex((char*)entities[i].Length, entities[i].LengthSize);
		int nLen = std::stoi(strLen, 0, 16);
		string strValue = StrToHex((char*)entities[i].Value, nLen);

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

/***********************************************************
* 功能： 解密指定的tag
************************************************************/
string YLDpParser::DecrptData(string tag, string value)
{
	string strResult;
	int len = value.length();
	if (len % 16 != 0)	//若解密字符串不为16字节整除，则后面添加0已补齐
	{
		int padding = 16 - len % 16;
		for (int i = 0; i < padding; i++)
		{
			value += "0";
		}
		len = value.length();
	}
	int unit = 16;
	for (int i = 0; i < len; i += unit)
	{
		char* pOutput = new char[unit + 1];
		memset(pOutput, 0, unit + 1);
		_Des3(pOutput, (char*)m_key.c_str(), (char*)value.substr(i, unit).c_str());
		strResult += string(pOutput);
	}

	return strResult;
}



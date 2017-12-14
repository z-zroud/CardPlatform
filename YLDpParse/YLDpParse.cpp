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
* ��������DP����
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
	if (dpFileSize <= m_reserved)   //����ļ��Ƿ��쳣
	{
		//Log->Error("����DP�ļ���С�쳣! ���DP�ļ���С[%l]", dpFileSize);
		return false;
	}
	dpFile.seekg(m_reserved, ios::beg);  //�����ļ�ָ������������ĩβλ��

	//��һ���� ��ȡDGI����
	ReadDGIName(dpFile, dpFile.tellg());


	vector<CPS_ITEM> vecCpsItem;
	//�ڶ����� �����������ݣ�ÿһ�α�������һ����Ƭ����
	// ע�⣬һ��DP�ļ����ܰ������ſ�Ƭ������
	while (dpFile.tellg() < dpFileSize)
	{
		//�������� ��ȡ��Ƭ���к�
		ReadCardSequence(dpFile, dpFile.tellg());

		//���Ĳ��� ��ȡ�ÿ�Ƭ���˻����������ܳ���
		ReadCardPersonalizedTotelLen(dpFile, dpFile.tellg());

		//����ÿ�ſ�Ƭ����
		CPS_ITEM cpsItem;
		for (unsigned int i = 0; i < m_vecDGI.size(); i++)
		{   //����ÿһ�ſ�Ƭ����
			DGI_ITEM dgiItem;
			dgiItem.dgi = m_vecDGI[i];

			//����DGIF001��PSE/PPSE���ݣ���Ҫ���⴦��
			if (dgiItem.dgi == "DGIF001" || dgiItem.dgi.substr(0, 3) != "DGI")
			{
				char tag = ReadDGIStartTag(dpFile, dpFile.tellg());
				if ((unsigned char)tag != 0x86)
				{
					//Log->Error("��ȡDGI�����ʶ[%X]����", tag);
					return false;
				}
				int nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());
				if (dgiItem.dgi == "DGIF001") {

					ReadDGISequence(dpFile, dpFile.tellg()); //��ʱsDgi == 0xF001

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
					
					/*if (dgiItem.dgi == "Store_PSE_1") {	//���ù�ģ�壬ֱ��д����
						ReadRecordTemplate(dpFile, dpFile.tellg());
						nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());
					}*/
					char* buffer = new char[nFollowedDataLen];
					memset(buffer, 0, nFollowedDataLen);
					dpFile.read(buffer, nFollowedDataLen);
					string value = StrToHex(buffer, nFollowedDataLen);	//ֱ�ӱ��棬���ý���TLV�ṹ
					dgiItem.value.InsertItem(dgiItem.dgi, value);
				}
				cpsItem.items.push_back(dgiItem);
				continue;
			}

			//���岽�� ��ȡDGI��ʼ��־
			char tag = ReadDGIStartTag(dpFile, dpFile.tellg());
			if ((unsigned char)tag != 0x86)
			{
				//Log->Error("��ȡDGI�����ʶ[%X]����", tag);
				return false;
			}

			//�������� ��ȡ��DGI�����ݳ���(�����˸�DGI���)
			int nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());

			unsigned short sDgi;

			//���߲��� ��ȡ��DGI���
			sDgi = ReadDGISequence(dpFile, dpFile.tellg());

			//�ڰ˲��� ��ȡ�������ݳ���(��DGI���ݲ���)
			nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());


			//�ھŲ��� ��ȡ��¼ģ��(��ģ�����DGI���С��0x0B01ʱ����)
			if (sDgi < 0x0b01)
			{
				ReadRecordTemplate(dpFile, dpFile.tellg());
				nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());
			}

			char* buffer = new char[nFollowedDataLen];
			memset(buffer, 0, nFollowedDataLen);
			dpFile.read(buffer, nFollowedDataLen);

			//��ʮһ���� �ж��Ƿ�������������ݣ����Ǳ�׼��TLV�ṹ
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

//����DGI����
YLDpParser::YLDpParser()
{
	//m_ValueOnlyDGI = { 0x8000,0x9000,0x8010,0x9010,0x8201,0x9103,
	//	0x8202,0x8203,0x8204,0x8205,0x9102, };
	//m_encryptTag = { "8000","8201","8202","8203" ,"8204","8205" };
}



/********************************************************************
* ���ܣ���ȡDGI��ʼ��׼��
*********************************************************************/
char YLDpParser::ReadDGIStartTag(ifstream &dpFile, streamoff offset)
{
	char cDGIStartTag;

	dpFile.read(&cDGIStartTag, 1);

	return cDGIStartTag;
}

/********************************************************************
* ���ܣ���ȡDGI�������ݳ���
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
* ���ܣ���ȡ��Ƭ���к�
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
* ���ܣ���ȡÿһ�ſ�Ƭ�ĸ��˻������ݳ���
*********************************************************************/
int YLDpParser::ReadCardPersonalizedTotelLen(ifstream &dpFile, streamoff offset)
{
	char szPersonlizedDataLen[3] = { 0 };
	dpFile.read(szPersonlizedDataLen, 2);
	unsigned short *pPersonlizedDataLen = (unsigned short*)(szPersonlizedDataLen);

	return *pPersonlizedDataLen;
}

/********************************************************************
* ���ܣ���ȡDGI��������
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
* ���ܣ���ȡDGI��Ҫ��Ҳ����DGI����
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
* ���ܣ���ȡÿһ�ſ�Ƭ�ļ�¼ģ��("70")
*********************************************************************/
char YLDpParser::ReadRecordTemplate(ifstream &dpFile, streamoff offset)
{
	char cRecordTemplate;
	dpFile.read(&cRecordTemplate, 1);

	return cRecordTemplate;
}

/********************************************************************
* ���ܣ�������׼TLV�ṹ�������浽DGI���ݽṹ��
*********************************************************************/
void YLDpParser::ParseTLV(char* buffer, int nBufferLen, Dict& tlvs)
{
	TLVEntity entities[1024] = { 0 };
	unsigned int entitiesCount = 0;
	TLVConstruct((unsigned char*)buffer, nBufferLen, entities, entitiesCount);
	unsigned char parseBuf[4096] = { 0 };
	unsigned int buf_count;
	//����TLV   
	TLVParseAndFindError(entities, entitiesCount, parseBuf, buf_count);
	for (unsigned int i = 0; i < entitiesCount; i++)
	{
		string strTag = StrToHex((char*)entities[i].Tag, entities[i].TagSize);
		string strLen = StrToHex((char*)entities[i].Length, entities[i].LengthSize);
		int nLen = std::stoi(strLen, 0, 16);
		string strValue = StrToHex((char*)entities[i].Value, nLen);

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

/***********************************************************
* ���ܣ� ����ָ����tag
************************************************************/
string YLDpParser::DecrptData(string tag, string value)
{
	string strResult;
	int len = value.length();
	if (len % 16 != 0)	//�������ַ�����Ϊ16�ֽ���������������0�Ѳ���
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



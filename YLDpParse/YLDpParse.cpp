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
		GetBCDBuffer(dpFile, dgi, 2);	//��ȡ��DGI���
        dgiItem.dgi = "F001";
		int nFollowedDataLen = GetFollowedDataLen(dpFile);
		if (nFollowedDataLen > 0)
		{
            string value;
            GetBCDBuffer(dpFile, value, nFollowedDataLen);
			dgiItem.value.InsertItem(dgiItem.dgi, value);
		}
	}
	else {
        string value;
        GetBCDBuffer(dpFile, value, nFollowedDataLen);
        if (dgiItem.dgi == "Store_PSE_1") {
            dgiItem.dgi = "PSE";
            char dataLen[3] = { 0 };
            Tool::GetBcdDataLen(value.c_str(), dataLen, 3);
            string tag = "0101";
            value = tag + string(dataLen) + value;
            dgiItem.value.InsertItem(tag, value);
        }
        else if (dgiItem.dgi == "Store_PSE_2") {
            dgiItem.dgi = "PSE";
            value = "880101" + value;
            char dataLen[3] = { 0 };
            Tool::GetBcdDataLen(value.c_str(), dataLen, 3);
            value = "A5" + string(dataLen) + value;
            memset(dataLen, 0, 3);
            Tool::GetBcdDataLen(value.c_str(), dataLen, 3);
            string tag = "9102";
            value = tag + dataLen + value;
            dgiItem.value.InsertItem(tag, value);
        }
        else {
            dgiItem.dgi = "PPSE";
            char dataLen[3] = { 0 };
            Tool::GetBcdDataLen(value.c_str(), dataLen, 3);
            value = "BF0C" + string(dataLen) + value;
            Tool::GetBcdDataLen(value.c_str(), dataLen, 3);
            value = "A5" + string(dataLen) + value;
            memset(dataLen, 0, 3);
            Tool::GetBcdDataLen(value.c_str(), dataLen, 3);
            string tag = "9102";
            value = tag + dataLen + value;
            dgiItem.value.InsertItem(tag, value);
        }
	}

	return 0;
}

/********************************************************************
* ��������DP����
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
	ReadDGIName(dpFile);		//��һ���� ��ȡDGI����

	vector<CPS_ITEM> vecCpsItem;
	while (dpFile.tellg() < dpFileSize)		//�����������ݣ�ÿһ�α�������һ����Ƭ����
	{
		string cardSeq;
		GetBCDBufferLittleEnd(dpFile, cardSeq, 4);	//��ȡ��Ƭ���к�		
		int oneCardDataLen = GetOneCardDpDataLen(dpFile); //��ȡ�ÿ�Ƭ���˻����������ܳ���
		
		CPS_ITEM cpsItem;
		for (unsigned int i = 0; i < m_vecDGI.size(); i++)	//����ÿ�ſ�Ƭ����
		{   
			DGI_ITEM dgiItem;
			dgiItem.dgi = m_vecDGI[i];			
			if (dgiItem.dgi == "DGIF001" || dgiItem.dgi.substr(0, 3) != "DGI")	//����DGIF001��PSE/PPSE���ݣ���Ҫ���⴦��
			{
				ParsePSE(dpFile, dgiItem);
				cpsItem.items.push_back(dgiItem);
				continue;
			}
		
			if (0x86 != (unsigned char)ReadDGIStartTag(dpFile)) { //��ȡDGI��ʼ��־
				return false;	
			}			
			int nFollowedDataLen = GetFollowedDataLen(dpFile);	//��ȡ��DGI�����ݳ���(�����˸�DGI���)

			unsigned short sDgi;			
			string dgi;
            streampos pos;
            pos = GetBCDBuffer(dpFile, dgi, 2);	//��ȡ��DGI���
            sDgi = stoi(dgi, 0, 16);
            if (dgi != dgiItem.dgi.substr(3)) {
                return false;   //��Ӧ��DGI����
            }
            dgiItem.dgi = dgi;  //ȥ��"DGI"�ַ�
			nFollowedDataLen = GetFollowedDataLen(dpFile);	// ��ȡ�������ݳ���(��DGI���ݲ���)		
			if (sDgi < 0x0b01)		// ��ȡ��¼ģ��(��ģ�����DGI���С��0x0B01ʱ����)
			{
				string templateTag;
                GetBCDBufferLittleEnd(dpFile, templateTag, 1);
				if (templateTag != "70") {
					return false;
				}
				nFollowedDataLen = GetFollowedDataLen(dpFile);
			}

            string buffer;
            GetBCDBuffer(dpFile, buffer, nFollowedDataLen);

			//�ж��Ƿ�������������ݣ����Ǳ�׼��TLV�ṹ
			if (IsTlvStruct((char*)buffer.c_str(),nFollowedDataLen * 2)){
                ParseTLV((char*)buffer.c_str(), nFollowedDataLen * 2, dgiItem.value);
			}else{  
				char szTag[5] = { 0 };
				sprintf_s(szTag, "%X", sDgi);
				dgiItem.value.InsertItem(szTag, buffer);
			}			
			cpsItem.items.push_back(dgiItem);			
		}

        //�������֮�󣬵��ù���
        if (ruleFile){
            HandleRule(ruleFile, cpsItem);
        }

        //��������
        int pos = string(fileName).find_last_of('\\');
        string path = string(fileName).substr(0, pos + 1);
		cpsItem.fileName = path + GetAccount(cpsItem) + ".txt";
		Save(cpsItem);
		vecCpsItem.push_back(cpsItem);
	}

	return true;
}


/********************************************************************
* ���ܣ���ȡDGI��ʼ��׼��
*********************************************************************/
char YLDpParser::ReadDGIStartTag(ifstream &dpFile)
{
	char cDGIStartTag;

	dpFile.read(&cDGIStartTag, 1);

	return cDGIStartTag;
}

/********************************************************************
* ���ܣ���ȡDGI�������ݳ���
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
* ���ܣ���ȡÿһ�ſ�Ƭ�ĸ��˻������ݳ���
*********************************************************************/
int YLDpParser::GetOneCardDpDataLen(ifstream &dpFile)
{
	char szPersonlizedDataLen[3] = { 0 };
	dpFile.read(szPersonlizedDataLen, 2);
	unsigned short *pPersonlizedDataLen = (unsigned short*)(szPersonlizedDataLen);

	return *pPersonlizedDataLen;
}

/********************************************************************
* ���ܣ���ȡDGI��������
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



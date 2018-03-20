// DpParse_ZJ_TL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DpParse_ZJ_TL.h"
#include "../Util/Tool.h"

#define DGI_NUMBER	4
#define DGI_LEN		2

bool HandleDp(const char* szFileName, const char* ruleFile, char** cpsFile, int& count)
{
	ZJTLDpParse parse;
    return parse.HandleDp(szFileName, ruleFile, cpsFile, count);
}

string ZJTLDpParse::GetDpMark(ifstream& dpFile)
{
    char mic[8] = { 0 };
    GetBuffer(dpFile, mic, 7);
    
    return mic;
}

void ZJTLDpParse::ParsePSE(ifstream& dpFile, CPS_ITEM& cpsItem, string dgiName)
{
    if ("86" != GetDGIStartMark(dpFile)) {
        return ;
    }
    int dgiLen;
    GetLenTypeBuffer(dpFile, dgiLen, 1);
    if (dgiLen == 0x81) {	//DGI����Ϊ2�ֽ�
        GetLenTypeBuffer(dpFile, dgiLen, 2);
    }
    DGI_ITEM dgiItem;
    string value;
    GetBCDBuffer(dpFile, value, dgiLen);
    string tag;
    if (dgiName.substr(0, 3) == "PSE")
    {
        dgiItem.dgi = "PSE";
        tag = dgiName.substr(4);
    }
    else {
        dgiItem.dgi = "PPSE";   //���ŵ����ݶ�������6Fģ�壬һ�㶼�̶�ֵΪ42�ֽڣ�ȥ��
        tag = dgiName.substr(5);
        char dataLen[3] = { 0 };
        Tool::GetBcdDataLen(value.substr(42).c_str(), dataLen, 3);
        value = "9102" + string(dataLen) + value.substr(42);
    }
    
    dgiItem.value.InsertItem(tag, value);
    //cpsItem.items.push_back(dgiItem);
    cpsItem.AddDgiItem(dgiItem);
}

string ZJTLDpParse::GetDGIStartMark(ifstream& dpFile)
{
    string dgiStartMark;
    GetBCDBuffer(dpFile, dgiStartMark, 1);
    
    return dgiStartMark;
}

int ZJTLDpParse::GetDGIDataLen(ifstream& dpFile)
{
    int dgiLen;
    GetLenTypeBuffer(dpFile, dgiLen, 1); //��ȡ����DGI���ݿ�ĳ���
    if (dgiLen == 0x81)
    {	//DGI����Ϊ2�ֽ�
        GetLenTypeBuffer(dpFile, dgiLen, 1);
    }

    string dgi;
    GetBCDBuffer(dpFile, dgi, 2);
    int dgiDataLen;
    GetLenTypeBuffer(dpFile, dgiDataLen, 1); //DGI���ݵĳ���
    unsigned short sDgi = stoi(dgi, 0, 16);

    if (sDgi < 0x0b01)		// ��ȡ��¼ģ��(��ģ�����DGI���С��0x0B01ʱ����)
    {
        string templateTag;
        GetBCDBuffer(dpFile, templateTag, 1);
        if (templateTag != "70") {
            return false;
        }
        GetLenTypeBuffer(dpFile, dgiDataLen, 1);
        if (dgiDataLen == 0x81) {	//DGI����Ϊ2�ֽ�
            GetLenTypeBuffer(dpFile, dgiDataLen, 1);
        }
    }

    return dgiDataLen;
}

bool ZJTLDpParse::HandleDp(const char* fileName, const char* ruleFile, char** cpsFile, int& count)
{
	ifstream dpFile;
	if (!OpenDpFile(fileName, dpFile)) {
		return false;
	}
    long dpFileSize = GetFileSize(dpFile);
    if ("CITICDP" != GetDpMark(dpFile)){
        return false;
    }
    int followedDataLen;
    GetLenTypeBuffer(dpFile, followedDataLen, 8);

	ReadDGIName(dpFile);		//��ȡDGI����

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
			if (dgiItem.dgi.substr(0, 3) == "PSE" ||
                dgiItem.dgi.substr(0,4) == "PPSE")	//����DGIF001��PSE/PPSE���ݣ���Ҫ���⴦��
			{
                ParsePSE(dpFile, cpsItem, dgiItem.dgi);
				continue;
			}
            if (dgiItem.dgi.substr(0, 3) == "DGI") {    //ɾ��DGI�е�"DGI"�ַ���
                dgiItem.dgi = dgiItem.dgi.substr(3);
            }
            if (GetDGIStartMark(dpFile) != "86") {
                return false;
            }
			
            int dgiDataLen = GetDGIDataLen(dpFile);
            string buffer;
            GetBCDBuffer(dpFile, buffer, dgiDataLen);
			//�ж��Ƿ�������������ݣ����Ǳ�׼��TLV�ṹ
			if (IsTlvStruct((char*)buffer.c_str(), dgiDataLen * 2)) {
				ParseTLV((char*)buffer.c_str(), dgiDataLen * 2, dgiItem.value);
			}
			else {	

				dgiItem.value.InsertItem(dgiItem.dgi, buffer);
			}
			cpsItem.items.push_back(dgiItem);
		}
		int pos = string(fileName).find_last_of('\\');
		string path = string(fileName).substr(0, pos + 1);
        cpsItem.fileName = path +  GetAccount(cpsItem) + ".txt";

        if (ruleFile) { //�������
            HandleRule(ruleFile, cpsItem);
        }

		Save(cpsItem);
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




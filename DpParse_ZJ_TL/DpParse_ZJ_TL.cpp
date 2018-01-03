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

            string buffer;
            GetBCDBuffer(dpFile, buffer, dgiLen);
			//�ж��Ƿ�������������ݣ����Ǳ�׼��TLV�ṹ
			if (IsTlvStruct((char*)buffer.c_str(), dgiLen * 2)) {
				ParseTLV((char*)buffer.c_str(), dgiLen * 2, dgiItem.value);
			}
			else {
				
				dgiItem.value.InsertItem(dgiItem.dgi, buffer);
			}
			cpsItem.items.push_back(dgiItem);
		}
		int pos = string(fileName).find_last_of('\\');
		string path = string(fileName).substr(0, pos + 1);

        cpsItem.fileName = path + "conv\\" + GetAccount(cpsItem) + ".txt";
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




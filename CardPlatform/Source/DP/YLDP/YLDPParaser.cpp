#include "stdafx.h"
#include "YLDPParaser.h"
#include "Util\Log.h"
#include "Util\Converter.h"
#include "Util\IniParaser.h"
#include "Util\StringParaser.h"
#include "Util\Des0.h"
#include <Windows.h>
#include <string>
using namespace std;

#define DGI_NUMBER	4
#define DGI_LEN		2

enum PSE_TYPE
{
	PSE1,
	PSE2,
	PPSE,
	END
};

/*********************************************************
* ���ܣ����ü������ݵĽ�����Կ
**********************************************************/
void YLDpParser::SetDecryptKey(const string &key)
{
    m_key = key;
}


/*****************************************************************
* ���ܣ� ���ý�����ֵ��GGI,�Ǳ�׼TLV�ṹ
******************************************************************/
void YLDpParser::SetOnlyValueOfDGI(vector<unsigned short> vecValueDGI)
{
    for (auto v : vecValueDGI)
    {
        m_valueOnlyDGI.push_back(v);
    }
}

/*****************************************************************
* ���ܣ����ü��ܵ�tag
******************************************************************/
void YLDpParser::SetEncryptTag(vector<string> vecEncryptDGI)
{
    for (auto v : vecEncryptDGI)
    {
        m_encryptTag.push_back(v);
    }
}

/****************************************************************
* ���ܣ�������Ҫ����ֵ��tag
*****************************************************************/
void YLDpParser::SetExchangeDGI(map<string, string> mapDGI)
{
    for (auto m : mapDGI)
    {
        m_exchangeDGI.insert(m);
    }
}

//����DGI����
YLDpParser::YLDpParser()
{
	//m_ValueOnlyDGI = { 0x8000,0x9000,0x8010,0x9010,0x8201,0x9103,
	//	0x8202,0x8203,0x8204,0x8205,0x9102, };
	//m_encryptTag = { "8000","8201","8202","8203" ,"8204","8205" };
}

/********************************************************************
* ���ܣ��ж��Ƿ�Ϊ����ֵ��tag
*********************************************************************/
bool YLDpParser::IsValueOnlyDGI(unsigned short sDGINO)
{
	auto result = find(m_valueOnlyDGI.begin(), m_valueOnlyDGI.end(), sDGINO);
	if (result != m_valueOnlyDGI.end())
	{
		return true;
	}

	return false;
}

/********************************************************************
* ���ܣ��ж��Ƿ�Ϊ����tag
*********************************************************************/
bool YLDpParser::IsEncryptDGI(string tag)
{
    for (auto v : m_encryptTag)
    {
        if (tag == v)
        {
            return true;
        }
    }

    return false;
}

void YLDpParser::ClearCurrentDPData()
{
	for (auto m : m_oneCardDpData.vecDpData)
	{
		m.Clear();
	}
	m_oneCardDpData.vecDpData.clear();
	m_oneCardDpData.strAccount = "";
	m_oneCardDpData.nCardSequence = -1;
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
        m_dGIName.push_back(DGIName);
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
void YLDpParser::ParseTLV(char* buffer, int nBufferLen, DGI &YLDGI)
{
	TLVParaser *parser = new TLVParaser();
	TLVEntity entities[1024] = { 0 };
	unsigned int entitiesCount = 0;
	parser->TLVConstruct((unsigned char*)buffer, nBufferLen, entities, entitiesCount);
	unsigned char parseBuf[4096] = { 0 };
	unsigned int buf_count;
	//����TLV   
	parser->TLVParseAndFindError(entities, entitiesCount, parseBuf, buf_count);
	for (unsigned int i = 0; i < entitiesCount; i++)
	{
		string strTag = Tool::Converter::StrToHex((char*)entities[i].Tag, entities[i].TagSize);
		string strLen = Tool::Converter::StrToHex((char*)entities[i].Length, entities[i].LengthSize);
		int nLen = std::stoi(strLen, 0, 16);
		string strValue = Tool::Converter::StrToHex((char*)entities[i].Value, nLen);

		/****************С���������������ļ����ļ���********************/
		string temp = Tool::Stringparser::DeleteSpace(strTag);
		if (temp.substr(0,2).compare("57") == 0)
		{
			int index = strValue.find('D');
			if (index != string::npos)
			{
				m_oneCardDpData.strAccount = strValue.substr(0, index);
			}
		}
		/*************************************************************/
		TLVItem item(strTag, strLen, strValue);
		YLDGI.vecItem.push_back(item);	
	}

	m_oneCardDpData.vecDpData.push_back(YLDGI);	//���һ��DGI����
}

/********************************************************************
* ���ܣ�����PSE��PPSE����
*********************************************************************/
void YLDpParser::DealPSEDataCompleted(ifstream &dpFile, streamoff offset)
{
	PSE_TYPE pseType = PSE_TYPE::PSE1;
	
	
	for (int i = PSE1; i < PSE_TYPE::END; i++)
	{
        DGI YLDGI;
		YLDGI.DGIName = m_dGIName[m_dGIName.size() + i - PSE_TYPE::END];
		//DGI��ʼ��־		
		ReadDGIStartTag(dpFile, dpFile.tellg());

		//�������ݳ���1

		int nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());
		switch (pseType)
		{
		case PSE1:
			
			//��¼ģ��
			//ReadRecordTemplate(dpFile, dpFile.tellg());
			//�������ݳ���2
			//nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());
			pseType = PSE_TYPE::PSE2;
			break;
		case PSE2:
			pseType = PSE_TYPE::PPSE;
			break;
		case PPSE:
			pseType = PSE_TYPE::END;
			break;
		default:
			break;
		}
		//��ȡTLV����
		char* buffer = new char[nFollowedDataLen];
		memset(buffer, 0, nFollowedDataLen);
		dpFile.read(buffer, nFollowedDataLen);
        string s;
        s = Tool::Converter::StrToHex(buffer, nFollowedDataLen);
        char len[12] = { 0 };
        sprintf_s(len, 12, "%02X", nFollowedDataLen * 2);
        TLVItem item(YLDGI.DGIName, len, s);
        YLDGI.vecItem.push_back(item);
        m_oneCardDpData.vecDpData.push_back(YLDGI);
	}
}

/********************************************************************
* ����: ��ȡ����DP���ݣ�������
*********************************************************************/
int YLDpParser::Read(const string& filePath)
{
	ifstream dpFile(filePath.c_str(), ios::binary);
	if (!dpFile)
		return -1;
	dpFile.seekg(0, ios::end);
	long dpFileSize =static_cast<long>(dpFile.tellg());
	if (dpFileSize <= m_reserved)   //����ļ��Ƿ��쳣
	{
		Log->Error("����DP�ļ���С�쳣! ���DP�ļ���С[%l]", dpFileSize);
		return -1;
	}	
	dpFile.seekg(m_reserved,ios::beg);  //�����ļ�ָ�뱣������ĩβλ��

	//��һ���� ��ȡDGI����
	ReadDGIName(dpFile, dpFile.tellg());

	//�ڶ����� �����������ݣ�ÿһ�α�������һ����Ƭ����
    // ע�⣬һ��DP�ļ����ܰ������ſ�Ƭ������
	while (dpFile.tellg() < dpFileSize)
	{
		//��յ�ǰDP����
		ClearCurrentDPData();

		//�������� ��ȡ��Ƭ���к�
		m_oneCardDpData.nCardSequence = ReadCardSequence(dpFile, dpFile.tellg());

		//���Ĳ��� ��ȡ�ÿ�Ƭ���˻����������ܳ���
		ReadCardPersonalizedTotelLen(dpFile, dpFile.tellg());
		for (unsigned int i = 0; i < m_dGIName.size() - PSE_TYPE::END; i++)
		{   //����ÿһ�ſ�Ƭ����
			DGI YLDGI;
			YLDGI.DGIName = m_dGIName[i];

			//���岽�� ��ȡDGI��ʼ��־
			char tag = ReadDGIStartTag(dpFile, dpFile.tellg());
			if ((unsigned char)tag != 0x86)
			{
				Log->Error("��ȡDGI�����ʶ[%X]����", tag);
				return -1;
			}

			//�������� ��ȡ��DGI�����ݳ���(�����˸�DGI���)
			int nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());

			//���߲��� ��ȡ��DGI���
			unsigned short sDGISeq = ReadDGISequence(dpFile, dpFile.tellg());

			//�ڰ˲��� ��ȡ�������ݳ���(��DGI���ݲ���)
			nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());

			//�ھŲ��� ��ȡ��¼ģ��(��ģ�����DGI���С��0x0B01ʱ����)
			bool isInnerDGI = false;			
			if (sDGISeq < 0x0b01)
			{
				ReadRecordTemplate(dpFile, dpFile.tellg());
			}
			else {
				isInnerDGI = true;
			}

			//��ʮ���� ��ȡ����TLV�ṹ�����ݳ���
			if (!isInnerDGI)
			{
				nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());
			}

			char* buffer = new char[nFollowedDataLen];
			memset(buffer, 0, nFollowedDataLen);
			dpFile.read(buffer, nFollowedDataLen);

			//��ʮһ���� �ж��Ƿ�������������ݣ����Ǳ�׼��TLV�ṹ
			if (IsValueOnlyDGI(sDGISeq))
			{
				char szTag[5] = { 0 };
				sprintf_s(szTag, "%X", sDGISeq);
				string strValue = Tool::Converter::StrToHex(buffer, nFollowedDataLen);
				TLVItem item(szTag, "0", strValue);
				YLDGI.vecItem.push_back(item);
				m_oneCardDpData.vecDpData.push_back(YLDGI);	//���һ��DGI����
			}
			else
			{   //������׼TLV�ṹ
				ParseTLV(buffer, nFollowedDataLen, YLDGI);
			}
		}

        //��ʮ������ ����PSE PPSE����
		DealPSEDataCompleted(dpFile,dpFile.tellg());
		m_manyCardDpData.push_back(m_oneCardDpData);
	}

	return 0;
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

/**********************************************************
* ���ܣ� ��ȡָ��tagֵ
**********************************************************/
string YLDpParser::GetTagValue(string Tag, OneCardPersoData &oneCardDpData)
{
    string result;

    for (auto iter = oneCardDpData.vecDpData.begin(); iter != oneCardDpData.vecDpData.end(); iter++)
    {
        if (iter->DGIName.substr(3) == Tag)
            return iter->vecItem[0].strValue;
    }

    return result;
}

/**********************************************************
* ���ܣ� �趨ָ��tagֵ
**********************************************************/
void YLDpParser::SetTagValue(string tag, string value, OneCardPersoData &oneCardDpData)
{
    string result;
    for (auto &iter = oneCardDpData.vecDpData.begin(); iter != oneCardDpData.vecDpData.end(); iter++)
    {
        if (iter->DGIName.substr(3) == tag)
        {
            iter->vecItem[0].strValue = value;
            return;
        }               
    }
}

/****************************************************************
* ���ܣ� ��DP���˻�����ת��ΪCPS�ṹ�ĸ��˻�����
*****************************************************************/
void YLDpParser::FilterDpData()
{
    for (auto &f : m_manyCardDpData)  //����tag8202��8203, tag8204��8205
    {      
        for (auto m : m_exchangeDGI)
        {
            string dgi1 = GetTagValue(m.first, f);
            string dgi2 = GetTagValue(m.second, f);
            SetTagValue(m.first, dgi2, f);
            SetTagValue(m.second, dgi1, f);
        }
    }

	for (auto &f : m_manyCardDpData)
	{	//����ÿһ���ļ�����
		CPSDP dpData;
        dpData.fileName = f.strAccount;	//�ļ�����
		for (auto v : f.vecDpData)	//����ÿһ��DGI
		{
			DTL dtl;
			string DGI = Tool::Stringparser::DeleteSpace(v.DGIName);
			if (DGI.substr(0, 3) == "DGI")
			{
				DGI = DGI.substr(3);
			}
			for (auto tlv : v.vecItem)	//����ÿһ��DGI�����TLV�ṹ
			{
                if (DGI == "0E01" && tlv.strTag == "9F1F")
                {
                    continue;
                }
				string Tag = Tool::Stringparser::DeleteSpace(tlv.strTag);
				if (stoi(tlv.strLen, 0, 16) > 0x80)
				{
					tlv.strLen = "81" + tlv.strLen;
				}
				if (tlv.strValue.empty())	//���valueΪ�գ�ֱ��������tag
				{
					continue;
				}
                string Value;
                if (DGI == "Store_PPSE" || 
                    DGI == "Store_PSE_1" || 
                    DGI == "Store_PSE_2" ||
                    (IsValueOnlyDGI(stoi(DGI,0,16)) && !IsEncryptDGI(DGI)))
                {
                    Value = tlv.strValue;
                }
                else {
                    Value = Tool::Stringparser::DeleteSpace(tlv.strTag + tlv.strLen + tlv.strValue);
                }
                   
				dtl.DGI = DGI;
				dtl.Tag = Tag;
				if (IsEncryptDGI(Tag))	//�Ƿ���Ҫ����
				{
					Value = DecrptData(Tag, tlv.strValue);
				}
				dtl.Value = Value;
				dpData.data.push_back(dtl);
			}
	
		}
        m_vecCPSDP.push_back(dpData);
	}
}

/*****************************************************************
* ���ܣ� ����CPS���˻������ݵ�ָ�����ļ�
******************************************************************/
void YLDpParser::Save(const string &strPath)
{
	if (!CheckFolderExist(strPath))
	{
		CreateDirectory(strPath.c_str(), NULL);
	}
	if (m_vecCPSDP.empty())
	{
		FilterDpData();
	}
	for (auto m : m_vecCPSDP)
	{
		string filePath = strPath + "\\" + m.fileName + ".txt";
		INIParser ini;

		for (auto v : m.data)
		{
            ini.SetValue(v.DGI, v.Tag, v.Value);
		}
		ini.Save(filePath);
	}
}

/**********************************************************
* ���ܣ� ���Ŀ¼�Ƿ����
***********************************************************/
bool  YLDpParser::CheckFolderExist(const string &strPath)
{
	WIN32_FIND_DATA  wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(strPath.c_str(), &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = true;
	}
	FindClose(hFind);
	return rValue;
}

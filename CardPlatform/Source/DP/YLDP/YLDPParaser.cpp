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

#define DGI_8202    "8202"
#define DGI_8203    "8203"
#define DGI_8204    "8204"
#define DGI_8205    "8205"

enum PSE_TYPE
{
	PSE1,
	PSE2,
	PPSE,
	END
};

//特殊DGI分组
YLDpParser::YLDpParser()
{
	m_ValueOnlyDGI = { 0x8000,0x9000,0x8010,0x9010,0x8201,0x9103,
		0x8202,0x8203,0x8204,0x8205,0x9102, };
	m_encryptTag = { "8000","8201","8202","8203" ,"8204","8205" };
}

bool YLDpParser::IsNeedConvertDGI(unsigned short sDGINO)
{
	auto result = find(m_ValueOnlyDGI.begin(), m_ValueOnlyDGI.end(), sDGINO);
	if (result != m_ValueOnlyDGI.end())
	{
		return true;
	}

	return false;
}

void YLDpParser::ClearCurrentDPData()
{
	for (auto m : m_CurrentDpData.vecDpData)
	{
		m.Clear();
	}
	m_CurrentDpData.vecDpData.clear();
	m_CurrentDpData.strAccount = "";
	m_CurrentDpData.nCardSequence = -1;
}

//读取标签
char YLDpParser::ReadDGIStartTag(ifstream &dpFile, streamoff offset)
{
	char cDGIStartTag;

	dpFile.read(&cDGIStartTag, 1);

	return cDGIStartTag;
}

//读取数据长度
unsigned short YLDpParser::ReadFollowedDataLength(ifstream &dpFile, streamoff offset)
{
	unsigned char szBehindLen = '\0';
	dpFile.read((char*)&szBehindLen, 1);
	if (szBehindLen == 0x81)
		dpFile.read((char*)&szBehindLen, 1);
	
	return (unsigned short)szBehindLen;
}

//读取卡片序列号
int YLDpParser::ReadCardSequence(ifstream &dpFile, streamoff offset)
{
	char szCardSeq[4] = { 0 };
	dpFile.read(szCardSeq, 4);
	int *pCardSeq = (int*)(szCardSeq);
	//m_CurrentDpData.nCardSequence = *pCardSeq;

	return *pCardSeq;
}

//读取卡片个人化数据总长度
int YLDpParser::ReadCardPersonalizedTotelLen(ifstream &dpFile, streamoff offset)
{
	char szPersonlizedDataLen[3] = { 0 };
	dpFile.read(szPersonlizedDataLen, 2);
	unsigned short *pPersonlizedDataLen = (unsigned short*)(szPersonlizedDataLen);
	
	return *pPersonlizedDataLen;
}

//读取DGI分组名称
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
		m_DGIName.push_back(DGIName);
		delete DGIName;
	}
}

//读取卡片DGI分组序号
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

//读取记录模板
char YLDpParser::ReadRecordTemplate(ifstream &dpFile, streamoff offset)
{
	char cRecordTemplate;
	dpFile.read(&cRecordTemplate, 1);

	return cRecordTemplate;
}

//解析TLV结构
void YLDpParser::ParseTLV(char* buffer, int nBufferLen, DGI &YLDGI)
{
	TLVParaser *parser = new TLVParaser();
	TLVEntity entities[1024] = { 0 };
	unsigned int entitiesCount = 0;
	parser->TLVConstruct((unsigned char*)buffer, nBufferLen, entities, entitiesCount);
	unsigned char parseBuf[4096] = { 0 };
	unsigned int buf_count;
	//解析TLV   
	parser->TLVParseAndFindError(entities, entitiesCount, parseBuf, buf_count);
	for (unsigned int i = 0; i < entitiesCount; i++)
	{
		string strTag = Tool::Converter::StrToHex((char*)entities[i].Tag, entities[i].TagSize);
		string strLen = Tool::Converter::StrToHex((char*)entities[i].Length, entities[i].LengthSize);
		int nLen = std::stoi(strLen, 0, 16);
		string strValue = Tool::Converter::StrToHex((char*)entities[i].Value, nLen);

		/****************小插曲，用于生成文件的文件名********************/
		string temp = Tool::Stringparser::DeleteSpace(strTag);
		if (temp.substr(0,2).compare("57") == 0)
		{
			int index = strValue.find('D');
			if (index != string::npos)
			{
				m_CurrentDpData.strAccount = strValue.substr(0, index);
			}
		}
		/*************************************************************/
		TLVItem item(strTag, strLen, strValue);
		YLDGI.vecItem.push_back(item);	
	}

	m_CurrentDpData.vecDpData.push_back(YLDGI);	//添加一个DGI分组
}

//处理PSE数据
void YLDpParser::DealPSEDataCompleted(ifstream &dpFile, streamoff offset)
{
	PSE_TYPE pseType = PSE_TYPE::PSE1;
	
	
	for (int i = PSE1; i < PSE_TYPE::END; i++)
	{
        DGI YLDGI;
		YLDGI.DGIName = m_DGIName[m_DGIName.size() + i - PSE_TYPE::END];
		//DGI起始标志		
		ReadDGIStartTag(dpFile, dpFile.tellg());

		//后续数据长度1

		int nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());
		switch (pseType)
		{
		case PSE1:
			
			//记录模板
			//ReadRecordTemplate(dpFile, dpFile.tellg());
			//后续数据长度2
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
		//读取TLV数据
		char* buffer = new char[nFollowedDataLen];
		memset(buffer, 0, nFollowedDataLen);
		dpFile.read(buffer, nFollowedDataLen);
        string s;
        s = Tool::Converter::StrToHex(buffer, nFollowedDataLen);
        char len[12] = { 0 };
        sprintf_s(len, 12, "%02X", nFollowedDataLen * 2);
        TLVItem item(YLDGI.DGIName, len, s);
        YLDGI.vecItem.push_back(item);
        m_CurrentDpData.vecDpData.push_back(YLDGI);
	}
}

//读取银联DP文件
int YLDpParser::Read(const string& filePath)
{
	ifstream dpFile(filePath.c_str(), ios::binary);
	if (!dpFile)
		return -1;
	dpFile.seekg(0, ios::end);
	long dpFileSize =static_cast<long>(dpFile.tellg());
	if (dpFileSize <= m_reserved)
	{
		Log->Error("银联DP文件大小异常! 检查DP文件大小[%l]", dpFileSize);
		return -1;
	}
	//重置文件指针保留数据末尾位置
	dpFile.seekg(m_reserved,ios::beg);

	//读取DGI分组名称
	ReadDGIName(dpFile, dpFile.tellg());

	//读取文件直到文件末尾
	while (dpFile.tellg() < dpFileSize)
	{
		//清空当前DP数据
		ClearCurrentDPData();

		//卡片序列号
		m_CurrentDpData.nCardSequence = ReadCardSequence(dpFile, dpFile.tellg());

		//卡片个人化数据内容总长度
		ReadCardPersonalizedTotelLen(dpFile, dpFile.tellg());
		for (unsigned int i = 0; i < m_DGIName.size() - PSE_TYPE::END; i++)
		{
			DGI YLDGI;
			YLDGI.DGIName = m_DGIName[i];

			//DGI起始标志
			char tag = ReadDGIStartTag(dpFile, dpFile.tellg());
			if ((unsigned char)tag != 0x86)
			{
				Log->Error("获取DGI分组标识[%X]错误", tag);
				return -1;
			}

			//后续数据长度1
			int nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());

			//DGI序号
			unsigned short sDGISeq = ReadDGISequence(dpFile, dpFile.tellg());

			//后续数据长度2
			nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());

			//记录模板
			bool isInnerDGI = false;			
			if (sDGISeq < 0x0b01)
			{
				ReadRecordTemplate(dpFile, dpFile.tellg());
			}
			else {
				isInnerDGI = true;
			}

			//后续数据长度3
			if (!isInnerDGI)
			{
				nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());
			}

			char* buffer = new char[nFollowedDataLen];
			memset(buffer, 0, nFollowedDataLen);
			dpFile.read(buffer, nFollowedDataLen);

			//是否仅包含数据内容，不是标准的TLV结构
			if (IsNeedConvertDGI(sDGISeq))
			{
				char szTag[5] = { 0 };
				sprintf_s(szTag, "%X", sDGISeq);
				string strValue = Tool::Converter::StrToHex(buffer, nFollowedDataLen);
				TLVItem item(szTag, "0", strValue);
				YLDGI.vecItem.push_back(item);
				m_CurrentDpData.vecDpData.push_back(YLDGI);	//添加一个DGI分组
			}
			else
			{
				ParseTLV(buffer, nFollowedDataLen, YLDGI);
			}
		}

		DealPSEDataCompleted(dpFile,dpFile.tellg());
		m_YLDpData.push_back(m_CurrentDpData);
	}

	return 0;
}

bool YLDpParser::IsNeedDecrpt(string tag)
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
		_Des3(pOutput, "0123456789ABCDEF1111111111111111", (char*)value.substr(i, unit).c_str());
		strResult += string(pOutput);
	}

	return strResult;
}

string YLDpParser::GetTagValue(string Tag)
{
    string result;
    for (auto f : m_YLDpData)
    {
        for (auto iter = f.vecDpData.begin(); iter != f.vecDpData.end(); iter++)
        {
            if (iter->DGIName.substr(3) == Tag)
                return iter->vecItem[0].strValue;
        }
    }

    return result;
}

void YLDpParser::SetTagValue(string tag, string value)
{
    string result;
    for (auto &f : m_YLDpData)
    {
        for (auto iter = f.vecDpData.begin(); iter != f.vecDpData.end(); iter++)
        {
            if (iter->DGIName.substr(3) == tag)
            {
                iter->vecItem[0].strValue = value;
                return;
            }               
        }
    }
}


void YLDpParser::FilterDpData()
{
    for (auto f : m_YLDpData)  //交换tag8202与8203, tag8204与8205
    {      
        string dgi8202 = GetTagValue(DGI_8202);
        string dgi8203 = GetTagValue(DGI_8203);
        string dgi8204 = GetTagValue(DGI_8204);
        string dgi8205 = GetTagValue(DGI_8205);
        SetTagValue(DGI_8202, dgi8203);
        SetTagValue(DGI_8203, dgi8202);
        SetTagValue(DGI_8204, dgi8205);
        SetTagValue(DGI_8205, dgi8204);
    }

	for (auto &f : m_YLDpData)
	{	//处理每一个文件数据
		DP dpData;
        dpData.fileName = f.strAccount;	//文件名称
		for (auto v : f.vecDpData)	//处理每一个DGI
		{
			DTL dtl;
			string DGI = Tool::Stringparser::DeleteSpace(v.DGIName);
			if (DGI.substr(0, 3) == "DGI")
			{
				DGI = DGI.substr(3);
			}
			for (auto tlv : v.vecItem)	//处理每一个DGI下面的TLV结构
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
				if (tlv.strValue.empty())	//如果value为空，直接抛弃该tag
				{
					continue;
				}
                string Value;
                if (DGI == "Store_PPSE" || 
                    DGI == "Store_PSE_1" || 
                    DGI == "Store_PSE_2" ||
                    (IsNeedConvertDGI(stoi(DGI,0,16)) && !IsNeedDecrpt(DGI)))
                {
                    Value = tlv.strValue;
                }
                else {
                    Value = Tool::Stringparser::DeleteSpace(tlv.strTag + tlv.strLen + tlv.strValue);
                }
                   
				dtl.DGI = DGI;
				dtl.Tag = Tag;
				if (IsNeedDecrpt(Tag))	//是否需要解密
				{
					Value = DecrptData(Tag, tlv.strValue);
				}
				dtl.Value = Value;
				dpData.data.push_back(dtl);
			}
	
		}
		m_vecDP.push_back(dpData);
	}
}

//保存DP数据
void YLDpParser::Save(const string &strPath)
{
	if (!CheckFolderExist(strPath))
	{
		CreateDirectory(strPath.c_str(), NULL);
	}
	if (m_vecDP.empty())
	{
		FilterDpData();
	}
	for (auto m : m_vecDP)
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

//目录是否存在的检查：
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

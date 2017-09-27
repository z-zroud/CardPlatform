#include "stdafx.h"
#include "YLDPParaser.h"
#include "Util\Log.h"
#include "Util\Tool.h"
#include "Util\IniParaser.h"
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
* 功能：设置加密数据的解密密钥
**********************************************************/
void YLDpParser::SetDecryptKey(const string &key)
{
    m_key = key;
}


/*****************************************************************
* 功能： 设置仅包含值的GGI,非标准TLV结构
******************************************************************/
void YLDpParser::SetOnlyValueOfDGI(vector<unsigned short> vecValueDGI)
{
    for (auto v : vecValueDGI)
    {
        m_valueOnlyDGI.push_back(v);
    }
}

/*****************************************************************
* 功能：设置加密的tag
******************************************************************/
void YLDpParser::SetEncryptTag(vector<string> vecEncryptDGI)
{
    for (auto v : vecEncryptDGI)
    {
        m_encryptTag.push_back(v);
    }
}

/****************************************************************
* 功能：设置需要交换值的tag
*****************************************************************/
void YLDpParser::SetExchangeDGI(map<string, string> mapDGI)
{
    for (auto m : mapDGI)
    {
        m_exchangeDGI.insert(m);
    }
}

//特殊DGI分组
YLDpParser::YLDpParser()
{
	//m_ValueOnlyDGI = { 0x8000,0x9000,0x8010,0x9010,0x8201,0x9103,
	//	0x8202,0x8203,0x8204,0x8205,0x9102, };
	//m_encryptTag = { "8000","8201","8202","8203" ,"8204","8205" };
}

/********************************************************************
* 功能：判断是否为仅有值的tag
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
* 功能：判断是否为加密tag
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
        m_dGIName.push_back(DGIName);
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
		string strTag = Tool::StrToHex((char*)entities[i].Tag, entities[i].TagSize);
		string strLen = Tool::StrToHex((char*)entities[i].Length, entities[i].LengthSize);
		int nLen = std::stoi(strLen, 0, 16);
		string strValue = Tool::StrToHex((char*)entities[i].Value, nLen);

		/****************小插曲，用于生成文件的文件名********************/
		string temp = Tool::DeleteSpace(strTag);
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

	m_oneCardDpData.vecDpData.push_back(YLDGI);	//添加一个DGI分组
}

/********************************************************************
* 功能：处理PSE、PPSE数据
*********************************************************************/
void YLDpParser::DealPSEDataCompleted(ifstream &dpFile, streamoff offset)
{
	PSE_TYPE pseType = PSE_TYPE::PSE1;
	
	
	for (int i = PSE1; i < PSE_TYPE::END; i++)
	{
        DGI YLDGI;
		YLDGI.DGIName = m_dGIName[m_dGIName.size() + i - PSE_TYPE::END];
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
        s = Tool::StrToHex(buffer, nFollowedDataLen);
        char len[12] = { 0 };
        sprintf_s(len, 12, "%02X", nFollowedDataLen * 2);
        TLVItem item(YLDGI.DGIName, len, s);
        YLDGI.vecItem.push_back(item);
        m_oneCardDpData.vecDpData.push_back(YLDGI);
	}
}

/********************************************************************
* 功能: 读取银联DP数据，并解析
*********************************************************************/
int YLDpParser::Read(const string& filePath)
{
	ifstream dpFile(filePath.c_str(), ios::binary);
	if (!dpFile)
		return -1;
	dpFile.seekg(0, ios::end);
	long dpFileSize =static_cast<long>(dpFile.tellg());
	if (dpFileSize <= m_reserved)   //检查文件是否异常
	{
		Log->Error("银联DP文件大小异常! 检查DP文件大小[%l]", dpFileSize);
		return -1;
	}	
	dpFile.seekg(m_reserved,ios::beg);  //重置文件指针保留数据末尾位置

	//第一步： 读取DGI分组
	ReadDGIName(dpFile, dpFile.tellg());

	//第二步： 遍历后续数据，每一次遍历解析一个卡片数据
    // 注意，一个DP文件可能包含几张卡片的数据
	while (dpFile.tellg() < dpFileSize)
	{
		//清空当前DP数据
		ClearCurrentDPData();

		//第三步： 读取卡片序列号
		m_oneCardDpData.nCardSequence = ReadCardSequence(dpFile, dpFile.tellg());

		//第四步： 读取该卡片个人化数据内容总长度
		ReadCardPersonalizedTotelLen(dpFile, dpFile.tellg());
		for (unsigned int i = 0; i < m_dGIName.size() - PSE_TYPE::END; i++)
		{   //解析每一张卡片数据
			DGI YLDGI;
			YLDGI.DGIName = m_dGIName[i];

			//第五步： 读取DGI起始标志
			char tag = ReadDGIStartTag(dpFile, dpFile.tellg());
			if ((unsigned char)tag != 0x86)
			{
				Log->Error("获取DGI分组标识[%X]错误", tag);
				return -1;
			}

			//第六步： 读取该DGI总数据长度(包含了该DGI序号)
			int nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());

			//第七步： 读取该DGI序号
			unsigned short sDGISeq = ReadDGISequence(dpFile, dpFile.tellg());

			//第八步： 读取后续数据长度(该DGI数据部分)
			nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());

			//第九步： 读取记录模板(该模板仅当DGI序号小于0x0B01时出现)
			bool isInnerDGI = false;			
			if (sDGISeq < 0x0b01)
			{
				ReadRecordTemplate(dpFile, dpFile.tellg());
			}
			else {
				isInnerDGI = true;
			}

			//第十步： 读取后续TLV结构的数据长度
			if (!isInnerDGI)
			{
				nFollowedDataLen = ReadFollowedDataLength(dpFile, dpFile.tellg());
			}

			char* buffer = new char[nFollowedDataLen];
			memset(buffer, 0, nFollowedDataLen);
			dpFile.read(buffer, nFollowedDataLen);

			//第十一步： 判断是否仅包含数据内容，不是标准的TLV结构
			if (IsValueOnlyDGI(sDGISeq))
			{
				char szTag[5] = { 0 };
				sprintf_s(szTag, "%X", sDGISeq);
				string strValue = Tool::StrToHex(buffer, nFollowedDataLen);
				TLVItem item(szTag, "0", strValue);
				YLDGI.vecItem.push_back(item);
				m_oneCardDpData.vecDpData.push_back(YLDGI);	//添加一个DGI分组
			}
			else
			{   //解析标准TLV结构
				ParseTLV(buffer, nFollowedDataLen, YLDGI);
			}
		}

        //第十二步： 解析PSE PPSE数据
		DealPSEDataCompleted(dpFile,dpFile.tellg());
		m_manyCardDpData.push_back(m_oneCardDpData);
	}

	return 0;
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

/**********************************************************
* 功能： 获取指定tag值
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
* 功能： 设定指定tag值
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
* 功能： 将DP个人化数据转化为CPS结构的个人化数据
*****************************************************************/
void YLDpParser::FilterDpData()
{
    for (auto &f : m_manyCardDpData)  //交换tag8202与8203, tag8204与8205
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
	{	//处理每一个文件数据
		CPSDP dpData;
        dpData.fileName = f.strAccount;	//文件名称
		for (auto v : f.vecDpData)	//处理每一个DGI
		{
			DTL dtl;
			string DGI = Tool::DeleteSpace(v.DGIName);
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
				string Tag = Tool::DeleteSpace(tlv.strTag);
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
                    (IsValueOnlyDGI(stoi(DGI,0,16)) && !IsEncryptDGI(DGI)))
                {
                    Value = tlv.strValue;
                }
                else if (DGI == _T("0301")) //对银联0301分组特殊处理，为90发卡行公钥证书
                {
                    dtl.DGI = DGI;
                    dtl.Tag = _T("90");
                    dtl.Value = tlv.strValue;
                    dpData.data.push_back(dtl);
                    continue;
                }
                else {
                    Value = Tool::DeleteSpace(tlv.strTag + tlv.strLen + tlv.strValue);
                }
                   
				dtl.DGI = DGI;
				dtl.Tag = Tag;
				if (IsEncryptDGI(Tag))	//是否需要解密
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
* 功能： 保存CPS个人化的数据到指定的文件
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
		IniParser ini;

		for (auto v : m.data)
		{
            ini.SetValue(v.DGI, v.Tag, v.Value);
		}
		ini.Save(filePath);
	}
}

/**********************************************************
* 功能： 检查目录是否存在
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

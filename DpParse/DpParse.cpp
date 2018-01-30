#include "stdafx.h"
#include "IDpParse.h"
#include "../Util/Des0.h"
#include "../Util/SM.hpp"
#include <fstream>
#include "../Util/IniConfig.h"
#include "../Util/Tool.h"
#include "../Util/ParseTLV.h"

//**********************************************************
//* 功能： 检查目录是否存在
//***********************************************************/
bool  CheckFolderExist(const string &strPath)
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

string StrToHex(const char* strBin, int len, bool bIsUpper)
{
	char *pCopy = new char[len + 1];
	memset(pCopy, 0, len + 1);
	memcpy(pCopy, strBin, len);
	string strHex;
	//strHex.resize(strBin.size() * 2);
	strHex.resize(len * 2);
	for (int i = 0; i < len; i++)
	{
		uint8_t cTemp = pCopy[i];
		for (size_t j = 0; j < 2; j++)
		{
			uint8_t cCur = (cTemp & 0x0f);
			if (cCur < 10)
			{
				cCur += '0';
			}
			else
			{
				cCur += ((bIsUpper ? 'A' : 'a') - 10);
			}
			strHex[2 * i + 1 - j] = cCur;
			cTemp >>= 4;
		}
	}

	return strHex;
}

//删除字符串中的空格
string DeleteSpace(string s)
{
	string strResult;
	int len = s.length();
	for (auto c : s)
	{
		if (c != ' ')
		{
			strResult.push_back(c);
		}
	}
	return strResult;
	int index = 0;
	if (!s.empty())
	{
		while ((index = s.find(' ', index)) != string::npos)
		{
			s.erase(index, 1);
		}
	}
	return strResult;
}

bool ExistedInList(string value, vector<string> collection)
{
    for (auto item : collection)
    {
        if (item == value) {
            return true;
        }
    }
    return false;
}

string Dict::GetItem(string tag)
{
    for (auto item : m_vecItems) {
        if (item.first == tag) {
            return item.second;
        }
    }
    
    return "";
}

void Dict::ReplaceItem(string key, string value)
{
    for (auto& item : m_vecItems) {
        if (item.first == key) {
            item.second = value;
            break;
        }
    }
}

void Dict::ReplaceKey(string oldKey, string newKey)
{
    for (auto& item : m_vecItems) {
        if (item.first == oldKey) {
            item.first = newKey;
            break;
        }
    }
}

void Dict::DeleteItem(string key)
{
    for (auto iter = m_vecItems.begin(); iter != m_vecItems.end(); iter++) {
        if (iter->first == key) {
            m_vecItems.erase(iter);
            break;
        }
    }
}

/*****************************************************
* 保存CPS数据
******************************************************/
void IDpParse::Save(CPS_ITEM cpsItem)
{
	int pos = cpsItem.fileName.find_last_of('\\');
	string folder = cpsItem.fileName.substr(0, pos);
	if (!CheckFolderExist(folder))
	{
		CreateDirectory(folder.c_str(), NULL);
	}
	ofstream outputFile(cpsItem.fileName.c_str());
	if (!outputFile)
		return;
    string privousDGI = "";
	for (auto iter : cpsItem.items)
	{
        if (iter.dgi != privousDGI) {
            outputFile << "[" << iter.dgi << "]" << endl;
        }
		for (auto tlvData : iter.value.GetItems())
		{
			outputFile << tlvData.first << "=" << tlvData.second << endl;
		}
        privousDGI = iter.dgi;
	}
	outputFile.close();
	outputFile.clear();
}

string IDpParse::SMDecryptDGI(string tk, string encryptData)
{
    string result;
    PDllSM4_CBC_DEC SM4_CBC_DEC = GetSMFunc<PDllSM4_CBC_DEC>("dllSM4_CBC_DEC");
    int len = encryptData.length();
    char* decryptedData = new char[len];
    memset(decryptedData, 0, len);
    if (SM4_CBC_DEC)
    {
        SM4_CBC_DEC((char*)tk.c_str(), (char*)encryptData.c_str(), decryptedData);
        result = decryptedData;
        delete [] decryptedData;
    }
    return result;
}

string IDpParse::DecryptDGI(string tk, string encryptData)
{
	string strResult;

	int len = encryptData.length();
	int unit = 16;
	for (int i = 0; i < len; i += unit)
	{
		char* pOutput = new char[unit + 1];
		memset(pOutput, 0, unit + 1);
		_Des3(pOutput, (char*)tk.c_str(), (char*)encryptData.substr(i, unit).c_str());
		strResult += string(pOutput);
	}

	return strResult;
}

/******************************************************************
* 获取文件大小
*******************************************************************/
long IDpParse::GetFileSize(ifstream& dpFile)
{
	dpFile.seekg(0, ios::end);
	long dpFileSize = static_cast<long>(dpFile.tellg());
	dpFile.seekg(0, ios::beg);  //重置文件指针至保留数据末尾位置

	return dpFileSize;
}

/******************************************************************
* 打开文件
*******************************************************************/
bool IDpParse::OpenDpFile(const char* fileName, ifstream& dpFile)
{
	int pos = string(fileName).find_last_of('\\');
	string path = string(fileName).substr(0, pos + 1);
	dpFile.open(fileName, ios::in | ios::binary);
	if (!dpFile.is_open())
		return false;

	return true;
}


/******************************************************************
* 获取DP文件中，从当前流指针位置到bufferLen长的数据，返回流指针位置
*******************************************************************/
streampos IDpParse::GetBuffer(ifstream &dpFile, char* buffer, int bufferLen)
{
	dpFile.read(buffer, bufferLen);

	return dpFile.tellg();
}

streampos IDpParse::GetBCDBuffer(ifstream &dpFile, string& buffer, int len)
{
	char * readBuffer = new char[len + 1];
	memset(readBuffer, 0, len + 1);

	dpFile.read(readBuffer, len);
	buffer = StrToHex(readBuffer, len);
	delete[] readBuffer;

	return dpFile.tellg();
}

streampos IDpParse::GetBCDBufferLittleEnd(ifstream &dpFile, string &buffer, int len)
{
    string data;
    streampos pos = GetBCDBuffer(dpFile, data, len);
    if (data.length() % 2 != 0) {
        return pos;
    }
    int i = data.length() - 2;
    while(i >= 0) {
        buffer += data.substr(i, 2);
        i -= 2;
    }

    return pos;
}

streampos IDpParse::GetLenTypeBuffer(ifstream &dpFile, int &dataLen, int len)
{
	string buffer;
	GetBCDBuffer(dpFile, buffer, len);
	dataLen = stoi(buffer, 0, 16);

	return dpFile.tellg();
}

streampos IDpParse::GetLenTypeBufferLittleEnd(ifstream &dpFile, int &dataLen, int len)
{
    string buffer;
    GetBCDBufferLittleEnd(dpFile, buffer, len);
    dataLen = stoi(buffer, 0, 16);

    return dpFile.tellg();
}

void IDpParse::ParseTLV(char* dgiBuffer, unsigned int bufferLen, Dict& tlvs, bool littleEnd)
{
    BCD_TLV entities[1024] = { 0 };
    unsigned int entitiesCount = 0;
    ParseBcdTLV(dgiBuffer, entities, entitiesCount);
    unsigned char parseBuf[4096] = { 0 };
    for (unsigned int i = 0; i < entitiesCount; i++)
    {
        string strLen = (char*)entities[i].length;
        string strValue = (char*)entities[i].value;
        string strTag = (char*)entities[i].tag;
        int nLen = std::stoi((char*)entities[i].length, 0, 16);
        if (nLen > 0xFF) {
            char dataLen[5] = { 0 };
            Tool::GetBcdDataLen(strValue.c_str(), dataLen, 5);
            strLen = "82" + string(dataLen);        
        }
        else if (nLen > 0x79) {
            char dataLen[5] = { 0 };
            Tool::GetBcdDataLen(strValue.c_str(), dataLen, 5);
            strLen = "81" + string(dataLen);
        }
        
        strValue = strTag + strLen + strValue;
        tlvs.InsertItem(strTag, strValue);
    }
}

string IDpParse::GetAccount(CPS_ITEM& cpsItem)
{
    for (auto item : cpsItem.items) {
        string tag57 = item.value.GetItem("57");
        if (tag57.length()) {
            int index = tag57.find('D');
            if (index != string::npos)
            {
                return tag57.substr(4, index - 4);
            }
        }
    }
    return "";
}

bool IDpParse::IsTlvStruct(char* buffer, unsigned int bufferLength, bool littleEnd)
{
    return IsBcdTlvStruct(buffer, bufferLength);
}

void IDpParse::HandleRule(string ruleConfig, CPS_ITEM& cpsItem)
{
    IRule ruleObj;
	ruleObj.SetRule(ruleConfig);
	ruleObj.HandleRule(cpsItem);
}

/////////////////////////////////////////////////////////////////////////////////////
/**********************************************************
用于解析ruleFile文件
***********************************************************/
void IRule::SetRule(const string& ruleFile)
{
    IniConfig ruleCfg;
    if (ruleCfg.Read(ruleFile)) 
    {
        m_tk = ruleCfg.GetValue("TK", "key");

        vector<pair<string, string>> deleteDGIs;
        ruleCfg.GetValue("DeleteDGI", deleteDGIs);
        for (auto item : deleteDGIs) {
            m_vecDGIDelete.push_back(item.second);
        }

        vector<pair<string, string>> encryptDGIs;
        ruleCfg.GetValue("EncryptDGI", encryptDGIs);
        for (auto item : encryptDGIs) {
            m_vecDGIEncrypt.push_back(item.second);
        }

        vector<pair<string, string>> padding80DGIs;
        ruleCfg.GetValue("EncryptDGIPadding80", padding80DGIs);
        for (auto item : padding80DGIs) {
            m_vecDGIpadding80.push_back(item.second);
        }

        ruleCfg.GetValue("ExchangeDGIData", m_vecDGIExchange);
        ruleCfg.GetValue("DGIMap", m_vecDGIMap);
        ruleCfg.GetValue("DeleteTag", m_vecTagDelete);
    }
}

void IRule::HandleRule(CPS_ITEM& cpsItem)
{
    //处理顺序：DGI映射-->DGI交换-->DGI删除-->DGI解密-->DGI删除tag
    HandleDGIMap(cpsItem);
    HandleDGIExchange(cpsItem);
    HandleDGIDelete(cpsItem);
    HandleDGIEncrypt(cpsItem);
    HandleTagDelete(cpsItem);
}

void IRule::HandleDGIMap(CPS_ITEM& cpsItem)
{
	for (auto item : m_vecDGIMap)
	{
		for (auto& dgiItem : cpsItem.items)
		{
			if (item.first == dgiItem.dgi)
			{
				dgiItem.dgi = item.second;
                dgiItem.value.ReplaceKey(item.first, item.second);
				break;
			}
		}
	}
}
void IRule::HandleDGIExchange(CPS_ITEM& cpsItem)
{
	for (auto item : m_vecDGIExchange)
	{
		int firstIndex = 0;
		int secondIndex = 0;
		while (item.first != cpsItem.items[firstIndex].dgi)
			firstIndex++;
		while (item.second != cpsItem.items[secondIndex].dgi)
			secondIndex++;
        cpsItem.items[firstIndex].dgi = item.second;
        cpsItem.items[firstIndex].value.ReplaceKey(item.first, item.second);
        cpsItem.items[secondIndex].dgi = item.first;
        cpsItem.items[secondIndex].value.ReplaceKey(item.second, item.first);
	}
}

/***************************************************************
* 解密DGI数据，注意该DGI只应只有一个tag=encryptData, 并且tag==DGI
****************************************************************/
void IRule::HandleDGIEncrypt(CPS_ITEM& cpsItem)
{
   
    for (auto& item : cpsItem.items)
    {
        string decryptedData;
        if (ExistedInList(item.dgi, m_vecDGIEncrypt))
        {
            string encryptData = item.value.GetItem(item.dgi);
            if (ExistedInList(item.dgi,m_vecDGIpadding80))
            {
                if (encryptData.length() % 16 == 0)
                {
                    encryptData += "8000000000000000";
                }
                else {
                    encryptData += "80";
                    int remaindZero = encryptData.length() % 16;
                    encryptData.append(remaindZero, '0');
                }
            }

            int len = encryptData.length();
            int unit = 16;
            for (int i = 0; i < len; i += unit)
            {
                char* pOutput = new char[unit + 1];
                memset(pOutput, 0, unit + 1);
                _Des3(pOutput, (char*)m_tk.c_str(), (char*)encryptData.substr(i, unit).c_str());
                decryptedData += string(pOutput);
            }
            item.value.ReplaceItem(item.dgi, decryptedData);
        }
    }
}

void IRule::HandleDGIDelete(CPS_ITEM& cpsItem)
{
	for (auto item : m_vecDGIDelete)
	{
		for (auto iter = cpsItem.items.begin(); iter != cpsItem.items.end(); iter++)
		{
			if (item == iter->dgi)
			{
				iter = cpsItem.items.erase(iter);
                break;
			}
		}
	}
}

void IRule::HandleTagDelete(CPS_ITEM& cpsItem)
{
	for (auto item : m_vecTagDelete)
	{
		for (auto &dgiItem : cpsItem.items)
		{
			if (item.first == dgiItem.dgi)
			{
                dgiItem.value.DeleteItem(item.second);
                break;
			}
		}
	}
}



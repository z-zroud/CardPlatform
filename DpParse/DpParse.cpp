#include "stdafx.h"
#include "IDpParse.h"
#include "../Util/Des0.h"
#include "../Util/SM.hpp"
#include <fstream>
#include "../Util/IniConfig.h"
#include "../Util/Tool.h"
#include "../Util/ParseTLV.h"
#include "../Util/rapidxml/rapidxml.hpp"
#include "../Util/rapidxml/rapidxml_utils.hpp"

using namespace rapidxml;

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

bool Dict::TagExisted(string tag)
{
    for (auto item : m_vecItems) {
        if (item.first == tag) {
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
        else if (nLen > 0x80) {
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

void IDpParse::HandleRule(const char* szRuleConfig, CPS_ITEM& cpsItem)
{
    IRule ruleObj;
	if(ruleObj.SetRuleCfg(szRuleConfig))
	    ruleObj.HandleRule(cpsItem);
}

/////////////////////////////////////////////////////////////////////////////////////
/**********************************************************
用于解析ruleFile文件
***********************************************************/
bool IRule::SetRuleCfg(const char* szRuleConfig)
{
    //使用rapidxml::file读取文件更方便  
    rapidxml::file<char> fdoc(szRuleConfig);

    rapidxml::xml_document<> doc;
    doc.parse<0>(fdoc.data());

    //在XML文档中寻找第一个节点  
    const rapidxml::xml_node<> *root = doc.first_node("Rule");
    if (NULL == root)
    {
        return false;
    }
    for (rapidxml::xml_node<> *node = root->first_node(); node != NULL; node = node->next_sibling())
    {
        string nodeName = node->name();
        if (nodeName == "Encrypt") {
            DGIEncrypt encryptDGI;
            encryptDGI.dgi = node->first_attribute("DGI")->value();
            encryptDGI.type = node->first_attribute("type")->value();
            encryptDGI.key = node->first_attribute("key")->value();
            m_vecDGIEncrypt.push_back(encryptDGI);
        }
        else if (nodeName == "TagEncrypt") {
            TagEncrypt encryptTag;
            encryptTag.tag = node->first_attribute("tag")->value();
            encryptTag.type = node->first_attribute("type")->value();
            encryptTag.key = node->first_attribute("key")->value();
            if (node->first_attribute("startPos") == NULL) {
                encryptTag.startPos = 0;
            }
            else {
                string startPos = node->first_attribute("startPos")->value();
                encryptTag.startPos = stoi(startPos);
            }

            if (node->first_attribute("len") == NULL) {
                encryptTag.len = 0;
            }
            else {
                string tagLen = node->first_attribute("len")->value();
                encryptTag.len = stoi(tagLen);
            }           
            m_vecTagEncrypt.push_back(encryptTag);
        }
        else if (nodeName == "Map") {
            DGIMap mappedDGI;
            mappedDGI.srcDgi = node->first_attribute("srcDGI")->value();
            mappedDGI.dstDgi = node->first_attribute("dstDGI")->value();
            m_vecDGIMap.push_back(mappedDGI);
        }
        else if (nodeName == "Exchange") {
            DGIExchange exchangedDGI;
            exchangedDGI.srcDgi = node->first_attribute("srcDGI")->value();
            exchangedDGI.exchangedDgi = node->first_attribute("exchangedDGI")->value();
            m_vecDGIExchange.push_back(exchangedDGI);
        }
        else if (nodeName == "AddTagFromDGI") {
            AddTagFromDGI vecDGI;
            vecDGI.srcDgi = node->first_attribute("srcDGI")->value();
            vecDGI.dstDgi = node->first_attribute("dstDGI")->value();
            vecDGI.dstTag = node->first_attribute("dstTag")->value();
            m_vecAddTagFromDGI.push_back(vecDGI);
        }
        else if (nodeName == "AddFixedTag") {
            AddFixedTagValue vecDGI;
            vecDGI.srcDgi = node->first_attribute("srcDGI")->value();
            vecDGI.tag = node->first_attribute("tag")->value();
            vecDGI.tagValue = node->first_attribute("tagValue")->value();
            m_vecFixedTagAdd.push_back(vecDGI);
        }
        else if (nodeName == "DeletedDGI") {
            string deletedDgi = node->first_attribute("DGI")->value();
            m_vecDGIDelete.push_back(deletedDgi);
        }
        else if (nodeName == "DeletedTag") {
            DGIDeleteTag deleteTag;
            deleteTag.srcDgi = node->first_attribute("DGI")->value();
            deleteTag.tag = node->first_attribute("tag")->value();
            m_vecTagDelete.push_back(deleteTag);
        }
    }
  
    return true;
}

void IRule::HandleRule(CPS_ITEM& cpsItem)
{
    //处理顺序：DGI映射-->DGI交换-->DGI删除-->DGI解密-->DGI删除tag
    HandleDGIDecrypt(cpsItem);
    HandleTagDecrypt(cpsItem);
    HandleDGIMap(cpsItem);
    HandleDGIExchange(cpsItem);
    HandleDGIAddTag(cpsItem);
    HandleDGIAddFixedTag(cpsItem);
    HandleDGIDelete(cpsItem);    
    HandleTagDelete(cpsItem);
}

/****************************************************************
* 功能：更换DGI分组，例如将DGI8020映射为DGI9307
*****************************************************************/
void IRule::HandleDGIMap(CPS_ITEM& cpsItem)
{
	for (auto item : m_vecDGIMap)
	{
		for (auto& dgiItem : cpsItem.items)
		{
			if (item.srcDgi == dgiItem.dgi)
			{
				dgiItem.dgi = item.dstDgi;
                dgiItem.value.ReplaceKey(item.srcDgi, item.dstDgi);
				break;
			}
		}
	}
}

/**************************************************************
* 功能：交换DGI分组的值
***************************************************************/
void IRule::HandleDGIExchange(CPS_ITEM& cpsItem)
{
	for (auto item : m_vecDGIExchange)
	{
		int firstIndex = 0;
		int secondIndex = 0;
		while (item.srcDgi != cpsItem.items[firstIndex].dgi)
			firstIndex++;
		while (item.exchangedDgi != cpsItem.items[secondIndex].dgi)
			secondIndex++;
        cpsItem.items[firstIndex].dgi = item.exchangedDgi;
        cpsItem.items[firstIndex].value.ReplaceKey(item.srcDgi, item.exchangedDgi);
        cpsItem.items[secondIndex].dgi = item.srcDgi;
        cpsItem.items[secondIndex].value.ReplaceKey(item.exchangedDgi, item.srcDgi);
	}
}

void IRule::HandleTagDecrypt(CPS_ITEM& cpsItem)
{
    for (auto& encryptedItem : m_vecTagEncrypt)
    {
        for (auto& item : cpsItem.items) {
            if (item.value.TagExisted(encryptedItem.tag)) {
                string decryptedData;
                string encryptData = item.value.GetItem(encryptedItem.tag).substr(encryptedItem.tag.length() + 2);
                if (encryptedItem.type == "SM") {
                    decryptedData = SMDecryptDGI(encryptedItem.key, encryptData);
                }
                else {
                    decryptedData = DesDecryptDGI(encryptedItem.key, encryptData);
                }
                if (decryptedData.length() > encryptedItem.len && encryptedItem.len > 0) {
                    decryptedData = decryptedData.substr(encryptedItem.startPos, encryptedItem.len);
                }
                char decryptedDataLen[3];
                Tool::GetBcdDataLen(decryptedData.c_str(), decryptedDataLen, 3);
                decryptedData = encryptedItem.tag + decryptedDataLen + decryptedData;
                item.value.ReplaceItem(encryptedItem.tag, decryptedData);
            }
        }
    }
}

/***************************************************************
* 解密DGI数据，注意该DGI只应只有一个tag=encryptData, 并且tag==DGI
****************************************************************/
void IRule::HandleDGIDecrypt(CPS_ITEM& cpsItem)
{ 
    for (auto& encryptedItem : m_vecDGIEncrypt)
    {
        for(auto& item : cpsItem.items)
        {
            if (item.dgi == encryptedItem.dgi) {
                string decryptedData;
                if (encryptedItem.type == "SM") {
                    decryptedData = SMDecryptDGI(encryptedItem.key, item.value.GetItem(item.dgi));
                }
                else {
                    decryptedData = DesDecryptDGI(encryptedItem.key, item.value.GetItem(item.dgi));
                }
                item.value.ReplaceItem(item.dgi, decryptedData);
            }            
        }
    }
}

/*****************************************************************
* 功能：删除DGI分组
******************************************************************/
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

/***************************************************
* 功能：删除某个DGI分组中的tag
****************************************************/
void IRule::HandleTagDelete(CPS_ITEM& cpsItem)
{
	for (auto item : m_vecTagDelete)
	{
		for (auto &dgiItem : cpsItem.items)
		{
			if (item.srcDgi == dgiItem.dgi)
			{
                dgiItem.value.DeleteItem(item.tag);
                break;
			}
		}
	}
}

/********************************************************
* 功能：给某个DGI分组添加固定的tag值
*********************************************************/
void IRule::HandleDGIAddFixedTag(CPS_ITEM& cpsItem)
{
    for (auto& dgiItem : cpsItem.items)
    {
        for (auto item : m_vecFixedTagAdd) {
            if (dgiItem.dgi == item.srcDgi) {
                char tagValueLen[3] = { 0 };
                Tool::GetBcdDataLen(item.tagValue.c_str(), tagValueLen, 3);
                string value = item.tag + tagValueLen + item.tagValue;
                dgiItem.value.InsertItem(item.tag, value);
            }
        }        
    }
}

/********************************************************
* 功能： 将某个DGI分组的tag值添加到指定的DGI分组中
*********************************************************/
void IRule::HandleDGIAddTag(CPS_ITEM& cpsItem)
{  
    for (auto &item : m_vecAddTagFromDGI) {
        string tagValue;
        for (auto &dstItem : cpsItem.items) {
            if (dstItem.dgi == item.dstDgi) {
                tagValue = dstItem.value.GetItem(item.dstTag);
                break;
            }
        }
        for (auto &dgiItem : cpsItem.items) {
            if (dgiItem.dgi == item.srcDgi) {
                dgiItem.value.InsertItem(item.dstTag, tagValue);
                break;
            }
        }
    }
}


string IRule::SMDecryptDGI(string tk, string encryptData)
{
    string result;
    PDllSM4_ECB_DEC SM4_ECB_DEC = GetSMFunc<PDllSM4_ECB_DEC>("dllSM4_ECB_DEC");
    if (SM4_ECB_DEC)
    {
        int len = encryptData.length();
        char* decryptedData = new char[len];
        memset(decryptedData, 0, len);
        SM4_ECB_DEC((char*)tk.c_str(), (char*)encryptData.c_str(), decryptedData);
        result = decryptedData;
    }
    return result;
}

string IRule::DesDecryptDGI(string tk, string encryptData)
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




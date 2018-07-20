#include "stdafx.h"
#include "IDpParse.h"
#include "../Util/Des0.h"
#include "../Util/SM.hpp"
#include "../Authencation/IGenKey.h"
#include <fstream>
#include <algorithm>
#include "../Util/IniConfig.h"
#include "../Util/Tool.h"
#include "../Util/ParseTLV.h"
#include "../DataParse/IDataParse.h"
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

bool ExistedInDGIs(string dgi, CPS& cpsItem)
{
    for (auto item : cpsItem.dgis)
    {
        if (item.dgi == dgi)
            return true;
    }
    return false;
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

void Dict::Clear()
{
    m_vecItems.clear();
}

bool Dict::IsExisted(string tag)
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

string Dict::GetItem()
{
    for (auto item : m_vecItems) {
        return item.second;
    }

    return "";
}

void Dict::ReplaceValue(string key, string value)
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

/*************************************************************
* 功能：将DGI item添加到容器中，若item所在DGI存在，则合并。
**************************************************************/
void CPS::AddDGI(DGI item)
{
    bool hasDgiExisted = false;
    for (auto &dgiItem : dgis) {
        if (dgiItem.dgi == item.dgi) //说明有重复项，将该item下的所有tag合并到原DGI中
        {
            auto mergedItems = item.value.GetItems();
            for (auto mergedItem : mergedItems)
            {
                dgiItem.value.AddItem(mergedItem.first, mergedItem.second);
            }
            hasDgiExisted = true;
            break;
        }
    }
    if (!hasDgiExisted)
        dgis.push_back(item);
}


bool GreaterSort(DGI item1, DGI item2)
{
    string compare1 = item1.dgi;
    string compare2 = item2.dgi;
    if (compare1.substr(0, 3) == "PSE")
    {
        compare1 = "EEEE";
    }
    else if (compare1.substr(0, 4) == "PPSE") {
        compare1 = "FFFF";
    }
    else if (compare1 == "A001") {
        compare1 = "8019";   //确保A001在8020之前个人化
    }

    if (compare2.substr(0, 3) == "PSE")
    {
        compare2 = "EEEE";
    }
    else if (compare2.substr(0, 4) == "PPSE") {
        compare2 = "FFFF";
    }
    else if (compare2 == "A001") {
        compare2 = "8019";
    }

    int nItem1 = stoi(compare1, 0, 16);
    int nItem2 = stoi(compare2, 0, 16);

    return nItem1 < nItem2;

    return false;
}


/*****************************************************
* 保存CPS数据
******************************************************/
void IDpParse::Save(CPS cpsItem)
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

    sort(cpsItem.dgis.begin(), cpsItem.dgis.end(), GreaterSort);
	for (auto iter : cpsItem.dgis)
	{
        if (iter.dgi != privousDGI) {
            outputFile << "[" << iter.dgi << "]" << endl;
        }
		for (auto tlvData : iter.value.GetItems())
		{
			outputFile << tlvData.first << "=" << tlvData.second << endl;
		}
        privousDGI = iter.dgi;
        outputFile << endl;
	}
	
	outputFile.clear();
    outputFile.close();
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
******************************************************************/
string IDpParse::GetLine(ifstream& dpFile)
{
    char* data = new char[100 * 1024];
    memset(data, 0, 100 * 1024);
    dpFile.getline(data, 100 * 1024);

    string oneLineData(data);
    delete data;

    return oneLineData;
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
        tlvs.AddItem(strTag, strValue);
    }
}

string IDpParse::GetAccount2(string magstripData)
{
    return "";
}

string IDpParse::GetAccount(CPS& cpsItem)
{
    for (auto item : cpsItem.dgis) {
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

void IDpParse::HandleRule(const char* szRuleConfig, CPS& cpsItem)
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
            if (node->first_attribute("delete80") == NULL) {
                encryptDGI.isDelete80 = false;
            }
            else {
                encryptDGI.isDelete80 = true;
            }
            m_vecDGIEncrypt.push_back(encryptDGI);
        }
        else if (nodeName == "AddTagToValue") {
            string dgi = node->first_attribute("DGI")->value();
            m_vecAddTagAndTemplate.push_back(dgi);
        }
        else if (nodeName == "SpliteEF02"){
            m_hasEF02 = true;
        }
        else if (nodeName == "AddPSEAndPPSE") {
            m_aid = node->first_attribute("aid")->value();
            m_addPse = true;
        }
        else if (nodeName == "AddKcv") {
            AddKCV kcvItem;
            kcvItem.srcDgi = node->first_attribute("srcDGI")->value();
            kcvItem.dstDgi = node->first_attribute("dstDGI")->value();
            kcvItem.keyType = node->first_attribute("keyType")->value();
            m_vecAddKcv.push_back(kcvItem);
        }
        else if (nodeName == "TagEncrypt") {
            TagEncrypt encryptTag;
            encryptTag.tag = node->first_attribute("tag")->value();
            encryptTag.type = node->first_attribute("type")->value();
            if (node->first_attribute("key") == NULL)
            {
                encryptTag.key = "";
            }
            else {
                encryptTag.key = node->first_attribute("key")->value();
            }
            
            if (node->first_attribute("delete80") == NULL || node->first_attribute("delete80")->value() == "false") {
                encryptTag.isDelete80 = false;
            }
            else {
                encryptTag.isDelete80 = true;
            }
            if (node->first_attribute("isWholeDecrypted") == NULL || node->first_attribute("isWholeDecrypted")->value() == "false") {
                encryptTag.isWholeDecrypted = false;
            }
            else {
                encryptTag.isWholeDecrypted = true;
            }
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
        else if (nodeName == "MergeTag")
        {
            TagMerge mergeTag;
            mergeTag.srcDgi = node->first_attribute("srcDGI")->value();
            mergeTag.srcTag = node->first_attribute("srcTag")->value();
            mergeTag.dstDgi = node->first_attribute("dstDGI")->value();
            mergeTag.dstTag = node->first_attribute("dstTag")->value();
            m_vecTagMerge.push_back(mergeTag);
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
            if (node->first_attribute("srcTag") != NULL)    //如果srcTag没配置，则默认和dstTag一致
            {
                vecDGI.srcTag = node->first_attribute("srcTag")->value();
            }
            else {
                vecDGI.srcTag = node->first_attribute("dstTag")->value();
            }
            
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
        else if (nodeName == "InsertValue") {
            TagInsert insertedTag;
            insertedTag.insertedValue = node->first_attribute("value")->value();
            insertedTag.dgi = node->first_attribute("DGI")->value();
            insertedTag.tag = node->first_attribute("tag")->value();
            string strPos = node->first_attribute("pos")->value();
            insertedTag.pos = stoi(strPos);
            m_vecTagInsert.push_back(insertedTag);
        }
    }
  
    return true;
}

void IRule::HandleRule(CPS& cpsItem)
{
    //处理原则
    //1. 调整DGI、tag
    //2. 解密数据
    //3. 增加kcv
    //4. 添加tag到value
    //5. 删除DGI、tag
    //处理顺序：DGI映射-->DGI交换-->DGI删除-->DGI解密-->DGI删除tag

    HandleDGIMap(cpsItem);
    HandleDGIExchange(cpsItem);   

    SpliteEF02(cpsItem);

    HandleDGIAddTagFromOtherDGI(cpsItem);
    HandleTagMergeTagFromOtherTag(cpsItem);
    
    HandleDGIDecrypt(cpsItem);
    HandleTagDecrypt(cpsItem);

    HandleDGIAddFixedTagValue(cpsItem);
    


    //HandleTagInsertValue(cpsItem);
    AddKcv(cpsItem);
    AddTagPrefix(cpsItem);
     
    HandleDGIDelete(cpsItem);
    HandleTagDelete(cpsItem);
}

string Get9102TagItem(string tag, TLV* pTlvs, unsigned int count)
{
    for (int i = 0; i < count; i++)
    {
        if (pTlvs[i].tag == tag)
            return pTlvs[i].value;
    }
    return "";
}

string GetTLVData(string tag, string value)
{
    if (value.empty())
        return "";
    string len = Tool::GetBcdStrLen(value);

    return tag + len + value;
}

void IRule::AddPseAndPPSE(CPS& cpsItem)
{
    if (m_addPse)
    {
        string dgi9102;
        for (auto item : cpsItem.dgis)
        {
            if (item.dgi == "9102") {
                dgi9102 = item.value.GetItem();
                break;
            }
        }
        TLV pTlvs[32];
        unsigned int tlvCount = 32;
        ParseTLV((char*)dgi9102.c_str(), pTlvs, tlvCount);

        string tag50, tag9F12, tag87, tag88, tag5F2D, tag9F11;
        tag50 = Get9102TagItem("50", pTlvs, tlvCount);
        tag9F12 = Get9102TagItem("9F12", pTlvs, tlvCount);
        tag87 = Get9102TagItem("87", pTlvs, tlvCount);
        tag88 = Get9102TagItem("88", pTlvs, tlvCount);
        tag5F2D = Get9102TagItem("5F2D", pTlvs, tlvCount);
        tag9F11 = Get9102TagItem("9F11", pTlvs, tlvCount);

        string pseA5 = GetTLVData("88", tag88) +
            GetTLVData("5F2D", tag5F2D) +
            GetTLVData("9F11", tag9F11);
        string pse9102 = GetTLVData("A5", pseA5);

        string template61Data = GetTLVData("4F", m_aid) +
            GetTLVData("50", tag50) +
            GetTLVData("9F12", tag9F12) +
            GetTLVData("87", tag87);
        string dgi0101 = GetTLVData("61", template61Data);

        string ppse61Template = GetTLVData("4F", m_aid) +
            GetTLVData("50", tag50) +
            GetTLVData("87", tag87);
        string ppseBF0CTemplate = GetTLVData("61", ppse61Template);
        string ppseA5 = GetTLVData("BF0C", ppseBF0CTemplate);
        string ppse9102 = GetTLVData("A5", ppseA5);

        DGI dgiPSE;
        dgiPSE.dgi = "PSE";
        dgiPSE.value.AddItem("0101", dgi0101);
        dgiPSE.value.AddItem("9102", dgi9102);

        DGI dgiPPSE;
        dgiPPSE.dgi = "PPSE";
        dgiPPSE.value.AddItem("9102", ppse9102);


        cpsItem.AddDGI(dgiPSE);
        cpsItem.AddDGI(dgiPPSE);
    }   
}

/****************************************************************
* 
*****************************************************************/
void IRule::AddTagPrefix(CPS& cpsItem)
{
    for (auto& dgi : m_vecAddTagAndTemplate)
    {
        for (auto& dgiItem : cpsItem.dgis)
        {
            if (dgi == dgiItem.dgi) {
                vector<pair<string, string>> tls = dgiItem.value.GetItems();                
                for (auto item : tls)
                {
                    char bcdLen[5] = { 0 };
                    string newValue;
                    Tool::GetBcdDataLen(item.second.c_str(), bcdLen, 4);
                    if (item.second.length() > 0x80 * 2) {
                        newValue = item.first + "81" + bcdLen + item.second;
                    }
                    else {
                        newValue = item.first + bcdLen + item.second;
                    }
                    dgiItem.value.ReplaceValue(item.first, newValue);
                }
            }
        }
    }
}

void IRule::SpliteEF02(CPS& cpsItem)
{
    if (m_hasEF02)
    {
        DGI item8201, item8202, item8203, item8204, item8205;
        item8201.dgi = "8201";
        item8202.dgi = "8202";
        item8203.dgi = "8203";
        item8204.dgi = "8204";
        item8205.dgi = "8205";
        for (auto& dgiItem : cpsItem.dgis)
        {
            if (dgiItem.dgi == "01")
            {
                string EF02 = dgiItem.value.GetItem("EF02");
                if (EF02.length() < 0)
                    return;
                const int len = 8;
                EF02 = EF02.substr(len);
                int index = 0;
                for (int i = 0; i < 8; i++) //循环处理8201.8202...
                {
                    string bcdLen = EF02.substr(index, len);
                    char ascStr[5] = { 0 };
                    Tool::BcdToStr(bcdLen.c_str(), ascStr, 4);
                    int iLen = 2 * stoi(ascStr, 0, 10);                  
                    string value = EF02.substr(index + len, iLen);
                    index += iLen + len;
                    if (i == 3) { item8205.value.AddItem(item8205.dgi, value); cpsItem.AddDGI(item8205); }
                    if (i == 4) { item8204.value.AddItem(item8204.dgi, value); cpsItem.AddDGI(item8204); }
                    if (i == 5) { item8203.value.AddItem(item8203.dgi, value); cpsItem.AddDGI(item8203); }
                    if (i == 6) { item8202.value.AddItem(item8202.dgi, value); cpsItem.AddDGI(item8202); }
                    if (i == 7) { item8201.value.AddItem(item8201.dgi, value); cpsItem.AddDGI(item8201); }
                }
                break;  //避免多余的循环
            }
        }
    }
}

void IRule::AddKcv(CPS& cpsItem)
{
    for (auto item : m_vecAddKcv)
    {
        string keys;
        for (auto& dgiItem : cpsItem.dgis)
        {
            if (dgiItem.dgi == item.dstDgi) {
                keys = dgiItem.value.GetItem(dgiItem.dgi);
                break;
            }
        }
        if (keys.length() % 32 != 0)
            return;
        string kcv;
        for (int i = 0; i < keys.length(); i += 32)
        {
            string key = keys.substr(i, 32);
            char singleKcv[7] = { 0 };
            if (item.keyType == "DES") {
                GenDesKcv(key.c_str(), singleKcv, 6);
            }
            else {
                GenSmKcv(key.c_str(), singleKcv, 6);
            }
            kcv += singleKcv;
        }
        DGI newDgiItem;
        newDgiItem.dgi = item.srcDgi;
        newDgiItem.value.AddItem(item.srcDgi, kcv);
        cpsItem.AddDGI(newDgiItem);
    }
}

/****************************************************************
* 功能：更换DGI分组，例如将DGI8020映射为DGI9307,也就是将DGI更换下
* DGI号，注意，新的DGI号必须是CPS中没有的，否则会造成CPS中存在两个
* 相同的DGI分组
*****************************************************************/
void IRule::HandleDGIMap(CPS& cpsItem)
{
	for (auto item : m_vecDGIMap)
	{
		for (auto& dgiItem : cpsItem.dgis)
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
* 功能：交换DGI分组的值，两个DGI分组必须是已经存在于CPS中。
***************************************************************/
void IRule::HandleDGIExchange(CPS& cpsItem)
{
	for (auto item : m_vecDGIExchange)
	{
		int firstIndex = 0;
		int secondIndex = 0;
		while (item.srcDgi != cpsItem.dgis[firstIndex].dgi)
			firstIndex++;
		while (item.exchangedDgi != cpsItem.dgis[secondIndex].dgi)
			secondIndex++;
        cpsItem.dgis[firstIndex].dgi = item.exchangedDgi;
        cpsItem.dgis[firstIndex].value.ReplaceKey(item.srcDgi, item.exchangedDgi);
        cpsItem.dgis[secondIndex].dgi = item.srcDgi;
        cpsItem.dgis[secondIndex].value.ReplaceKey(item.exchangedDgi, item.srcDgi);
	}
}

void IRule::HandleTagDecrypt(CPS& cpsItem)
{
    for (auto& encryptedItem : m_vecTagEncrypt)
    {
        for (auto& item : cpsItem.dgis) {
            if (item.value.IsExisted(encryptedItem.tag))
            {
                string decryptedData, encryptData;
                string tagValue = item.value.GetItem(encryptedItem.tag);
                if (encryptedItem.isWholeDecrypted || encryptedItem.type == "BASE64" || encryptedItem.type == "BCD")
                {
                    encryptData = tagValue;
                }
                else {
                    encryptData = tagValue.substr(encryptedItem.tag.length() + 2);
                }
                
                //string encryptData = item.value.GetItem(encryptedItem.tag);
                if (encryptedItem.type == "SM") {
                    decryptedData = SMDecryptDGI(encryptedItem.key, encryptData);
                }
                else if(encryptedItem.type == "DES"){
                    decryptedData = DesDecryptDGI(encryptedItem.key, encryptData);
                }
                else if (encryptedItem.type == "BCD") {
                    decryptedData = Tool::StrToBcd(encryptData.c_str(),encryptData.size());
                }
                else if (encryptedItem.type == "BASE64") {
                    int len = 0;
                    string decodData = Tool::base64_decode(encryptData,len);
                    decryptedData = Tool::StrToBcd(decodData.c_str(), len);
                }
                else {
                    continue;   //防止出现错误
                }

                
                if (encryptedItem.type == "DES" || encryptedItem.type == "SM")
                {
                    if (encryptedItem.isDelete80)
                    {
                        int index = decryptedData.rfind("80");
                        if (index != string::npos)
                        {
                            decryptedData = decryptedData.substr(0, index);
                        }
                    }
                    if (decryptedData.length() > encryptedItem.len && encryptedItem.len >= 0) 
                    {
                        if (encryptedItem.startPos != 0 || encryptedItem.len != 0)
                        {
                            decryptedData = decryptedData.substr(encryptedItem.startPos, encryptedItem.len);
                        }                               
                    }
                }
                if (encryptedItem.isWholeDecrypted)
                {
                    item.value.ReplaceValue(encryptedItem.tag, decryptedData); //将数据替换为临时解密数据
                }
                else {                   
                    string len = Tool::GetBcdStrLen(decryptedData);
                    decryptedData = encryptedItem.tag + len + decryptedData;
                    item.value.ReplaceValue(encryptedItem.tag, decryptedData); //将数据替换为临时解密数据
                }
               
            }
        }
    }
}

/***************************************************************
* 解密DGI数据，注意该DGI只应只有一个tag=encryptData, 并且tag==DGI
****************************************************************/
void IRule::HandleDGIDecrypt(CPS& cpsItem)
{ 
    for (auto& encryptedItem : m_vecDGIEncrypt)
    {
        for(auto& item : cpsItem.dgis)
        {
            if (item.dgi == encryptedItem.dgi) {
                string decryptedData;
                if (encryptedItem.type == "SM") {
                    decryptedData = SMDecryptDGI(encryptedItem.key, item.value.GetItem(item.dgi));
                }
                else {
                    decryptedData = DesDecryptDGI(encryptedItem.key, item.value.GetItem(item.dgi));
                }
                if (encryptedItem.isDelete80) {
                    int found = decryptedData.length();
                    found = decryptedData.rfind("80");
                    if(found != string::npos)
                        decryptedData = decryptedData.substr(0, found);
                }
                item.value.ReplaceValue(item.dgi, decryptedData);
            }            
        }
    }
}

/*****************************************************************
* 功能：删除DGI分组
******************************************************************/
void IRule::HandleDGIDelete(CPS& cpsItem)
{
	for (auto item : m_vecDGIDelete)
	{
		for (auto iter = cpsItem.dgis.begin(); iter != cpsItem.dgis.end(); iter++)
		{
			if (item == iter->dgi)
			{
				iter = cpsItem.dgis.erase(iter);
                break;
			}
		}
	}
}

/***************************************************
* 功能：删除某个DGI分组中的tag
****************************************************/
void IRule::HandleTagDelete(CPS& cpsItem)
{
	for (auto item : m_vecTagDelete)
	{
		for (auto &dgiItem : cpsItem.dgis)
		{
			if (item.srcDgi == dgiItem.dgi)
			{
                dgiItem.value.DeleteItem(item.tag);
                break;
			}
		}
	}
}

void IRule::HandleTagInsertValue(CPS& cpsItem)
{
    for (auto item : m_vecTagInsert)
    {
        for (auto &dgiItem : cpsItem.dgis)
        {
            if (item.dgi == dgiItem.dgi && dgiItem.value.IsExisted(item.tag))
            {
                string oldTagValue = dgiItem.value.GetItem(item.tag).substr(item.tag.length() + 2); //默认tag值长度不超过7F,且更改后的新值也捕超过7F
                string newTagValue = oldTagValue.substr(0, item.pos) + item.insertedValue + oldTagValue.substr(item.pos);
                string strLen = Tool::GetBcdStrLen(newTagValue);
                newTagValue = item.tag + strLen + newTagValue;
                dgiItem.value.ReplaceValue(item.tag, newTagValue);
                
                break;
            }
        }
    }
}

/********************************************************
* 功能：给某个DGI分组添加固定的tag值
*********************************************************/
void IRule::HandleDGIAddFixedTagValue(CPS& cpsItem)
{
    for (auto item : m_vecFixedTagAdd) 
    {
        bool hasExisted = false;
        for (auto& dgiItem : cpsItem.dgis){
            if (dgiItem.dgi == item.srcDgi) {
                hasExisted = true;
                char tagValueLen[3] = { 0 };
                Tool::GetBcdDataLen(item.tagValue.c_str(), tagValueLen, 3);
                string value = item.tagValue;
                dgiItem.value.AddItem(item.tag, value);
            }
        }
        if (!hasExisted) {
            DGI newItem;
            newItem.dgi = item.srcDgi;
            newItem.value.AddItem(item.tag, item.tagValue);
            cpsItem.AddDGI(newItem);
        }
    }
}

void IRule::HandleTagMergeTagFromOtherTag(CPS& cpsItem)
{
    for (auto &item : m_vecTagMerge) {
        string tagMergedValue;
        for (auto &dstItem : cpsItem.dgis) {
            if (dstItem.dgi == item.dstDgi) {
                tagMergedValue = dstItem.value.GetItem(item.dstTag);
                break;
            }
        }
        if (!ExistedInDGIs(item.srcDgi, cpsItem))
        {
            DGI newDgiItem;
            newDgiItem.dgi = item.srcDgi;
            cpsItem.AddDGI(newDgiItem);
        }
        for (auto &dgiItem : cpsItem.dgis) {
            if (dgiItem.dgi == item.srcDgi) {
                if (!dgiItem.value.IsExisted(item.srcTag))
                {
                    dgiItem.value.AddItem(item.srcTag, "");  //如果tag不存在，添加一个空的tag
                }
                string oldValue = dgiItem.value.GetItem(item.srcTag);
                string mergedValue =  oldValue + tagMergedValue;
                dgiItem.value.ReplaceValue(item.srcTag, mergedValue);
                break;
            }
        }
    }
}


/********************************************************
* 功能： 将某个DGI分组的tag值添加到指定的DGI分组中
*********************************************************/
void IRule::HandleDGIAddTagFromOtherDGI(CPS& cpsItem)
{  
    for (auto &item : m_vecAddTagFromDGI) {
        string tagValue;
        for (auto &dstItem : cpsItem.dgis) {
            if (dstItem.dgi == item.dstDgi) {
                tagValue = dstItem.value.GetItem(item.dstTag);
                break;
            }
        }
        if (!ExistedInDGIs(item.srcDgi, cpsItem))
        {
            DGI newDgiItem;
            newDgiItem.dgi = item.srcDgi;
            cpsItem.AddDGI(newDgiItem);
        }
        for (auto &dgiItem : cpsItem.dgis) {
            if (dgiItem.dgi == item.srcDgi) {
                dgiItem.value.AddItem(item.srcTag, tagValue);
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




#include "stdafx.h"
#include "DpParse_YL_MC.h"
#include <Windows.h>
#include <string>
#include <fstream>
#include "../Util/Tool.h"
using namespace std;

#define DGI_NUMBER	4
#define DGI_LEN		2

bool HandleDp(const char* szFileName, const char* szRuleFile, char** cpsFile, int& count)
{
    YLMCDpParser parse;
    return parse.HandleDp(szFileName, szRuleFile, cpsFile, count);
}

YLMCDpParser::YLMCDpParser()
{
}


int YLMCDpParser::ParsePSE(ifstream &dpFile, DGI &dgiItem)
{
    if (0x86 != (unsigned char)ReadDGIStartTag(dpFile)) {
        return 1;
    }
    int nFollowedDataLen = GetFollowedDataLen(dpFile);
    if (dgiItem.dgi == "DGIF001") {

        string dgi;
        GetBCDBuffer(dpFile, dgi, 2);	//读取该DGI序号
        dgiItem.dgi = "F001";
        int nFollowedDataLen = GetFollowedDataLen(dpFile);
        if (nFollowedDataLen > 0)
        {
            string value;
            GetBCDBuffer(dpFile, value, nFollowedDataLen);
            dgiItem.value.AddItem(dgiItem.dgi, value);
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
            dgiItem.value.AddItem(tag, value);
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
            dgiItem.value.AddItem(tag, value);
        }
        else {
            if(dgiItem.dgi == "Store_PPSE")
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
            dgiItem.value.AddItem(tag, value);
        }
    }

    return 0;
}

/********************************************************************
* 解析银联DP数据
********************************************************************/
bool YLMCDpParser::HandleDp(const char* fileName, const char* ruleFile, char** cpsFile, int& count)
{
    ifstream dpFile;
    if (!OpenDpFile(fileName, dpFile)) {
        return false;
    }
    long dpFileSize = GetFileSize(dpFile);
    if (dpFileSize <= m_reserved) {
        return false;
    }
    dpFile.seekg(m_reserved);
    ReadDGIName(dpFile);		//第一步： 读取DGI分组

    vector<CPS> vecCpsItem;
    int cpsCount = 0;
    while (dpFile.tellg() < dpFileSize)		//遍历后续数据，每一次遍历解析一个卡片数据
    {
        string cardSeq;
        GetBCDBufferLittleEnd(dpFile, cardSeq, 4);	//读取卡片序列号		
        int oneCardDataLen = GetOneCardDpDataLen(dpFile); //读取该卡片个人化数据内容总长度

        CPS cpsItem;
        for (unsigned int i = 0; i < m_vecDGI.size(); i++)	//解析每张卡片数据
        {
            DGI dgiItem;
            dgiItem.dgi = m_vecDGI[i];
            if (dgiItem.dgi == "DGIF001" ||
                dgiItem.dgi == "DGI9212"
                || (dgiItem.dgi.substr(0, 3) != "DGI" && dgiItem.dgi.substr(0, 4) != "MCDA"))	//对于DGIF001和PSE/PPSE数据，需要特殊处理
            {
                ParsePSE(dpFile, dgiItem);
                cpsItem.dgis.push_back(dgiItem);
                continue;
            }

            if (0x86 != (unsigned char)ReadDGIStartTag(dpFile)) { //读取DGI起始标志
                return false;
            }
            int nFollowedDataLen = GetFollowedDataLen(dpFile);	//读取该DGI总数据长度(包含了该DGI序号)

            unsigned short sDgi;
            string dgi;
            streampos pos;
            pos = GetBCDBuffer(dpFile, dgi, 2);	//读取该DGI序号
            sDgi = stoi(dgi, 0, 16);
            if (dgi != dgiItem.dgi.substr(3) &&
                dgi != dgiItem.dgi.substr(4)) {
                return false;   //对应的DGI有误
            }
            if(dgiItem.dgi.substr(0,3) == "DGI")
                dgiItem.dgi = dgi;  //去掉"DGI"字符
            nFollowedDataLen = GetFollowedDataLen(dpFile);	// 读取后续数据长度(该DGI数据部分)		
            if (sDgi < 0x0b01)		// 读取记录模板(该模板仅当DGI序号小于0x0B01时出现)
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

            //判断是否仅包含数据内容，不是标准的TLV结构
            if (sDgi != 0x8201
                && sDgi != 0x8202
                && sDgi != 0x8203
                && sDgi != 0x8204
                && sDgi != 0x8205
                &&IsTlvStruct((char*)buffer.c_str(), nFollowedDataLen * 2)) {
                ParseTLV((char*)buffer.c_str(), nFollowedDataLen * 2, dgiItem.value);
            }
            else {
                char szTag[5] = { 0 };
                sprintf_s(szTag, "%X", sDgi);
                dgiItem.value.AddItem(szTag, buffer);
            }
            cpsItem.dgis.push_back(dgiItem);
        }

        //解析完成之后，调用规则
        if (ruleFile) {
            HandleRule(ruleFile, cpsItem);
        }

        //保存数据
        int pos = string(fileName).find_last_of('\\');
        string path = string(fileName).substr(0, pos + 1);
        cpsItem.fileName = path + "conv\\" + GetAccount(cpsItem) + ".txt";
        Save(cpsItem);
        vecCpsItem.push_back(cpsItem);

        cpsFile[cpsCount] = new char[1024];
        memset(cpsFile[cpsCount], 0, 1024);
        strncpy_s(cpsFile[cpsCount], 1024, cpsItem.fileName.c_str(), cpsItem.fileName.length());
        cpsCount++;
        count = cpsCount;
    }

    return true;
}


/********************************************************************
* 功能：读取DGI起始标准符
*********************************************************************/
char YLMCDpParser::ReadDGIStartTag(ifstream &dpFile)
{
    char cDGIStartTag;

    dpFile.read(&cDGIStartTag, 1);

    return cDGIStartTag;
}

/********************************************************************
* 功能：读取DGI后续数据长度
*********************************************************************/
int YLMCDpParser::GetFollowedDataLen(ifstream &dpFile)
{
    int dataLen = 0;

    GetLenTypeBuffer(dpFile, dataLen, 1);
    if (dataLen == 0x81)
        GetLenTypeBuffer(dpFile, dataLen, 1);
    else if (dataLen == 0x82 || dataLen == 0xFF) {
        GetLenTypeBuffer(dpFile, dataLen, 2);
    }

    return dataLen;
}


/********************************************************************
* 功能：读取每一张卡片的个人化总数据长度
*********************************************************************/
int YLMCDpParser::GetOneCardDpDataLen(ifstream &dpFile)
{
    char szPersonlizedDataLen[3] = { 0 };
    dpFile.read(szPersonlizedDataLen, 2);
    unsigned short *pPersonlizedDataLen = (unsigned short*)(szPersonlizedDataLen);

    return *pPersonlizedDataLen;
}

/********************************************************************
* 功能：读取DGI分组名称
*********************************************************************/
void YLMCDpParser::ReadDGIName(ifstream &dpFile)
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



// DpParse_HT.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DpParse_HT.h"
#include "../Util/Tool.h"

bool HandleDp(const char* szFileName, const char* ruleFile)
{
    HTDpParser parser;
   return parser.HandleDp(szFileName, ruleFile); 
}

bool HTDpParser::HandleDp(const char* szFileName, const char* szRuleFile)
{
    if (szFileName == NULL){
        return false;
    }
    if (!OpenDpFile(szFileName, m_DpFile)){
        return false;
    }
    if (GetCardSequence() == -1) {
        return false;
    }
    GetCardMark();
    GetCardType();
    GetCardDataLen();
    ParseEmbossData();
    ParseMagstripData(szFileName);
    ParseCardData(szFileName, szRuleFile);

    return true;
}

int HTDpParser::GetCardSequence()
{
    string seq;
    GetBCDBuffer(m_DpFile, seq, 4);
    if (seq.length())
    {
        return stoi(seq, 0, 16);
    }
    return -1;
}

string HTDpParser::GetCardMark()
{
    string mark;
    GetBCDBuffer(m_DpFile, mark, 10);
    return mark;
}

char HTDpParser::GetCardType()
{
    string type;
    GetBCDBuffer(m_DpFile, type, 1);
    return stoi(type, 0, 16);
}

int HTDpParser::GetCardDataLen()
{
    string cardDataLen;
    GetBCDBuffer(m_DpFile, cardDataLen, 4);
    if (cardDataLen.length())
    {
        return stoi(cardDataLen, 0, 16);
    }
    return -1;
}

int HTDpParser::ParseEmbossData()
{
    string mark;
    GetBCDBuffer(m_DpFile, mark, 6);
    char szMark[12] = { 0 };
    Tool::BcdToStr(mark.c_str(), szMark, 12);
    if (string(szMark) != "000PRN")
    {
        return -1;
    }
    string strLen;
    GetBCDBuffer(m_DpFile, strLen, 4);
    int embossDataLen = stoi(strLen, 0, 16);
    string embossData;
    GetBCDBuffer(m_DpFile, embossData, embossDataLen);
    return 0;
}

int HTDpParser::ParseMagstripData(const char* szFileName)
{
    string mark;
    GetBCDBuffer(m_DpFile, mark, 6);
    char szMark[12] = { 0 };
    Tool::BcdToStr(mark.c_str(), szMark, 12);
    if (string(szMark) != "000MAG")
    {
        return -1;
    }
    string strLen;
    GetBCDBuffer(m_DpFile, strLen, 4);
    int magstripDataLen = stoi(strLen, 0, 16);
    string magstripData;
    GetBCDBuffer(m_DpFile, magstripData, magstripDataLen);

    string path = Tool::GetDirectory(szFileName) + "Magstrip.txt";
    FILE* pFile = NULL;
    fopen_s(&pFile, path.c_str(), "w+");
    if (pFile)
    {
        fwrite(magstripData.c_str(), 1, magstripData.length(), pFile);
        fclose(pFile);
    }
   
    return 0;
}

int HTDpParser::ParseCardData(const char* szFileName, const char* szRuleFile)
{
    string mark;
    GetBCDBuffer(m_DpFile, mark, 6);
    char szMark[12] = { 0 };
    Tool::BcdToStr(mark.c_str(), szMark, 12);
    if (string(szMark) != "000EMV") //卡片数据起始标志判断
    {
        return -1;
    }
    string strLen;
    GetBCDBuffer(m_DpFile, strLen, 4);      //卡片数据长度
    int appsDataLen = stoi(strLen, 0, 16);
    int apps;
    GetLenTypeBuffer(m_DpFile, apps, 1);    //获取应用数量
    for (int i = 0; i < apps; i++)
    {
        int aidLen;
        GetLenTypeBuffer(m_DpFile, aidLen, 1); //获取AID的长度
        string aid;
        GetBCDBuffer(m_DpFile, aid, aidLen);    //获取AID
        int appDataLen;
        GetLenTypeBuffer(m_DpFile, appDataLen, 4);  //获取应用数据长度

        int dgiListLen;
        GetLenTypeBuffer(m_DpFile, dgiListLen, 2);  
        string dgiList;
        GetBCDBuffer(m_DpFile, dgiList, dgiListLen); //获取每个应用的DGI列表
        int encryptDgiLen;
        GetLenTypeBuffer(m_DpFile, encryptDgiLen, 2); //加密的DGI列表
        string encryptDgiList;
        GetBCDBuffer(m_DpFile, encryptDgiList, encryptDgiLen);
        int needRecordLogDgiLen;
        GetLenTypeBuffer(m_DpFile, needRecordLogDgiLen, 2); //需要记录日志的DGI列表
        string needRecordLogDgiList;
        GetBCDBuffer(m_DpFile, needRecordLogDgiList, needRecordLogDgiLen);

        CPS_ITEM cpsItem;
        for (int j = 0; j < dgiListLen; j += 2) //遍历每个DGI，并解析
        {
            string dgi;
            GetBCDBuffer(m_DpFile, dgi, 2);
            int dgiLen;
            GetLenTypeBuffer(m_DpFile, dgiLen, 1);
            string dgiValue;
            GetBCDBuffer(m_DpFile, dgiValue, dgiLen);
            int nDgi = stoi(dgi, 0, 16);
            
            if (nDgi < 0x0B01)  //一般DGI小于0B01的都是包含70模板，可用Read Record命令读取
            {
                if (dgiValue.substr(0, 2) != "70") {
                    return -1;
                }
                if (dgiValue.substr(2, 2) == "81" && dgiValue.substr(4).length() > 0x80 * 2) {  //包含前缀81  
                    dgiValue = dgiValue.substr(6);
                }
                else {
                    dgiValue = dgiValue.substr(4);
                }
            }
            DGI_ITEM dgiItem;
            dgiItem.dgi = dgi;
            //判断是否仅包含数据内容，不是标准的TLV结构
            if (IsTlvStruct((char*)dgiValue.c_str(), dgiValue.length())) {
                ParseTLV((char*)dgiValue.c_str(), dgiValue.length(), dgiItem.value);
            }
            else {
                dgiItem.value.InsertItem(dgi, dgiValue);
            }
            cpsItem.items.push_back(dgiItem);
        }

        //解析完成之后，调用规则
        if (szRuleFile) {
            HandleRule(szRuleFile, cpsItem);
        }

        cpsItem.fileName = Tool::GetDirectory(szFileName) + GetAccount(cpsItem) + ".txt";
        Save(cpsItem);
    }
    return 0;
}







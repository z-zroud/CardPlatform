// DpParse_Goldpac.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DpParse_Goldpac.h"
#include "../Util/Tool.h"

struct GoldpacData
{
    string dgi;
    int len;
    string data;
};

bool HandleDp(const char* szFileName, const char* ruleFile, char** cpsFile, int& count)
{
    GoldpacDpParser parser;
    return parser.HandleDp(szFileName, ruleFile, cpsFile, count);
}

bool GoldpacDpParser::HandleDp(const char* szFileName, const char* szRuleFile, char** cpsFile, int& count)
{
    ifstream dpFile;
    if (szFileName == NULL) {
        return false;
    }

    if (!OpenDpFile(szFileName, dpFile)) {
        return false;
    }
    string header;
    GetBCDBuffer(dpFile, header, 26);
    int dgiCount = 0;
    GetLenTypeBuffer(dpFile, dgiCount, 2);
    vector<GoldpacData> goldpacDatas;
    for (int i = 0; i < dgiCount; i++)
    {
        GoldpacData goldpacData;
        GetBCDBuffer(dpFile, goldpacData.dgi, 4);
        GetLenTypeBuffer(dpFile, goldpacData.len, 2);
        goldpacDatas.push_back(goldpacData);
    }

    CPS cpsItem;
    for (int i = 0; i < dgiCount; i++)
    {
        GetBCDBuffer(dpFile, goldpacDatas[i].data, goldpacDatas[i].len);
        DGI dgiItem;
        dgiItem.dgi = goldpacDatas[i].dgi;
        dgiItem.value.AddItem(goldpacDatas[i].dgi,goldpacDatas[i].data);
        cpsItem.AddDGI(dgiItem);
    }
    //解析完成之后，调用规则
    if (szRuleFile) {
        HandleRule(szRuleFile, cpsItem);
    }
    cpsItem.fileName = Tool::GetDirectory(szFileName) + "conv\\" + GetAccount(cpsItem) + ".txt";
    Save(cpsItem);

    int cpsCount = 0;
    cpsFile[cpsCount] = new char[1024];
    memset(cpsFile[cpsCount], 0, 1024);
    strncpy_s(cpsFile[cpsCount], 1024, cpsItem.fileName.c_str(), cpsItem.fileName.length());
    cpsCount++;
    count = cpsCount;

    return true;
}



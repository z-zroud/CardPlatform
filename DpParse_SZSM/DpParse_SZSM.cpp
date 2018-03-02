// DpParse_SZSM.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DpParse_SZSM.h"
#include "../Util/Tool.h"

bool HandleDp(const char* szFileName, const char* ruleFile)
{
    SZSMDpParser parser;
    return parser.HandleDp(szFileName, ruleFile);
}

bool SZSMDpParser::HandleDp(const char* szFileName, const char* szRuleFile)
{
    ifstream dpFile;
    if (szFileName == NULL) {
        return false;
    }

    if (!OpenDpFile(szFileName, dpFile)) {
        return false;
    }
    while (dpFile)
    {
        CPS_ITEM cpsItem;
        string oneCardData = GetLine(dpFile);
        if (oneCardData.empty())    //忽略空格
            continue;
        
        int magstripIndex = oneCardData.find("[00]");
        int dcIndex = oneCardData.find("[01]");
        int ecIndex = oneCardData.find("[02]");
        int qpbocIndex = oneCardData.find("[03]");

        string magstrip = oneCardData.substr(4, dcIndex - magstripIndex - 4);
        string dcData = oneCardData.substr(dcIndex + 4, ecIndex - dcIndex - 4);
        string ecData = oneCardData.substr(ecIndex + 4, qpbocIndex - ecIndex -4);
        string qpbocData = oneCardData.substr(qpbocIndex + 4);
        
        DGI_ITEM dcItem;
        dcItem.dgi = "01";
        HandleData(dcData, dcItem.value);
        cpsItem.AddDgiItem(dcItem);

        DGI_ITEM ecItem;
        ecItem.dgi = "02";
        HandleData(ecData, ecItem.value);
        cpsItem.AddDgiItem(ecItem);

        DGI_ITEM qpbocItem;
        qpbocItem.dgi = "03";
        HandleData(qpbocData, qpbocItem.value);
        cpsItem.AddDgiItem(qpbocItem);

        //解析完成之后，调用规则
        if (szRuleFile) {
            HandleRule(szRuleFile, cpsItem);
        }

        cpsItem.fileName = Tool::GetDirectory(szFileName) + GetAccount2(magstrip) + ".txt";
        Save(cpsItem);
    }
    
    return true;
}

string SZSMDpParser::GetAccount2(string magstripData)
{
    int index = magstripData.find_first_of(' ');
    return magstripData.substr(0, index);
}

void SZSMDpParser::HandleData(const string data, Dict& dgiItem)
{
    vector<string> subStrs;
    Tool::SplitStr(data, "|", subStrs);
    if (subStrs.size() % 3 != 0)
        return;
    for (int i = 0; i < subStrs.size(); i += 3)
    {
        dgiItem.InsertItem(subStrs[i], subStrs[i + 2]);
    }
}

#include "stdafx.h"
#include "DpParse_GEER.h"
#include "../Util/Tool.h"

bool HandleDp(const char* szFileName, const char* ruleFile, char** cpsFile, int& count)
{
    GEERDpParser parser;
    return parser.HandleDp(szFileName, ruleFile, cpsFile, count);
}

bool GEERDpParser::HandleDp(const char* szFileName, const char* szRuleFile, char** cpsFile, int& count)
{
    ifstream dpFile;
    if (szFileName == NULL) {
        return false;
    }

    if (!OpenDpFile(szFileName, dpFile)) {
        return false;
    }
    int cpsCount = 0;
    string dgi;
    DGI_ITEM dgiItem;
    CPS_ITEM cpsItem;
    while (dpFile)
    {
        string data = GetLine(dpFile);
        if (data.empty())    //忽略空格
            continue;

        if (data == "[01]")
        {
            dgiItem.value.Clear();
            dgiItem.dgi = "01";
            continue;
        }
        else if (data == "[02]")
        {
            cpsItem.AddDgiItem(dgiItem);
            dgiItem.value.Clear();
            dgi = "02";
            continue;
        }
        else if (data == "[03]")
        {
            cpsItem.AddDgiItem(dgiItem);
            dgiItem.value.Clear();
            dgi = "03";
            continue;
        }

        vector<string> vecItem;
        Tool::SplitStr(data, "|", vecItem);
        if (vecItem.size() != 3)
        {
            return false;
        }
        dgiItem.value.InsertItem(vecItem[0], vecItem[2]);
    }
    cpsItem.AddDgiItem(dgiItem);
    //解析完成之后，调用规则
    if (szRuleFile) {
        HandleRule(szRuleFile, cpsItem);
    }
    cpsItem.fileName = Tool::GetDirectory(szFileName) + "conv\\" + GetAccount(cpsItem) + ".txt";
    Save(cpsItem);

    cpsFile[cpsCount] = new char[1024];
    memset(cpsFile[cpsCount], 0, 1024);
    strncpy_s(cpsFile[cpsCount], 1024, cpsItem.fileName.c_str(), cpsItem.fileName.length());
    cpsCount++;
    count = cpsCount;

    return true;
}

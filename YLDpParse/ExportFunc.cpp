#include "stdafx.h"
#include "../DpParse/IDpParse.h"
#include "YlDpParse.h"


bool HandleDp(const char* szFileName, const char* szRuleFile, char** cpsFile, int& count)
{
	YLDpParser parse;
    return parse.HandleDp(szFileName, szRuleFile, cpsFile, count);
}
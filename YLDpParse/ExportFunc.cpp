#include "stdafx.h"
#include "IDpParse.h"
#include "YlDpParse.h"


bool HandleDp(const char* szFileName)
{
	YLDpParser parse;
	return parse.HandleDp(szFileName);
}
// LNTCardIssueTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\LNTCardIssue\DllExport.h"

#pragma comment(lib,"LNTCardIssue.lib")

int main()
{
    dp_file_path("D:\\Goldpac\\项目\\2017\\岭南通\\岭南通项目\\LNTdll\\LNTCardIssue\\Debug\\LNTDP.dpi", NULL);
    card_issue(NULL, NULL);
    return 0;
}


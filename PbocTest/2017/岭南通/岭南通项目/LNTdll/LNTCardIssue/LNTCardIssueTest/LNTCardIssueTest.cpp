// LNTCardIssueTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\LNTCardIssue\DllExport.h"

#pragma comment(lib,"LNTCardIssue.lib")

int main()
{
    dp_file_path("D:\\Goldpac\\��Ŀ\\2017\\����ͨ\\����ͨ��Ŀ\\LNTdll\\LNTCardIssue\\Debug\\LNTDP.dpi", NULL);
    card_issue(NULL, NULL);
    return 0;
}


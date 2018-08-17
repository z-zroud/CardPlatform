// DpParseDemo.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../CPS/ICPS.h"
#include<string>
#include<Windows.h>
#include <iostream>
using namespace std;

int main()
{
    char appPath[256] = { 0 };
    GetModuleFileName(NULL, appPath, 256);
    string temp = appPath;
    int pos = temp.find_last_of('\\');
    string dpDir = temp.substr(0, pos);
    string rulePath = temp.substr(0, pos);
    string dpPath = dpDir + "\\goldpac.dp";
    rulePath += "\\goldpacRule.xml";
    cout << dpPath << endl;
    cout << rulePath << endl;
    string dll = dpDir + "\\DpParse_Goldpac.dll";
    HINSTANCE hInst = NULL;
    hInst = LoadLibrary(dll.c_str());
    if (hInst == NULL)
    {
        cout << GetLastError() << endl;
        return false;
    }
    GenCpsFile(dll.c_str(), "HandleDp", dpPath.c_str(), rulePath.c_str());
}


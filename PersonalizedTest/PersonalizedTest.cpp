// PersonalizedTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include "../DpParse/IDpParse.h"
#include "../CPS/ICPS.h"
#include "../ApduCmd/IApdu.h"
#include "../PCSC/PCSC.h"
#include<io.h>
#include<iostream>
using namespace std;

void getFiles(string path, vector<string>& files)
{
    //文件句柄  
    long   hFile = 0;
    //文件信息  
    _finddata_t fileinfo;
    string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            //如果是目录,迭代之  
            //如果不是,加入列表  
            if ((fileinfo.attrib &  _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
            }
            else
            {
                files.push_back(p.assign(path).append("\\").append(fileinfo.name));
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}

int main()
{

	int count = 0;
	char* readers[2] = { 0 };
	GetReaders(readers, count);
	for (int i = 0; i < count; i++)
	{
		printf("%s\n", readers[i]);
	}


	//char atr[128] = { 0 };
	//GetATR(atr, sizeof(atr));
	//printf("%s\n", atr);

   GenCpsFile("DpParse_SZSM.dll", "HandleDp", "F:\\Goldpac\\项目\\DP\\28_808021_20180109_20180109465_ic.txt", "F:\\CardPlatform\\bin\\Configuration\\Rule\\HeDongRule.xml");
   /*  GenCpsFile("DpParse_SZSM.dll", "HandleDp", "F:\\Goldpac\\项目\\DP\\2.txt", "F:\\CardPlatform\\bin\\Configuration\\Rule\\HeDongRule.xml");
    GenCpsFile("DpParse_SZSM.dll", "HandleDp", "F:\\Goldpac\\项目\\DP\\3.txt", "F:\\CardPlatform\\bin\\Configuration\\Rule\\HeDongRule.xml");
    GenCpsFile("DpParse_SZSM.dll", "HandleDp", "F:\\Goldpac\\项目\\DP\\4.txt", "F:\\CardPlatform\\bin\\Configuration\\Rule\\HeDongRule.xml");
*/
    SetPersonlizationConfig("A000000003000000", "404142434445464748494A4B4C4D4E4F", DIV_NONE, SECURE_NONE);
    string path = "F:\\Goldpac\\项目\\DP";
    vector<string> files;
    getFiles(path, files);
    for (auto filePath : files)
    {
        cout << filePath << endl;
        char c;
        cin >> c;
        ColdReset();
        if (!OpenReader(readers[0]))
        {
            return 1;
        }
        DoPersonlization(filePath.c_str(), "F:\\CardPlatform\\bin\\Configuration\\InstallParams\\HuaDa_0034_UICS.xml");
    }
    
    //GenCpsFile("DpParse_HT.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\HuaTeng.dp", "F:\\CardPlatform\\bin\\Configuration\\Rule\\HuaTengRule.xml");
    //GenCpsFile("DpParse_DXT.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\员工卡1500张_12_180131143629.dat", NULL);
    //GenCpsFile("DpParse_YL.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\石嘴山20180202.DP", NULL);
    //GenCpsFile("DpParse_ZJ_TL.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\6214809938020000043.bin","F:\\CardPlatform\\bin\\Configuration\\Rule\\ZhongXingRule.xml");
	//SetPersonlizationConfig("A000000003000000", "404142434445464748494A4B4C4D4E4F", DIV_NONE, SECURE_NONE);
	//DoPersonlization("F:\\CardPlatform\\bin\\DPTest\\6214809938020000043.txt", "F:\\CardPlatform\\bin\\Configuration\\InstallParams\\HuaDa_0034_UICS.xml");
    return 0;
}


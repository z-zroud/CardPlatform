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
    string dpDir = "F:\\CardPlatform\\bin\\Debug\\DPTest\\";
    string dpFormat = "神舟数码";
    string rulePath = "F:\\CardPlatform\\bin\\Debug\\Configuration\\Rule\\";
    string dpPath = dpDir + dpFormat + "\\33_496121_20201018_20201018803_ic.txt";
    rulePath += "HeDongRule.xml";
    GenCpsFile("DpParse_SZSM.dll", "HandleDp", dpPath.c_str(), rulePath.c_str());
    char* cpsFile[30] = { 0 };
    int cpsCount = 0;
    GetCpsFiles(cpsFile, cpsCount);
    //GenCpsFile("DpParse_SZSM.dll", "HandleDp", "F:\\Goldpac\\项目\\DP\\26_808021_20180109_20180109421_ic.txt", "F:\\CardPlatform\\bin\\Configuration\\Rule\\HeDongRule.xml");
    //GenCpsFile("DpParse_SZSM.dll", "HandleDp", "F:\\Goldpac\\项目\\DP\\27_808021_20180109_20180109441_ic.txt", "F:\\CardPlatform\\bin\\Configuration\\Rule\\HeDongRule.xml");
    //GenCpsFile("DpParse_SZSM.dll", "HandleDp", "F:\\Goldpac\\项目\\DP\\23_808021_20180109_20180109363_ic (1).txt", "F:\\CardPlatform\\bin\\Configuration\\Rule\\HeDongRule.xml");
    //GenCpsFile("DpParse_HT.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\华腾\\鞍山银行.dat", "F:\\CardPlatform\\bin\\Configuration\\Rule\\JR1792_鞍山银行_华腾_华大_G81140034.xml");
    //GenCpsFile("DpParse_HT.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\华腾\\石阡村镇银行.dat", "F:\\CardPlatform\\bin\\Configuration\\Rule\\HuaTengRule.xml");
    //GenCpsFile("DpParse_HT.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\华腾\\HuaTeng.dp", "F:\\CardPlatform\\bin\\Configuration\\Rule\\HuaTengRule.xml");
    //GenCpsFile("DpParse_HT.dll", "HandleDp", "F:\\CardPlatform\\bin\\Debug\\DPTest\\华腾\\ICP623553586000021715.dp", "F:\\CardPlatform\\bin\\Debug\\Configuration\\Rule\\JR1454_沂源博商村镇银行_华腾_华大_G81140037.xml");
    //GenCpsFile("DpParse_DXT.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\员工卡1500张_12_180131143629.dat", NULL);
    //GenCpsFile("DpParse_YL.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\石嘴山20180202.DP", NULL);
    //GenCpsFile("DpParse_ZJ_TL.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\浙江泰隆\\6214809938020000043.bin","F:\\CardPlatform\\bin\\Configuration\\Rule\\JR17239_浙江泰隆银行_华大_G81140034.xml");

    SetPersonlizationConfig("A000000003000000", "404142434445464748494A4B4C4D4E4F", DIV_NONE, SECURE_NONE);
    
    string path = dpDir + dpFormat + "\\conv";
    vector<string> files;
    getFiles(path, files);
    string installParamDir = "F:\\CardPlatform\\bin\\Debug\\Configuration\\InstallParams\\";
    string installParamPath = installParamDir + "JR1838_河东村镇银行_华大_G81140042.xml";


    bool isSongJan = false;
    if (isSongJan)
    {
        while(true)
        {
            cout << files[0] << endl;
            char c;
            cin >> c;
            if (!OpenReader(readers[0]))
            {
                return 1;
            }

            //DoPersonlization(filePath.c_str(), "F:\\CardPlatform\\bin\\Configuration\\InstallParams\\JR1792_鞍山银行_华大_G81140034.xml");
            //DoPersonlization(filePath.c_str(), "F:\\CardPlatform\\bin\\Configuration\\InstallParams\\JR17239_浙江泰隆银行_华大_G81140034.xml");
            DoPersonlization(files[0].c_str(), installParamPath.c_str());
        }
    }
    else {
        for (auto filePath : files)
        {
            cout << filePath << endl;
            char c;
            cin >> c;
            if (!OpenReader(readers[0]))
            {
                return 1;
            }

            //DoPersonlization(filePath.c_str(), "F:\\CardPlatform\\bin\\Configuration\\InstallParams\\JR1792_鞍山银行_华大_G81140034.xml");
            //DoPersonlization(filePath.c_str(), "F:\\CardPlatform\\bin\\Configuration\\InstallParams\\JR17239_浙江泰隆银行_华大_G81140034.xml");
            DoPersonlization(filePath.c_str(), installParamPath.c_str());
        }
    }

   	
    return 0;
}


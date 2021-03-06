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
#include "../DpParse_Goldpac/DpParse_Goldpac.h"
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
    string dpDir = "D:\\CardPlatform\\bin\\Debug\\DPTest\\";  
    string rulePath = "D:\\CardPlatform\\bin\\Debug\\Configuration\\Rule\\";
    string dpFormat = "神舟数码";
    string dpPath = dpDir + dpFormat + "\\23_808021_20180109_20180109363_ic (1).txt";
    rulePath += "河东村镇银行_神码_华大_G81140042.xml";
    string installParamCfg = "JR1838_河东村镇银行_华大_G81140042.xml";
    GenCpsFile("DpParse_SZSM.dll", "HandleDp", dpPath.c_str(), rulePath.c_str());

    SetPersonlizationConfig("A000000003000000", "404142434445464748494A4B4C4D4E4F", DIV_NONE, SECURE_NONE);
    
    string path = dpDir + dpFormat + "\\conv";
    vector<string> files;
    getFiles(path, files);
    string installParamDir = "D:\\CardPlatform\\bin\\Debug\\Configuration\\InstallParams\\";
    string installParamPath = installParamDir + installParamCfg;


    bool isSongJan = false;
    cout << "Put card into selected reader to do personlization(1 or 2): ";
    int readerIndex;
    cin >> readerIndex;
    if (isSongJan)
    {
        while(true)
        {
            cout << files[0] << endl;
            cout << "Please confirm to do personlization(Y/N): ";
            char c;
            cin >> c;
            if (c != 'Y' && c != 'y')
            {
                break;
            }
            if (!OpenReader(readers[readerIndex - 1]))
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
            cout << "Please confirm to do personlization(Y/N): ";
            char c;
            cin >> c;
            if (c != 'Y' && c != 'y')
            {
                break;
            }
            if (!OpenReader(readers[readerIndex - 1]))
            {
                return 1;
            }

            //DoPersonlization(filePath.c_str(), "F:\\CardPlatform\\bin\\Configuration\\InstallParams\\JR1792_鞍山银行_华大_G81140034.xml");
            //DoPersonlization(filePath.c_str(), "F:\\CardPlatform\\bin\\Configuration\\InstallParams\\JR17239_浙江泰隆银行_华大_G81140034.xml");
            DoPersonlization(filePath.c_str(), installParamPath.c_str());
        }
    }

    getchar();
    return 0;
}


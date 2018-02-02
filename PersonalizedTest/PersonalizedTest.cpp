// PersonalizedTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include "../DpParse/IDpParse.h"
#include "../CPS/ICPS.h"
#include "../ApduCmd/IApdu.h"
#include "../PCSC/PCSC.h"

int main()
{
	int count = 0;
	char* readers[2] = { 0 };
	GetReaders(readers, count);
	for (int i = 0; i < count; i++)
	{
		printf("%s\n", readers[i]);
	}
	if (!OpenReader(readers[0]))
	{
		return 1;
	}

	char atr[128] = { 0 };
	GetATR(atr, sizeof(atr));
	printf("%s\n", atr);


    //GenCpsFile("DpParse_HT.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\HuaTeng.dp", "F:\\CardPlatform\\bin\\Configuration\\Rule\\HuaTengRule.xml");
    //GenCpsFile("DpParse_DXT.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\员工卡1500张_12_180131143629.dat", NULL);
	//GenCpsFile("DpParse_YL.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\石嘴山.DP","F:\\CardPlatform\\bin\\Configuration\\Rule\\石嘴山.cfg");
    //GenCpsFile("DpParse_ZJ_TL.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\6214809938020000043.bin","F:\\CardPlatform\\bin\\Configuration\\Rule\\ZhongXingRule.xml");
	SetPersonlizationConfig("A000000003000000", "404142434445464748494A4B4C4D4E4F", DIV_NONE, SECURE_NONE);
	DoPersonlization("F:\\CardPlatform\\bin\\DPTest\\6214809938020000043.txt", "F:\\CardPlatform\\bin\\Configuration\\InstallParams\\HuaDa_0034_UICS.xml");
    return 0;
}


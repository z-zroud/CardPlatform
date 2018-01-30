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
	//int count = 0;
	//char* readers[2] = { 0 };
	//GetReaders(readers, count);
	//for (int i = 0; i < count; i++)
	//{
	//	printf("%s\n", readers[i]);
	//}
	//if (!OpenReader(readers[0]))
	//{
	//	return 1;
	//}

	//char atr[128] = { 0 };
	//GetATR(atr, sizeof(atr));
	//printf("%s\n", atr);

    GenCpsFile("DpParse_HT.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\HuaTeng.dp", NULL);
    //GenCpsFile("DpParse_DXT.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\huaxin3.dat", NULL);
	//GenCpsFile("DpParse_YL.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\石嘴山.DP","F:\\CardPlatform\\bin\\Configuration\\Rule\\石嘴山.cfg");
    //GenCpsFile("DpParse_ZJ_TL.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\浙江泰隆.bin", NULL);
	//SetPersonlizationConfig("A000000003000000", "404142434445464748494A4B4C4D4E4F", DIV_NONE, SECURE_NONE);
	//DoPersonlization("F:\\CardPlatform\\bin\\DPTest\\conv\\6283550000001338.txt", "F:\\CardPlatform\\bin\\Configuration\\InstallParams\\石嘴山_英飞凌_05006046_PBOC_ESP.txt");
    return 0;
}


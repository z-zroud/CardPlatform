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
	//PHandleDpCallback pHandleDp = GetHandleDpFunc<PHandleDpCallback>("DpParse_DXT.dll", "HandleDp");

	//if (pHandleDp)
	//{
	//	pHandleDp("F:\\CardPlatform\\bin\\huaxin.dat");
	//}
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

	GenCpsFile("DpParse_YL.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\yinlian2.DP","F:\\CardPlatform\\bin\\Configuration\\Rule\\projectName.cfg");
    //GenCpsFile("DpParse_YL.dll", "HandleDp", "F:\\CardPlatform\\bin\\DPTest\\yinlian2.DP", NULL);
	//SetPersonlizationConfig("A000000003000000", "404142434445464748494A4B4C4D4E4F", DIV_NONE, SECURE_NONE);
	//DoPersonlizationWithOrderdDGI("F:\\CardPlatform\\bin\\huaxin.txt", "F:\\CardPlatform\\bin\\Configuration\\InstallParams\\DMT_G87140034_PBOC.txt");
    return 0;
}


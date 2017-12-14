// PbocTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\PCSC\PCSC.h"
#include "..\ApduCmd\IApdu.h"

#define SW_9000 0x9000
int main()
{
	/********************** 打开读卡器 ********************************/
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

	/************************* 选择应用 ****************************/
	int sw = 0;
	char selectAppResp[256] = { 0 };
	sw = SelectAppCmd("315041592E5359532E4444463031", selectAppResp);
	if (sw != SW_9000)
	{
		printf("选择应用失败!\n");
		return 2;
	}

    return 0;
}


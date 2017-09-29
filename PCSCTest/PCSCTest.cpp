// PCSCTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../PCSC/PCSC.h"
#include <Windows.h>
#ifdef _DEBUG
#pragma comment(lib,"../Debug/PCSC.lib")
#else
#pragma comment(lib,"../Release/PCSC.lib")
#endif

int main()
{
	int count = 0;
	char* readers[2] = { 0 };
	GetReaders(readers, count);
	for (int i = 0; i < count; i++)
	{
		printf("%s", readers[i]);
	}
	OpenReader(readers[0]);
	WarmReset();
	ColdReset();

	char atr[33] = { 0 };
	GetATR(atr, sizeof(atr));
	printf("%s", atr);

	GetCardStatus();
	GetTransProtocol();

	char output[256] = { 0 };
	SendApdu("00A40400 08 A000000003000000", output, sizeof(output));

    return 0;
}


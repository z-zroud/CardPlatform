// UtilTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\Util\Tool.h"

int main()
{
	//Test IntToStr
	int value = 12;
	char hexStr[5] = { 0 };
	Tool::IntToStr(value, hexStr, Two_byte);

	value = 160;
	Tool::IntToStr(value, hexStr, Four_byte);


	//Test SubStr
	char* src = "Hello! Welcome to C++ world.";
	char dst[32] = { 0 };
	Tool::SubStr(src, 7, 7, dst);

    return 0;
}


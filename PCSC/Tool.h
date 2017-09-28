#pragma once
#include <string>

using namespace std;
namespace Tool 
{
	typedef unsigned char byte;
	void AscToBcd(byte *bcd, byte *asc, long asc_len);	//Ascii 转 BCD
	long AscToBin(char *ptr, int len);	//Ascii 转 二进制
	void BcdToAsc(char *asc, char *bcd, long asc_len);	//BCD转Ascii
	void StrToHex(byte *pDest, byte *pSrc, int nLen);
    void DeleteSpace(const char* src, char* dest, int len);
}


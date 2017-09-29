#pragma once
#include <string>

using namespace std;
namespace Tool 
{
	typedef unsigned char byte;
	void BcdToAsc(byte *bcd, byte *asc, long asc_len);	//Ascii ת BCD
	long AscToBin(char *ptr, int len);	//Ascii ת ������
	void AscToBcd(char *asc, char *bcd, long bcd_len);	//BCDתAscii
	void StrToHex(byte *pDest, byte *pSrc, int nLen);
    void DeleteSpace(const char* src, char* dest, int len);
}


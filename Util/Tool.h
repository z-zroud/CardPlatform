#pragma once
#include <string>

using namespace std;
#ifdef __cplusplus
extern "C"
{
#endif
namespace Tool 
{
	typedef unsigned char byte;
	void BcdToAsc(byte *asc, byte *bcd, long bcd_len);	//Ascii 转 BCD
	long AscToBin(char *ptr, int len);	//Ascii 转 二进制
	void AscToBcd(char *bcd, char *asc, long asc_len);	//BCD转Ascii
	void StrToHex(byte *pDest, byte *pSrc, int nLen);
    void DeleteSpace(const char* src, char* dest, int len);
	void HexStr(const char* bcdLen, char* output, int len);
	void IncreaseStep(const char* strLen, int step, char* output, int len);
}

#ifdef __cplusplus
}
#endif


#pragma once
#include <string>

enum Hex_Len
{
	Two_byte = 2,
	Four_byte = 4
};


using namespace std;
#ifdef __cplusplus
extern "C"
{
#endif
namespace Tool 
{
	//typedef unsigned char byte;
	void BcdToAsc(unsigned char *asc, unsigned char *bcd, long bcd_len);	//Ascii 转 BCD
	long AscToBin(char *ptr, int len);	//Ascii 转 二进制
	void AscToBcd(char *bcd, char *asc, long asc_len);	//BCD转Ascii
	void StrToHex(unsigned char *pDest, unsigned char *pSrc, int nLen);
    void DeleteSpace(const char* src, char* dest, int len);

	/**********************************************************
	* 功能：
	***********************************************************/
	void HexStr(const char* bcdLen, char* output, int len);
	void IncreaseStep(const char* strLen, int step, char* output, int len);

	/***********************************************************
	* 功能： 截取子字符串
	* 参数： src 源字符串
	* 参数： start 截取源字符串的起始位置
	* 参数： len 截取的长度
	* 参数： dst 存放子字符串
	************************************************************/
	bool SubStr(const char* src, int start, int len, char* dst);

	/*********************************************************
	* 功能： 将整形转换为字符串形式
	* 参数： value 整形数字
	* 参数： hexStr 输出的十六进制字符串形式
	* 参数： 指定输出的长度 Two_byte 或者 Four_byte
	**********************************************************/
	void IntToStr(int value, char* hexStr, Hex_Len hexLen);
}

#ifdef __cplusplus
}
#endif


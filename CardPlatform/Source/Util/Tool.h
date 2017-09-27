#pragma once
#include <string>

using namespace std;
namespace Tool 
{
	typedef unsigned char byte;
	void AscToBcd(byte *bcd, byte *asc, long asc_len);	//Ascii ת BCD
	long AscToBin(char *ptr, int len);	//Ascii ת ������
	void BcdToAsc(char *asc, char *bcd, long asc_len);	//BCDתAscii
	void StrToHex(byte *pDest, byte *pSrc, int nLen);
	string StrToHex(const char* strBin, int len, bool bIsUpper = true);
	void HexToStr(byte *pDest, byte *pSrc, int nLen);
	int HexToInt(char c);
	string StrUpper(const char* str);
	int HexStrToInt(const char* strHex, int len);
    void SplitString(const string& s, vector<string>& v, const string& c);
    void SplitString(const string &s, std::vector<string> &vec, const string& start, const string& end);
    string DeleteSpace(string s);
    string IncStringLenStep(const string& len, int step);
    string GetStringLen(const string& str);
}


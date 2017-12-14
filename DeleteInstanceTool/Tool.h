#pragma once
#include <string>
#include <vector>
using namespace std;

struct Tool
{
	typedef unsigned char byte;

	static void SplitString(const string& s, std::vector<std::string>& v, const string& c);
	static string DeleteSpace(string s);	
	static void AscToBcd(byte *bcd, byte *asc, long asc_len);	//Ascii ת BCD
	static void BcdToAsc(char *asc, char *bcd, long asc_len);	//BCDתAscii

	static string IncStringLenStep(string currentLen, int step);
	static string GetStringLen(string data);
	static long GetFileSize(FILE* file);
};


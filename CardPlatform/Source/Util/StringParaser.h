#pragma once
#include <string>
#include <vector>
using namespace std;

namespace Tool
{
	namespace Stringparser
	{
		void SplitString(const string& s, vector<string>& v, const string& c);
		string DeleteSpace(string s);
		string StrToHexStr(const char* strBin, int len, bool bIsUpper = true);
		string HexStrToStr(const char* str);
		string IntToHexStr(const int n);
		int HexStrToInt(const string hexStr);
		string ToUpper(string& str);
		string ToLower(string& str);
	}
}


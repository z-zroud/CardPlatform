// UtilTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\Util\Tool.h"
#include "..\Util\SockServer.h"
#include <vector>
using namespace std;

int main()
{
    //Test Base64 decode
    int len = 0;
    string testBase64 = Tool::base64_decode("fccMwd6wotmr/UhcqZL1kdvI0BqmcJwI6fBEF374ecrbAZwPgp9b2hukA+PwoXeWRhyeoQV4jq4G//Wp4/Axp3OLeRxdYMRJiNJGr2HHPZdeXQcE9v41vA==",len);
    string testBase641 = Tool::StrToBcd(testBase64.c_str(),len);
    char tmp[2048] = { 0 };
    Tool::AscToBcd(tmp, (char*)testBase64.c_str(), len * 2);
    //Test StrToBcd 6C696A6965
    char* a = "lijie";
    string test = Tool::StrToBcd(a,5);
    //Test GetDirectory
    const char* filePath = "a\\b\\c.txt";
    string dir = Tool::GetDirectory(filePath);

    //Test BcdToStr
    char* bcd = "50525950463030303031";
    char str[32] = { 0 };
    Tool::BcdToStr(bcd, str, 32);
    string asciiStr = Tool::BcdToStr(bcd);
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

    //Test SplitStr
    string strSplit = "9F56|1|00|9F13|2|0000|9F4D|2|0B0A|";
    vector<string> subStrs;
    Tool::SplitStr(strSplit, "|", subStrs);
    return 0;
}


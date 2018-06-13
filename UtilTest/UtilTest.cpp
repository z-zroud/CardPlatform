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
    string testBase64 = Tool::base64_decode("UBi/YRBxHM9s0ODGUqG8q4s7aLVXpbH4Gohu4uV/H4h34tAtN1B/2v/vuzJiippQW7jpOuXwK6aIKGEbMKsaCmp/+fEBJDh8z0cdcc4AjyYcmUdWfuXvbRCsGYz97zbR7OyTJBMEVwwJbSggnBwt/+iUkf5FKCk8NStyF+ohvw8mdq75+j9RoOWm3rOa+DJcLCY7NSlc/UQee9XSDpyq+/+yiv4Ui3dT5CdV2Y5Rds7/YF4tbpKyoiH9DqxdecvV/e1f+F0iVmHdGA0jxPQWp2asx++oWDwGW2hXF3ooFK3f4EIEG2UQj9Xgzai6cG5WOd3fV+lzp9k=");
    string testBase641 = Tool::StrToBcd(testBase64.c_str());
    //Test StrToBcd
    string test = Tool::StrToBcd("en");
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


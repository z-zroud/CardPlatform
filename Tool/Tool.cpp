// Tool.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include<stdio.h>
#include <cmath>
#include "tool.h"

//将字符串型十六进制转为整形
int ctoi(unsigned char c)
{
    switch (toupper(c))
    {
    case '0':		return 0;
    case '1':		return 1;
    case '2':		return 2;
    case '3':		return 3;
    case '4':		return 4;
    case '5':		return 5;
    case '6':		return 6;
    case '7':		return 7;
    case '8':		return 8;
    case '9':		return 9;
    case 'A':		return 10;
    case 'B':		return 11;
    case 'C':		return 12;
    case 'D':		return 13;
    case 'E':		return 14;
    case 'F':		return 15;
    default:		return -1;
    }

    return -1;
}

void IntToHexStr(int input, char* output, int outputLen)
{
    if (input >= 0 && input <= 0xFF){
        sprintf_s(output, outputLen, "%02X", input);
    }
    else if (input > 0xFF && input <= 0xFFFF) {
        sprintf_s(output, outputLen, "%04X", input);
    }
}

int HexStrToInt(const char* hexStr)
{
    int ret = 0;
    for (int i = 0; i < strlen(hexStr); i++)
    {
        int x = ctoi(hexStr[i]);
        ret += x * pow(16, strlen(hexStr) - i - 1);
    }
    return ret;
}

void GetSubString(const char* str, int start, int len, char* subStr)
{
    if (start < 0 || start > strlen(str))
        return;
    if (strlen(str) < len)
        len = strlen(str) - start;
        
    strncpy_s(subStr, len + 1, str + start, len);
}

void GetStringRightPart(const char* str, int start, char* subStr)
{
    if (start > strlen(str))
        return;
    int len = strlen(str) - start;

    strncpy_s(subStr, len + 1, str + start, len);
}

void StrToBcd(const char* str, int strLen, char* bcd, int bcdLen)
{
    for (int i = 0; i < strLen; i++)
    {
        unsigned char ascii = (unsigned char)str[i];
        char c[3] = { 0 };
        snprintf(c, 3, "%02X", ascii);
        strcat_s(bcd, bcdLen, c);
    }
}


void BcdToStr(const char* bcd, char* str, int strLen)
{
    int bcdLen = strlen(bcd);
    if (bcdLen % 2 != 0) {
        return;
    }
    memset(str, 0, strLen);
    for (int i = 0; i < bcdLen; i += 2) {
        char temp[3] = { 0 };
        GetSubString(bcd, i, 2, temp);
        int ascii = strtol(temp, 0, 16);
        //if (ascii > 0 && ascii < 128)
        //{
        //    str[i / 2] = ascii;
        //}
        str[i / 2] = ascii;
    }
}

void Trim(const char* intput, char* output, int outputLen)
{
    int intputLen = strlen(intput);
    int j = 0;
    for (int i = 0; i < intputLen; i++)
    {
        if (intput[i] != ' ' && j < outputLen)
        {
            output[j] = intput[i];
            j++;
        }
    }
}

bool IsHexStr(const char* hexStr)
{
    for(int i = 0; i < strlen(hexStr); i++)
    {
        if (ctoi(hexStr[i]) == -1)
            return false;
    }
    return true;
}


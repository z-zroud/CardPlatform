#pragma once
/****************************************************
* 该动态库提供一系列的字符串与整形之间的转换接口，
* ASCII和BCD码之间的转换等
*****************************************************/

#ifdef TOOL_EXPORTS
#define TOOL_API __declspec(dllexport)
#else
#define TOOL_API __declspec(dllimport)
#endif

extern "C"
{
    /*************************************************
    * 功能：将input转换为十六进制的字符串形式，若input
    * 在0~FF之间，output为2字节。FF~FFFF之间依次类推
    **************************************************/
    void TOOL_API IntToHexStr(int input, char* output, int outputLen);

    int TOOL_API HexStrToInt(const char* hexStr);

    /*************************************************
    * 字符串和BCD码之间的互相转换
    **************************************************/
    void TOOL_API StrToBcd(const char* str, int strLen, char* bcd, int bcdLen);

    void TOOL_API BcdToStr(const char* bcd, char* str, int strLen);

    /*************************************************
    * 功能：获取str从指定位置到指定长度的子字符串
    **************************************************/
    void TOOL_API GetSubString(const char* str, int start, int len, char* subStr);

    /*************************************************
    * 功能：从start位置截取字符串右半部分
    **************************************************/
    void TOOL_API GetStringRightPart(const char* str, int start, char* subStr);

    /*************************************************
    * 功能：删除字符串中所有的空格
    **************************************************/
    void TOOL_API Trim(const char* intput, char* output, int outputLen);

    /*************************************************
    * 功能：判断hexStr是否全是1-9,a-z,A-Z字符
    **************************************************/
    bool TOOL_API IsHexStr(const char* hexStr);

    /**************************************************
    * 功能: Base64字符串转BCD码
    ***************************************************/
    void TOOL_API Base64ToBcd(const char* base64, char* bcd, int bcdLen);
}
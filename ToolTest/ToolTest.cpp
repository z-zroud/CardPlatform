// ToolTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\Tool\tool.h"
#include <string.h>

int main()
{
    const char* base64 = "Lm595NXdgKqzX53sb1oEjviRYZ+lTW7rrg9LcJZ/60Hy8R+7Eq6x7srFTydmxEI/S2jkD1jkXHxM103oy1JXK2sqZG3TiIHB";
    char bcdRet[1024] = { 0 };
    Base64ToBcd(base64, bcdRet, 1024);
    int a = 11, b = 18, c = 400, d = 960;
    char szA[3] = { 0 };
    char szB[3] = { 0 };
    char szC[5] = { 0 };
    char szD[5] = { 0 };
    IntToHexStr(a, szA, sizeof(szA));
    IntToHexStr(b, szB, sizeof(szB));
    IntToHexStr(c, szC, sizeof(szC));
    IntToHexStr(d, szD, sizeof(szD));

    const char* str = "abcdefghigklmnopqrst";
    char subStr[32] = { 0 };
    GetSubString(str, 3, 5, subStr);
    GetSubString(str, 3, 25, subStr);

    GetStringRightPart(str, 0, subStr);
    GetStringRightPart(str, 3, subStr);

    char bcd[64] = { 0 };
    char asc[12] = "Hello World";
    StrToBcd(asc, strlen(asc), bcd, sizeof(bcd));
    BcdToStr(bcd, asc, sizeof(asc));

    char bcd1[64] = { 0 };
    char asc1[12] = "你 好";
    StrToBcd(asc1, strlen(asc1), bcd1, sizeof(bcd1));
    BcdToStr(bcd1, asc1, sizeof(asc1));

    int intValue = HexStrToInt("0A");
    intValue = HexStrToInt("AA");
    intValue = HexStrToInt("AAAA");
    return 0;
}
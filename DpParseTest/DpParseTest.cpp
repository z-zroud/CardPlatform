// DpParseTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../DpParse/ProcessFile.h"

int main()
{
    ProcessFile file("D:\\Test2.DP");
    int offset = 0;
    string buffer;
    int value = 0;
    for (int i = 0; i < 25; i++)
    {
        buffer = file.Read(offset, 2);
        buffer = file.ReadBinary(offset, 2);
        value = file.ReadInt(offset);
    }

    return 0;
}


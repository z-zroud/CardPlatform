// ApduCmdTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../ApduCmd/IApdu.h"
#include "../PCSC/PCSC.h"

int main()
{
    int count = 0;
    char* readers[2] = { 0 };
    GetReaders(readers, count);
    for (int i = 0; i < count; i++)
    {
        printf("%s\n", readers[i]);
    }
    if (!OpenReader(readers[0]))
    {
        return 1;
    }
    OpenSecureChannel("404142434445464748494A4B4C4D4E4F", DIV_NONE, SECURE_NONE);

    return 0;
}


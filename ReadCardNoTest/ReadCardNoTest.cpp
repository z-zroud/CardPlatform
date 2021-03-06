// ReadCardNoTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../ReadCardNo/ReadCardNo.h"

#ifdef DEBUG
#pragma comment(lib,"../lib/Debug/ReadCardNo.lib")
#else
#pragma comment(lib,"../lib/Release/ReadCardNo.lib")
#endif // DEBUG
#include<iostream>
using namespace std;


int main()
{
    int readerCount = 2;
    char* readers[2] = { 0 };
    int result = -1;
    result = GetReadersEx(readers, readerCount);
    if (result != 0)
    {
        return -1;  //无法获取读卡器
    }
    for (int i = 0; i < readerCount; i++)
    {
        printf("%s\n", readers[i]);
    }
    cout << "Put card into selected reader to do personlization(1 or 2): ";
    int readerIndex;
    cin >> readerIndex;
    result = OpenReaderEx(readers[readerIndex - 1]);
    if (result != 0)
    {
        return -2;  //无法打开读卡器
    }
    char cardNo[32] = { 0 };
    result = ReadCardNo("A000000333010101", 2, 1, cardNo, sizeof(cardNo));
    if (result == 0)
    {
        printf("card number: %s\n", cardNo);
    }
    getchar();
    getchar();
    return 0;
}


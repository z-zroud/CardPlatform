// PythonInterfaceTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../PythonInterface/PythonInterface.h"
#include "../PCSC/PCSC.h"

int main()
{
    if (!InitPythonEnv()) {
        return false;
    }

    InitWorkingEnv("F:\\CardPlatform\\PyScript");
    int count = 0;
    char* readers[2] = { 0 };
    GetReaders(readers, count);
    for (int i = 0; i < count; i++)
    {
        printf("%s\n", readers[i]);
    }
    SetReaderName(readers[0]);
    ClosePythonEnv();
    return 0;
}


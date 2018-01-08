// PythonInterfaceTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../PythonInterface/PythonInterface.h"
#include "../PCSC/PCSC.h"

int main()
{
    if (!PY_InitPythonEnv()) {
        return false;
    }

    PY_InitWorkingEnv("F:\\CardPlatform\\PyScript");
    int count = 0;
    char* readers[2] = { 0 };
    GetReaders(readers, count);
    for (int i = 0; i < count; i++)
    {
        printf("%s\n", readers[i]);
    }
    PY_SetReaderName("Identive uTrust 4700 F CL Reader 7");
    OpenReader(readers[0]);
    PY_DoPbocContactless();
    PY_ClosePythonEnv();

    getchar();

    return 0;
}


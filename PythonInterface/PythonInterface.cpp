// PythonInterface.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "PythonInterface.h"
#include <Python.h>

bool InitPythonEnv()
{
    Py_Initialize();

    return Py_IsInitialized();
}

void ClosePythonEnv()
{
    Py_FinalizeEx();
}
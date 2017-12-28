// PythonInterface.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "PythonInterface.h"
#include <Python.h>

PyObject* g_pModule;    //C++与Python通信模块

bool InitPythonEnv()
{
    Py_Initialize();

    return Py_IsInitialized();
}

void ClosePythonEnv()
{
    Py_FinalizeEx();
}

bool InitWorkingEnv(const char* dir)
{ 
    char cmd[256] = { 0 };
    sprintf_s(cmd, 256, "sys.path.append(\"%s\")", dir);
    PyRun_SimpleString("import sys");
    PyRun_SimpleString(cmd);
    PyObject* moduleName = Py_BuildValue("s", "CInterface");
    g_pModule = PyImport_Import(moduleName);

    return g_pModule != NULL;
}

void SetReaderName(const char* readerName)
{
    PyObject* pv = PyObject_GetAttrString(g_pModule, "GetReaderName");
    PyObject* args = PyTuple_New(1);
    PyObject* arg1 = Py_BuildValue("s", readerName);
    PyTuple_SetItem(args, 0, arg1);

    PyObject* pRet = PyObject_CallObject(pv, args);
}
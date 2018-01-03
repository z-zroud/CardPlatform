// PythonInterface.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "PythonInterface.h"
#include <Python.h>

PyObject* g_pParamModule;    //C++与Python通信模块
PyObject* g_pBussinessModule;   //C++ 调用 Python业务逻辑模块

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

    PyObject* paramModuleName = Py_BuildValue("s", "card_check.util.CInterface");
    PyObject* bussinessModuleName = Py_BuildValue("s", "PyScript");
    g_pParamModule = PyImport_Import(paramModuleName);
    g_pBussinessModule = PyImport_Import(bussinessModuleName);

    return g_pParamModule != NULL && g_pBussinessModule != NULL;
}

void SetReaderName(const char* readerName)
{
    PyObject* pv = PyObject_GetAttrString(g_pParamModule, "GetReaderName");
    PyObject* args = PyTuple_New(1);
    PyObject* arg1 = Py_BuildValue("s", readerName);
    PyTuple_SetItem(args, 0, arg1);

    PyObject* pRet = PyObject_CallObject(pv, args);
}

void SetSMMark(bool bMark)
{
    PyObject* pv = PyObject_GetAttrString(g_pParamModule, "GetSMMark");
    PyObject* args = PyTuple_New(1);
    PyObject* arg1 = Py_BuildValue("O", bMark ? Py_True : Py_False);
    PyTuple_SetItem(args, 0, arg1);

    PyObject* pRet = PyObject_CallObject(pv, args);
}

bool OpenReader(const char* readerName)
{
    PyObject* pv = PyObject_GetAttrString(g_pBussinessModule, "OpenCardReader");
    PyObject* args = PyTuple_New(1);
    PyObject* arg1 = Py_BuildValue("s", readerName);
    PyTuple_SetItem(args, 0, arg1);

    PyObject* pRet = PyObject_CallObject(pv, args);

    return pRet != NULL;
}

void DoPbocContact()
{
    PyObject* pv = PyObject_GetAttrString(g_pBussinessModule, "DoPbocContact");
    //PyObject* args = PyTuple_New(1);
    //PyObject* arg1 = Py_BuildValue("s", readerName);
    //PyTuple_SetItem(args, 0, arg1);

    PyObject* pRet = PyObject_CallObject(pv, NULL);
}
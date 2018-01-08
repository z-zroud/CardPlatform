// PythonInterface.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "PythonInterface.h"
#include <Python.h>

PyObject* g_pParamModule;    //C++与Python通信模块
PyObject* g_pPyScriptModule;   //C++ 调用 Python业务逻辑模块

bool PY_InitPythonEnv()
{
    Py_Initialize();

    return Py_IsInitialized();
}

void PY_ClosePythonEnv()
{
    Py_FinalizeEx();
}

bool PY_InitWorkingEnv(const char* dir)
{ 
    char cmd[256] = { 0 };
    sprintf_s(cmd, 256, "sys.path.append(\"%s\")", dir);

    PyRun_SimpleString("import sys");
    PyRun_SimpleString(cmd);

    PyObject* paramModuleName = Py_BuildValue("s", "card_check.util.CInterface");
    PyObject* bussinessModuleName = Py_BuildValue("s", "PyScript");
    g_pParamModule = PyImport_Import(paramModuleName);
    g_pPyScriptModule = PyImport_Import(bussinessModuleName);

    return g_pParamModule != NULL && g_pPyScriptModule != NULL;
}

PyObject* SetParam(const char* format, ...)
{
    int argsCount = strlen(format);
    PyObject* args = PyTuple_New(argsCount);
    va_list ap;
    va_start(ap, format);
    for (int i = 0; i < argsCount; i++) {
        PyObject* arg = NULL;
        if (format[i] == 's')    arg = Py_BuildValue("s", va_arg(ap, char*));
        else if (format[i] == 'i') arg = Py_BuildValue("i", va_arg(ap, int));
        else if(format[i] == 'O') arg = Py_BuildValue("O", va_arg(ap, bool) ? Py_True : Py_False);
        PyTuple_SetItem(args, i, arg);
    }
    va_end(ap);
    return args;
}

bool PY_SetReaderName(const char* readerName)
{
    PyObject* pv = PyObject_GetAttrString(g_pParamModule, "SetReaderName");
    PyObject* args = SetParam("s", readerName);
    if (args == NULL || pv == NULL) {
        return false;
    }
    if (PyObject_CallObject(pv, args)) {
        return true;
    }
    return false;
}

bool PY_SetSMMark(bool bMark)
{
    PyObject* pv = PyObject_GetAttrString(g_pParamModule, "GetSMMark");
    PyObject* args = SetParam("O", bMark);
    if (args == NULL || pv == NULL) {
        return false;
    }
    if (PyObject_CallObject(pv, args)) {
        return true;
    }
    return false;
}


int PY_DoPbocContact()
{
    PyObject* pv = PyObject_GetAttrString(g_pPyScriptModule, "DoPbocContact");
    if (PyObject_CallObject(pv, NULL)) {
        return 0;
    }
    return 1;
}

int PY_DoPbocContactless()
{
    PyObject* pv = PyObject_GetAttrString(g_pPyScriptModule, "DoPbocContactless");
    if (PyObject_CallObject(pv, NULL)) {
        return 0;
    }
    return 1;
}
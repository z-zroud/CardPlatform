#pragma once

#pragma once
#ifdef PYTHONINTERFACE_EXPORTS
#define PYTHONINTERFACE_API __declspec(dllexport)
#else
#define PYTHONINTERFACE_API __declspec(dllimport)
#endif

/*************************************************************
* 该动态库定义了C++调用python函数的接口
**************************************************************/

extern "C" PYTHONINTERFACE_API bool PY_InitPythonEnv();
extern "C" PYTHONINTERFACE_API void PY_ClosePythonEnv();
extern "C" PYTHONINTERFACE_API bool PY_InitWorkingEnv(const char* dir);
extern "C" PYTHONINTERFACE_API bool PY_SetReaderName(const char* readerName);
extern "C" PYTHONINTERFACE_API bool PY_SetSMMark(bool bMark);

extern "C" PYTHONINTERFACE_API int PY_DoPbocContact();
extern "C" PYTHONINTERFACE_API int PY_DoPbocContactless();
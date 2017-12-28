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

extern "C" PYTHONINTERFACE_API bool InitPythonEnv();
extern "C" PYTHONINTERFACE_API void ClosePythonEnv();
extern "C" PYTHONINTERFACE_API bool InitWorkingEnv(const char* dir);
extern "C" PYTHONINTERFACE_API void SetReaderName(const char* readerName);
#pragma once

#pragma once
#ifdef PYTHONINTERFACE_EXPORTS
#define PYTHONINTERFACE_API __declspec(dllexport)
#else
#define PYTHONINTERFACE_API __declspec(dllimport)
#endif

/*************************************************************
* �ö�̬�ⶨ����C++����python�����Ľӿ�
**************************************************************/

extern "C" PYTHONINTERFACE_API bool InitPythonEnv();
extern "C" PYTHONINTERFACE_API void ClosePythonEnv();
extern "C" PYTHONINTERFACE_API bool InitWorkingEnv(const char* dir);
extern "C" PYTHONINTERFACE_API void SetReaderName(const char* readerName);
extern "C" PYTHONINTERFACE_API bool OpenReader(const char* readerName);
extern "C" PYTHONINTERFACE_API void SetSMMark(bool bMark);

extern "C" PYTHONINTERFACE_API void DoPbocContact();
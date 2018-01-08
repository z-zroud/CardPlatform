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

extern "C" PYTHONINTERFACE_API bool PY_InitPythonEnv();
extern "C" PYTHONINTERFACE_API void PY_ClosePythonEnv();
extern "C" PYTHONINTERFACE_API bool PY_InitWorkingEnv(const char* dir);
extern "C" PYTHONINTERFACE_API bool PY_SetReaderName(const char* readerName);
extern "C" PYTHONINTERFACE_API bool PY_SetSMMark(bool bMark);

extern "C" PYTHONINTERFACE_API int PY_DoPbocContact();
extern "C" PYTHONINTERFACE_API int PY_DoPbocContactless();
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
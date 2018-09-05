#pragma once

/***************************************************************
* ����־���Ϊ��Щ��Ҫ��¼�򵥵���־��������ͬʱΪ���ܿ�����
* ��¼��־�������˽ӿ����ΪC���Խӿ���ʽ
****************************************************************/
#ifdef LOG_EXPORTS
#define LOG_API __declspec(dllexport)
#else
#define LOG_API __declspec(dllimport)
#endif

typedef int     log_t;

//��־�����ʽ
#define OUT_CONSOLE     1
#define OUT_FILE        2

extern "C" LOG_API log_t CreateLog(const char* path, int type = 2);
extern "C" LOG_API log_t GetCurLog();
extern "C" LOG_API void TraceInfo(log_t, const char* formatString, ...);
extern "C" LOG_API void TraceWarn(log_t, const char* formatString, ...);
extern "C" LOG_API void TraceDebug(log_t, const char* formatString, ...);
extern "C" LOG_API void TraceError(log_t, const char* foramtString, ...);
extern "C" LOG_API void TraceLog(log_t, const char* info);
#pragma once

/***************************************************************
* 本日志库仅为那些需要记录简单的日志而建立，同时为了能跨语言
* 记录日志，仅将此接口设计为C语言接口形式
****************************************************************/
#ifdef LOG_EXPORTS
#define LOG_API __declspec(dllexport)
#else
#define LOG_API __declspec(dllimport)
#endif

typedef int     log_t;

//日志输出方式
#define OUT_CONSOLE     1
#define OUT_FILE        2

extern "C" LOG_API log_t CreateLog(const char* path, int type = 2);
extern "C" LOG_API log_t GetCurLog();
extern "C" LOG_API void TraceInfo(log_t, const char* formatString, ...);
extern "C" LOG_API void TraceWarn(log_t, const char* formatString, ...);
extern "C" LOG_API void TraceDebug(log_t, const char* formatString, ...);
extern "C" LOG_API void TraceError(log_t, const char* foramtString, ...);
extern "C" LOG_API void TraceLog(log_t, const char* info);
extern "C" LOG_API void ClearLog(log_t);
extern "C" LOG_API int GetCurLine();
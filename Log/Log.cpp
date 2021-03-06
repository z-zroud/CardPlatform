// Log.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <stdarg.h>
#include "Configuration.h"

#define MaxMsgLen   1024 * 2

log_t g_latest = 0;
int g_curLine = 0;

log_t CreateLog(const char* path, int type)
{
    //if (type == OUT_CONSOLE)
    //{
    //    return IncreaseAddr(kBaseAddr);
    //}
    g_latest = AddNewLog(path);
    return g_latest;
}

log_t GetCurLog()
{
    return g_latest;
}

void ClearLog(log_t log)
{
    g_curLine = 0;
    LogState state = GetExistedLog(log);
    if (state.path)
    {
        fopen_s(&state.file, state.path, "w+");
        if (state.file) {
            fflush(state.file);
            fclose(state.file);
        }
    }
}

int GetCurLine()
{
    return g_curLine;
}

void TraceLog(log_t log, const char* info)
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    char buf[1024 * 2] = { 0 };
    //sprintf_s(buf, 1024 * 2 - 1, "%.4d-%.2d-%.2d  %.2d:%.2d:%.2d %s", 
    //    st.wYear, 
    //    st.wMonth, 
    //    st.wDay, 
    //    st.wHour, 
    //    st.wMinute, 
    //    st.wSecond, 
    //    info);
    sprintf_s(buf, 2048, info);
    if (log == 0)
    {
        printf("%s", buf);
    }
    else {
        LogState state = GetExistedLog(log);
        if (state.path)
        {
            fopen_s(&state.file, state.path, "a+");
            if (state.file) {
                fwrite(buf, strlen(buf), 1, state.file);
                fflush(state.file);
                fclose(state.file);
                for (int i = 0; i < strlen(buf); i++)
                {
                    if (buf[i] == '\n')
                        g_curLine++;
                }
            }
        }
    }
}

void TraceInfo(log_t handle, const char* formatString, ...)
{
    char msg[MaxMsgLen] = { 0 };
    char tmp[MaxMsgLen] = { 0 };
    va_list ap;
    va_start(ap, formatString);
    _vsnprintf_s(tmp, MaxMsgLen - 1, formatString, ap);
    va_end(ap);

    sprintf_s(msg, MaxMsgLen - 1, "%s", tmp);
    TraceLog(handle, msg);
}

void TraceWarn(log_t handle, const char* formatString, ...)
{
    char msg[MaxMsgLen] = { 0 };
    char tmp[MaxMsgLen] = { 0 };
    va_list ap;
    va_start(ap, formatString);
    _vsnprintf_s(tmp, MaxMsgLen - 1, formatString, ap);
    va_end(ap);

    sprintf_s(msg, MaxMsgLen - 1, "%s", tmp);
    TraceLog(handle, msg);
}

void TraceDebug(log_t handle, const char* formatString, ...)
{
    char msg[MaxMsgLen] = { 0 };
    char tmp[MaxMsgLen] = { 0 };
    va_list ap;
    va_start(ap, formatString);
    _vsnprintf_s(tmp, MaxMsgLen - 1, formatString, ap);
    va_end(ap);

    sprintf_s(msg, MaxMsgLen - 1, "%s", tmp);
    TraceLog(handle, msg);
}

void TraceError(log_t handle, const char* formatString, ...)
{
    char msg[MaxMsgLen] = { 0 };
    char tmp[MaxMsgLen] = { 0 };
    va_list ap;
    va_start(ap, formatString);
    _vsnprintf_s(tmp, MaxMsgLen - 1, formatString, ap);
    va_end(ap);

    sprintf_s(msg, MaxMsgLen - 1, "%s", tmp);
    TraceLog(handle, msg);
}


#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>

using namespace std;

//日志输出等级
enum LOG_LEVEL
{
	LEVEL_NO = 0,
	LEVEL_ERROR,
	LEVEL_WARNING,
	LEVEL_INFO,
	LEVEL_DEBUG,
};

//日志输出方式

#define OUT_CONSOLE  1
#define OUT_FILE 2


class SimpleLog
{
public:
	static SimpleLog* GetInstance(LOG_LEVEL level=LEVEL_DEBUG, int outputType = OUT_CONSOLE | OUT_FILE);
	void SetLogLevel(LOG_LEVEL level);
	void SetLogOutputType(int outputType);
	void Info(char* szFormatString, ...);
	void Debug(char* szFormatString, ...);
	void Warning(char* szFormatString, ...);
	void Error(char* szFormatString, ...);
    vector<string> GetLog() { return m_vecLog; }
    vector<string> GetInfoLog() { return m_vecInfoLog; }
    vector<string> GetWarningLog() { return m_vecWarningLog; }
    vector<string> GetDebugLog() { return m_vecDebugLog; }
    vector<string> GetErrorLog() { return m_vecErrorLog; }
private:
	void WriteLog(char* szLogHeader, char* szFormatString);

private:
	SimpleLog(LOG_LEVEL level, int outputType);
	~SimpleLog();
	SimpleLog(const SimpleLog &log);
	SimpleLog& operator=(const SimpleLog &log) = default;
	static SimpleLog* m_log;
	FILE* m_file;
	LOG_LEVEL m_level;
	int m_outputType;
    vector<string> m_vecLog;
    vector<string> m_vecInfoLog;
    vector<string> m_vecWarningLog;
    vector<string> m_vecDebugLog;
    vector<string> m_vecErrorLog;
};

#define Log SimpleLog::GetInstance() 

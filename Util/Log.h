#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "../../DuiLib_R/UIlib.h"
using namespace std;
using namespace DuiLib;
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
#define OUT_CONTROL 4


class SimpleLog
{
public:
	static SimpleLog* GetInstance(LOG_LEVEL level=LEVEL_DEBUG, int outputType = OUT_CONTROL | OUT_FILE);
    void SetLogControl(CRichEditUI* pControl) { m_pControl = pControl; }
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
	SimpleLog(LOG_LEVEL level, int outputType);     //日志为单例类
	~SimpleLog();
	SimpleLog(const SimpleLog &log);
	SimpleLog& operator=(const SimpleLog &log) = default;
	static SimpleLog* m_log;
	FILE*           m_file;             //文件句柄
	LOG_LEVEL       m_level;            //日志等级
	int             m_outputType;       //输出方式
    vector<string>  m_vecLog;           //保存所有日志到内存中
    vector<string>  m_vecInfoLog;       //保存所有Info日志
    vector<string>  m_vecWarningLog;    //保存Warning日志
    vector<string>  m_vecDebugLog;      //保存Debug日志
    vector<string>  m_vecErrorLog;      //保存Error日志

    CRichEditUI*     m_pControl;         //将日志输出到该控件上
};

#define Log SimpleLog::GetInstance() 

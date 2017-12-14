#include "stdafx.h"
#include "Log.h"
#include <time.h>

#define LOG_INFO		_T("Info")
#define LOG_DEBUG		_T("Debug")
#define LOG_WARNING		_T("Warning")
#define LOG_ERROR		_T("Error")

static void FormatCurrentTime(char * buf,int len)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf_s(buf,len, _T("%.4d-%.2d-%.2d  %.2d:%.2d:%.2d"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);

	return ;
}

SimpleLog* SimpleLog::m_log = nullptr;


SimpleLog::SimpleLog(LOG_LEVEL level, int outputType)
{
	char szDir[256] = { 0 };
	char szFilePath[256] = { 0 };
	if (outputType & OUT_FILE)
	{
		GetCurrentDirectoryA(255, szDir);
		sprintf_s(szFilePath, 255, _T("%s\\simple.log"), szDir);
		fopen_s(&m_file, szFilePath, _T("a+"));
	}

	m_level = level;
	m_outputType = outputType;
}

SimpleLog::~SimpleLog()
{
	if (m_file)
		fclose(m_file);
	if (m_log)
		delete m_log;
}

SimpleLog* SimpleLog::GetInstance(LOG_LEVEL level, int outputType)
{
	if (m_log == nullptr)
	{
		m_log = new SimpleLog(level, outputType);
	}

	return m_log;
}

void SimpleLog::SetLogLevel(LOG_LEVEL level)
{
	m_level = level;
}

void SimpleLog::SetLogOutputType(int outputType)
{
	m_outputType = outputType;
}

void SimpleLog::Info(char* szFormatString, ...)
{
	if (m_level < LOG_LEVEL::LEVEL_INFO)
		return;

	char szMessage[2048] = { 0 };
	
	va_list ap;
	va_start(ap, szFormatString);
	_vsnprintf_s(szMessage, 2048, szFormatString, ap);
	va_end(ap);

	WriteLog(LOG_INFO, szMessage);
}

void SimpleLog::Debug(char* szFormatString, ...)
{
	if (m_level < LOG_LEVEL::LEVEL_DEBUG)
		return;

	char szMessage[2048] = { 0 };

	va_list ap;
	va_start(ap, szFormatString);
	_vsnprintf_s(szMessage, 2048, szFormatString, ap);
	va_end(ap);

	WriteLog(LOG_DEBUG, szMessage);
}

void SimpleLog::Warning(char* szFormatString, ...)
{
	if (m_level < LOG_LEVEL::LEVEL_WARNING)
		return;

	char szMessage[2048] = { 0 };

	va_list ap;
	va_start(ap, szFormatString);
	_vsnprintf_s(szMessage, 2048, szFormatString, ap);
	va_end(ap);

	WriteLog(LOG_WARNING, szMessage);
}

void SimpleLog::Error(char* szFormatString, ...)
{
	if (m_level < LOG_LEVEL::LEVEL_ERROR)
		return;

	char szMessage[2048] = { 0 };

	va_list ap;
	va_start(ap, szFormatString);
	_vsnprintf_s(szMessage, 2048, szFormatString, ap);
	va_end(ap);

	WriteLog(LOG_ERROR, szMessage);
}

void SimpleLog::WriteLog(char* szLogHeader, char* szFormatString)
{
	char szTime[128] = { 0 };
	char szMessage[2048] = { 0 };

	FormatCurrentTime(szTime, sizeof(szTime));
	sprintf_s(szMessage, sizeof(szMessage), _T("%s [%s]:   %s\n"), szTime, szLogHeader, szFormatString);

	if (m_outputType & OUT_FILE)
	{
		if (m_file)
		{
			fwrite(szMessage, strlen(szMessage), 1, m_file);
		}
		fflush(m_file);
	}
	if(m_outputType & OUT_CONSOLE) 
	{
		printf(szMessage);
	}
    if (m_outputType & OUT_CONTROL && m_pControl)
    {
        m_pControl->AppendText(szMessage);
    }

    if(string(szLogHeader) == LOG_INFO)     m_vecInfoLog.push_back(szMessage);
    if (string(szLogHeader) == LOG_WARNING)  m_vecWarningLog.push_back(szMessage);
    if (string(szLogHeader) == LOG_DEBUG)    m_vecDebugLog.push_back(szMessage);
    if (string(szLogHeader) == LOG_ERROR)    m_vecErrorLog.push_back(szMessage);

    m_vecLog.push_back(szMessage);
}
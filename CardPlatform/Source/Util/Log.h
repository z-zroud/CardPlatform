#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "../../DuiLib_R/UIlib.h"
using namespace std;
using namespace DuiLib;
//��־����ȼ�
enum LOG_LEVEL
{
	LEVEL_NO = 0,
	LEVEL_ERROR,
	LEVEL_WARNING,
	LEVEL_INFO,
	LEVEL_DEBUG,
};

//��־�����ʽ

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
	SimpleLog(LOG_LEVEL level, int outputType);     //��־Ϊ������
	~SimpleLog();
	SimpleLog(const SimpleLog &log);
	SimpleLog& operator=(const SimpleLog &log) = default;
	static SimpleLog* m_log;
	FILE*           m_file;             //�ļ����
	LOG_LEVEL       m_level;            //��־�ȼ�
	int             m_outputType;       //�����ʽ
    vector<string>  m_vecLog;           //����������־���ڴ���
    vector<string>  m_vecInfoLog;       //��������Info��־
    vector<string>  m_vecWarningLog;    //����Warning��־
    vector<string>  m_vecDebugLog;      //����Debug��־
    vector<string>  m_vecErrorLog;      //����Error��־

    CRichEditUI*     m_pControl;         //����־������ÿؼ���
};

#define Log SimpleLog::GetInstance() 

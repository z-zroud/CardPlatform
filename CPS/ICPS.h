#pragma once

/*******************************************************
* ���dll��ʵ�ֵĽӿڣ�ͳһ�Ľӿ�����Ϊ��
* bool HandleDp(const char* fileName);
********************************************************/
typedef bool(*PHandleDpCallback)(const char* fileName);

/**********************************************************
* �����dll����ӿ�
***********************************************************/
#ifdef CPS_EXPORTS
#define CPS_API __declspec(dllexport)
#else
#define CPS_API __declspec(dllimport)
#endif

/******************************************************
* ����ͳһCPS�ļ�,���ڸ��˻�
* ����˵���� szDllName ���ڴ���DP�ļ���Dll����
* ����˵���� szFileName DP�ļ�·��
* ����ֵ�� DP�ļ�����ɹ�����true, ���򷵻�false
*******************************************************/
extern "C" bool GenCpsFile(const char* szDllName, const char* szFuncName, const char* szFileName);

/******************************************************
* ͨ��CPS�ļ�����ɿ�Ƭ���˻�
*******************************************************/
extern "C" bool DoPersonlization(const char* szCpsFile,const char* iniConfigFile);

extern "C" bool SetPersonlizationConfig(const char* kmc, int divMethod, int secureLevel);
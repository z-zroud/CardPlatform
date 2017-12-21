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

/**********************************************************
* ����ͳһCPS�ļ�,���ڸ��˻�
* ����˵���� szDllName ���ڴ���DP�ļ���Dll����
* ����˵���� szFuncName ���õ�����dll����DP�ļ��ĺ����ӿ�����
* ����˵���� szFileName DP�ļ�·��
* ����ֵ�� DP�ļ�����ɹ�����true, ���򷵻�false
***********************************************************/
extern "C" CPS_API bool GenCpsFile(const char* szDllName, const char* szFuncName, const char* szFileName);


/**********************************************************
* ���ø��˻�ʱ��KMC��Ϣ
***********************************************************/
extern "C" CPS_API void SetPersonlizationConfig(const char* isd, const char* kmc, int divMethod, int secureLevel);

/******************************************************
* ͨ��CPS�ļ�����ɿ�Ƭ���˻�
* ������ szCpsFile CPS�ļ�·��
* ������ iniConfigFile ���˻�������Ϣ(���Ӧ�ò�������)
*******************************************************/
extern "C" CPS_API bool DoPersonlization(const char* szCpsFile, const char* iniConfigFile);

/******************************************************
* ͨ��CPS�ļ�����ɿ�Ƭ���˻�
* ������ szCpsFile CPS�ļ�·��
* ������ iniConfigFile ���˻�������Ϣ(���Ӧ�ò�������)
* ע�⣺��CPS�ļ�û��ϸ��DGI�е�tagֵ�����ǽ���Щtagƴ��֮���
*      �ַ�����ΪDGI��ֵ
*******************************************************/
extern "C" CPS_API int DoPersonlizationWithOrderdDGI(const char* szCpsFile, const char* iniConfigFile);
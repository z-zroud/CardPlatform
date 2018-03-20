#pragma once

/*******************************************************
* 外界dll需实现的接口，统一的接口名称为：
* bool HandleDp(const char* fileName);
********************************************************/
typedef bool(*PHandleDpCallback)(const char* dpFile,const char* ruleFile, char** cpsFile, int& count);

/**********************************************************
* 定义该dll对外接口
***********************************************************/
#ifdef CPS_EXPORTS
#define CPS_API __declspec(dllexport)
#else
#define CPS_API __declspec(dllimport)
#endif

/**********************************************************
* 生成统一CPS文件,便于个人化
* 参数说明： szDllName 用于处理DP文件的Dll名称
* 参数说明： szFuncName 调用第三方dll解析DP文件的函数接口名称
* 参数说明： szFileName DP文件路径
* 返回值： DP文件处理成功返回true, 否则返回false
***********************************************************/
extern "C" CPS_API bool GenCpsFile(const char* szDllName, const char* szFuncName, const char* szDpFile, const char* szRuleFile);

extern "C" CPS_API void GetCpsFiles(char** cpsFiles, int& count);

/**********************************************************
* 设置个人化时的KMC信息
***********************************************************/
extern "C" CPS_API void SetPersonlizationConfig(const char* isd, const char* kmc, int divMethod, int secureLevel);

/******************************************************
* 通过CPS文件，完成卡片个人化
* 参数： szCpsFile CPS文件路径
* 参数： iniConfigFile 个人化配置信息(相关应用参数配置)
*******************************************************/
extern "C" CPS_API bool DoPersonlization(const char* szCpsFile, const char* iniConfigFile);

/******************************************************
* 通过CPS文件，完成卡片个人化
* 参数： szCpsFile CPS文件路径
* 参数： iniConfigFile 个人化配置信息(相关应用参数配置)
* 注意：此CPS文件没有细分DGI中的tag值，而是将这些tag拼接之后的
*      字符串作为DGI的值
*******************************************************/
extern "C" CPS_API int DoPersonlizationWithOrderdDGI(const char* szCpsFile, const char* iniConfigFile);
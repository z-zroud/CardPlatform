#pragma once

/*******************************************************
* 外界dll需实现的接口，统一的接口名称为：
* bool HandleDp(const char* fileName);
********************************************************/
typedef bool(*PHandleDpCallback)(const char* fileName);

/**********************************************************
* 定义该dll对外接口
***********************************************************/
#ifdef CPS_EXPORTS
#define CPS_API __declspec(dllexport)
#else
#define CPS_API __declspec(dllimport)
#endif

/******************************************************
* 生成统一CPS文件,便于个人化
* 参数说明： szDllName 用于处理DP文件的Dll名称
* 参数说明： szFileName DP文件路径
* 返回值： DP文件处理成功返回true, 否则返回false
*******************************************************/
extern "C" CPS_API bool GenCpsFile(const char* szDllName, const char* szFuncName, const char* szFileName);

/******************************************************
* 通过CPS文件，完成卡片个人化
*******************************************************/
extern "C" CPS_API bool DoPersonlization(const char* szCpsFile,const char* iniConfigFile);

extern "C" CPS_API void SetPersonlizationConfig(const char* kmc, int divMethod, int secureLevel);
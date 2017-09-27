#pragma once
#ifdef PCSC_EXPORTS
#define PCSC_API __declspec(dllexport)
#else
#define PCSC_API __declspec(dllimport)
#endif

/******************************************************
* 本动态库定义了与PCSC读卡器相关的接口
*******************************************************/

/*****************************************************
* 功能： 获取读卡器所有名称
* 参数： readers   存储读卡器名称数组
* 参数： count 读卡器接口个数
******************************************************/
extern "C" void GetReaders(char** readers, int count);

/****************************************************
* 功能： 热复位，SCARD_LEAVE_CARD
*****************************************************/
extern "C" void WarmReset();

/****************************************************
* 功能： 冷复位，SCARD_UNPOWER_CARD 断电复位
*****************************************************/
extern "C" void ColdReset();

/***************************************************
****************************************************/
extern "C" bool OpenReader(const char* reader);
extern "C" void CloseReader();
extern "C" void GetATR(char* atr, int len);
extern "C" int  GetCardStatus();
extern "C" int  GetTransProtocol();


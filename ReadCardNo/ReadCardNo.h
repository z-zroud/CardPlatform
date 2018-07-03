#pragma once

#ifdef READCARDNO_EXPORTS
#define READCARDNO_API __declspec(dllexport)
#else
#define READCARDNO_API __declspec(dllimport)
#endif



/**********************************************************
* 功能：读取卡号信息
* 参数：aid	应用实例，借记卡:A000000333010101 贷记卡:A000000333010102
sfi 短文件标识符 指明卡号所在文件， 一般写 2
recordNo 记录号，指明卡号所在记录，一般填 1
cardNo 存储接口返回的卡片信息
cardNoLen cardNo长度
* 返回：0表示成功，非0表示失败
***********************************************************/
extern "C" READCARDNO_API int ReadCardNo(const char* aid, int sfi, int recordNo, char* cardNo, int cardNoLen);

/**********************************************************
* 功能：获取读卡器列表
* 参数：readers 存储接口返回的读卡器列表
*		count 接收的读卡器个数
* 返回：0表示成功，非0表示失败
***********************************************************/
extern "C" READCARDNO_API int GetReadersEx(char** readers, int& count);

/**********************************************************
* 功能：打开读卡器
* 参数：readerName 读卡器名称
* 返回：0表示成功，非0表示失败
***********************************************************/
extern "C" READCARDNO_API int OpenReaderEx(char* readerName);
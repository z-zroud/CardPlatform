#pragma once
#include <string>

using namespace std;

#ifdef APDUCMD_EXPORTS
#define APDUCMD_API __declspec(dllexport)
#else
#define APDUCMD_API __declspec(dllimport)
#endif

/****************************************************
* 该接口定义了所有APDU指令集，符合PBOC3.0规范
*****************************************************/
#define APDU_OK		0x9000
#define APDU_LEN	1024
#define RESP_LEN	1024
#define KEY_LEN		33
#define STORE_DATA_PLANT	0
#define STORE_DATA_ENCRYPT	1
#define STORE_DATA_END		2

#define DIV_NONE	0
#define DIV_CPG202	1
#define DIV_CPG212	2

#define SECURE_NONE	0
#define SECURE_MAC	1

#define AAC			0x00
#define ARQC		0x80
#define TC			0x40
#define CDA			0x10


#ifdef __cplusplus
extern "C"
{
#endif
APDUCMD_API bool OpenSecureChannel(const char* kmc, int divMethod, int secureLevel = 0);
APDUCMD_API bool SetKmc(const char* kmc, int divMethod);

/**************************************************
* 通过AID选择应用
***************************************************/
APDUCMD_API	UINT SelectAppCmd(const char* aid, char* resp);

/***********************************************************
* 终端发送GPO命令
* 参数： terminalData 由Tag9F38指定的终端数据拼接而成
* 参数： resp 卡片返回的数据域
* 返回： 成功返回 0x9000 其他值表示失败
************************************************************/
APDUCMD_API UINT GPOCmd(const char* terminalData, char* resp);

/**************************************************
* 读取记录文件信息
* 参数： sfi 读取记录文件的短文件标识
* 参数： recordNum 记录号
* 参数： resp APDU返回的数据域，不包括SW状态码
* 返回： 成功返回 0x9000 其他值表示失败
***************************************************/
APDUCMD_API UINT ReadRecordCmd(int sfi, int recordNum, char* resp);

/******************************************************************************
* 设置tag值命令
* 参数： tag 要修改的数据对象的标签
* 参数： value 数据对象的新值
* 参数： mac 由MAC session获得MAC校验码
* 返回： 成功返回 0x9000 其他值表示失败
*******************************************************************************/
APDUCMD_API UINT PutDataCmd(const char* tag, const char* value, const char* mac);

/**************************************************
* 通过AID删除应用
* 参数： aid 被删除的应用的AID
***************************************************/
APDUCMD_API	UINT DeleteAppCmd(const char* aid);
APDUCMD_API	UINT StoreDataCmd(const char* data, int type, bool reset = false);
APDUCMD_API	UINT InitializeUpdateCmd(const char* random, char* resp);

/*************************************************************************
* 内部认证命令，终端发送该命令用来验证卡片的合法性
* 参数： ddolData 即Tag9F49指定的终端数据
* 参数： resp 卡片生成的动态数据签名 即Tag9F4B
**************************************************************************/
APDUCMD_API UINT InternalAuthencationCmd(const char* ddolData, char* resp);

/*************************************************************************
* 外部认证命令
**************************************************************************/
APDUCMD_API UINT ExternalAuthencationCmd(const char* arpc, const char* authCode, char* resp);
/***************************************************************************
* 生成应用密文，终端传送交易相关数据到IC卡中，IC卡计算并返回一个密文
* 参数：terminalCryptogramType 终端请求的密文类型 AAC/ARQC/TC
* 参数：cdolData CDOL指定的终端数据
* 参数：resp 生成的应用密文
****************************************************************************/
APDUCMD_API UINT GACCmd(int terminalCryptogramType, const char* cdolData, char* resp);

/****************************************************************
* 卡片生成动态数据，终端通过该数据使用IC卡公钥
*****************************************************************/
APDUCMD_API void GenDynamicData(const char* ddolData, char* resp);

//获取打开安全通道之后的会话密钥，调用前，需调用OpenSecureChannel来打开安全通道
APDUCMD_API	void GetScureChannelSessionAuthKey(char* scureChannelSessionAuthKey);
APDUCMD_API	void GetScureChannelSessionMacKey(char* scureChannelSessionMacKey);
APDUCMD_API	void GetScureChannelSessionEncKey(char* scureChannelSessionEncKey);
APDUCMD_API	void GetScureChannelSessionKekKey(char* scureChannelSessionKekKey);

#ifdef __cplusplus
}
#endif

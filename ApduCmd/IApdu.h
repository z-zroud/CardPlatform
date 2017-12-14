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
#define APDU_LEN	2048
#define KEY_LEN		33
#define STORE_DATA_PLANT	0
#define STORE_DATA_ENCRYPT	1
#define STORE_DATA_END		2

#define DIV_NONE	0
#define DIV_CPG202	1
#define DIV_CPG212	2

#define SECURE_NONE	0
#define SECURE_MAC	1



#ifdef __cplusplus
extern "C"
{
#endif
APDUCMD_API bool OpenSecureChannel(const char* kmc, int divMethod, int secureLevel = 0);
APDUCMD_API bool SetKmc(const char* kmc, int divMethod);
APDUCMD_API	UINT SelectAppCmd(const char* aid, char* resp);
APDUCMD_API UINT ReadRecordCmd(int sfi, int recordNum);
APDUCMD_API	UINT DeleteAppCmd(const char* aid);
APDUCMD_API	UINT StoreDataCmd(const char* data, int type, bool reset = false);
APDUCMD_API	UINT InitializeUpdateCmd(const char* random, char* resp);

	//获取打开安全通道之后的会话密钥，调用前，需调用OpenSecureChannel来打开安全通道
APDUCMD_API	void GetScureChannelSessionAuthKey(char* scureChannelSessionAuthKey);
APDUCMD_API	void GetScureChannelSessionMacKey(char* scureChannelSessionMacKey);
APDUCMD_API	void GetScureChannelSessionEncKey(char* scureChannelSessionEncKey);
APDUCMD_API	void GetScureChannelSessionKekKey(char* scureChannelSessionKekKey);

#ifdef __cplusplus
}
#endif

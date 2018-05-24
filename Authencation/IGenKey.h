#pragma once
#include "CardBase.h"
#include <string>
#include <vector>
using namespace std;

#ifdef GENKEY_EXPORTS
#define GENKEY_API __declspec(dllexport)
#else
#define GENKEY_API __declspec(dllimport)
#endif

const int DES_KEY = 0;
const int SM_KEY = 1;



/********************************************************************************
* 功能： 生成秘钥的KCV校验值
* 参数： key 秘钥
*********************************************************************************/
extern "C" GENKEY_API int GenDesKcv(const char* key, char* kcv, int kcvLen);

extern "C" GENKEY_API int GenSmKcv(const char* key, char* kcv, int kcvLen);

/********************************************************************************
* 功能：从数据库(sqlite)中获取CA公钥
* 参数：caIndex: 公钥索引
* 参数：rid 和caIndex 获取唯一的CA公钥
*********************************************************************************/
extern "C" GENKEY_API int GenCAPublicKey(const char* caIndex, const char* rid, char* caPublicKey);

/********************************************************************************
* 功能： 获取发卡行公钥
* 参数： issuerPulicCert 发卡行公钥证书
*		ipkRemainder 发卡行公钥余项
*		type
*********************************************************************************/
extern "C" GENKEY_API  int GenDesIssuerPublicKey(
	const char* caPublicKey, 
	const char* issuerPublicCert, 
	const char* ipkRemainder, 
	const char* issuerExponent,
	char* issuerPublicKey);

/********************************************************************************
* 功能： 获取IC卡公钥
* 参数： issuerPulicCert 发卡行公钥证书
* 参数： iccPublicCert IC卡公钥证书
* 参数： iccRemainder IC卡公钥余项
* 参数： sigStaticData 即：读AFL时，需要签名的数据
* 参数： iccExponent IC卡公钥指数
* 返回： 发卡行公钥
*********************************************************************************/
extern "C" GENKEY_API  int GenDesICCPublicKey(
	const char* issuerPublicKey,
	const char* iccPublicCert,
	const char* iccRemainder,
	const char* sigStaticData,
	const char* iccExponent,
	const char* tag82,
	char* iccPublicKey);

/********************************************************************************
* 功能： 获取发卡行公钥
* 参数： issuerPulicCert 发卡行公钥证书
*		ipkRemainder 发卡行公钥余项
*       PAN 账号
* 返回：	发卡行公钥
*********************************************************************************/
extern "C" GENKEY_API  int GenSMIssuerPublicKey(
	const char* caPublicKey, 
	const char* issuerPublicCert,
    const char* PAN,
	char* issuerPublicKey);

/********************************************************************************
* 功能： 获取发卡行公钥
* 参数： issuerPulicCert 发卡行公钥证书
*		ipkRemainder 发卡行公钥余项
*		type
*********************************************************************************/
extern "C" GENKEY_API  int GenSMICCPublicKey(
	const char* issuerPublicKey,
	const char* iccPublicCert,
	const char* needAuthStaticData,
    const char* tag82,
    const char* PAN,
	char* iccPublicKey);

/********************************************************************************
* 功能： 脱机数据认证 SDA
* 参数： issuerPublicKey 发卡行公钥
* 参数： tag93 签名的静态应用数据 (用发卡行私钥签名的数据，在SDA过程中由终端验证)
* 参数： tag82 AIP(应用交互特征)
* 返回： 0 表示SDA验证成功，非零表示验证失败
*********************************************************************************/
extern "C" GENKEY_API int DES_SDA(const char* issuerPublicKey, 
	const char*ipkExponent,  
	const char* tag93, 
	const char* sigStaticData,
	const char* tag82);
extern "C" GENKEY_API int SM_SDA(const char* issuerPublicKey, const char* toBeSignedStaticAppData, const char* tag93, const char* tag82);

extern "C" GENKEY_API int DES_DDA(const char* iccPublicKey, 
	const char*iccExponent,
	const char* tag9F4B,
	const char* dynamicData);
extern "C" GENKEY_API int SM_DDA(const char* iccPublicKey, const char* tag9F4B, const char* dynamicData);

/********************************************************************************
* 通过UDK_AC  UDK_MAC  UDK_ENC 生成对应的会话密钥
* 参数udkSubKey 可以传入UDK_AC/UDK_MAC/UDK_ENC 生成对应的sessionKey
* 参数atc 应用交易计数器
*********************************************************************************/
extern "C" GENKEY_API void GenUdkSessionKey(const char* udkSubKey, const char* atc, char* udkSessionKey, int keyType=0);

/********************************************************************************
* 通过对应的MDK_AC MDK_MAC MDK_ENC 生成对应的UDK_AC UDK_MAC  UDK_ENC
* 参数 mdk 可以传入MDK_AC/MDK_MAC/MDK_ENC 生成对应的UDK_AC/UDK_MAC/UDK_ENC
* 参数 cardNo 卡号
* 参数cardSequence 卡片序列号
*********************************************************************************/
extern "C" GENKEY_API void GenUdk(const char* mdk,const char* cardNo,const char* cardSequence,char* udk, int keyType = 0);

/********************************************************************************
* 生成ARPC
* 参数： udkAcSessionKey 为UDK_AC生成的会话密钥
* 参数：ac 应用密文
*********************************************************************************/
extern "C" GENKEY_API void GenArpc(const char* udkAuthSessionKey, char* ac, char* authCode, char* arpc, int keyType = 0);

/*******************************************************************************
* 生成发卡行脚本MAC
********************************************************************************/
extern "C" GENKEY_API void GenPBOCMac(const char* udkMacSessionKey, const char* data, char* mac, int keyType = 0);
extern "C" GENKEY_API void GenEMVMac(const char* udkAC, const char* data, char* mac);


extern "C" GENKEY_API void Decrypt_Des3_CBC(const char* key, const char* input, char* output, int outputLen);

extern "C" GENKEY_API int GenHash(const char* input, char* hash, int len);
extern "C" GENKEY_API int GenSMHash(const char* input, const char* publicKey, char* hash, int len);
extern "C" GENKEY_API int DES_GenRecovery(const char* publicKey, const char* publicKeyExp, const char* encryptionData, char* recoveryData, int len);

extern "C" GENKEY_API int SM2Verify(const char *pPublicKey, const char *pMSG, const char *pSignData);

extern "C" GENKEY_API  int GenSMHash2(char *input, int inlen, char *output);
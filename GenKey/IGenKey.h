#pragma once
#include "CardBase.h"
#include <string>
#include <vector>
using namespace std;

struct IGenKey
{
	/*****************************************************************
	* 功能： 生成KMC子密钥
	* 参数： divData 分散数据，由InitUpdate命令返回值提供
	*		divFlag 分散方法
	*		acKey	K_DEK 数据加密密钥(用于敏感数据的加密)
	*		macKey	K_MAC 安全通道消息认证码密钥(安全通道MAC校验)
	*		encKey	K_ENC 安全通道加密密钥(安全通道的认证和加密)
	* 描述： K_MAC和K_ENC仅仅用于在安全通道初始化过程中生成安全通道会话密钥
	******************************************************************/
	virtual void GenSubKey(const string kmc,	//获取所有KMC子密钥
		string divData,
		DIV_METHOD_FLAG divFlag,
		string &K_DEK,
		string &K_MAC,
		string &K_ENC) = 0;

	virtual string GenK_DEK(const string kmc, string divData, DIV_METHOD_FLAG divFlag) = 0;
	virtual string GenK_MAC(const string kmc, string divData, DIV_METHOD_FLAG divFlag) = 0;
	virtual string GenK_ENC(const string kmc, string divData, DIV_METHOD_FLAG divFlag) = 0;

	/********************************************************************
	* 功能： 生成子密钥会话密钥
	*********************************************************************/
	virtual void GenSessionKey(string cardRandom, 
		string termRandom, 
		int scp,
		string K_DEK,
		string K_MAC,
		string K_ENC,
		string &sessionDEK,
		string &sessionMAC,
		string &sessionENC) = 0;

	virtual string GenSessionDEK(string K_DEK, string cardRandom, string termRandom, int scp) = 0;
	virtual string GenSessionMAC(string K_MAC, string cardRandom, string termRandom, int scp) = 0;
	virtual string GenSessionENC(string K_ENC, string cardRandom, string termRandom, int scp) = 0;
	/********************************************************************************
	* 功能：生成会话密钥
	* 参数：udkAuth: 过程密钥
	*		atc:	卡片计数器
	* 返回： 会话密钥
	*********************************************************************************/
	virtual string GenSUDK_DEK(const string sudkAuth, const string ATC) = 0;		
	
	/********************************************************************************
	*********************************************************************************/																						
	virtual string GenUDK_DEK(const string mdkAuth, const string ATC, const string account, const string cardSeq) = 0;	//获取UDK_Auth

	/********************************************************************************
	* 功能：获取CA公钥
	* 参数：index 公钥索引
	*		rid	公钥ID
	* 返回： 公钥
	*********************************************************************************/
	virtual string GenCAPublicKey(const string caIndex, string rid) = 0; //获取CA公钥

	virtual void GetCARid(vector<char*> &rid) = 0;
	virtual void GetCAIndex(char* rid, vector<char*>& index) = 0;

	/********************************************************************************
	* 功能： 获取发卡行公钥
	* 参数： issuerPulicCert 发卡行公钥证书
	*		ipkRemainder 发卡行公钥余项
	*		type
	*********************************************************************************/
	virtual string GenDesIssuerPublicKey(const string caPublicKey, const string issuerPublicCert, const string ipkRemainder, const string issuerExponent = "03") = 0;

	/********************************************************************************
	* 功能： 获取发卡行公钥
	* 参数： issuerPulicCert 发卡行公钥证书
	*		ipkRemainder 发卡行公钥余项
	* 返回： 发卡行公钥
	*********************************************************************************/
	virtual string GenDesICCpublicKey(const string issuerPublicKey, const string iccPublicCert, const string iccRemainder, const string signedData, const string issuerExponent = "03") = 0;

	/********************************************************************************
	* 功能： 获取发卡行公钥
	* 参数： issuerPulicCert 发卡行公钥证书
	*		ipkRemainder 发卡行公钥余项
	* 返回：	发卡行公钥		
	*********************************************************************************/
	virtual string GenSMIssuerPublcKey(const string caPublicKey, const string issuerPublicCert) = 0;

	/********************************************************************************
	* 功能： 获取发卡行公钥
	* 参数： issuerPulicCert 发卡行公钥证书
	*		ipkRemainder 发卡行公钥余项
	*		type
	*********************************************************************************/
	virtual string GenSMICCpublicKey(const string issuerPublicKey, const string iccPublicCert, const string needAuthStaticData) = 0;

	/********************************************************************************
	* 功能： 获取发卡行公钥
	* 参数： issuerPulicCert 发卡行公钥证书
	*		ipkRemainder 发卡行公钥余项
	*		type
	*********************************************************************************/
	virtual string GenARPC1(string udkAuth, string AC, string authCode, string atc) = 0;

	/********************************************************************************
	* 功能： 获取发卡行公钥
	* 参数： issuerPulicCert 发卡行公钥证书
	*		ipkRemainder 发卡行公钥余项
	*		type
	*********************************************************************************/
	virtual string GenARPC2(string mdkAuth, string AC, string authCode, string atc, string cardSeq, string pan) = 0;

	/********************************************************************************
	* 功能： 获取发卡行公钥
	* 参数： issuerPulicCert 发卡行公钥证书
	*		ipkRemainder 发卡行公钥余项
	*		type
	*********************************************************************************/
	virtual string GenScriptMac(string mac, string atc, string data) = 0;
};

#ifdef GENKEY_EXPORTS
#define GENKEY_API __declspec(dllexport)
#else
#define GENKEY_API __declspec(dllimport)
#endif

extern "C" GENKEY_API IGenKey* GetGenKeyInstance();
extern "C" GENKEY_API void DeleteGenKeyInstance(IGenKey* obj);



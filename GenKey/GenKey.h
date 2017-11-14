#pragma once
#include "IGenKey.h"
#include "CardBase.h"
#include "Sqlite3\sqlite3.h"

//此结构体仅限于获取sql某列数据的集合
struct SqlParam
{
	char* colName;
	vector<char*> result;
};

#define DIV_NONE	0
#define DIV_CPG202	1
#define DIV_CPG212	2

#define SECURE_NONE	0
#define SECURE_MAC	1

//生成各类子密钥
class KeyGenerator : public IGenKey
{
public:
	/*****************************************************************
	* 功能： 生成KMC子密钥
	* 参数： divData 分散数据，由InitUpdate命令返回值提供
	*		divFlag 分散方法
	*		acKey	K_DEK 数据加密密钥(用于敏感数据的加密)
	*		macKey	K_MAC 安全通道消息认证码密钥(安全通道MAC校验)
	*		encKey	K_ENC 安全通道加密密钥(安全通道的认证和加密)
	* 描述： K_MAC和K_ENC仅仅用于在安全通道初始化过程中生成安全通道会话密钥
	******************************************************************/
	virtual void GenKmcSubKey(
		const string& kmc,
		int divMethod,
		const string& divData,
		string& kmcAuthKey,
		string& kmcMacKey,
		string& kmcEncKey);


	/********************************************************************
	* 功能： 生成子密钥会话密钥
	*********************************************************************/
	virtual void GenScureChannelSessionKey(
		const string kmc,
		int divMethod,
		const string& terminalRandom,
		const string& initialializeUpdateResp,
		string& sessionAuthKey,
		string& sessionMacKey,
		string& sessionEncKey,
		string& kekKey);

private:
	string GenKmcSubKey(const string& kmc,  string leftDivData,  string rightDivData);
	string GenKmcAuthKey(const string& kmc,  string leftDivData,  string rightDivData);
	string GenKmcMacKey(const string& kmc,  string leftDivData,  string rightDivData);
	string GenKmcEncKey(const string& kmc, string leftDivData, string rightDivData);

	string GenSecureChannelSessionKeyScp1(string kmcSubKey, string cardRandom, string termRandom);
	string GenSecureChannelSessionKey(const string& kmcSubKey, string leftDivData, string rightDivData);
	string GenSecureChannelSessionAuthKey(const string& AuthKey, string seqNo);
	string GenSecureChannelSessionMacKey(const string& macKey, string seqNo);
	string GenSecureChannelSessionEncKey(const string& encKey, string seqNo);


///////////////////////////////////////////////////////////////////////////////////////////////////////
public:	
	string GenSUDK_DEK(const string sudkAuth, const string ATC);		//获取SUDK_Auth
	string GenUDK_DEK(const string mdkAuth, const string ATC, const string account, const string cardSeq);	//获取UDK_Auth
	string GenARPC1(string udkAuth, string AC, string authCode, string atc);
	string GenARPC2(string mdkAuth, string AC, string authCode, string atc, string cardSeq, string pan);
	string EvenOddCheck(string input);	//奇偶校验
	string GenScriptMac(string mac, string atc, string data);

	string GenCAPublicKey(const string caIndex, string rid);
	void GetCARid(vector<char*> &ridList);
	void GetCAIndex(char* rid, vector<char*> &indexList);
	string GenIssuerPublicKey(const string caPublicKey,
		const string issuerPublicCert,
		const string ipkRemainder,
		ENCRYPT_TYPE type,
		const string issuerExponent = "03");
	string GenICCPublicKey(const string issuerPublicKey,
		const string iccPublicCert,
		const string iccRemainder,
		const string signedData,
		ENCRYPT_TYPE type,
		const string issuerExponent = "03");

	static int QueryResult(void *params, int columnNum, char **columnVal, char **columnName);
	static int QuerySingleCol(void *params, int columnNum, char **columnVal, char **columnName);

	string GenDesIssuerPublicKey(const string caPublicKey, const string issuerPublicCert, const string ipkRemainder, const string issuerExponent = "03");
	string GenDesICCpublicKey(const string issuerPublicKey, const string iccPublicCert, const string iccRemainder, const string signedData, const string issuerExponent = "03");
	string GenSMIssuerPublcKey(const string caPublicKey, const string issuerPublicCert);
	string GenSMICCpublicKey(const string issuerPublicKey, const string iccPublicCert, const string needAuthStaticData);


private:

	

	void QueryPcscTable(string sql, sqlite3_callback callbackFunc, SqlParam& sqlParam);
private:
	static string m_caPublicKey;
};




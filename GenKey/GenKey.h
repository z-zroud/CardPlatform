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

//生成各类子密钥
class KeyGenerator : public IGenKey
{
public:
	virtual void GenSubKey(const string kmc,	//获取所有KMC子密钥
		string divData,
		DIV_METHOD_FLAG divFlag,
		string &K_DEK,
		string &K_MAC,
		string &K_ENC);

	virtual string GenK_DEK(const string kmc, string divData, DIV_METHOD_FLAG divFlag);
	virtual string GenK_MAC(const string kmc, string divData, DIV_METHOD_FLAG divFlag);
	virtual string GenK_ENC(const string kmc, string divData, DIV_METHOD_FLAG divFlag);

	void GenSessionKey(string cardRandomNum, string termRandomNum, int nSCP,	//获取SKU会话密钥
		string K_DEK,
		string K_MAC,
		string K_ENC,
		string &sessionDEK,
		string &sessionMAC,
		string &sessionENC);

	virtual string GenSessionDEK(string K_DEK, string cardRandom, string termRandom, int scp);
	virtual string GenSessionMAC(string K_MAC, string cardRandom, string termRandom, int scp);
	virtual string GenSessionENC(string K_ENC, string cardRandom, string termRandom, int scp);

	
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
	string GenKey(const string strKMC, string leftDivFactor, string rightDivFactor);
	string GenSessionKeyScp1(string key, string cardRandom, string termRandom);
	string GenSessionKeyScp2(string key, const string leftDivFactor, const string rightDivFactor);
	void QueryPcscTable(string sql, sqlite3_callback callbackFunc, SqlParam& sqlParam);
private:
	static string m_caPublicKey;
};




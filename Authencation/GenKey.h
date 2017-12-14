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

////生成各类子密钥
//class KeyGenerator : public IGenKey
//{
//
//public:	
//	string GenSUDK_DEK(const string sudkAuth, const string ATC);		//获取SUDK_Auth
//	string GenUDK_DEK(const string mdkAuth, const string ATC, const string account, const string cardSeq);	//获取UDK_Auth
//	string GenARPC1(string udkAuth, string AC, string authCode, string atc);
//	string GenARPC2(string mdkAuth, string AC, string authCode, string atc, string cardSeq, string pan);
//	string EvenOddCheck(string input);	//奇偶校验
//	string GenScriptMac(string mac, string atc, string data);
//
//	string GenCAPublicKey(const string caIndex, string rid);
//	void GetCARid(vector<char*> &ridList);
//	void GetCAIndex(char* rid, vector<char*> &indexList);
//	string GenIssuerPublicKey(const string caPublicKey,
//		const string issuerPublicCert,
//		const string ipkRemainder,
//		ENCRYPT_TYPE type,
//		const string issuerExponent = "03");
//	string GenICCPublicKey(const string issuerPublicKey,
//		const string iccPublicCert,
//		const string iccRemainder,
//		const string signedData,
//		ENCRYPT_TYPE type,
//		const string issuerExponent = "03");
//
//	static int QueryResult(void *params, int columnNum, char **columnVal, char **columnName);
//	static int QuerySingleCol(void *params, int columnNum, char **columnVal, char **columnName);
//
//	string GenDesIssuerPublicKey(const string caPublicKey, const string issuerPublicCert, const string ipkRemainder, const string issuerExponent = "03");
//	string GenDesICCpublicKey(const string issuerPublicKey, const string iccPublicCert, const string iccRemainder, const string signedData, const string issuerExponent = "03");
//	string GenSMIssuerPublcKey(const string caPublicKey, const string issuerPublicCert);
//	string GenSMICCpublicKey(const string issuerPublicKey, const string iccPublicCert, const string needAuthStaticData);
//
//
//private:
//
//	
//
//	void QueryPcscTable(string sql, sqlite3_callback callbackFunc, SqlParam& sqlParam);
//private:
//	static string m_caPublicKey;
//};




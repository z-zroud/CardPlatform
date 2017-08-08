#pragma once
#include "Interface\IKeyGenerator.h"
#include "Interface\CardBase.h"
#include "Sqlite3\sqlite3.h"

//�˽ṹ������ڻ�ȡsqlĳ�����ݵļ���
struct SqlParam
{
    char* colName;  
    vector<char*> result;
};

//���ɸ�������Կ
class KeyGenerator : public IKeyGenerator
{
public:
	void GenAllSubKey(const string strKMC,	//��ȡ����KMC����Կ
		string divFactorData,
		DIV_METHOD_FLAG flag,
		string &strAuthKey,
		string &strMacKey,
		string &strEncKey);
	void GenAllSessionKey(string cardRandomNum, string termRandomNum, int nSCP,	//��ȡSKU�Ự��Կ
		string strAuthKey,
		string strMacKey,
		string strEncKey,
		string &strSessionAuthKey,
		string &strSessionMacKey,
		string &strSessionEncKey);
	string GenSUDKAuthFromUDKAuth(const string sudkAuth, const string ATC);		//��ȡSUDK_Auth
	string GenUDKAuthFromMDKAuth(const string mdkAuth, const string ATC, const string account, const string cardSeq);	//��ȡUDK_Auth
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

    string GenARPCByUdkAuth(string udkAuth, string AC, string authCode, string atc);
    string GenARPCByMdkAuth(string mdkAuth, string AC, string authCode, string atc, string cardSeq, string pan);

private:
	string GenAuthKey(const string strKMC, string partLeftDivFactor, string partRightDivFactor);
	string GenMacKey(const string strKMC, string partLeftDivFactor, string partRightDivFactor);
	string GenEncKey(const string strKMC, string partLeftDivFactor, string partRightDivFactor);
	string GenKey(const string strKMC, string leftDivFactor, string rightDivFactor);
	string GenSessionAuthKey(string seqNo, string strAuthKey);
	string GenSessionMacKey(string seqNo, string strMacKey);
	string GenSessionEncKey(string seqNo, string strEncKey);
	string GenSessionKey(const string leftDivFactor, const string rightDivFactor, string key);
	string EvenOddCheck(string input);	//��żУ��
    void QueryPcscTable(string sql, sqlite3_callback callbackFunc, SqlParam& sqlParam);

private:
    static string m_caPublicKey;
};


#pragma once
#include "IGenKey.h"
#include "CardBase.h"
#include "Sqlite3\sqlite3.h"

//�˽ṹ������ڻ�ȡsqlĳ�����ݵļ���
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

//���ɸ�������Կ
class KeyGenerator : public IGenKey
{
public:
	/*****************************************************************
	* ���ܣ� ����KMC����Կ
	* ������ divData ��ɢ���ݣ���InitUpdate�����ֵ�ṩ
	*		divFlag ��ɢ����
	*		acKey	K_DEK ���ݼ�����Կ(�����������ݵļ���)
	*		macKey	K_MAC ��ȫͨ����Ϣ��֤����Կ(��ȫͨ��MACУ��)
	*		encKey	K_ENC ��ȫͨ��������Կ(��ȫͨ������֤�ͼ���)
	* ������ K_MAC��K_ENC���������ڰ�ȫͨ����ʼ�����������ɰ�ȫͨ���Ự��Կ
	******************************************************************/
	virtual void GenKmcSubKey(
		const string& kmc,
		int divMethod,
		const string& divData,
		string& kmcAuthKey,
		string& kmcMacKey,
		string& kmcEncKey);


	/********************************************************************
	* ���ܣ� ��������Կ�Ự��Կ
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
	string GenSUDK_DEK(const string sudkAuth, const string ATC);		//��ȡSUDK_Auth
	string GenUDK_DEK(const string mdkAuth, const string ATC, const string account, const string cardSeq);	//��ȡUDK_Auth
	string GenARPC1(string udkAuth, string AC, string authCode, string atc);
	string GenARPC2(string mdkAuth, string AC, string authCode, string atc, string cardSeq, string pan);
	string EvenOddCheck(string input);	//��żУ��
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




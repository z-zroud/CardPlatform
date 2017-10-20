#include "stdafx.h"
#include "GenKey.h"
#include "Des0.h"
#include "SHA1.h"
#include "Sqlite3\sqlite3.h"
#include "SM.hpp"
#include <string>
#include <bitset>

using namespace std;
string KeyGenerator::m_caPublicKey = "";

/***************************************************************************/


bool SplitDivData(const string kmc,
	string divData,
	DIV_METHOD_FLAG divFlag,
	string& leftDivData,
	string& rightDivData)
{
	if (divData.length() != 20)
	{
		return false;
	}
	switch (divFlag)
	{
	case DIV_CPG202:
		leftDivData = divData.substr(0, 4) + divData.substr(8, 8);
		rightDivData = leftDivData;
		break;
	case DIV_CPG212:
		leftDivData = divData.substr(8, 12);
		rightDivData = leftDivData;
		break;
	default:
		//m_Error = "未知的分散方法";
		return false;
	}

	return true;
}

string KeyGenerator::GenK_DEK(const string kmc, string divData, DIV_METHOD_FLAG divFlag)
{
	string leftDivData, rightDivData;
	if (!SplitDivData(kmc, divData, divFlag, leftDivData, rightDivData)) {
		return "";
	}
	leftDivData += string("F001");
	rightDivData += string("0F01");

	return GenKey(kmc, leftDivData, rightDivData);
}

string KeyGenerator::GenK_MAC(const string kmc, string divData, DIV_METHOD_FLAG divFlag)
{
	string leftDivData, rightDivData;
	if (!SplitDivData(kmc, divData, divFlag, leftDivData, rightDivData)) {
		return "";
	}
	leftDivData += string("F002");
	rightDivData += string("0F02");

	return GenKey(kmc, leftDivData, rightDivData);
}

string KeyGenerator::GenK_ENC(const string kmc, string divData, DIV_METHOD_FLAG divFlag)
{
	string leftDivData, rightDivData;
	if (!SplitDivData(kmc, divData, divFlag, leftDivData, rightDivData)) {
		return "";
	}
	leftDivData += string("F003");
	rightDivData += string("0F03");

	return GenKey(kmc, leftDivData, rightDivData);
}

//生成 子密钥
void KeyGenerator::GenSubKey(const string kmc,
	string divData,
	DIV_METHOD_FLAG divFlag,
	string &K_DEK,
	string &K_MAC,
	string &K_ENC)
{
	string leftDivData, rightDivData;

	if (divFlag == NO_DIV)
	{
		K_DEK = kmc;
		K_MAC = kmc;
		K_ENC = kmc;
	}
	else {
		K_DEK = GenK_DEK(kmc, divData, divFlag);
		K_MAC = GenK_MAC(kmc, divData, divFlag);
		K_ENC = GenK_ENC(kmc, divData, divFlag);
	}
}


string KeyGenerator::GenKey(const string kmc, string leftDivData, string rightDivData)
{
	char leftSubKey[17] = { 0 };
	char rightSubKey[17] = { 0 };

	Des3(leftSubKey, (char*)kmc.c_str(), (char*)leftDivData.c_str());
	Des3(rightSubKey, (char*)kmc.c_str(), (char*)rightDivData.c_str());

	return string(leftSubKey) + string(rightSubKey);
}
//=====================================================================================
void KeyGenerator::GenSessionKey(
	string cardRandom,
	string termRandom,
	int scp,
	string K_DEK,
	string K_MAC,
	string K_ENC,
	string &sessionDEK,
	string &sessionMAC,
	string &sessionENC)
{
	sessionDEK = GenSessionDEK(K_DEK, cardRandom, termRandom, scp);
	sessionMAC = GenSessionDEK(K_MAC, cardRandom, termRandom, scp);
	sessionENC = GenSessionDEK(K_ENC, cardRandom, termRandom, scp);
}

string KeyGenerator::GenSessionDEK(string K_DEK, string cardRandom, string termRandom, int scp)
{
	string key;
	if (scp == 1)
	{
		key = GenSessionKeyScp1(K_DEK, cardRandom, termRandom);
	}
	else if (scp == 2)
	{
		string leftDivFactor = "0182" + cardRandom.substr(0, 4) + "00000000";
		string rightDivFactor = "0000000000000000";

		key = GenSessionKeyScp2(K_DEK, leftDivFactor, rightDivFactor);
	}
	
	return key;
}

string KeyGenerator::GenSessionMAC(string K_MAC, string cardRandom, string termRandom, int scp)
{
	string key;
	if (scp == 1)
	{
		key = GenSessionKeyScp1(K_MAC, cardRandom, termRandom);
	}
	else if (scp == 2)
	{
		string leftDivFactor = "0101" + cardRandom.substr(0, 4) + "00000000";
		string rightDivFactor = "0000000000000000";

		key = GenSessionKeyScp2(K_MAC, leftDivFactor, rightDivFactor);
	}

	return key;
}

string KeyGenerator::GenSessionENC(string K_ENC, string cardRandom, string termRandom, int scp)
{
	string key;
	if (scp == 1)
	{
		key = GenSessionKeyScp1(K_ENC, cardRandom, termRandom);
	}
	else if (scp == 2)
	{
		string leftDivFactor = "0181" + cardRandom.substr(0, 4) + "00000000";
		string rightDivFactor = "0000000000000000";

		key = GenSessionKeyScp2(K_ENC, leftDivFactor, rightDivFactor);
	}

	return key;
}

string KeyGenerator::GenSessionKeyScp1(string key, string cardRandom, string termRandom)
{
	string leftDivData, rightDivData, DivData;
	char sessionKey[33] = { 0 };

	leftDivData = cardRandom.substr(8, 8) + termRandom.substr(0, 8);
	rightDivData = cardRandom.substr(0, 8) + termRandom.substr(8, 8);
	DivData = leftDivData + rightDivData;

	Des3_ECB(sessionKey, (char*)key.c_str(), (char*)DivData.c_str(), 16);

	return sessionKey;
}

//生成 会话密钥
string KeyGenerator::GenSessionKeyScp2(string key, const string leftDivFactor, const string rightDivFactor)
{
	char leftDivKey[17] = { 0 };
	char rightDivKey[17] = { 0 };

	Des3(leftDivKey, (char*)key.c_str(), (char*)leftDivFactor.c_str());
	str_xor(leftDivKey, (char*)rightDivFactor.c_str(), 16);
	Des3(rightDivKey, (char*)key.c_str(), leftDivKey);

	return string(leftDivKey) + string(rightDivKey);
}

//==================================================================================================
//获取SUDK_Auth
string KeyGenerator::GenSUDK_DEK(const string sudkAuth, const string ATC)
{
	string leftInput = "000000000000" + ATC;

	char szATC[32] = { 0 };
	memcpy(szATC, ATC.c_str(), ATC.length());
	str_xor(szATC, "FFFF", 4);
	string rightInput = "000000000000" + string(szATC);

	char szLeftKey[32] = { 0 };
	char szRightKey[32] = { 0 };
	Des3(szLeftKey, (char*)sudkAuth.c_str(), (char*)leftInput.c_str());
	Des3(szRightKey, (char*)sudkAuth.c_str(), (char*)rightInput.c_str());

	string szKey = string(szLeftKey) + string(szRightKey);

	return EvenOddCheck(szKey);
}

//获取SUDK_Auth
string KeyGenerator::GenUDK_DEK(const string mdkAuth, const string ATC, const string account, const string cardSeq)
{
	string input = account + cardSeq;
	if (input.length() > 16)
	{
		input = input.substr(input.length() - 16, 16);
	}
	char szInput[32] = { 0 };
	memcpy(szInput, input.c_str(), input.length());
	str_xor(szInput, "FFFFFFFFFFFFFFFF", 16);
	char szLeftKey[32] = { 0 };
	char szRightKey[32] = { 0 };
	Des3(szLeftKey, (char*)mdkAuth.c_str(), (char*)input.c_str());
	Des3(szRightKey, (char*)mdkAuth.c_str(), szInput);

	string szKey = string(szLeftKey) + string(szRightKey);

	return EvenOddCheck(szKey);
}

string KeyGenerator::EvenOddCheck(string input)
{
	string strResult;
	if (input.length() % 2 != 0)
	{
		return "";
	}
	for (unsigned int i = 0; i < input.length(); i += 2)
	{
		int num = stoi(input.substr(i, 2), 0, 16);
		bitset<8> b(num);
		if (b.count() % 2 == 0 && b[0] == 0)
		{//前7位是偶数，且最后一位是0
			b.set(0);
		}
		else if (b.count() % 2 == 0 && b[0] == 1)
		{//前7位是奇数，且最后一位是1
			b.reset(0);
		}

		unsigned long res = b.to_ulong();
		char szResult[3] = { 0 };
		sprintf_s(szResult, 3, "%02X", res);
		strResult += szResult;
	}

	return strResult;
}

string KeyGenerator::GenARPC1(string udkAuth, string ac, string authCode, string atc)
{
	authCode += "000000000000";
	char* szAC = (char*)ac.c_str();
	char* szAuthCode = (char*)authCode.c_str();
	str_xor(szAC, szAuthCode, 16);

	string sessionUdkAuth = GenSUDK_DEK(udkAuth, atc);
	if (sessionUdkAuth.empty())
	{
		return "";
	}
	char szARPC[32] = { 0 };
	Des3(szARPC, (char*)sessionUdkAuth.c_str(), szAC);

	return string(szARPC);
}

string KeyGenerator::GenARPC2(string mdkAuth, string ac, string authCode, string atc, string cardSeq, string pan)
{

	string udkAuth = GenUDK_DEK(mdkAuth, atc, pan, cardSeq);
	if (udkAuth.empty())
	{
		return "";
	}

	return GenARPC1(udkAuth, ac, authCode, atc);
}

string KeyGenerator::GenScriptMac(string mac, string atc, string data)
{
	if (mac.length() != 32 || data.length() % 16 != 0)
	{
		return "";
	}
	string sessionMac = GenSUDK_DEK(mac, atc);

	if (sessionMac.length() != 32)
	{
		return "";
	}

	string sessionMacLeft = sessionMac.substr(0, 16);
	string sessionMacRight = sessionMac.substr(16);

	int blocks = data.length() / 16;
	char szOutput[17] = { 0 };

	string blockData = data.substr(0, 16);
	for (int i = 1; i < blocks; i++)
	{
		Des(szOutput, (char*)sessionMacLeft.c_str(), (char*)blockData.c_str());
		str_xor(szOutput, (char*)data.substr(i * 16, 16).c_str(), 16);
		blockData = szOutput;
	}
	Des(szOutput, (char*)sessionMacLeft.c_str(), szOutput);
	_Des(szOutput, (char*)sessionMacRight.c_str(), szOutput);
	Des(szOutput, (char*)sessionMacLeft.c_str(), szOutput);

	return string(szOutput).substr(0, 8);
}








int KeyGenerator::QueryResult(void *params, int columnNum, char **columnVal, char **columnName)
{
	int i = 0;
	//printf("columnNum=%d\n",columnNum);
	for (i = 0; i < columnNum; i++)
	{
		//printf("%s:%s\t", columnName[i], columnVal[i]);
		if (string(columnName[i]) == "modulus")
		{
			m_caPublicKey = columnVal[i];
			break;
		}
	}

	return 0;
}

int KeyGenerator::QuerySingleCol(void *params, int columnNum, char **columnVal, char **columnName)
{
	SqlParam* sqlParam = (SqlParam*)params;
	for (int i = 0; i < columnNum; i++)
	{
		if (_strcmpi(columnName[i], sqlParam->colName) == 0)
		{
			char *temp = new char[4096];
			memcpy(temp, columnVal[i], 4096);
			sqlParam->result.push_back(temp);
		}
	}

	return 0;
}

void KeyGenerator::GetCARid(vector<char*> &ridList)
{
	string sql = "select distinct ridList from CA";
	SqlParam param = { 0 };
	param.colName = "ridList";

	QueryPcscTable(sql, QuerySingleCol, param);

	for (auto v : param.result)
	{
		ridList.push_back(v);
	}
}

void KeyGenerator::GetCAIndex(char* rid, vector<char*> &indexList)
{
	string sql = "select ridIndex from CA where ridList = \"" + string(rid) + "\"";
	SqlParam param = { 0 };
	param.colName = "ridIndex";

	QueryPcscTable(sql, QuerySingleCol, param);

	for (auto v : param.result)
	{
		indexList.push_back(v);
	}
}

void KeyGenerator::QueryPcscTable(string sql, sqlite3_callback callbackFunc, SqlParam& sqlParam)
{
	sqlite3 *db = NULL;
	char *errmsg = NULL;

	//打开数据库
	int ret = sqlite3_open("./Configuration/pcsc.db", &db);
	if (ret != SQLITE_OK)
	{
		sqlite3_close(db);
		return;
	}
	//构建查询语句
	ret = sqlite3_exec(db, sql.c_str(), callbackFunc, (void*)&sqlParam, &errmsg);
	if (ret != SQLITE_OK)
	{
		sqlite3_free(errmsg);
		return;
	}

	//关闭数据库
	sqlite3_close(db);

}

string KeyGenerator::GenCAPublicKey(const string caIndex, string rid)
{
	sqlite3 *db = NULL;
	char *errmsg = NULL;

	//打开数据库
	int ret = sqlite3_open("./Configuration/pcsc.db", &db);
	if (ret != SQLITE_OK)
	{
		sqlite3_close(db);
		return "";
	}
	//构建查询语句
	char sql[128] = { 0 };
	sprintf_s(sql, sizeof(sql), "select * from CA where ridList = \"%s\" and ridIndex = \"%s\";", rid.c_str(), caIndex.c_str());
	ret = sqlite3_exec(db, sql, QueryResult, NULL, &errmsg);
	if (ret != SQLITE_OK)
	{
		sqlite3_free(errmsg);
		return "";
	}

	//关闭数据库
	sqlite3_close(db);

	return m_caPublicKey;
}

string KeyGenerator::GenDesIssuerPublicKey(const string caPublicKey, const string issuerPublicCert, const string ipkRemainder, const string issuerExponent)
{
	char recoveryData[2046] = { 0 };

	//从发卡行证书中获取恢复数据
	RSA_STD((char*)caPublicKey.c_str(), (char*)issuerExponent.c_str(), (char*)issuerPublicCert.c_str(), recoveryData);
	int recoveryDataLen = strlen(recoveryData);
	string strRecoveryData(recoveryData);

	if (strRecoveryData.substr(0, 4) != "6A02" || strRecoveryData.substr(recoveryDataLen - 2, 2) != "BC")
	{//如果恢复数据的开头不是"6A02"并且结尾不是"BC",认证失败
		return "";
	}
	string hashData = strRecoveryData.substr(recoveryDataLen - 42, 40);
	string hashDataInput = strRecoveryData.substr(2, recoveryDataLen - 44) + ipkRemainder + issuerExponent;
	//Log->Info("Hash Input: [%s]", hashDataInput.c_str());
	string hashResult;
	CSHA1 sha1;
	hashResult = sha1.GetBCDHash(hashDataInput);
	if (hashResult != hashData)
	{
		return "";
	}
	//printf("%s", strRecoveryData.c_str());
	int caLen = caPublicKey.length();
	int issuerLen = stoi(strRecoveryData.substr(26, 2), 0, 16) * 2;
	if (issuerLen <= caLen - 72)    //说明strRecoveryData包含了caLen - 72 - issuserLen 长度的"BB"
	{
		return strRecoveryData.substr(30, issuerLen);
	}
	return strRecoveryData.substr(30, recoveryDataLen - 72) + ipkRemainder;
}

string KeyGenerator::GenDesICCpublicKey(const string issuerPublicKey, const string iccPublicCert, const string iccRemainder, const string signedData, const string issuerExponent)
{
	char recoveryData[2046] = { 0 };

	//从发卡行证书中获取恢复数据
	RSA_STD((char*)issuerPublicKey.c_str(), (char*)issuerExponent.c_str(), (char*)iccPublicCert.c_str(), recoveryData);
	int recoveryDataLen = strlen(recoveryData);
	string strRecoveryData(recoveryData);

	if (strRecoveryData.substr(0, 4) != "6A04" || strRecoveryData.substr(recoveryDataLen - 2, 2) != "BC")
	{//如果恢复数据的开头不是"6A02"并且结尾不是"BC",认证失败
		return "";
	}
	string hashData = strRecoveryData.substr(recoveryDataLen - 42, 40);
	//Log->Info("Recovery Data:[%s]", strRecoveryData.c_str());
	string hashDataInput = strRecoveryData.substr(2, recoveryDataLen - 44) + iccRemainder + issuerExponent + signedData;
	//Log->Info("Hash Input: [%s]", hashDataInput.c_str());

	CSHA1 sha1;
	string hashResult = sha1.GetBCDHash(hashDataInput);
	if (hashResult != hashData)
	{
		return "";
	}
	int issuerLen = issuerPublicKey.length();
	int iccLen = stoi(strRecoveryData.substr(38, 2), 0, 16) * 2;
	if (iccLen <= issuerLen - 84)
	{
		return strRecoveryData.substr(42, iccLen);
	}

	return strRecoveryData.substr(42, recoveryDataLen - 84) + iccRemainder;
}

string KeyGenerator::GenSMIssuerPublcKey(const string caPublicKey, const string issuerPublicCert)
{
	PDllPBOC_SM2_Verify DllPBOC_SM2_Verify = GetSMFunc<PDllPBOC_SM2_Verify>("dllPBOC_SM2_Verify");
	if (DllPBOC_SM2_Verify)
	{
		int signedResultLen = caPublicKey.length();
		int signDataLen = issuerPublicCert.length() - signedResultLen;
		string signData = issuerPublicCert.substr(0, signDataLen);
		string signedResult = issuerPublicCert.substr(signDataLen);
		int ret = DllPBOC_SM2_Verify((char*)caPublicKey.c_str(), (char*)signData.c_str(), (char*)signedResult.c_str());
		if (ret == SM_OK)
		{
			int ipkLen = stoi(issuerPublicCert.substr(26, 2), 0, 16) * 2;
			int ipkLen2 = signDataLen - 28;
			if (ipkLen != ipkLen2)
			{
				return "";
			}
			return issuerPublicCert.substr(28, ipkLen);
		}
	}

	return "";
}

string KeyGenerator::GenSMICCpublicKey(const string issuerPublicKey, const string iccPublicCert, const string needAuthStaticData)
{
	PDllPBOC_SM2_Verify DllPBOC_SM2_Verify = GetSMFunc<PDllPBOC_SM2_Verify>("dllPBOC_SM2_Verify");
	if (DllPBOC_SM2_Verify)
	{
		int signedResultLen = issuerPublicKey.length();
		int signDataLen = iccPublicCert.length() - signedResultLen;
		string signData = iccPublicCert.substr(0, signDataLen) + needAuthStaticData;
		string signedResult = iccPublicCert.substr(signDataLen);
		int ret = DllPBOC_SM2_Verify((char*)issuerPublicKey.c_str(), (char*)signData.c_str(), (char*)signedResult.c_str());
		if (ret == SM_OK)
		{
			int iccLen = stoi(iccPublicCert.substr(38, 2), 0, 16) * 2;
			int iccLen2 = signDataLen - 40;
			if (iccLen != iccLen2)
			{
				return "";
			}
			return iccPublicCert.substr(40, iccLen);
		}
	}

	return "";
}

string KeyGenerator::GenIssuerPublicKey(const string caPublicKey,
	const string issuerPublicCert,
	const string ipkRemainder,
	ENCRYPT_TYPE type,
	const string issuerExponent)
{
	switch (type)
	{
	case ENCRYPT_DES:  return GenDesIssuerPublicKey(caPublicKey, issuerPublicCert, ipkRemainder, issuerExponent);
	case ENCRYPT_SM:   return GenSMIssuerPublcKey(caPublicKey, issuerPublicCert);
	}

	return "";
}

string KeyGenerator::GenICCPublicKey(const string issuerPublicKey,
	const string iccPublicCert,
	const string iccRemainder,
	const string signedData,
	ENCRYPT_TYPE type,
	const string issuerExponent)
{
	switch (type)
	{
	case ENCRYPT_DES:  return GenDesICCpublicKey(issuerPublicKey, iccPublicCert, iccRemainder, signedData, issuerExponent);
	case ENCRYPT_SM:   return GenSMICCpublicKey(issuerPublicKey, iccPublicCert, signedData);
	}

	return "";
}

/***************************************************************************/
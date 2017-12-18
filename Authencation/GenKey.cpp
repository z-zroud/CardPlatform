#include "stdafx.h"
#include "GenKey.h"
#include "Des0.h"
#include "SHA1.h"
#include "..\Util\Tool.h"
#include "SqliteDB.h"
#include "SM.hpp"
#include <string>
#include <bitset>

using namespace std;
//string KeyGenerator::m_caPublicKey = "";

/***************************************************************************/
/*******************************************************************
* Func: 奇偶校验
*******************************************************************/
string EvenOddCheck(string input)
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

void GenUdkSessionKey(const char* udkSubKey, const char* atc, char* udkSessionKey)
{
	string leftInput = "000000000000" + string(atc);

	char tmp[5] = { 0 };
	memcpy(tmp, atc, strlen(atc));
	str_xor(tmp, "FFFF", 4);
	string rightInput = "000000000000" + string(tmp);

	char leftKey[32] = { 0 };
	char rightKey[32] = { 0 };
	Des3(leftKey, (char*)udkSubKey, (char*)leftInput.c_str());
	Des3(rightKey, (char*)udkSubKey, (char*)rightInput.c_str());

	string key = string(leftKey) + string(rightKey);
	string sessionKey = EvenOddCheck(key);
	strcpy(udkSessionKey, sessionKey.c_str());
}

void GenUdk(const char* mdk, const char* cardNo, const char* cardSequence, char* udk)
{
	string leftInput = string(cardNo) + string(cardSequence);
	if (leftInput.length() > 16)
	{
		leftInput = leftInput.substr(leftInput.length() - 16, 16);
	}
	char rightInput[17] = { 0 };
	memcpy(rightInput, leftInput.c_str(), leftInput.length());
	str_xor(rightInput, "FFFFFFFFFFFFFFFF", 16);

	char leftKey[32] = { 0 };
	char rightKey[32] = { 0 };
	Des3(leftKey, (char*)mdk, (char*)leftInput.c_str());
	Des3(rightKey, (char*)mdk, rightInput);

	string key = string(leftKey) + string(rightKey);
	string result = EvenOddCheck(key);
	strcpy(udk, result.c_str());
}

void GenArpc(const char* udkAuthSessionKey, char* ac, char* authCode, char* arpc)
{
	char temp[17] = { 0 };
	sprintf(temp, "%s000000000000", authCode);
	str_xor(ac, temp, 16);

	Des3(arpc, (char*)udkAuthSessionKey, ac);
}

void GenIssuerScriptMac(const char* udkMacSessionKey, const char* data, char* mac)
{
	string sessionMacKey = udkMacSessionKey;
	string leftSessionMacKey = sessionMacKey.substr(0, 16);
	string rightSessionMacKey = sessionMacKey.substr(16);
	string calcData = data;

	int blocks = calcData.length() / 16;
	char szOutput[17] = { 0 };

	string blockData = calcData.substr(0, 16);
	for (int i = 1; i < blocks; i++)
	{
		Des(szOutput, (char*)leftSessionMacKey.c_str(), (char*)blockData.c_str());
		str_xor(szOutput, (char*)calcData.substr(i * 16, 16).c_str(), 16);
		blockData = szOutput;
	}
	Des(szOutput, (char*)leftSessionMacKey.c_str(), szOutput);
	_Des(szOutput, (char*)rightSessionMacKey.c_str(), szOutput);
	Des(szOutput, (char*)leftSessionMacKey.c_str(), szOutput);

	string result = string(szOutput).substr(0, 8);

	strcpy(mac, result.c_str());
}

int GenCAPublicKey(const char* caIndex, const char* rid, char* caPublicKey)
{
	SQLite db;
	char *errmsg = NULL;
	char appPath[256] = { 0 };
	GetModuleFileName(NULL, appPath, 256);
	string temp = appPath;
	int pos = temp.find_last_of('\\');
	string dbPath = temp.substr(0, pos) + "/Configuration/pcsc.db";

	//打开数据库
	if(!db.Open(dbPath.c_str()))
	{
		db.Close();
		return 1;
	}
	//构建查询语句
	char sql[128] = { 0 };
	sprintf_s(sql, sizeof(sql), "select * from CA where ridList = \"%s\" and ridIndex = \"%s\";", rid, caIndex);
	SQLiteDataReader dbReader = db.ExcuteQuery(sql);
	
	while (dbReader.Read())
	{
		LPCTSTR result = dbReader.GetStringValue(6);
		strcpy(caPublicKey, result);
	}

	return 0;
}

/********************************************************************************
* 功能： 获取发卡行公钥
* 参数： issuerPulicCert 发卡行公钥证书
*		ipkRemainder 发卡行公钥余项
*		type
*********************************************************************************/
int GenDesIssuerPublicKey(
	const char* caPublicKey,
	const char* issuerPublicCert,
	const char* ipkRemainder,
	const char* issuerExponent,
	char* issuerPublicKey)
{
	char recoveryData[2046] = { 0 };

	//从发卡行证书中获取恢复数据
	RSA_STD((char*)caPublicKey, (char*)issuerExponent, (char*)issuerPublicCert, recoveryData);
	int recoveryDataLen = strlen(recoveryData);
	string strRecoveryData(recoveryData);

	if (strRecoveryData.substr(0, 4) != "6A02" || strRecoveryData.substr(recoveryDataLen - 2, 2) != "BC")
	{
		return 1;	//如果恢复数据的开头不是"6A02"并且结尾不是"BC",认证失败
	}
	string hashData = strRecoveryData.substr(recoveryDataLen - 42, 40);
	string hashDataInput;
	if (ipkRemainder == NULL){
		hashDataInput = strRecoveryData.substr(2, recoveryDataLen - 44) + issuerExponent;
	}
	else {
		hashDataInput = strRecoveryData.substr(2, recoveryDataLen - 44) + ipkRemainder + issuerExponent;
	}
	 
	//Log->Info("Hash Input: [%s]", hashDataInput.c_str());
	string hashResult;
	CSHA1 sha1;
	hashResult = sha1.GetBCDHash(hashDataInput);
	if (hashResult != hashData)
	{
		return 2;
	}
	//printf("%s", strRecoveryData.c_str());
	int caLen = strlen(caPublicKey);
	int issuerLen = stoi(strRecoveryData.substr(26, 2), 0, 16) * 2;
	if (issuerLen <= caLen - 72)    //说明strRecoveryData包含了caLen - 72 - issuserLen 长度的"BB"
	{
		strcpy(issuerPublicKey, strRecoveryData.substr(30, issuerLen).c_str());
		return 0;
	}
	strcpy(issuerPublicKey, (strRecoveryData.substr(30, recoveryDataLen - 72) + ipkRemainder).c_str());

	return 0;
}

int GenDesICCPublicKey(
	const char* issuerPublicKey,
	const char* iccPublicCert,
	const char* iccRemainder,
	const char* sigStaticData,
	const char* iccExponent,
	const char* tag82,
	char* iccPublicKey)
{
	char recoveryData[2046] = { 0 };

	//从发卡行证书中获取恢复数据
	RSA_STD((char*)issuerPublicKey, (char*)iccExponent, (char*)iccPublicCert, recoveryData);
	int recoveryDataLen = strlen(recoveryData);
	string strRecoveryData(recoveryData);

	if (strRecoveryData.substr(0, 4) != "6A04" || strRecoveryData.substr(recoveryDataLen - 2, 2) != "BC")
	{//如果恢复数据的开头不是"6A02"并且结尾不是"BC",认证失败
		return 1;
	}
	string hashData = strRecoveryData.substr(recoveryDataLen - 42, 40);
	//Log->Info("Recovery Data:[%s]", strRecoveryData.c_str());
	string hashDataInput = strRecoveryData.substr(2, recoveryDataLen - 44) + iccRemainder + iccExponent + sigStaticData + tag82;
	//Log->Info("Hash Input: [%s]", hashDataInput.c_str());

	CSHA1 sha1;
	string hashResult = sha1.GetBCDHash(hashDataInput);
	if (hashResult != hashData)
	{
		return 2;
	}
	int issuerLen = strlen(issuerPublicKey);
	int iccLen = stoi(strRecoveryData.substr(38, 2), 0, 16) * 2;
	if (iccLen <= issuerLen - 84)
	{
		strcpy(iccPublicKey, strRecoveryData.substr(42, iccLen).c_str());
		return 0;
	}

	strcpy(iccPublicKey, (strRecoveryData.substr(42, recoveryDataLen - 84) + iccRemainder).c_str());

	return 0;
}

int GenSMIssuerPublicKey(
	const char* caPublicKey,
	const char* issuerPublicCert,
	char* issuerPublicKey)
{
	PDllPBOC_SM2_Verify DllPBOC_SM2_Verify = GetSMFunc<PDllPBOC_SM2_Verify>("dllPBOC_SM2_Verify");
	if (DllPBOC_SM2_Verify)
	{
		int signedResultLen = strlen(caPublicKey);
		int signDataLen = strlen(issuerPublicCert) - signedResultLen;
		//string signData = issuerPublicCert.substr(0, signDataLen);
		char signData[1024] = { 0 };
		Tool::SubStr(issuerPublicCert, 0, signDataLen, signData);
		char signedResult[1024] = { 0 };
		//string signedResult = issuerPublicCert.substr(signDataLen);
		Tool::SubStr(issuerPublicCert, signDataLen, strlen(issuerPublicCert) - signDataLen, signedResult);
		int ret = DllPBOC_SM2_Verify((char*)caPublicKey, signData, signedResult);
		if (ret == SM_OK)
		{
			char issuerPublicCertLen[3] = { 0 };
			Tool::SubStr(issuerPublicCert, 26, 2, issuerPublicCertLen);
			int ipkLen = stoi(issuerPublicCertLen, 0, 16) * 2;
			int ipkLen2 = signDataLen - 28;
			if (ipkLen != ipkLen2)
			{
				return 2;
			}
			Tool::SubStr(issuerPublicCert, 28, ipkLen, issuerPublicKey);
			//return issuerPublicCert.substr(28, ipkLen);
		}
	}

	return 0;
}

int GenSMICCPublicKey(
	const char* issuerPublicKey,
	const char* iccPublicCert,
	const char* needAuthStaticData,
	char* iccPublicKey)
{
	PDllPBOC_SM2_Verify DllPBOC_SM2_Verify = GetSMFunc<PDllPBOC_SM2_Verify>("dllPBOC_SM2_Verify");
	if (DllPBOC_SM2_Verify)
	{
		int signedResultLen = strlen(issuerPublicKey);
		int signDataLen = strlen(iccPublicCert) - signedResultLen;
		//string signData = iccPublicCert.substr(0, signDataLen) + needAuthStaticData;
		char signData[2048] = { 0 };
		Tool::SubStr(iccPublicCert, 0, signDataLen, signData);
		strcat_s(signData, 2048, needAuthStaticData);
		//string signedResult = iccPublicCert.substr(signDataLen);
		char signedResult[1024] = { 0 };
		Tool::SubStr(iccPublicCert, signDataLen, strlen(iccPublicCert) - signDataLen, signedResult);
		int ret = DllPBOC_SM2_Verify((char*)issuerPublicKey, signData, signedResult);
		if (ret == SM_OK)
		{
			char icPublicCertLen[3] = { 0 };
			Tool::SubStr(iccPublicCert, 38, 2, icPublicCertLen);
			int iccLen = stoi(icPublicCertLen, 0, 16) * 2;
			int iccLen2 = signDataLen - 40;
			if (iccLen != iccLen2)
			{
				return 2;
			}
			Tool::SubStr(iccPublicCert, 40, iccLen, iccPublicKey);
			//return iccPublicCert.substr(40, iccLen);
		}
	}

	return 0;
}

int DES_SDA(const char* issuerPublicKey, const char*ipkExponent, const char* tag93, const char* sigStaticData, const char* tag82)
{
	//解密签名的静态应用数据
	char recoveryData[2048] = { 0 };
	RSA_STD((char*)issuerPublicKey, (char*)ipkExponent, (char*)tag93, recoveryData);
	int recoveryDataLen = strlen(recoveryData);
	string strRecoveryData(recoveryData);

	if (strRecoveryData.substr(0, 4) != "6A03" || strRecoveryData.substr(recoveryDataLen - 2, 2) != "BC")
	{
		return 2; //如果恢复数据的开头不是"6A03"并且结尾不是"BC",认证失败
	}
	string hashData = strRecoveryData.substr(recoveryDataLen - 42, 40);


	string hashDataInput = strRecoveryData.substr(2, recoveryDataLen - 44) + sigStaticData + tag82;

	CSHA1 sha1;
	string hashResult = sha1.GetBCDHash(hashDataInput);
	if (hashResult == hashData)
	{
		return 0;
	}

	return 1;
}

int SM_SDA(const char* issuerPublicKey, const char*ipkExponent, const char* sigStaticData, const char* tag93, const char* tag82)
{
	PDllPBOC_SM2_Verify DllPBOC_SM2_Verify = GetSMFunc<PDllPBOC_SM2_Verify>("dllPBOC_SM2_Verify");
	if (DllPBOC_SM2_Verify)
	{
		char signedData[1024] = { 0 };
		Tool::SubStr(sigStaticData, 6, strlen(tag93) - 6, signedData);
		char sigHeader[32] = { 0 };
		Tool::SubStr(sigStaticData, 0, 6, sigHeader);
		string needValidateData = string(sigHeader) + tag93 + tag82;
		int ret = DllPBOC_SM2_Verify((char*)issuerPublicKey, (char*)needValidateData.c_str(), (char*)signedData);
		if (ret == SM_OK)
		{
			return 0;
		}
	}

	return 1;
}

int DES_DDA(const char* iccPublicKey, const char*iccExponent, const char* tag9F4B, const char* dynamicData)
{
	//从动态签名数据中获取恢复数据
	char recoveryData[2048] = { 0 };
	RSA_STD((char*)iccPublicKey, (char*)iccExponent, (char*)tag9F4B, recoveryData);
	string strRecoveryData = recoveryData;
	int recoveryDataLen = strlen(recoveryData);
	if (recoveryDataLen == 0)
	{
		return 1;
	}
	string hashData = strRecoveryData.substr(recoveryDataLen - 42, 40);
	string hashDataInput = strRecoveryData.substr(2, recoveryDataLen - 44) + dynamicData;

	CSHA1 sha1;
	string hashResult = sha1.GetBCDHash(hashDataInput);
	if (hashResult == hashData)
	{
		return 0;
	}
	return 1;
}

int SM_DDA(const char* iccPublicKey, const char* dynamicData)
{
	PDllPBOC_SM2_Verify DllPBOC_SM2_Verify = GetSMFunc<PDllPBOC_SM2_Verify>("dllPBOC_SM2_Verify");
	if (DllPBOC_SM2_Verify)
	{
		//if (signedData.substr(0, 2) != "15")
		//{
		//	return false;
		//}
		//int iccDynamicDataLen = stoi(signedData.substr(2, 2), 0, 16) * 2;
		//string iccDynamicData = signedData.substr(4, iccDynamicDataLen);
		//string signedResult = signedData.substr(4 + iccDynamicDataLen);
		//string needValidateData = signedData.substr(0, 4 + iccDynamicDataLen) + terminalData;
		//int ret = DllPBOC_SM2_Verify((char*)iccPublicKey (char*)needValidateData.c_str(), (char*)signedResult.c_str());
		//if (ret == SM_OK)
		//{
		//	return true;
		//}
	}
	return 1;
}

/***************************************************************************/
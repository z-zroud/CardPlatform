#include "stdafx.h"
#include "IGenKey.h"
#include "..\Util\Tool.h"
#include "SqliteDB.h"
#include "..\Util\Des0.h"
#include "..\Util\SHA1.h"
#include "..\Util\SM.hpp"
#include <string>
#include <bitset>

using namespace std;
//string KeyGenerator::m_caPublicKey = "";

/***************************************************************************/

void Decrypt_Des3_CBC(const char* key, const char* input, char* output, int outputLen)
{
    int dataLen = strlen(input);
    int count = dataLen / 16;

    string leftKey = string(key).substr(0, 16);
    string rightKey = string(key).substr(16);
    string result;
    char output1[17] = { 0 };
    string data = input;
    for (int i = count - 1; i >= 0; i--)
    {
        _Des(output1, (char*)leftKey.c_str(), (char*)data.substr(i * 16, 16).c_str());
        Des(output1, (char*)rightKey.c_str(), output1);
        _Des(output1, (char*)leftKey.c_str(), output1);
        if (i != 0)
        {
            char temp[17] = { 0 };
            strncpy_s(temp, 17, data.substr((i - 1) * 16, 16).c_str(), 16);
            str_xor(output1, temp, 16);
        }
        result = output1 + result;
    }
    int len = result.length();
    strncpy_s(output, outputLen, result.c_str(), result.length());
}
/*******************************************************************
* Func: 奇偶校验
*******************************************************************/
string EvenOddCheck(string input)
{
	string result;
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
		result += szResult;
	}

	return result;
}

int GenDesKcv(const char* key, char* kcv, int kcvLen)
{
    char allKcv[32] = { 0 };
    Des3_ECB(allKcv, (char*)key, "0000000000000000", 16);
    if (strlen(allKcv) != 16)
        return -1;
    strncpy(kcv, allKcv, kcvLen);

    return 0;
}

int GenSmKcv(const char* key, char* kcv, int kcvLen)
{
    char allKcv[33] = { 0 };
    PDllSM4_ECB_ENC dllSM4_ECB_ENC = GetSMFunc<PDllSM4_ECB_ENC>("dllSM4_ECB_ENC");
    if (dllSM4_ECB_ENC)
    {        
        dllSM4_ECB_ENC((char*)key, "00000000000000000000000000000000", allKcv);
    }
    if (strlen(allKcv) != 32)
        return -1;
    strncpy(kcv, allKcv, kcvLen);

    return 0;
}

void GenUdkSessionKey(const char* udkSubKey, const char* atc, char* udkSessionKey, int keyType)
{
	string leftInput = "000000000000" + string(atc);

	char tmp[5] = { 0 };
	memcpy(tmp, atc, strlen(atc));
	str_xor(tmp, "FFFF", 4);
	string rightInput = "000000000000" + string(tmp);

	char leftKey[33] = { 0 };
	char rightKey[33] = { 0 };
    if (keyType == DES_KEY)
    {
        Des3(leftKey, (char*)udkSubKey, (char*)leftInput.c_str());
        Des3(rightKey, (char*)udkSubKey, (char*)rightInput.c_str());
    }
    else {
        PDllSM4_ECB_ENC Dll_SM4_ECB_ENC = GetSMFunc<PDllSM4_ECB_ENC>("dllSM4_ECB_ENC");
        if (Dll_SM4_ECB_ENC)
        {
            string input = leftInput + rightInput;
            Dll_SM4_ECB_ENC((char*)udkSubKey, (char*)input.c_str(), leftKey);
        }
    }

	string key = string(leftKey) + string(rightKey);
	string sessionKey = keyType == DES_KEY ? EvenOddCheck(key) : key;
	strcpy(udkSessionKey, sessionKey.c_str());
}

void GenUdk(const char* mdk, const char* cardNo, const char* cardSequence, char* udk, int keyType)
{
    //去掉F
    string cardNoDelF = string(cardNo);
    if (cardNoDelF[cardNoDelF.length() - 1] == 'F')
    {
        cardNoDelF = cardNoDelF.substr(0, cardNoDelF.length() - 1);
    }
	string leftInput = cardNoDelF + string(cardSequence);
	if (leftInput.length() > 16)
	{
		leftInput = leftInput.substr(leftInput.length() - 16, 16);
	}
	char rightInput[17] = { 0 };
	memcpy(rightInput, leftInput.c_str(), leftInput.length());
	str_xor(rightInput, "FFFFFFFFFFFFFFFF", 16);

	char leftKey[33] = { 0 };
	char rightKey[33] = { 0 };

    if (keyType == DES_KEY)
    {
        Des3(leftKey, (char*)mdk, (char*)leftInput.c_str());
        Des3(rightKey, (char*)mdk, rightInput);
    }
    else {
        PDllSM4_ECB_ENC Dll_SM4_ECB_ENC = GetSMFunc<PDllSM4_ECB_ENC>("dllSM4_ECB_ENC");
        if (Dll_SM4_ECB_ENC)
        {
            string input = leftInput + rightInput;
            Dll_SM4_ECB_ENC((char*)mdk, (char*)input.c_str(), leftKey);
        }
    }


	string key = string(leftKey) + string(rightKey);
	string result = keyType == DES_KEY ? EvenOddCheck(key) : key;
	strcpy(udk, result.c_str());
}

void GenArpc(const char* udkAuthSessionKey, char* ac, char* authCode, char* arpc, int keyType)
{
	char temp[17] = { 0 };
	sprintf(temp, "%s000000000000", authCode);
	str_xor(ac, temp, 16);
    if (keyType == DES_KEY)
    {
        Des3(arpc, (char*)udkAuthSessionKey, ac);
    }
    else {
        PDllSM4_ECB_ENC Dll_SM4_ECB_ENC = GetSMFunc<PDllSM4_ECB_ENC>("dllSM4_ECB_ENC");
        if (Dll_SM4_ECB_ENC)
        {
            string input = string(ac) + "0000000000000000";
            char result[33] = { 0 };
            Dll_SM4_ECB_ENC((char*)udkAuthSessionKey, (char*)input.c_str(), result);
            strncpy_s(arpc, 17, result, 16);
        }
    }	
}


void GenEMVMac(const char* udkAC, const char* data, char* mac)
{
    string sessionMacKey = udkAC;
    string leftSessionMacKey = sessionMacKey.substr(0, 16);
    string rightSessionMacKey = sessionMacKey.substr(16);
    string calcData = data;


    if (calcData.length() % 16 == 0)
    {
        calcData = data;
    }
    else {
        calcData += "00";
        int remaindZero = calcData.length() % 16;
        calcData.append(remaindZero, '0');
    }


    char szOutput[33] = { 0 };

    int blocks = calcData.length() / 16;

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



    string result = string(szOutput).substr(0, 16);

    strcpy(mac, result.c_str());
}

void GenPBOCMac(const char* udkMacSessionKey, const char* data, char* mac, int keyType)
{
	string sessionMacKey = udkMacSessionKey;
	string leftSessionMacKey = sessionMacKey.substr(0, 16);
	string rightSessionMacKey = sessionMacKey.substr(16);
	string calcData = data;

    if (keyType == 0)   //SM算法中，已经强制补80了，因此，这里无需再补
    {
        if (calcData.length() % 16 == 0)
        {
            calcData += "8000000000000000";
        }
        else {
            calcData += "80";
            int remaindZero = calcData.length() % 16;
            calcData.append(remaindZero, '0');
        }
    }

    char szOutput[33] = { 0 };
    if (keyType == DES_KEY)
    {
        int blocks = calcData.length() / 16;
        
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
    }
    else {
        PDllSM4_CBC_MAC DllSM4_CBC_MAC = GetSMFunc<PDllSM4_CBC_MAC>("dllSM4_CBC_MAC");
        if (DllSM4_CBC_MAC)
        {
            DllSM4_CBC_MAC((char*)udkMacSessionKey, (char*)calcData.c_str(), "0000000000000000", szOutput);
        }
    }


	string result = string(szOutput).substr(0, 16);

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
    //printf("DB PATH=%s", dbPath.c_str());
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
    const char* PAN,
	char* issuerPublicKey)
{
    if (strlen(issuerPublicCert) < 28 || (issuerPublicCert[0] != '1' || issuerPublicCert[1] != '2'))
    {
        return 1;   //format error
    }
    string issuerFlag = string(issuerPublicCert).substr(2, 8);
    int index = issuerFlag.find('F');
    if (index == string::npos)
    {
        index = 8;
    }
    string leftPAN = issuerFlag.substr(0, index);
    if (string(PAN).substr(0, index) != leftPAN)
    {
        return 2;   //issuer mark error
    }
    string issuerPublicKeySignAlogrithmFlag = string(issuerPublicCert).substr(20, 2);
    if (issuerPublicKeySignAlogrithmFlag != "04")
    {
        return 3;   //issuer public key signed alogrithm falg error
    }
	PDllPBOC_SM2_Verify DllPBOC_SM2_Verify = GetSMFunc<PDllPBOC_SM2_Verify>("dllPBOC_SM2_Verify");
	if (DllPBOC_SM2_Verify)
	{
		int signedResultLen = strlen(caPublicKey);  //数字签名的长度
		int toBeSignDataLen = strlen(issuerPublicCert) - signedResultLen; //需签名的数据的长度

		char toBeSignData[1024] = { 0 };
		Tool::SubStr(issuerPublicCert, 0, toBeSignDataLen, toBeSignData);   //需签名的数据

		char signedResult[1024] = { 0 };
		Tool::SubStr(issuerPublicCert, toBeSignDataLen, strlen(issuerPublicCert) - toBeSignDataLen, signedResult); //已签名的结果

		int ret = DllPBOC_SM2_Verify((char*)caPublicKey, toBeSignData, signedResult);
		if (ret == SM_OK)
		{
			char issuerPublicCertLen[3] = { 0 };
			Tool::SubStr(issuerPublicCert, 26, 2, issuerPublicCertLen);
			int ipkLen = stoi(issuerPublicCertLen, 0, 16) * 2;
			int ipkLen2 = toBeSignDataLen - 28;
			if (ipkLen != ipkLen2) //再次校验发卡行公钥长度是否一致
			{
				return 4;
			}
			Tool::SubStr(issuerPublicCert, 28, ipkLen, issuerPublicKey);
            return 0;
		}
	}

	return 5;
}

int GenSMICCPublicKey(
	const char* issuerPublicKey,
	const char* iccPublicCert,
	const char* signStaticAppData,
    const char* tag82,
    const char* PAN,
	char* iccPublicKey)
{

    if (strlen(iccPublicCert) < 40)
    {
        return 1;   //length error
    }

    if (iccPublicCert[0] != '1' || iccPublicCert[1] != '4')
    {
        return 2;   //format error
    }

    string iccPAN = string(iccPublicCert).substr(2, 20);
    if (iccPAN.substr(0, strlen(PAN)) != string(PAN))
    {
        return 3;   //pan invalid
    }
    PDllPBOC_SM2_Verify SM2_Verify = GetSMFunc<PDllPBOC_SM2_Verify>("dllPBOC_SM2_Verify");
	if (SM2_Verify)
	{
		int signedResultLen = strlen(issuerPublicKey);
		int toBeSignDataLen = strlen(iccPublicCert) - signedResultLen;

        //需要签名的数据
		char toBeSignData[2048] = { 0 };
		Tool::SubStr(iccPublicCert, 0, toBeSignDataLen, toBeSignData);
		strcat_s(toBeSignData, 2048, signStaticAppData);
        strcat_s(toBeSignData, 2048, tag82);
        //已签名的结果
		char signedResult[1024] = { 0 };
		Tool::SubStr(iccPublicCert, toBeSignDataLen, strlen(iccPublicCert) - toBeSignDataLen, signedResult);

		int ret = SM2_Verify((char*)issuerPublicKey, toBeSignData, signedResult);
		if (ret == SM_OK)
		{
			char icPublicCertLen[3] = { 0 };
			Tool::SubStr(iccPublicCert, 38, 2, icPublicCertLen);
			int iccLen = stoi(icPublicCertLen, 0, 16) * 2;
			int iccLen2 = toBeSignDataLen - 40;
			if (iccLen != iccLen2) //再次核实长度的正确性
			{
				return 4;
			}
			Tool::SubStr(iccPublicCert, 40, iccLen, iccPublicKey);
            return 0;
        }
        else {
            return 6;
        }
	}

	return 5;
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

	return 5;
}

int SM_SDA(const char* issuerPublicKey, const char* toBeSignedStaticAppData, const char* tag93, const char* tag82)
{
    if (tag93[0] != '1' || tag93[1] != '3')
    {
        return 1;   //format error
    }

	PDllPBOC_SM2_Verify DllPBOC_SM2_Verify = GetSMFunc<PDllPBOC_SM2_Verify>("dllPBOC_SM2_Verify");
	if (DllPBOC_SM2_Verify)
	{
		char signedData[1024] = { 0 };
		Tool::SubStr(tag93, 6, strlen(tag93) - 6, signedData);
		char sigHeader[32] = { 0 };
		Tool::SubStr(tag93, 0, 6, sigHeader);
		string toBeSignData = string(sigHeader) + toBeSignedStaticAppData + tag82;
		int ret = DllPBOC_SM2_Verify((char*)issuerPublicKey, (char*)toBeSignData.c_str(), (char*)signedData);
		if (ret == SM_OK)
		{
			return 0;
		}
	}

	return 2;   //校验不成功
}

int DES_GenRecovery(const char* publicKey, const char* publicKeyExp, const char* encryptionData, char* recoveryData, int len)
{
	//从动态签名数据中获取恢复数据
	char result[2048] = { 0 };
	RSA_STD((char*)publicKey, (char*)publicKeyExp, (char*)encryptionData, result);
	int resultLen = strlen(result);
	if (resultLen == 0)
	{
		return -1;
	}
	strncpy_s(recoveryData, len, result, resultLen);

	return 0;
}

int GenHash(const char* input, char* hash, int len)
{
	CSHA1 sha1;
	string hashResult = sha1.GetBCDHash(input);
	if (hashResult.length() == 0)
	{
		return -1;
	}
	strncpy_s(hash, len, hashResult.c_str(), hashResult.length());

	return 0;
}

int GenSMHash(const char* input, const char* publicKey, char* hash, int len)
{
    PDllPBOC_GETMSG_HASH SM3_Hash = GetSMFunc<PDllPBOC_GETMSG_HASH>("dllPBOC_GETMSG_HASH");
    if (SM3_Hash)
    {
        SM3_Hash((char*)input, (char*)publicKey, hash);
        return 0;
    }
    return -1;
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
		return 3;
	}
	string hashData = strRecoveryData.substr(recoveryDataLen - 42, 40);
	string hashDataInput = strRecoveryData.substr(2, recoveryDataLen - 44) + dynamicData;
    printf("hash input: %s", hashDataInput.c_str());
	CSHA1 sha1;
	string hashResult = sha1.GetBCDHash(hashDataInput);
	if (hashResult == hashData)
	{
		return 0;
	}
	return 2;
}

int SM_DDA(const char* iccPublicKey, const char* tag9F4B, const char* dynamicData)
{
    if (tag9F4B[0] != '1' || tag9F4B[1] != '5')
    {
        return 1;
    }
	PDllPBOC_SM2_Verify DllPBOC_SM2_Verify = GetSMFunc<PDllPBOC_SM2_Verify>("dllPBOC_SM2_Verify");
	if (DllPBOC_SM2_Verify)
	{
        string daynamicAppData = tag9F4B;
		int iccDynamicAppDataLen = stoi(daynamicAppData.substr(2, 2), 0, 16) * 2;
        if (daynamicAppData.length() < 4 + iccDynamicAppDataLen)
        {
            return 3;   //数据长度有误
        }

        //需签名的数据
		string iccDynamicAppData = daynamicAppData.substr(4, iccDynamicAppDataLen);
		string toBeSignedData = daynamicAppData.substr(0, 4 + iccDynamicAppDataLen) + dynamicData;

        //已签名的结果
        string signedResult = daynamicAppData.substr(4 + iccDynamicAppDataLen);

		int ret = DllPBOC_SM2_Verify((char*)iccPublicKey,(char*)toBeSignedData.c_str(), (char*)signedResult.c_str());
		if (ret == SM_OK)
		{
			return 0;
		}
	}
	return 2;
}

/***************************************************************************/
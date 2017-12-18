#include "stdafx.h"
#include "IAPDU.h"
#include "../PCSC/PCSC.h"
#include "../Util/Tool.h"
#include "Des0.h"

char g_secureChannelSessionAuthKey[KEY_LEN] = { 0 };
char g_secureChannelSessionMacKey[KEY_LEN] = { 0 };
char g_secureChannelSessionEncKey[KEY_LEN] = { 0 };
char g_secureChannelSessionKekKey[KEY_LEN] = { 0 };

void GetScureChannelSessionAuthKey(char* secureChannelSessionAuthKey)
{
	strcpy_s(secureChannelSessionAuthKey, KEY_LEN, g_secureChannelSessionAuthKey);
}
void GetScureChannelSessionMacKey(char* secureChannelSessionMacKey)
{
	strcpy_s(secureChannelSessionMacKey, KEY_LEN, g_secureChannelSessionMacKey);
}
void GetScureChannelSessionEncKey(char* secureChannelSessionEncKey)
{
	strcpy_s(secureChannelSessionEncKey, KEY_LEN, g_secureChannelSessionEncKey);
}
void GetScureChannelSessionKekKey(char* secureChannelSessionKekKey)
{
	strcpy_s(secureChannelSessionKekKey, KEY_LEN, g_secureChannelSessionKekKey);
}

string _GenSecureChannelSessionKey(const string leftDivFactor, const string rightDivFactor, string key)
{
	char leftDivKey[17] = { 0 };
	char rightDivKey[17] = { 0 };

	Des3(leftDivKey, (char*)key.c_str(), (char*)leftDivFactor.c_str());
	str_xor(leftDivKey, (char*)rightDivFactor.c_str(), 16);
	Des3(rightDivKey, (char*)key.c_str(), leftDivKey);

	return string(leftDivKey) + string(rightDivKey);
}

//获取 会话密钥 Auth Key
string _GenSecureChannelSessionAuthKey(string seqNo, string strAuthKey)
{
	string leftDivFactor = "0182" + seqNo + "00000000";
	string rightDivFactor = "0000000000000000";

	return _GenSecureChannelSessionKey(leftDivFactor, rightDivFactor, strAuthKey);
}


//获取 会话密钥 Mac Key
string _GenSecureChannelSessionMacKey(string seqNo, string strMacKey)
{
	string leftDivFactor = "0101" + seqNo + "00000000";
	string rightDivFactor = "0000000000000000";

	return _GenSecureChannelSessionKey(leftDivFactor, rightDivFactor, strMacKey);
}
//获取 会话密钥 Enc Key
string _GenSecureChannelSessionEncKey(string seqNo, string strEnKey)
{
	string leftDivFactor = "0181" + seqNo + "00000000";
	string rightDivFactor = "0000000000000000";

	return _GenSecureChannelSessionKey(leftDivFactor, rightDivFactor, strEnKey);
}


string _GenKmcSubKey(const string kmc, string leftDivFactor, string rightDivFactor)
{
	char leftDivKey[17] = { 0 };
	char rightDivKey[17] = { 0 };

	Des3(leftDivKey, (char*)kmc.c_str(), (char*)leftDivFactor.c_str());
	Des3(rightDivKey, (char*)kmc.c_str(), (char*)rightDivFactor.c_str());

	return string(leftDivKey) + string(rightDivKey);
}


string _GenKmcAuthKey(const string kmc, string partLeftDivFactor, string partRightDivFactor)
{
	string leftDivFactor = partLeftDivFactor + string("F001");
	string rightDivFactor = partRightDivFactor + string("0F01");

	return _GenKmcSubKey(kmc, leftDivFactor, rightDivFactor);
}

string _GenKmcMacKey(const string kmc, string partLeftDivFactor, string partRightDivFactor)
{
	string leftDivFactor = partLeftDivFactor + string("F002");
	string rightDivFactor = partRightDivFactor + string("0F02");

	return _GenKmcSubKey(kmc, leftDivFactor, rightDivFactor);
}

string _GenKmcEncKey(const string kmc, string partLeftDivFactor, string partRightDivFactor)
{
	string leftDivFactor = partLeftDivFactor + string("F003");
	string rightDivFactor = partRightDivFactor + string("0F03");

	return _GenKmcSubKey(kmc, leftDivFactor, rightDivFactor);
}



void GenKmcSubKey(string kmc, int divMethod, string divData, string &kmcAuthKey, string &kmcMacKey, string &kmcEncKey)
{
	string 	leftDivData;
	string	rightDivData;

	if (divMethod == DIV_CPG202)
	{
		leftDivData = divData.substr(0, 4) + divData.substr(8, 8);
		rightDivData = leftDivData;
	}
	else if (divMethod == DIV_CPG212) {
		leftDivData = divData.substr(8,20);
		rightDivData = leftDivData;
	}
	else {
		kmcAuthKey = kmc;
		kmcMacKey = kmc;
		kmcEncKey = kmc;
	}

	kmcAuthKey = _GenKmcAuthKey(kmc, leftDivData, rightDivData);
	kmcMacKey = _GenKmcMacKey(kmc, leftDivData, rightDivData);
	kmcEncKey = _GenKmcEncKey(kmc, leftDivData, rightDivData);
}




void GenSecureChannelSessionKey(string kmc,
	int divMethod,
	string termialRandom,
	string initializeUpdateResp,
	string sessionAuthKey,
	string sessionMacKey,
	string sessionEncKey,
	string kekKey)
{
	string leftDivData, rightDivData;
	string divData = initializeUpdateResp.substr(0, 20);
	string keyVersion = initializeUpdateResp.substr(20, 2);
	int scp = stoi(initializeUpdateResp.substr(22, 2));
	string cardChallenge = initializeUpdateResp.substr(24, 16);
	string cardCryptogram = initializeUpdateResp.substr(40, 16);

	string kmcAuthKey, kmcMacKey, kmcEncKey;
	GenKmcSubKey(kmc, divMethod, divData, kmcAuthKey, kmcMacKey, kmcEncKey);

	if (scp == 1) {
		char szSessionAuthKey[33] = { 0 };
		char szSessionMacKey[33] = { 0 };
		char szSessionEncKey[33] = { 0 };

		leftDivData = cardChallenge.substr(8, 8) + termialRandom.substr(0, 8);
		rightDivData = cardChallenge.substr(0, 8) + termialRandom.substr(8, 8);
		divData = leftDivData + rightDivData;

		Des3_ECB(szSessionAuthKey, (char*)kmcAuthKey.c_str(), (char*)divData.c_str(), 16);
		Des3_ECB(szSessionMacKey, (char*)kmcMacKey.c_str(), (char*)divData.c_str(), 16);
		Des3_ECB(szSessionEncKey, (char*)kmcEncKey.c_str(), (char*)divData.c_str(), 16);

		sessionAuthKey = szSessionAuthKey;
		sessionMacKey = szSessionMacKey;
		sessionEncKey = szSessionEncKey;
		kekKey = kmcEncKey;
	}
	else {
		string seqNo = cardChallenge.substr(0, 4);
		sessionAuthKey = _GenSecureChannelSessionAuthKey(seqNo, kmcAuthKey);
		sessionMacKey = _GenSecureChannelSessionMacKey(seqNo, kmcMacKey);
		sessionEncKey = _GenSecureChannelSessionEncKey(seqNo, kmcEncKey);
		kekKey = sessionEncKey;
	}
}


UINT ExternalAuthencationCmd2(const string& kmc, int divMethod, string terminalRandom, int secureLevel, string initializeUpdateResp)
{
	string cmd;
	string hostCryptogramAndMacLen = "10";

	if (secureLevel == SECURE_MAC) {
		cmd = "84820100";
	}
	else {
		cmd = "84830000";
	}
	string scp = initializeUpdateResp.substr(22, 2);
	string cardChanllenge = initializeUpdateResp.substr(24, 16);
	string cardCryptogram = initializeUpdateResp.substr(40, 56);

	memset(g_secureChannelSessionAuthKey, 0, KEY_LEN);
	memset(g_secureChannelSessionMacKey, 0, KEY_LEN);
	memset(g_secureChannelSessionEncKey, 0, KEY_LEN);
	memset(g_secureChannelSessionKekKey, 0, KEY_LEN);

	string sessionAuthKey, sessionMacKey, sessionEncKey, kekKey;
	GenSecureChannelSessionKey(kmc, divMethod, terminalRandom, initializeUpdateResp, sessionAuthKey, sessionMacKey, sessionEncKey, kekKey);
	string validateMacInput = terminalRandom + cardChanllenge;
	char validateMac[17] = { 0 };
	Full_3DES_CBC_MAC((char*)validateMacInput.c_str(), (char*)sessionAuthKey.c_str(), "0000000000000000", validateMac);

	memcpy(g_secureChannelSessionAuthKey, sessionAuthKey.c_str(), KEY_LEN - 1);
	memcpy(g_secureChannelSessionMacKey, sessionMacKey.c_str(), KEY_LEN - 1);
	memcpy(g_secureChannelSessionEncKey, sessionEncKey.c_str(), KEY_LEN - 1);
	memcpy(g_secureChannelSessionKekKey, kekKey.c_str(), KEY_LEN - 1);

	if (cardCryptogram != validateMac)
	{
		return 0x6300;
	}
	char szMac[17] = { 0 };

	string hostChallengeInput = cardChanllenge + terminalRandom;
	Full_3DES_CBC_MAC((char*)hostChallengeInput.c_str(), (char*)sessionAuthKey.c_str(), "0000000000000000", szMac);

	cmd += szMac;
	memset(szMac, 0, sizeof(szMac));
	if (scp == "01")
	{
		Full_3DES_CBC_MAC((char*)cmd.c_str(), (char*)sessionAuthKey.c_str(), "0000000000000000", szMac);
	}
	else {
		Common_MAC((char*)cmd.c_str(), (char*)sessionMacKey.c_str(), "0000000000000000", szMac);
	}

	cmd += szMac;

	return SendApdu2(cmd.c_str());
}
/******************************************************************
* 打开安全通道
*******************************************************************/
bool OpenSecureChannel(const char* kmc, int divMethod, int secureLevel)
{
	char random[] = "1122334455667788";
	char initializeUpdateResp[256] = { 0 };
	if (APDU_OK != InitializeUpdateCmd(random, initializeUpdateResp))
	{
		return false;
	}
	if (APDU_OK != ExternalAuthencationCmd2(kmc, divMethod, random, secureLevel, initializeUpdateResp))
	{
		return false;
	}
	return false;
}


/***********************************************************
* 重置安全通道或分散方式
************************************************************/
bool SetKmc(const char* kmc, int divMethod)
{
	return false;
}

/***********************************************************
* 用于打开安全通道更新初始化，
************************************************************/
UINT InitializeUpdateCmd(const char* random, char* resp)
{
	string cmd = "80500000 08" + string(random);
	int sw = SendApdu(cmd.c_str(), resp, RESP_LEN);
	return sw;
}
/**************************************************
* 通过AID选择应用
***************************************************/
UINT SelectAppCmd(const char* aid, char* resp)
{
	char dataLen[5] = { 0 };
	Tool::HexStr(aid, dataLen, 5);
	
	string cmd = "00A40400" + string(dataLen) + aid;
	int sw = SendApdu(cmd.c_str(), resp, RESP_LEN);


	return sw;
}

/**************************************************
* 读取记录文件信息
* 参数： sfi 读取记录文件的短文件标识
* 参数： recordNum 记录号
* 返回： 成功返回 0x9000 其他值表示失败
***************************************************/
UINT ReadRecordCmd(int sfi, int recordNum, char* resp)
{
	
	char p1[3] = { 0 };
	char p2[3] = { 0 };

	Tool::IntToStr(recordNum, p1, Two_byte);

	sfi = (sfi << 3) + 4;
	Tool::IntToStr(sfi, p2, Two_byte);
	

	string cmd = string("00B2") + p1 + p2;

	return SendApdu(cmd.c_str(), resp, RESP_LEN);
}

/**************************************************
* 通过AID删除应用
***************************************************/
UINT DeleteAppCmd(const char* aid)
{
	char aidLen[5] = { 0 };
	char dataLen[5] = { 0 };
	Tool::HexStr(aid, aidLen, 5);
	Tool::IncreaseStep(aidLen, 2, dataLen, 5);

	string cmd = "80E40000" + string(dataLen) + "4F" + aidLen + aid;
	int sw = SendApdu2(cmd.c_str());
	
	return sw;
}


/******************************************************
* 所有数据的重组均放到StoreData外面，简化StoreData命令流程
* 参数说明：data 包含了除命令头 及 data长度以外的数据部分
* 参数说明：type 表明data数据类型
*****************************************************/
UINT StoreDataCmd(const char* data, int type, bool reset)
{
	static int count = 0;
	
	if (reset) count = 0;	//强制重置，防止store data终端后，计数器不置0

	string cmd = "80E2";
	switch (type)
	{
	case STORE_DATA_PLANT:		cmd += "00"; break;
	case STORE_DATA_ENCRYPT:	cmd += "60"; break;
	case STORE_DATA_END:		cmd += "80"; break;
	}

	char szCount[3] = { 0 };
	char dataLen[3] = { 0 };
	sprintf_s(szCount, "%02X", count);		//命令头
	Tool::HexStr(data, dataLen, 3);

	cmd += string(szCount) + string(dataLen) + data;
	int sw = SendApdu2(cmd.c_str());
	count++;
	if (type == STORE_DATA_END) { count = 0; }	//重置计数器
	return sw;
}

UINT GPOCmd(const char* terminalData, char* resp)
{
	char terminalDataLen[3] = { 0 };
	Tool::HexStr(terminalData, terminalDataLen, 3);
	char dataLen[3] = { 0 };
	Tool::IncreaseStep(terminalDataLen, 2, dataLen, 3);

	string cmd = "80A80000" + string(dataLen) + "83" + terminalDataLen + terminalData;

	return SendApdu(cmd.c_str(), resp, RESP_LEN);
}

UINT InternalAuthencationCmd(const char* ddolData, char* resp)
{
	char ddolDataLen[3] = { 0 };

	Tool::HexStr(ddolData, ddolDataLen, 3);
	string cmd = "00880000" + string(ddolDataLen) + ddolData;

	return SendApdu(cmd.c_str(), resp, RESP_LEN);
}

void GenDynamicData(const char* ddolData, char* resp)
{
	char internalAuthencationResp[APDU_LEN] = { 0 };
	
	int sw = InternalAuthencationCmd(ddolData, internalAuthencationResp);
	if (sw != 0x9000)	return;

	char tag9F4B[RESP_LEN] = { 0 };
	Tool::SubStr(internalAuthencationResp, 6, strlen(internalAuthencationResp) - 6, tag9F4B);
	strcpy_s(resp, RESP_LEN, tag9F4B);
}

UINT GACCmd(int terminalCryptogramType, const char* cdolData, char* resp)
{
	char p1[3] = { 0 };
	Tool::IntToStr(terminalCryptogramType, p1, Two_byte);

	char cdolDataLen[3] = { 0 };
	Tool::HexStr(cdolData, cdolDataLen, 3);

	char p2[] = "00";
	string cmd = "80AE" + string(p1) + p2 + cdolDataLen + cdolData;

	return SendApdu(cmd.c_str(), resp, RESP_LEN);
}

UINT ExternalAuthencationCmd(const char* arpc, const char* authCode, char* resp)
{
	string extAuthData = string(arpc) + authCode;
	char extAuthDataLen[3] = { 0 };
	Tool::HexStr(extAuthData.c_str(), extAuthDataLen, 3);

	string cmd = "00820000" + string(extAuthDataLen) + extAuthData;

	return SendApdu(cmd.c_str(), resp, RESP_LEN);
}
///******************************************************
//* 根据给定的匹配/查找标准取得发行者安全域、可执行装载文件、
//* 可执行模块、应用和安全域的生命周期的状态信息。
//*******************************************************/
//bool APDU::GetAppStatusCmd(vector<APP_STATUS> &status, APDU_RESPONSE& response)
//{
//	string cmd = "80F24000 02 4F00";
//
//	if (SendAPDU(cmd, response))
//	{
//		string strResponse = response.data;
//		unsigned int i = 0;
//		while (i < strResponse.length())
//		{
//			APP_STATUS applicationStatus;
//			int len = 2 * stoi(strResponse.substr(i, 2), 0, 16);
//			applicationStatus.strAID = strResponse.substr(i + 2, len);
//			applicationStatus.lifeCycleStatus = strResponse.substr(i + 2 + len, 2);
//			applicationStatus.privilege = strResponse.substr(i + 4 + len, 2);
//			status.push_back(applicationStatus);
//
//			i += 6 + len;
//		}
//
//		return true;
//	}
//
//	return false;
//}
//

///******************************************************
//* 安装实例命令
//*******************************************************/
//bool APDU::InstallAppCmd(const string& package,
//	const string& applet,
//	const string& instance,
//	const string& privilege,
//	const string& installParam,
//	const string& token,
//	APDU_RESPONSE& response)
//{
//	char totalDataLen[3] = { 0 };	//数据总长度
//	string packageLen = Tool::GetStringLen(package);
//	string appletLen = Tool::GetStringLen(applet);
//	string instanceLen = Tool::GetStringLen(instance);
//	string privilegeLen = Tool::GetStringLen(privilege);
//	string installParamLen = Tool::GetStringLen(installParam);
//	string tokenLen = Tool::GetStringLen(token);
//
//	sprintf_s(totalDataLen, "%02X", 6 + package.length() / 2 +
//		applet.length() / 2 +
//		instance.length() / 2 +
//		privilege.length() / 2 +
//		installParam.length() / 2 +
//		token.length() / 2);
//
//	string cmd = "80E60C00" + string(totalDataLen) +
//		packageLen + package +
//		appletLen + applet +
//		instanceLen + instance +
//		privilegeLen + privilege +
//		installParamLen + installParam +
//		tokenLen + token;
//
//	return SendAPDU(cmd, response);
//}
//
///****************************************************************
//* 读取tag命令
//*****************************************************************/
//bool APDU::ReadTagCmd(const string &tag, APDU_RESPONSE &response)
//{
//	string cmd = "80CA";
//	if (tag.length() != 2 && tag.length() != 4)
//	{
//		return false;
//	}
//	if (tag.length() == 2)
//	{
//		cmd += "00";
//	}
//	cmd += tag;
//
//	return SendAPDU(cmd, response);
//}
//

//bool APDU::PutDataCommand(const string &tag, const string &value, const string &mac)
//{
//	if (mac.length() % 8 != 0)
//	{
//		return false;
//	}
//	string cmd = _T("04DA");
//	cmd += tag;
//	string dataLen = Base::GetDataHexLen(value + mac);
//	cmd += dataLen + value + mac;
//	APDU_RESPONSE response;
//
//	return SendAPDU(cmd, response) && (response.SW1 == 0x90 && response.SW2 == 0x00);
//}
//
//bool APDU::PutKeyCommand(const string keyVersion,
//	const string authKeyWithKcv,
//	const string macKeyWithKcv,
//	const string encKeyWithKcv)
//{
//	string cmd = "80D8";
//	if (keyVersion == "FF")
//	{
//		cmd += "00" + string("8143") + string("01");
//	}
//	else {
//		cmd += keyVersion + "8143" + keyVersion;
//	}
//	cmd += authKeyWithKcv + macKeyWithKcv + encKeyWithKcv;
//
//	APDU_RESPONSE response;
//	memset(&response, 0, sizeof(response));
//
//	return SendAPDU(cmd, response);
//}
//
//bool APDU::UpdateRecordCommand(const string &strCommand, APDU_RESPONSE &response)
//{
//	return false;
//}
//


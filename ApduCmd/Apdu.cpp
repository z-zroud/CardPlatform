#include "stdafx.h"
#include "IAPDU.h"
#include "../PCSC/PCSC.h"
#include "../Util/Tool.h"
#include "../GenKey/IGenKey.h"
#include "Des0.h"

char g_secureChannelSessionAuthKey[KEY_LEN] = { 0 };
char g_secureChannelSessionMacKey[KEY_LEN] = { 0 };
char g_secureChannelSessionEncKey[KEY_LEN] = { 0 };
char g_secureChannelSessionKekKey[KEY_LEN] = { 0 };

void GetScureChannelSessionAuthKey(char* secureChannelSessionAuthKey, int len)
{
	memcpy(secureChannelSessionAuthKey, g_secureChannelSessionAuthKey, len - 1);
}
void GetScureChannelSessionMacKey(char* secureChannelSessionMacKey, int len)
{
	memcpy(secureChannelSessionMacKey, g_secureChannelSessionMacKey, len - 1);
}
void GetScureChannelSessionEncKey(char* secureChannelSessionEncKey, int len)
{
	memcpy(secureChannelSessionEncKey, g_secureChannelSessionEncKey, len - 1);
}
void GetScureChannelSessionKekKey(char* secureChannelSessionKekKey, int len)
{
	memcpy(secureChannelSessionKekKey, g_secureChannelSessionKekKey, len - 1);
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

	IGenKey *pKey = GetGenKeyInstance();
	string sessionAuthKey, sessionMacKey, sessionEncKey, kekKey;
	pKey->GenScureChannelSessionKey(kmc, divMethod, terminalRandom, initializeUpdateResp, sessionAuthKey, sessionMacKey, sessionEncKey, kekKey);
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
bool OpenSecureChannel(const string &kmc, int divMethod, int secureLevel)
{
	string random = "1122334455667788";
	string initializeUpdateResp;
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
bool SetKmc(const string &kmc, int divMethod)
{
	return false;
}

/***********************************************************
* 用于打开安全通道更新初始化，
************************************************************/
UINT InitializeUpdateCmd(const string& random, string& resp)
{
	char apduResp[APDU_LEN] = { 0 };

	string cmd = "80500000 08" + random;
	int sw = SendApdu(cmd.c_str(), apduResp, APDU_LEN);
	resp = apduResp;

	return sw;
}
/**************************************************
* 通过AID选择应用
***************************************************/
UINT SelectAppCmd(const string& aid, string& resp)
{
	char apduResp[APDU_LEN] = { 0 };
	char dataLen[5] = { 0 };
	Tool::HexStr(aid.c_str(), dataLen, 5);
	
	string cmd = "00A40400" + string(dataLen) + aid;
	int sw = SendApdu(cmd.c_str(), apduResp, APDU_LEN);
	resp = apduResp;

	return sw;
}

/**************************************************
* 通过AID删除应用
***************************************************/
UINT DeleteAppCmd(const string& aid)
{
	char aidLen[5] = { 0 };
	char dataLen[5] = { 0 };
	Tool::HexStr(aid.c_str(), aidLen, 5);
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
UINT StoreDataCmd(const string data, int type, bool reset)
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
	Tool::HexStr(data.c_str(), dataLen, 3);

	cmd += string(szCount) + string(dataLen) + data;
	int sw = SendApdu2(cmd.c_str());
	count++;
	if (type == STORE_DATA_END) { count = 0; }	//重置计数器
	return sw;
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
////内部认证 命令
//bool APDU::InternalAuthCommand(const string &randNumber, APDU_RESPONSE &response)
//{
//	char szLen[16] = { 0 };
//	sprintf_s(szLen, "%02X", randNumber.length() / 2);
//	string strCommand = "00880000" + string(szLen) + randNumber;
//
//	return SendAPDU(strCommand, response);
//}
//
////外部认证 命令
//bool APDU::ExternalAuthCommand(const string cardRandomNum,
//	const string termRandomNum,
//	const string sessionAuthKey,
//	const string seesionMacKey,
//	SECURE_LEVEL nSecureLevel,
//	int nSCP,
//	APDU_RESPONSE &response)
//{
//	//char szMac[17] = { 0 };
//	//string strCommand;
//	//string input = cardRandomNum + termRandomNum;
//	//Full_3DES_CBC_MAC((char*)input.c_str(), (char*)sessionAuthKey.c_str(), "0000000000000000", szMac);
//	//switch (nSecureLevel)
//	//{
//	//case SL_NO_SECURE:	strCommand = "8482000010"; break;
//	//case SL_MAC:		strCommand = "8482010010"; break;
//	//case SL_MAC_ENC:	strCommand = "8482030010"; break;
//	//default:
//	//	return false;
//	//}
//	//strCommand += szMac;
//	//memset(szMac, 0, sizeof(szMac));
//	//switch (nSCP)
//	//{
//	//case 1: Full_3DES_CBC_MAC((char*)strCommand.c_str(), (char*)sessionAuthKey.c_str(), "0000000000000000", szMac); break;
//	//case 2: Common_MAC((char*)strCommand.c_str(), (char*)seesionMacKey.c_str(), "0000000000000000", szMac); break;
//	//default:
//	//	return false;
//	//}
//	//strCommand += szMac;
//
//	//return SendAPDU(strCommand, response);
//}
//
////外部认证
//bool APDU::ExternalAuthcateCommand(const string ARPC, const string authCode, APDU_RESPONSE &response)
//{
//	if (ARPC.length() != 16 && authCode.length() != 4)
//	{
//		return false;
//	}
//	string cmd = "008200000A" + ARPC + authCode;
//
//	return SendAPDU(cmd, response);
//}
//
//bool APDU::ReadRecordCommand(const string &SFI, const string strRecordNumber, APDU_RESPONSE &response)
//{
//	int temp = (stoi(SFI, 0, 16) << 3) + 4;
//	char szP2[3] = { 0 };
//
//	sprintf_s(szP2, "%02X", temp);
//
//	string strCommand = "00B2" + strRecordNumber + string(szP2);
//
//	return SendAPDU(strCommand, response);
//
//}
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
////获取处理选项 GPO命令
//bool APDU::GPOCommand(const string &strCommand, APDU_RESPONSE &response)
//{
//
//	int dataLen = strCommand.length() / 2;
//	int totalLen = dataLen + 2;
//	char szTotalLen[3] = { 0 };
//	char szDataLen[3] = { 0 };
//
//	sprintf_s(szTotalLen, "%02X", totalLen);
//	sprintf_s(szDataLen, "%02X", dataLen);
//
//	string cmd = "80A80000" + string(szTotalLen) + "83" + string(szDataLen) + strCommand;
//
//	return SendAPDU(cmd, response);
//}
//
////生成应用密文 命令
//bool APDU::GACCommand(GACControlParam parm1, const string terminalData, APDU_RESPONSE &response)
//{
//	char szDataLen[3] = { 0 };
//	sprintf_s(szDataLen, "%02X", terminalData.length() / 2);
//	string P1;
//	switch (parm1)
//	{
//	case AAC:
//		P1 = "00";
//		break;
//	case ARQC:
//		P1 = "80";
//		break;
//	case TC:
//		P1 = "40";
//		break;
//	case CDA:
//		P1 = "10";
//		break;
//	default:
//		break;
//	}
//	string cmd = "80AE" + P1 + "00" + szDataLen + terminalData;
//
//	return SendAPDU(cmd, response);
//}

#include "stdafx.h"
#include "APDU.h"
#include "CardBase.h"
#include "Tool.h"
#include "Base.h"

APDU::APDU(SCARDHANDLE scardHandle, CARD_TRANSMISSION_PROTOCOL protocol)
{
	m_scardHandle = scardHandle;
	m_dwActiveProtocol = protocol;
}

/*****************************************************************
* 选择应用命令
******************************************************************/
bool APDU::SelectAppCmd(const string &aid, APDU_RESPONSE &response)
{
	if (aid.length() % 2 != 0)
	{
		return false;	//AID有误
	}
	string len = Tool::GetStringLen(aid);
	string cmd = "00A40400" + len + aid;

	return SendAPDU(cmd, response);
}


/******************************************************************
* 初始化更新命令
*******************************************************************/
bool APDU::InitUpdateCmd(const string& randomNum, APDU_RESPONSE &response)
{
	if (randomNum.length() != 16)
	{
		return false;
	}
	string cmd = string("8050000008") + randomNum;

	return SendAPDU(cmd, response);
}

/******************************************************
* 根据给定的匹配/查找标准取得发行者安全域、可执行装载文件、
* 可执行模块、应用和安全域的生命周期的状态信息。
*******************************************************/
bool APDU::GetAppStatusCmd(vector<APP_STATUS> &status, APDU_RESPONSE& response)
{
	string cmd = "80F24000 02 4F00";

	if (SendAPDU(cmd, response))
	{
		string strResponse = response.data;
		unsigned int i = 0;
		while (i < strResponse.length())
		{
			APP_STATUS applicationStatus;
			int len = 2 * stoi(strResponse.substr(i, 2), 0, 16);
			applicationStatus.strAID = strResponse.substr(i + 2, len);
			applicationStatus.lifeCycleStatus = strResponse.substr(i + 2 + len, 2);
			applicationStatus.privilege = strResponse.substr(i + 4 + len, 2);
			status.push_back(applicationStatus);

			i += 6 + len;
		}

		return true;
	}

	return false;
}

/******************************************************
* 删除应用
*******************************************************/
bool APDU::DeleteAppCmd(const string& aid)
{
	APDU_RESPONSE response;

	string aidLen = Tool::GetStringLen(aid);
	string totalDataLen = Tool::IncStringLenStep(aidLen, 2);
	string cmd = "80E40000" + totalDataLen + "4F" + aidLen + aid;

	return SendAPDU(cmd, response);
}



/******************************************************
* 个人化数据命令
*******************************************************/
bool APDU::StoreDataCmd(const string& dgi,
	const string& dgiData,
	STORE_DATA_TYPE type,
	bool reset,
	APDU_RESPONSE& response)
{
	static int count = -1;		//用于计数
	if (reset) {
		count = 0;	//重置计数器
	}
	else {
		count++;
	}

	char szCount[3] = { 0 };
	sprintf_s(szCount, "%02X", count);
	string cmd = "80E2";
	switch (type)
	{
	case STORE_DATA_COMMON:		cmd += "00"; break;
	case STORE_DATA_ENCRYPT:	cmd += "60"; break;
	case STORE_DATA_LAST:		cmd += "80"; break;
	}
	cmd += string(szCount);     //构造Install Data头部命令
	string dataLen = Tool::GetStringLen(dgiData);

	if (dgiData.length() / 2 >= 0xFC)	//如果DGI分组数据过长，需要两次或多次上传
	{
		if (dgiData.length() / 2 > 0xFF)
		{
			dataLen = "FF" + dataLen;	//符合BER-TLV格式解析()，若操作0xFFFF，则需要另外处理
		}

		string data1 = dgi + dataLen + dgiData.substr(0, 0xDD * 2);    // DGI + 总数据长度 + 第一个存储的data数据
		string cmd1 = cmd + Tool::GetStringLen(data1) + data1;

		if (!SendAPDU(cmd1, response))
		{
			return false;
		}
		int remainderDataLen = dgiData.length() - 0xDD * 2;
		while (remainderDataLen > 0)
		{
			count++;
			memset(szCount, 0, sizeof(szCount));
			sprintf_s(szCount, "%02X", count);

			string data = dgiData.substr(0xDD * 2, 0xDD * 2);
			string cmd2 = cmd.substr(0, 6) + szCount + Tool::GetStringLen(data) + data;
			if (!SendAPDU(cmd2, response))
			{
				return false;
			}
			remainderDataLen -= data.length();
		}
	}
	else {
		cmd += Tool::IncStringLenStep(dataLen, 3) + dgi + dataLen + dgiData;
		if (!SendAPDU(cmd, response))
		{
			return false;
		}
	}

	return true;
}

/******************************************************
* 安装实例命令
*******************************************************/
bool APDU::InstallAppCmd(const string& package,
	const string& applet,
	const string& instance,
	const string& privilege,
	const string& installParam,
	const string& token,
	APDU_RESPONSE& response)
{
	char totalDataLen[3] = { 0 };	//数据总长度
	string packageLen = Tool::GetStringLen(package);
	string appletLen = Tool::GetStringLen(applet);
	string instanceLen = Tool::GetStringLen(instance);
	string privilegeLen = Tool::GetStringLen(privilege);
	string installParamLen = Tool::GetStringLen(installParam);
	string tokenLen = Tool::GetStringLen(token);

	sprintf_s(totalDataLen, "%02X", 6 + package.length() / 2 +
		applet.length() / 2 +
		instance.length() / 2 +
		privilege.length() / 2 +
		installParam.length() / 2 +
		token.length() / 2);

	string cmd = "80E60C00" + string(totalDataLen) +
		packageLen + package +
		appletLen + applet +
		instanceLen + instance +
		privilegeLen + privilege +
		installParamLen + installParam +
		tokenLen + token;

	return SendAPDU(cmd, response);
}

/****************************************************************
* 读取tag命令
*****************************************************************/
bool APDU::ReadTagCmd(const string &tag, APDU_RESPONSE &response)
{
	string cmd = "80CA";
	if (tag.length() != 2 && tag.length() != 4)
	{
		return false;
	}
	if (tag.length() == 2)
	{
		cmd += "00";
	}
	cmd += tag;

	return SendAPDU(cmd, response);
}

//内部认证 命令
bool APDU::InternalAuthCommand(const string &randNumber, APDU_RESPONSE &response)
{
	char szLen[16] = { 0 };
	sprintf_s(szLen, "%02X", randNumber.length() / 2);
	string strCommand = "00880000" + string(szLen) + randNumber;

	return SendAPDU(strCommand, response);
}

//外部认证 命令
bool APDU::ExternalAuthCommand(const string cardRandomNum,
	const string termRandomNum,
	const string sessionAuthKey,
	const string seesionMacKey,
	SECURE_LEVEL nSecureLevel,
	int nSCP,
	APDU_RESPONSE &response)
{
	//char szMac[17] = { 0 };
	//string strCommand;
	//string input = cardRandomNum + termRandomNum;
	//Full_3DES_CBC_MAC((char*)input.c_str(), (char*)sessionAuthKey.c_str(), "0000000000000000", szMac);
	//switch (nSecureLevel)
	//{
	//case SL_NO_SECURE:	strCommand = "8482000010"; break;
	//case SL_MAC:		strCommand = "8482010010"; break;
	//case SL_MAC_ENC:	strCommand = "8482030010"; break;
	//default:
	//	return false;
	//}
	//strCommand += szMac;
	//memset(szMac, 0, sizeof(szMac));
	//switch (nSCP)
	//{
	//case 1: Full_3DES_CBC_MAC((char*)strCommand.c_str(), (char*)sessionAuthKey.c_str(), "0000000000000000", szMac); break;
	//case 2: Common_MAC((char*)strCommand.c_str(), (char*)seesionMacKey.c_str(), "0000000000000000", szMac); break;
	//default:
	//	return false;
	//}
	//strCommand += szMac;

	//return SendAPDU(strCommand, response);
}

//外部认证
bool APDU::ExternalAuthcateCommand(const string ARPC, const string authCode, APDU_RESPONSE &response)
{
	if (ARPC.length() != 16 && authCode.length() != 4)
	{
		return false;
	}
	string cmd = "008200000A" + ARPC + authCode;

	return SendAPDU(cmd, response);
}

bool APDU::ReadRecordCommand(const string &SFI, const string strRecordNumber, APDU_RESPONSE &response)
{
	int temp = (stoi(SFI, 0, 16) << 3) + 4;
	char szP2[3] = { 0 };

	sprintf_s(szP2, "%02X", temp);

	string strCommand = "00B2" + strRecordNumber + string(szP2);

	return SendAPDU(strCommand, response);

}
bool APDU::PutDataCommand(const string &tag, const string &value, const string &mac)
{
	if (mac.length() % 8 != 0)
	{
		return false;
	}
	string cmd = _T("04DA");
	cmd += tag;
	string dataLen = Base::GetDataHexLen(value + mac);
	cmd += dataLen + value + mac;
	APDU_RESPONSE response;

	return SendAPDU(cmd, response) && (response.SW1 == 0x90 && response.SW2 == 0x00);
}

bool APDU::PutKeyCommand(const string keyVersion,
	const string authKeyWithKcv,
	const string macKeyWithKcv,
	const string encKeyWithKcv)
{
	string cmd = "80D8";
	if (keyVersion == "FF")
	{
		cmd += "00" + string("8143") + string("01");
	}
	else {
		cmd += keyVersion + "8143" + keyVersion;
	}
	cmd += authKeyWithKcv + macKeyWithKcv + encKeyWithKcv;

	APDU_RESPONSE response;
	memset(&response, 0, sizeof(response));

	return SendAPDU(cmd, response);
}

bool APDU::UpdateRecordCommand(const string &strCommand, APDU_RESPONSE &response)
{
	return false;
}

//获取处理选项 GPO命令
bool APDU::GPOCommand(const string &strCommand, APDU_RESPONSE &response)
{

	int dataLen = strCommand.length() / 2;
	int totalLen = dataLen + 2;
	char szTotalLen[3] = { 0 };
	char szDataLen[3] = { 0 };

	sprintf_s(szTotalLen, "%02X", totalLen);
	sprintf_s(szDataLen, "%02X", dataLen);

	string cmd = "80A80000" + string(szTotalLen) + "83" + string(szDataLen) + strCommand;

	return SendAPDU(cmd, response);
}

//生成应用密文 命令
bool APDU::GACCommand(GACControlParam parm1, const string terminalData, APDU_RESPONSE &response)
{
	char szDataLen[3] = { 0 };
	sprintf_s(szDataLen, "%02X", terminalData.length() / 2);
	string P1;
	switch (parm1)
	{
	case AAC:
		P1 = "00";
		break;
	case ARQC:
		P1 = "80";
		break;
	case TC:
		P1 = "40";
		break;
	case CDA:
		P1 = "10";
		break;
	default:
		break;
	}
	string cmd = "80AE" + P1 + "00" + szDataLen + terminalData;

	return SendAPDU(cmd, response);
}

bool APDU::WriteDataCommand(const string &strCommand, APDU_RESPONSE &response)
{
	return false;
}

//APDU响应报文
bool APDU::GetAPDUResponseCommand(unsigned int len, unsigned char* szResponse, DWORD* pResponseLen)
{
	char szLen[3] = { 0 };
	//_itoa_s(len, szLen, 16);
	sprintf_s(szLen, "%02X", len);
	string szCommand = "00C00000" + string(szLen);
	if (*pResponseLen < len)
	{
		m_Error = "接收APDU响应数据的缓冲区过小";
		return false;
	}
	return SendAPDU(szCommand, szResponse, pResponseLen);
}

/***************************************************************
* 发送APDU指令
****************************************************************/
bool APDU::SendAPDU(string &strCommand, APDU_RESPONSE &response)
{
	bool bResult = false;
	unsigned char SW1, SW2;

	const int outputLen = 1024;
	DWORD dwResponseLen = outputLen;
	unsigned char szResponse[outputLen] = { 0 };
	Log->Info("%s", strCommand.c_str());
	if (!SendAPDU(strCommand, szResponse, &dwResponseLen))
	{
		return false;
	}

	SW1 = szResponse[dwResponseLen - 2];
	SW2 = szResponse[dwResponseLen - 1];

	if (SW1 == 0x61)	//通过00c0命令获取响应数据, SW2为响应数据返回的长度
	{
		memset(szResponse, 0, outputLen);
		dwResponseLen = outputLen;
		bResult = GetAPDUResponseCommand(SW2, szResponse, &dwResponseLen);
		SW1 = szResponse[dwResponseLen - 2];
		SW2 = szResponse[dwResponseLen - 1];
	}
	else if (SW1 == 0x6C)	//通过原先的命令获取响应数据, SW2为响应数据返回的长度
	{
		memset(szResponse, 0, outputLen);
		dwResponseLen = outputLen;
		char temp[3] = { 0 };
		_itoa_s(SW2, temp, 16);
		sprintf_s(temp, 3, "%02X", SW2);
		strCommand.append(temp);
		bResult = SendAPDU(strCommand, szResponse, &dwResponseLen);
		SW1 = szResponse[dwResponseLen - 2];
		SW2 = szResponse[dwResponseLen - 1];
	}
	else {
		bResult = true;
	}
	memset(response.data, 0, MAX_DATA_LEN);
	Tool::BcdToAsc(response.data, (char*)szResponse, dwResponseLen * 2 - 4);
	response.SW1 = SW1;
	response.SW2 = SW2;

	return bResult & (SW1 == 0x90 && SW2 == 0x00);
}

bool APDU::SendAPDU(string strCommand, unsigned char* szResponse, DWORD* pResponseLen)
{
	HRESULT hRet = S_FALSE;

	//删除字符串中的空格
	strCommand = Tool::DeleteSpace(strCommand);
	if (strCommand.length() < 8 || strCommand.length() % 2 != 0)
	{
		Log->Error("APDU Command length error. [%s]", strCommand.c_str());
		return false;
	}
	unsigned char szAPDU[256] = { 0 };
	Tool::AscToBcd(szAPDU, (unsigned char*)strCommand.c_str(), strCommand.length());

	switch (m_dwActiveProtocol)
	{
	case T0:
		hRet = SCardTransmit(m_scardHandle, SCARD_PCI_T0, szAPDU, strCommand.length() / 2, NULL, szResponse, pResponseLen);
		break;
	case T1:
		hRet = SCardTransmit(m_scardHandle, SCARD_PCI_T1, szAPDU, strCommand.length() / 2, NULL, szResponse, pResponseLen);
		break;
	default:
		Log->Warning("数据传输协议未知 %d", m_dwActiveProtocol);
		return false;
	}
	if (hRet != SCARD_S_SUCCESS)
	{
		return false;
	}

	return true;
}

string APDU::GetAPDUError()
{
	return m_Error;
}

//获取APDU错误信息
void APDU::GetAPDUError(unsigned char SW1, unsigned char SW2, char* szError, const int len)
{
	memset(szError, 0x00, len);
	switch (SW1)
	{
	case 0x61:
		sprintf_s(szError, len, "Command processed without error. The command prepared %02X bytes of data that can be retrieved using the Get Response command", SW2);
		break;
	case 0x62:
		if (SW2 == 0x81)
			sprintf_s(szError, len, "the relevant data may be corrupted");//FCI文件创建不正确选DF时报该错
		else
			sprintf_s(szError, len, "Unknown error!");
		break;
	case 0x63:
		if (SW2 == 0x00)
			sprintf_s(szError, len, "Authentication failed");
		else
		{
			SW2 = SW2 - 0xC0;
			sprintf_s(szError, len, "Authentication failed, %d tries left", SW2);
		}
		break;
	case 0x64:
		if (SW2 == 0x00)
			sprintf_s(szError, len, "Wrong context(for PK Crypto commands)");
		else
			sprintf_s(szError, len, "Unkonwn error!");
		break;
	case 0x65:
		if (SW2 == 0x81)
			sprintf_s(szError, len, "Write problem / Memory failure (ISO class byte)");
		else
			sprintf_s(szError, len, "Unkonwn error!");
		break;
	case 0x67:
		if (SW2 == 0x00)
			sprintf_s(szError, len, "Incorrect length (Lc or Le)");
		else
			sprintf_s(szError, len, "Unkonwn error!");
		break;
	case 0x69:
		switch (SW2)
		{
		case 0x00:sprintf_s(szError, len, "Unable to process"); break;
		case 0x01:sprintf_s(szError, len, "Command not allowed, invalid state"); break;
		case 0x81:sprintf_s(szError, len, "Command incompatible with file organization"); break;
		case 0x82:sprintf_s(szError, len, "Security status not satisfied"); break;
		case 0x83:sprintf_s(szError, len, "Authentication method blocked"); break;
		case 0x84:sprintf_s(szError, len, "Challenge not present or PIN is blocked"); break;
		case 0x85:sprintf_s(szError, len, "Conditions of use not satisfied"); break;//GemGold R3 PBOC EF15文件是否支持ED/EP等
		case 0x86:sprintf_s(szError, len, "No current EF selected"); break;
		case 0x87:sprintf_s(szError, len, "Secure messaging object missing (that is, MAC missing)"); break;
		case 0x88:sprintf_s(szError, len, "Secure messaging data object incorrect"); break;
		default:sprintf_s(szError, len, "Unkonwn error!"); break;
		}
		break;
	case 0x6A:
		switch (SW2)
		{
		case 0x80:sprintf_s(szError, len, "Incorrect parameters in the data field"); break;
		case 0x81:sprintf_s(szError, len, "Function not supported, invalidated file"); break;
		case 0x82:sprintf_s(szError, len, "File not found"); break;
		case 0x83:sprintf_s(szError, len, "Record not found"); break;
		case 0x84:sprintf_s(szError, len, "Not enough memory space in the file"); break;
		case 0x86:sprintf_s(szError, len, "Incorrect parameters(P1 & P2)"); break;
		case 0x88:sprintf_s(szError, len, "Referenced data not found"); break;
		default:sprintf_s(szError, len, "Unkonwn error!"); break;
		}
		break;
	case 0x6B:
		if (SW2 == 0x00)
			sprintf_s(szError, len, "Incorrect offset");
		else
			sprintf_s(szError, len, "Unkonwn error!");
		break;
	case 0x6C:
		sprintf_s(szError, len, "Repeat the command using Le=%02X", SW2);
		break;
	case 0x6D:
		if (SW2 == 0x00)
			sprintf_s(szError, len, "Unknown instruction code(command is not present)");
		else
			sprintf_s(szError, len, "Unkonwn error!");
		break;
	case 0x6E:
		if (SW2 == 0x00)
			sprintf_s(szError, len, "Wrong instruction class given in the command");
		else
			sprintf_s(szError, len, "Unkonwn error!");
		break;
	case 0x6F:
		if (SW2 == 0x00)
			sprintf_s(szError, len, "No data available for GET RESPONSE");
		else
			sprintf_s(szError, len, "Unkonwn error!");
		break;
	case 0x90:
		//sprintf(info,"命令执行成功");
		break;
	case 0x93:
		switch (SW2) {
		case 0x02:
			sprintf_s(szError, len, "Invalid MAC.");
			break;
		case 0x03:
			sprintf_s(szError, len, "Application permanently blocked.");
			break;
		default:
			sprintf_s(szError, len, "Unkonwn error!");
			break;
		}
		break;
	case 0x94:
		switch (SW2) {
		case 0x01:
			sprintf_s(szError, len, "Insufficient funds");
			break;
		case 0x02:
			sprintf_s(szError, len, "Transaction counter overflow");
			break;
		case 0x03:
			sprintf_s(szError, len, "Key Index not supported");//可能密钥不存在或无TAC密钥
			break;
		case 0x06:
			sprintf_s(szError, len, "Requested TAC/MAC not available");
			break;
		default:
			sprintf_s(szError, len, "Unkonwn error!");
			break;
		}
		break;
	default:
		sprintf_s(szError, len, "Unkonwn error!");
		break;
	}
	return;
}
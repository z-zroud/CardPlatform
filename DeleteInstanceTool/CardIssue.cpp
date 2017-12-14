#include "stdafx.h"
#include "CardIssue.h"
#include "Tool.h"
#include "Interface.hpp"

#define STORE_DATA_COMM     0
#define STORE_DATA_ENCRYPT  1
#define STORE_DATA_LAST     2
#define FILE_1E     0       //1E文件
#define FILE_1A     1       //0A文件

#define EC	0
#define EP	1

#define ED_UP_LIMIT		0
#define EP_UP_LIMIT		1



#define LEN		1024

CardIssue::CardIssue()
{

}


CardIssue::~CardIssue()
{
}

/*********************************************************
* 发送APDU指令
**********************************************************/
bool CardIssue::SendApdu(string& cmd, string& out)
{
	char result[LEN] = { 0 };
	cmd = Tool::DeleteSpace(cmd);
	//LogInfo("APDU: %s %s %s", cmd.substr(0, 8).c_str(), cmd.substr(8, 2).c_str(), cmd.substr(10).c_str());
	int apduRet = -999;
	apduRet = m_card_apdu((char*)cmd.c_str(), result);
	if (apduRet != 0)
	{
		//m_errorInfo = //LogError("Send APDU %s failed. m_card_apdu return : %d", cmd.c_str(), apduRet);
		char info[1024] = { 0 };
		sprintf(info, "Send APDU: %s failed.\n APDU return sw: %s", cmd.c_str(), result);
		m_ErrorInfo = info;
		return false;
	}
	//LogDebug("Apdu return: %s", result);
	string tmp = result;
	if (GetApduResult(result)) 
	{
		out = tmp.substr(0, tmp.length() - 4);
		//LogInfo("OK");
		return true;
	}
	else
	{
		m_ErrorInfo = "Send APDU " + cmd + "return error code: " + tmp.substr(tmp.length() - 4, 4);
		return false;
	}
	return false;
}

/*******************************************************
* 判断APDU指令返回结果
********************************************************/
bool CardIssue::GetApduResult(string data)
{
	if (data.length() < 4)
	{
		return false;
	}
	if (data.substr(data.length() - 4, 4) == "9000")
	{
		return true;
	}

	return false;
}

/****************************************************
* 模拟接口发送APDU指令
*****************************************************/
#ifdef PCSC
DWORD CardIssue::m_card_apdu(char* in, char* out)
{
	APDU_RESPONSE response;
	if (SendAPDU(string(in), response))
	{
		sprintf_s(out, LEN, "%s%02X%02X", response.data, response.SW1, response.SW2);
		return 0;
	}
	
	return -1;
}
#endif
/********************************************************
* 初始化回调指针，供后续应用
*********************************************************/
bool CardIssue::Init()
{
    m_card_verify			= GetDllFunc<Func_card_verify>("card_verify", "des_rsa_jmj.dll");
	m_generate_en_kmc		= GetDllFunc<Func_generate_en_kmc>("generate_en_kmc", "des_rsa_jmj.dll");
	return 
		m_card_verify &&
		m_generate_en_kmc;
}

bool CardIssue::ChangeKmc()
{
	string result;
	string cmd = "00A40400 00";
	if (!SendApdu(cmd, result))
	{
		return false;
	}
	if (!ExtAuth())
	{
		m_ErrorInfo = "change kmc external auth failed.\n";
		return false;
	}

	string input1 = m_keyData + "F001" + m_keyData + "0F01";
	string input2 = m_keyData + "F002" + m_keyData + "0F02";
	string input3 = m_keyData + "F003" + m_keyData + "0F03";
	string input = m_dekKey + input1 + input2 + input3;
	char output[1024] = { 0 };
	int ret = m_generate_en_kmc((char*)input.c_str(), output);
	if (ret != 0)
	{
		//Log->Error("call generate_en_kmc failed. error code %d", ret);
		char result[32] = { 0 };
		itoa(ret, result, 10);
		m_ErrorInfo = "call generate_en_kmc failed. return code: " + string(result);
		return false;
	}
	string keys = output;
	string enc = keys.substr(0, 32);
	string mac = keys.substr(32, 32);
	string dek = keys.substr(64, 32);
	string encKcv = keys.substr(96, 6);
	string macKcv = keys.substr(102, 6);
	string dekKcv = keys.substr(108, 6);

	string putkeyCmd = "80D8018143" + m_keyVersion + "8010" + enc + "03" + encKcv
		+ "8010" + mac + "03" + macKcv
		+ "8010" + dek + "03" + dekKcv;

	if (!SendApdu(putkeyCmd, result))
	{
		m_ErrorInfo = "put key failed.";
		return false;
	}
	//Log->Info("change kmc sucess.");
	return true;
}

/*********************************************************
* 外部认证
**********************************************************/
bool CardIssue::ExtAuth()
{
	string result;
	const string termianlRandom = "0102030405060709";
	string initUpdateCmd = "8050000008" + termianlRandom;
	if (SendApdu(initUpdateCmd, result))
	{		
		if (result.length() < 40)
		{
			return false;
		}
		m_keyData				= result.substr(8, 12);
		m_keyVersion			= result.substr(20, 2);
		string seqNo			= result.substr(24, 4);
		string cardChallenge	= result.substr(28, 12);
		string verifyInfo		= m_keyData + seqNo + seqNo + cardChallenge + termianlRandom + "8000000000000000";
		
		//LogDebug("card verify input: %s", verifyInfo.c_str());
		char output[LEN] = { 0 };
		int ret = m_card_verify((char*)verifyInfo.c_str(), output);
		if (ret == 0)
		{
			string tmp = output;
			m_dekKey = tmp.substr(96, 32);
			string cmd = "8482000010" + tmp.substr(128, 32);
			if (SendApdu(cmd, result))
			{
				//LogInfo("Ext Auth success");
				return true;
			}				
		}
		else {
			//m_errorInfo = //LogError("call m_card_verify failed. card_verify return code: %d", ret);
			char result[32] = { 0 };
			itoa(ret, result, 10);
			m_ErrorInfo = "call m_card_verify failed. card_verify return code: " + string(result);
			return false;
		}
	}
	//m_errorInfo = //LogError("Ext Auth failed %s", result.c_str());

	return false;
}




#ifdef PCSC
bool CardIssue::GetAPDUResponseCommand(unsigned int len, unsigned char* szResponse, DWORD* pResponseLen)
{
	char szLen[3] = { 0 };
	//_itoa_s(len, szLen, 16);
	sprintf_s(szLen, "%02X", len);
	string szCommand = "00C00000" + string(szLen);
	if (*pResponseLen < len)
	{
		//m_Error = "接收APDU响应数据的缓冲区过小";
		return false;
	}
	return SendAPDU(szCommand, szResponse, pResponseLen);
}

//发送APDU命令
bool CardIssue::SendAPDU(string &strCommand, APDU_RESPONSE &response)
{
	bool bResult = false;
	unsigned char SW1, SW2;

	const int outputLen = LEN;
	DWORD dwResponseLen = outputLen;
	unsigned char szResponse[outputLen] = { 0 };
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

	return bResult;
}

bool CardIssue::SendAPDU(string strCommand, unsigned char* szResponse, DWORD* pResponseLen)
{
	HRESULT hRet = S_FALSE;

	//删除字符串中的空格
	strCommand = Tool::DeleteSpace(strCommand);
	if (strCommand.length() < 8 || strCommand.length() % 2 != 0)
	{
		////LogError("APDU Command length error. [%s]", strCommand.c_str());
		return false;
	}
	unsigned char szAPDU[LEN] = { 0 };
	Tool::AscToBcd(szAPDU, (unsigned char*)strCommand.c_str(), strCommand.length());
	////LogInfo("%s  %s %s\n",strCommand.substr(0,8).c_str(), strCommand.substr(8,2).c_str(), strCommand.substr(10).c_str());
	switch (m_dwActiveProtocol)
	{
	case T0:
		hRet = SCardTransmit(m_scardHandle, SCARD_PCI_T0, szAPDU, strCommand.length() / 2, NULL, szResponse, pResponseLen);
		break;
	case T1:
		hRet = SCardTransmit(m_scardHandle, SCARD_PCI_T1, szAPDU, strCommand.length() / 2, NULL, szResponse, pResponseLen);
		break;
	default:
		////LogWarning("数据传输协议未知 %d", m_dwActiveProtocol);
		return false;
	}
	if (hRet != SCARD_S_SUCCESS)
	{
		return false;
	}

	return true;
}

//建立资源管理器上下文
BOOL CardIssue::EstablishContext()
{
	HRESULT hResult = SCARD_F_INTERNAL_ERROR;
	hResult = SCardEstablishContext(SCARD_SCOPE_SYSTEM,//SCARD_SCOPE_USER,	//数据库操作在用户作用域
		NULL,	//保留
		NULL,	//保留
		&m_scardContextHandle);	//上下文句柄
	if (hResult != SCARD_S_SUCCESS)
	{
		////LogError("SCardEstablishContext Fail! RetCode %08X,Windows的智能卡服务未启动", hResult);
		return FALSE;
	}
	return TRUE;
}

//释放资源管理器上下文
void CardIssue::RealseContext()
{
	if (m_scardContextHandle != NULL)
	{
		SCardReleaseContext(m_scardContextHandle);
	}
}

//获取所有读卡器的名称
vector<char*> CardIssue::GetAllReaders()
{
	DWORD nReadersLen = 256;
	char szReaders[MAX_READERS_LEN] = { 0 };
	EstablishContext();
	HRESULT hRet = SCardListReaders(m_scardContextHandle,
		NULL,
		szReaders,
		&nReadersLen);
	if (hRet != SCARD_S_SUCCESS)
	{
		////LogError("SCardListReaders Fail! RetCode %08X,请检查Windows的智能卡服务", hRet);
		return m_szReaderNames;
	}
	char* szReaderName = szReaders;
	DWORD i = 0;
	m_szReaderNames.clear();
	while (szReaderName[i] != '\0' && i < nReadersLen)
	{
		i++;
		if (szReaderName[i] == '\0')
		{
			////LogInfo("读取读卡器名称: %s", szReaderName);

			//复制一份读卡器名字到vector中
			char* temp = new char[256];
			memset(temp, 0, 256);
			memcpy(temp, szReaderName, strlen(szReaderName));
			m_szReaderNames.push_back(temp);
			szReaderName += i + 1;
		}
	}

	return m_szReaderNames;
}

//连接IC卡
BOOL CardIssue::ConnectSmartCard()
{
	DWORD dwActiveProtocol = 0;
	HRESULT hResult = SCARD_F_INTERNAL_ERROR;
	hResult = SCardConnect(m_scardContextHandle,
		m_scardReaderName,
		SCARD_SHARE_SHARED,
		SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
		&m_scardHandle,
		&dwActiveProtocol);
	if (hResult != SCARD_S_SUCCESS)
	{
		return FALSE;
	}

	m_dwActiveProtocol = (CARD_TRANSMISSION_PROTOCOL)dwActiveProtocol;
	//m_APDU = new APDU(m_scardHandle, m_dwActiveProtocol);

	return TRUE;
}

//打开读卡器，连接IC卡
bool CardIssue::OpenReader(const char* szReaderName)
{
	DWORD dwActiveProtocol = 0;
	HRESULT hResult = SCARD_F_INTERNAL_ERROR;
	hResult = SCardConnect(m_scardContextHandle,
		szReaderName,
		SCARD_SHARE_SHARED,
		SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
		&m_scardHandle,
		&dwActiveProtocol);
	if (hResult != SCARD_S_SUCCESS)
	{
		return false;
	}

	m_dwActiveProtocol = (CARD_TRANSMISSION_PROTOCOL)dwActiveProtocol;
	//m_APDU = new APDU(m_scardHandle, m_dwActiveProtocol);

	return true;
}

void CardIssue::ResetCard(const char* szReaderName)
{
	CloseReader();
	OpenReader(szReaderName);
}

//关闭读卡器
void CardIssue::CloseReader()
{
	HRESULT hRet = SCardDisconnect(m_scardHandle, SCARD_LEAVE_CARD);
	if (hRet != SCARD_S_SUCCESS)
	{
		////LogWarning("关闭读卡器失败");
	}
}

#endif


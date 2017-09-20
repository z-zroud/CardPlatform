#include "stdafx.h"
#include "PCSC.h"
#include "APDU.h"
#include "KeyGenerator.h"
#include "Util\Log.h"
#include "Util\Converter.h"
#include "Util\StringParaser.h"
#include "Util\Des0.h"
#include <bitset>
using namespace std;

PCSC::PCSC()
{
	m_scardContextHandle = NULL;
	EstablishContext();
}

PCSC::~PCSC()
{
	RealseContext();
}

//建立资源管理器上下文
BOOL PCSC::EstablishContext()
{
	HRESULT hResult = SCARD_F_INTERNAL_ERROR;
	hResult = SCardEstablishContext(SCARD_SCOPE_SYSTEM,//SCARD_SCOPE_USER,	//数据库操作在用户作用域
		NULL,	//保留
		NULL,	//保留
		&m_scardContextHandle);	//上下文句柄
	if (hResult != SCARD_S_SUCCESS)
	{
		Log->Error("SCardEstablishContext Fail! RetCode %08X,Windows的智能卡服务未启动", hResult);
		return FALSE;
	}
	return TRUE;
}

//释放资源管理器上下文
void PCSC::RealseContext()
{
	if (m_scardContextHandle != NULL)
	{
		SCardReleaseContext(m_scardContextHandle);
	}	
}

//获取所有读卡器的名称
vector<char*> PCSC::GetAllReaders()
{
	DWORD nReadersLen = MAX_READERS_LEN;
	char szReaders[MAX_READERS_LEN] = { 0 };

	HRESULT hRet = SCardListReaders(m_scardContextHandle,
		NULL,
		szReaders,
		&nReadersLen);
	if (hRet != SCARD_S_SUCCESS)
	{
		Log->Error("SCardListReaders Fail! RetCode %08X,请检查Windows的智能卡服务", hRet);
		return m_szReaderNames;
	}
	char* szReaderName = szReaders;
	DWORD i = 0;
    m_szReaderNames.clear();
	while (szReaderName[i] != _T('\0') && i < nReadersLen)
	{
		i++;
		if (szReaderName[i] == _T('\0'))
		{
			Log->Info("读取读卡器名称: %s", szReaderName);

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
BOOL PCSC::ConnectSmartCard()
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
	m_APDU = new APDU(m_scardHandle, m_dwActiveProtocol);

	return TRUE;
}

//打开读卡器，连接IC卡
bool PCSC::OpenReader(const char* szReaderName)
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
	m_APDU = new APDU(m_scardHandle, m_dwActiveProtocol);

	return true;
}


//关闭读卡器
void PCSC::CloseReader()
{
	HRESULT hRet = SCardDisconnect(m_scardHandle, SCARD_UNPOWER_CARD);
	if (hRet != SCARD_S_SUCCESS)
	{
		Log->Warning("关闭读卡器失败");
	}
}

//获取ATR字符串
string PCSC::GetATR()
{
	char			szHexATR[256] = { 0 };
	BYTE			szATR[SCARD_ATR_LENGTH] = { 0 };
	DWORD			szReaderNameLenght = MAX_READER_NAME_LEN;
	DWORD			dwATRLen;
	DWORD			dwProtocol;
	DWORD			dwState;

	HRESULT hRet = SCardStatus(m_scardHandle,			//智能卡连接句柄
		m_scardReaderName,	//读卡器名称
		&szReaderNameLenght, //读卡器名称长度
		&dwState,	//智能卡的状态
		&dwProtocol, //智能卡的协议，当前卡的状态须是SCARD_SPECIFIC
		szATR,	//32字节的ATR字符串
		&dwATRLen);
	if (hRet != SCARD_S_SUCCESS)
	{
		Log->Error("GetATR Fail! RetCode %08X, 请检查读卡器和IC卡", hRet);
		return "";
	}
	for (unsigned int i = 0; i < dwATRLen; i++)
	{
		char temp[3] = { 0 };
		_snprintf_s(temp, SCARD_ATR_LENGTH, "%02X", szATR[i]);
		strcat_s(szHexATR, 256, temp);
	}
	Log->Info("ATR: %s", szHexATR);

	return string(szHexATR);
}

//获取卡片状态
CARD_STATUS PCSC::GetCardStatus()
{
	BYTE			szATR[SCARD_ATR_LENGTH] = { 0 };
	char			szHexATR[SCARD_ATR_LENGTH] = { 0 };
	DWORD			szReaderNameLenght = MAX_READER_NAME_LEN;
	DWORD			dwATRLen;
	DWORD			dwProtocol;
	DWORD			dwState;

	HRESULT hRet = SCardStatus(m_scardHandle,			//智能卡连接句柄
		m_scardReaderName,	//读卡器名称
		&szReaderNameLenght, //读卡器名称长度
		&dwState,	//智能卡的状态
		&dwProtocol, //智能卡的协议，当前卡的状态须是SCARD_SPECIFIC
		szATR,	//32字节的ATR字符串
		&dwATRLen);
	if (hRet != SCARD_S_SUCCESS)
	{
		Log->Error("GetATR Fail! RetCode %08X, 请检查读卡器和IC卡", hRet);
		return CARD_STATUS::UNKNOWN;
	}
	switch (dwState)
	{
	case SCARD_UNKNOWN:		return CARD_STATUS::UNKNOWN;
	case SCARD_ABSENT:		return CARD_STATUS::ABSENT;
	case SCARD_PRESENT:		return CARD_STATUS::PRESENT;
	case SCARD_SWALLOWED:	return CARD_STATUS::SWALLOWED;
	case SCARD_POWERED:		return CARD_STATUS::POWERED;
	case SCARD_NEGOTIABLE:	return CARD_STATUS::NEGOTIABLE;
	case SCARD_SPECIFIC:	return CARD_STATUS::SPECIFIC;
	default:
		break;
	}

	return CARD_STATUS::UNKNOWN;
}

//获取读卡器与卡片之间的数据传输协议
CARD_TRANSMISSION_PROTOCOL PCSC::GetCardTransmissionProtocol()
{
	BYTE			szATR[SCARD_ATR_LENGTH] = { 0 };
	char			szHexATR[SCARD_ATR_LENGTH] = { 0 };
	DWORD			szReaderNameLenght = MAX_READER_NAME_LEN;
	DWORD			dwATRLen;
	DWORD			dwProtocol;
	DWORD			dwState;
	HRESULT hRet = SCardStatus(m_scardHandle,			//智能卡连接句柄
		m_scardReaderName,	//读卡器名称
		&szReaderNameLenght, //读卡器名称长度
		&dwState,	//智能卡的状态
		&dwProtocol, //智能卡的协议，当前卡的状态须是SCARD_SPECIFIC
		szATR,	//32字节的ATR字符串
		&dwATRLen);
	if (hRet != SCARD_S_SUCCESS)
	{
		Log->Error("GetATR Fail! RetCode %08X, 请检查读卡器和IC卡", hRet);
		return CARD_TRANSMISSION_PROTOCOL::UNDEFINED;
	}
	switch (dwProtocol)
	{
	case SCARD_PROTOCOL_UNDEFINED:	return CARD_TRANSMISSION_PROTOCOL::UNDEFINED;
	case SCARD_PROTOCOL_T0:			return CARD_TRANSMISSION_PROTOCOL::T0;
	case SCARD_PROTOCOL_T1:			return CARD_TRANSMISSION_PROTOCOL::T1;
	case SCARD_PROTOCOL_RAW:		return CARD_TRANSMISSION_PROTOCOL::RAW;
	default:
		break;
	}

	return CARD_TRANSMISSION_PROTOCOL::UNDEFINED;
}

//重置KMC
bool PCSC::SetKMC(const string &strKMC, DIV_METHOD_FLAG flag)
{
    string authKey;
    string macKey;
    string encKey;

    if (m_keyVersion.empty() || m_divFactor.empty() || m_KEK_Key.empty())
    {
        return false;
    }
    IKeyGenerator* pKg = new KeyGenerator();
    pKg->GenAllSubKey(strKMC, m_divFactor, flag, authKey, macKey, encKey);

    char szAuthKeyKcv[17] = { 0 };
    char szMacKeyKcv[17] = { 0 };
    char szEncKeyKcv[17] = { 0 };

    char szEncAuthKey[33] = { 0 };
    char szEncMacKey[33] = { 0 };
    char szEncEncKey[33] = { 0 };

    Des3(szAuthKeyKcv, (char*)authKey.c_str(), "0000000000000000");
    string authKeyKcv = string(szAuthKeyKcv).substr(0, 6);
    Des3_ECB(szEncAuthKey, (char*)m_KEK_Key.c_str(), (char*)authKey.c_str(), 32);

    Des3(szMacKeyKcv, (char*)macKey.c_str(), "0000000000000000");
    string macKeyKcv = string(szMacKeyKcv).substr(0, 6);
    Des3_ECB(szEncMacKey, (char*)m_KEK_Key.c_str(), (char*)macKey.c_str(), 32);

    Des3(szEncKeyKcv, (char*)encKey.c_str(), "0000000000000000");
    string encKeyKcv = string(szEncKeyKcv).substr(0, 6);
    Des3_ECB(szEncEncKey, (char*)m_KEK_Key.c_str(), (char*)encKey.c_str(), 32);

    string key1;
    string key2;
    string key3;

    //默认采用8010
    string header = "8010";
    key1 = header + szEncAuthKey + string("03") + authKeyKcv;
    key2 = header + szEncMacKey + string("03") + macKeyKcv;
    key3 = header + szEncEncKey + string("03") + encKeyKcv;

    return m_APDU->PutKeyCommand(m_keyVersion, key1, key2, key3);
}

//获取卡片安全等级
SECURE_LEVEL PCSC::GetSecureLevel()
{
	return SECURE_LEVEL::SL_NO_SECURE;
}



//获取操作错误信息
string PCSC::GetLastError()
{
	return m_Error;
}

//应用选择
bool PCSC::SelectAID(const string &strAID)
{
	APDU_RESPONSE output;
	memset(&output, 0, sizeof(output));
	
	if (!m_APDU->SelectAppCmd(strAID, output))
	{
		m_Error = m_APDU->GetAPDUError();
		return false;
	}

	return true;
}


//打开安全通道
bool PCSC::OpenSecureChannel(const string &strKMC)
{
	if (strKMC.size() != 32)
	{
		m_Error = "KMC[" + strKMC + "]长度错误";
		return false;
	}
	//更新初始化
	APDU_RESPONSE output;
    static int count = 1;
    char szTemp[3] = { 0 };
    sprintf_s(szTemp, 3, "%02X", count);
	string termRandomNum = "1122334455667788";
    termRandomNum = termRandomNum.substr(2) + szTemp;
    count++;
	memset(&output, 0, sizeof(output));
	if (!m_APDU->InitUpdateCmd(termRandomNum, output))
	{
		return false;
	}

	//解析更新初始化响应数据
	string initUpdateResponseData = output.data;
	m_divFactor = initUpdateResponseData.substr(0, 20);	//分散数据
    m_keyVersion = initUpdateResponseData.substr(20, 2);    //密钥版本
	string strSCP = initUpdateResponseData.substr(22, 2);
	int nSCP = atoi((char*)strSCP.c_str());		//卡片安全通道协议
	string cardRandomNum = initUpdateResponseData.substr(24, 16);	//卡片随机数
	string strCardMac = initUpdateResponseData.substr(40, 16);	//卡片MAC值


	//校验卡片MAC值
	bool bFind = false;	
	KeyGenerator kg;
	for (int i = 0; i < DIV_METHOD_FLAG::DIV_MAX; i++)
	{
		kg.GenAllSubKey(strKMC, m_divFactor, (DIV_METHOD_FLAG)i, m_AuthKey, m_MacKey, m_EncKey);
		kg.GenAllSessionKey(cardRandomNum, termRandomNum, nSCP, m_AuthKey, m_MacKey, m_EncKey, m_sessionAuthKey, m_sessionMacKey, m_sessionEncKey);
		if (VlidateCardMAC(strCardMac,termRandomNum,cardRandomNum))
		{
			m_flag = (DIV_METHOD_FLAG)i;
			bFind = true;
			break;
		}
	}
	if (!bFind)
	{
		return false;
	}

    if (nSCP == 1)
    {
        m_KEK_Key = m_EncKey;
    }
    else if (nSCP == 2)
    {
        m_KEK_Key = m_sessionEncKey;
    }

	
	//外部认证
	memset(&output, 0, sizeof(output));
	if (!m_APDU->ExternalAuthCommand(cardRandomNum, termRandomNum, m_sessionAuthKey, m_sessionMacKey, SL_NO_SECURE, nSCP, output))
	{
		return false;
	}

	return true;
}

//校验卡片MAC值
bool PCSC::VlidateCardMAC(string szMac, string termRandomNum, string cardRandomNum)
{
	string randomNum = termRandomNum + cardRandomNum;
	char szGenMac[17] = { 0 };
	Full_3DES_CBC_MAC((char*)randomNum.c_str(), (char*)m_sessionAuthKey.c_str(), "0000000000000000", szGenMac);

	return szMac == string(szGenMac);
}





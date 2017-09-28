// PCSC.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "PCSC.h"
#include "Tool.h"
#include <stdio.h>
#include <winscard.h>

#pragma comment(lib,"WINSCARD.LIB")

SCARDCONTEXT g_scardContext = NULL;
SCARDHANDLE g_scardHandle = NULL;
DWORD g_dwActiveProtocol = 0;

char g_readerName[128] = { 0 };     //保存当前打开的读卡器

//建立资源管理器上下文
BOOL EstablishContext()
{
    HRESULT hResult = SCARD_F_INTERNAL_ERROR;
    hResult = SCardEstablishContext(SCARD_SCOPE_SYSTEM,//SCARD_SCOPE_USER,	//数据库操作在用户作用域
        NULL,	//保留
        NULL,	//保留
        &g_scardContext);	//上下文句柄
    if (hResult != SCARD_S_SUCCESS)
    {
        //Log->Error("SCardEstablishContext Fail! RetCode %08X,Windows的智能卡服务未启动", hResult);
        return FALSE;
    }
    return TRUE;
}

//释放资源管理器上下文
void RealseContext()
{
    if (g_scardContext != NULL)
    {
        SCardReleaseContext(g_scardContext);
        g_scardContext = NULL;
    }
}

extern "C" int GetReaders(char** readers, int& count)
{
    DWORD nReadersLen = 1024;
    char szReaders[1024] = { 0 };

	if (g_scardContext == NULL)
	{
		EstablishContext();
	}
    HRESULT hRet = SCardListReaders(g_scardContext,
        NULL,
        szReaders,
        &nReadersLen);
    if (hRet != SCARD_S_SUCCESS)
    {
        return -1;
    }

    char* szReaderName = szReaders;
    int pos = 0;
    int num = 0;
    while (szReaderName[pos] != '\0' && pos < nReadersLen)
    {
        pos++;
        if (szReaderName[pos] == '\0')
        {
			int len = strlen(szReaderName);
			readers[num] = new char[len + 1];
			memset(readers[num], 0, len + 1);
            memcpy(readers[num], szReaderName, len);
            num++;           
            szReaderName += pos + 1;
        }
    }
	count = num;

    return 0;
}

/****************************************************
* 功能： 热复位，SCARD_LEAVE_CARD
*****************************************************/
bool WarmReset()
{
    HRESULT hRet = SCardDisconnect(g_scardHandle, SCARD_LEAVE_CARD);
    if (hRet != SCARD_S_SUCCESS)
    {
        return false;
    }
    return OpenReader(g_readerName);
}

/****************************************************
* 功能： 冷复位，SCARD_UNPOWER_CARD 断电复位
*****************************************************/
bool ColdReset()
{
    HRESULT hRet = SCardDisconnect(g_scardHandle, SCARD_UNPOWER_CARD);
    if (hRet != SCARD_S_SUCCESS)
    {
        return false;
    }
    return OpenReader(g_readerName);
}

/***************************************************
* 功能： 选择某个读卡器与智能卡进行交互
****************************************************/
bool OpenReader(const char* reader)
{
    strncpy_s(g_readerName, sizeof(g_readerName), reader, strlen(reader));
	if (g_scardContext == NULL)
	{
		EstablishContext();
	}
    HRESULT hResult = SCardConnect(g_scardContext,
        g_readerName,
        SCARD_SHARE_SHARED,
        SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
        &g_scardHandle,
        &g_dwActiveProtocol);
    if (hResult != SCARD_S_SUCCESS)
    {
        return false;
    }

    return true;
}

/**************************************************
* 功能： 关闭读卡器
***************************************************/
void CloseReader()
{    
    SCardDisconnect(g_scardHandle, SCARD_UNPOWER_CARD);
    if (g_scardContext)
    {
        RealseContext();
    }
}

/*************************************************
* 功能： 获取ATR
**************************************************/
void GetATR(char* atr, int len)
{
    char			szHexATR[256] = { 0 };
    BYTE			szATR[SCARD_ATR_LENGTH] = { 0 };
    DWORD			szReaderNameLenght = 256;
    DWORD			dwATRLen;
    DWORD			dwProtocol;
    DWORD			dwState;

    HRESULT hRet = SCardStatus(g_scardHandle,			//智能卡连接句柄
        g_readerName,	//读卡器名称
        &szReaderNameLenght, //读卡器名称长度
        &dwState,	//智能卡的状态
        &dwProtocol, //智能卡的协议，当前卡的状态须是SCARD_SPECIFIC
        szATR,	//32字节的ATR字符串
        &dwATRLen);
    if (hRet != SCARD_S_SUCCESS)
    {
        return;
    }
    for (unsigned int i = 0; i < dwATRLen; i++)
    {
        char temp[3] = { 0 };
        _snprintf_s(temp, SCARD_ATR_LENGTH, "%02X", szATR[i]);
        strcat_s(atr, len, temp);
    }
}

/*************************************************
* 功能： 获取卡片状态
*************************************************/
int  GetCardStatus()
{
    BYTE			szATR[SCARD_ATR_LENGTH] = { 0 };
    char			szHexATR[SCARD_ATR_LENGTH] = { 0 };
    DWORD			szReaderNameLenght = 256;
    DWORD			dwATRLen;
    DWORD			dwProtocol;
    DWORD			dwState;

    HRESULT hRet = SCardStatus(g_scardHandle,			//智能卡连接句柄
        g_readerName,	//读卡器名称
        &szReaderNameLenght, //读卡器名称长度
        &dwState,	//智能卡的状态
        &dwProtocol, //智能卡的协议，当前卡的状态须是SCARD_SPECIFIC
        szATR,	//32字节的ATR字符串
        &dwATRLen);
    if (hRet != SCARD_S_SUCCESS)
    {
        return SCARD_UNKNOWN;
    }

    return dwState;
}

/************************************************
* 功能： 获取与卡片通讯协议
*************************************************/
int  GetTransProtocol()
{
    BYTE			szATR[SCARD_ATR_LENGTH] = { 0 };
    char			szHexATR[SCARD_ATR_LENGTH] = { 0 };
    DWORD			szReaderNameLenght = 256;
    DWORD			dwATRLen;
    DWORD			dwProtocol;
    DWORD			dwState;
    HRESULT hRet = SCardStatus(g_scardHandle,			//智能卡连接句柄
        g_readerName,	//读卡器名称
        &szReaderNameLenght, //读卡器名称长度
        &dwState,	//智能卡的状态
        &dwProtocol, //智能卡的协议，当前卡的状态须是SCARD_SPECIFIC
        szATR,	//32字节的ATR字符串
        &dwATRLen);
    if (hRet != SCARD_S_SUCCESS)
    {
        return SCARD_PROTOCOL_UNDEFINED;
    }

    return dwProtocol;
}

bool  SendApduCmd(const char* cmd, char* output, int &len)
{
    HRESULT hRet = S_FALSE;

    //删除字符串中的空格
    int cmdLen = strlen(cmd);
	char noSpaceCmd[1024] = { 0 };

    Tool::DeleteSpace(cmd, noSpaceCmd, sizeof(noSpaceCmd));

    cmdLen = strlen(noSpaceCmd);
    if (cmdLen < 8 || cmdLen % 2 != 0)
    {
        return false;
    }
    unsigned char szAPDU[256] = { 0 };
    Tool::AscToBcd(szAPDU, (unsigned char*)noSpaceCmd, cmdLen);
    switch (g_dwActiveProtocol)
    {
    case 1:
        hRet = SCardTransmit(g_scardHandle, SCARD_PCI_T0, szAPDU, cmdLen / 2, NULL, (unsigned char*)output, (DWORD*)&len);
        break;
    case 2:
        hRet = SCardTransmit(g_scardHandle, SCARD_PCI_T1, szAPDU, cmdLen / 2, NULL, (unsigned char*)output, (DWORD*)&len);
        break;
    default:
        //Log->Warning("数据传输协议未知 %d", m_dwActiveProtocol);
        return false;
    }
    if (hRet != SCARD_S_SUCCESS)
    {
        return false;
    }

    return true;
}

//APDU响应报文
bool GetAPDUResponseCommand(unsigned char SW1, char* szResponse, int& len)
{
    char szLen[3] = { 0 };
    sprintf_s(szLen, "%02X", SW1);
    char cmd[12] = { 0 };
    sprintf_s(cmd, 12, "00C00000%s", szLen);

    return SendApduCmd(cmd, szResponse, len);
}

int  SendApdu(const char* cmd, char* output, int len)
{
    bool bResult = false;
    unsigned char SW1, SW2;
	char szResponse[1024] = {0};
    int dwResponseLen = 1024;
    if (!SendApduCmd(cmd, szResponse, len))
    {
        return false;
    }

    SW1 = output[len - 2];
    SW2 = output[len - 1];

    if (SW1 == 0x61)	//通过00c0命令获取响应数据, SW2为响应数据返回的长度
    {
        GetAPDUResponseCommand(SW2, szResponse, dwResponseLen);		
    }
    else if (SW1 == 0x6C)	//通过原先的命令获取响应数据, SW2为响应数据返回的长度
    {
        char temp[3] = { 0 };
        _itoa_s(SW2, temp, 16);
        sprintf_s(temp, 3, "%02X", SW2);
       		
        char cmd2[128] = { 0 };
		sprintf_s(cmd2, 128, "%s%s", cmd, temp);
        SendApduCmd(cmd2, szResponse, dwResponseLen);
    }

    Tool::BcdToAsc(szResponse, output, dwResponseLen * 2 - 4);

	int result = atoi(szResponse + dwResponseLen - 2);

	return result;
}


/***********************************************
* 功能： 通过APDU指令与卡片进行交互
************************************************/
int  SendApdu2(const char* cmd)
{
    return -1;
}

char* GetApduError(int status)
{
    return NULL;
}



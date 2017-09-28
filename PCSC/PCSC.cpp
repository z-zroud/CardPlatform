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

char g_readerName[128] = { 0 };     //���浱ǰ�򿪵Ķ�����

//������Դ������������
BOOL EstablishContext()
{
    HRESULT hResult = SCARD_F_INTERNAL_ERROR;
    hResult = SCardEstablishContext(SCARD_SCOPE_SYSTEM,//SCARD_SCOPE_USER,	//���ݿ�������û�������
        NULL,	//����
        NULL,	//����
        &g_scardContext);	//�����ľ��
    if (hResult != SCARD_S_SUCCESS)
    {
        //Log->Error("SCardEstablishContext Fail! RetCode %08X,Windows�����ܿ�����δ����", hResult);
        return FALSE;
    }
    return TRUE;
}

//�ͷ���Դ������������
void RealseContext()
{
    if (g_scardContext != NULL)
    {
        SCardReleaseContext(g_scardContext);
        g_scardContext = NULL;
    }
}

int GetReaders(char** readers, int& count, int len)
{
    DWORD nReadersLen = 1024;
    char szReaders[1024] = { 0 };

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
            if (num < count)
            {
                memcpy(readers[num], szReaderName, strlen(szReaderName));
                num++;
            }            
            szReaderName += pos + 1;
        }
    }

    return 0;
}

/****************************************************
* ���ܣ� �ȸ�λ��SCARD_LEAVE_CARD
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
* ���ܣ� �临λ��SCARD_UNPOWER_CARD �ϵ縴λ
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
* ���ܣ� ѡ��ĳ�������������ܿ����н���
****************************************************/
bool OpenReader(const char* reader)
{
    strncpy_s(g_readerName, sizeof(g_readerName), reader, strlen(reader));

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
* ���ܣ� �رն�����
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
* ���ܣ� ��ȡATR
**************************************************/
void GetATR(char* atr, int len)
{
    char			szHexATR[256] = { 0 };
    BYTE			szATR[SCARD_ATR_LENGTH] = { 0 };
    DWORD			szReaderNameLenght = strlen(g_readerName);
    DWORD			dwATRLen;
    DWORD			dwProtocol;
    DWORD			dwState;

    HRESULT hRet = SCardStatus(g_scardHandle,			//���ܿ����Ӿ��
        g_readerName,	//����������
        &szReaderNameLenght, //���������Ƴ���
        &dwState,	//���ܿ���״̬
        &dwProtocol, //���ܿ���Э�飬��ǰ����״̬����SCARD_SPECIFIC
        szATR,	//32�ֽڵ�ATR�ַ���
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
* ���ܣ� ��ȡ��Ƭ״̬
*************************************************/
int  GetCardStatus()
{
    BYTE			szATR[SCARD_ATR_LENGTH] = { 0 };
    char			szHexATR[SCARD_ATR_LENGTH] = { 0 };
    DWORD			szReaderNameLenght = strlen(g_readerName);
    DWORD			dwATRLen;
    DWORD			dwProtocol;
    DWORD			dwState;

    HRESULT hRet = SCardStatus(g_scardHandle,			//���ܿ����Ӿ��
        g_readerName,	//����������
        &szReaderNameLenght, //���������Ƴ���
        &dwState,	//���ܿ���״̬
        &dwProtocol, //���ܿ���Э�飬��ǰ����״̬����SCARD_SPECIFIC
        szATR,	//32�ֽڵ�ATR�ַ���
        &dwATRLen);
    if (hRet != SCARD_S_SUCCESS)
    {
        return SCARD_UNKNOWN;
    }

    return dwState;
}

/************************************************
* ���ܣ� ��ȡ�뿨ƬͨѶЭ��
*************************************************/
int  GetTransProtocol()
{
    BYTE			szATR[SCARD_ATR_LENGTH] = { 0 };
    char			szHexATR[SCARD_ATR_LENGTH] = { 0 };
    DWORD			szReaderNameLenght = strlen(g_readerName);
    DWORD			dwATRLen;
    DWORD			dwProtocol;
    DWORD			dwState;
    HRESULT hRet = SCardStatus(g_scardHandle,			//���ܿ����Ӿ��
        g_readerName,	//����������
        &szReaderNameLenght, //���������Ƴ���
        &dwState,	//���ܿ���״̬
        &dwProtocol, //���ܿ���Э�飬��ǰ����״̬����SCARD_SPECIFIC
        szATR,	//32�ֽڵ�ATR�ַ���
        &dwATRLen);
    if (hRet != SCARD_S_SUCCESS)
    {
        return SCARD_PROTOCOL_UNDEFINED;
    }

    return dwProtocol;
}

int  SendApduCmd(const char* cmd, char* output, int &len)
{
    HRESULT hRet = S_FALSE;

    //ɾ���ַ����еĿո�
    int cmdLen = strlen(cmd);
    char* noSpaceCmd = new char[cmdLen];
    memset(noSpaceCmd, 0, cmdLen);

    Tool::DeleteSpace(cmd, noSpaceCmd, cmdLen);

    cmdLen = strlen(noSpaceCmd);
    if (cmdLen < 8 || cmdLen % 2 != 0)
    {
        return -1;
    }
    unsigned char szAPDU[256] = { 0 };
    Tool::AscToBcd(szAPDU, (unsigned char*)noSpaceCmd, cmdLen);

    unsigned char* result = NULL;
    DWORD resultLen = 0;

    switch (g_dwActiveProtocol)
    {
    case 1:
        hRet = SCardTransmit(g_scardHandle, SCARD_PCI_T0, szAPDU, cmdLen / 2, NULL, result, &resultLen);
        break;
    case 2:
        hRet = SCardTransmit(g_scardHandle, SCARD_PCI_T1, szAPDU, cmdLen / 2, NULL, result, &resultLen);
        break;
    default:
        //Log->Warning("���ݴ���Э��δ֪ %d", m_dwActiveProtocol);
        return false;
    }
    if (hRet != SCARD_S_SUCCESS)
    {
        return -1;
    }

    return 0;
}

//APDU��Ӧ����
bool GetAPDUResponseCommand(unsigned char SW1, char* szResponse, int& len)
{
    char szLen[3] = { 0 };
    sprintf_s(szLen, "%02X", SW1);
    char cmd[12] = { 0 };
    sprintf_s(cmd, 12, "00C00000%s", szLen);

    return SendApduCmd(cmd, szResponse, len);
}

int  SendApdu(const char* cmd, char* output, int &len)
{
    bool bResult = false;
    unsigned char SW1, SW2;
    char* szResponse;
    DWORD dwResponseLen = 1024;
    if (!SendApduCmd(cmd, output, len))
    {
        return false;
    }

    SW1 = output[len - 2];
    SW2 = output[len - 1];

    if (SW1 == 0x61)	//ͨ��00c0�����ȡ��Ӧ����, SW2Ϊ��Ӧ���ݷ��صĳ���
    {
        //memset(szResponse, 0, outputLen);
        //dwResponseLen = outputLen;
        bResult = GetAPDUResponseCommand(SW2, szResponse, dwResponseLen);
        SW1 = szResponse[dwResponseLen - 2];
        SW2 = szResponse[dwResponseLen - 1];
    }
    else if (SW1 == 0x6C)	//ͨ��ԭ�ȵ������ȡ��Ӧ����, SW2Ϊ��Ӧ���ݷ��صĳ���
    {
        //memset(szResponse, 0, outputLen);
        //dwResponseLen = outputLen;
        char temp[3] = { 0 };
        _itoa_s(SW2, temp, 16);
        sprintf_s(temp, 3, "%02X", SW2);
        strCommand.append(temp);
        char cmd2[1024] = { 0 };

        bResult = SendApduCmd(strCommand, szResponse, &dwResponseLen);
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


/***********************************************
* ���ܣ� ͨ��APDUָ���뿨Ƭ���н���
************************************************/
int  SendApdu(const char* cmd)
{
    return -1;
}

char* GetLastError(int status)
{
    return NULL;
}



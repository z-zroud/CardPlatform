// PCSC.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "PCSC.h"
#include "../Util/Tool.h"
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
    unsigned int pos = 0;
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
    DWORD			szReaderNameLenght = 256;
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
    DWORD			szReaderNameLenght = 256;
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
    DWORD			szReaderNameLenght = 256;
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

/********************************************************
* ���ܣ�����ԭ��APDUָ��
*********************************************************/
bool  SendApduCmd(const char* cmd, char* output, int &len)
{
    HRESULT hRet = S_FALSE;

    //ɾ���ַ����еĿո�
    int cmdLen = strlen(cmd);
	char noSpaceCmd[512] = { 0 };
    Tool::DeleteSpace(cmd, noSpaceCmd, sizeof(noSpaceCmd));
    cmdLen = strlen(noSpaceCmd);

    if (cmdLen < 8 || cmdLen % 2 != 0)	//bcd����������������ͷ8�ֽڣ�������ż���ֽ�
    {
        return false;
    }
    unsigned char apdu[256] = { 0 };
    Tool::BcdToAsc(apdu, (unsigned char*)noSpaceCmd, cmdLen);
    switch (g_dwActiveProtocol)
    {
    case 1:
        hRet = SCardTransmit(g_scardHandle, SCARD_PCI_T0, apdu, cmdLen / 2, NULL, (unsigned char*)output, (DWORD*)&len);
        break;
    case 2:
        hRet = SCardTransmit(g_scardHandle, SCARD_PCI_T1, apdu, cmdLen / 2, NULL, (unsigned char*)output, (DWORD*)&len);
        break;
    default:
        return false;
    }
    if (hRet != SCARD_S_SUCCESS)
    {
        return false;
    }

    return true;
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

/********************************************************
* ���ܣ�����APDUָ���뿨Ƭ���н���
*********************************************************/
int  SendApdu(const char* cmd, char* output, int len)
{
    bool bResult = false;
    unsigned char SW1, SW2;

	char response[2048] = {0};	//����APDU����ֵ
    int responseLen = 2048;

    if (!SendApduCmd(cmd, response, responseLen))
    {
        return -1;
    }

    SW1 = response[responseLen - 2];
    SW2 = response[responseLen - 1];

    if (SW1 == 0x61)	//ͨ��00c0�����ȡ��Ӧ����, SW2Ϊ��Ӧ���ݷ��صĳ���
    {
        GetAPDUResponseCommand(SW2, response, responseLen);
    }
    else if (SW1 == 0x6C)	//ͨ��ԭ�ȵ������ȡ��Ӧ����, SW2Ϊ��Ӧ���ݷ��صĳ���
    {
        char temp[3] = { 0 };
        _itoa_s(SW2, temp, 16);
        sprintf_s(temp, 3, "%02X", SW2);
       		
        char cmd2[128] = { 0 };
		sprintf_s(cmd2, 128, "%s%s", cmd, temp);
		if (!SendApduCmd(cmd2, response, responseLen))
		{
			return -1;
		}
    }
	if (responseLen * 2 - 4 > len)
	{
		return 0x1000;	//buff is too small
	}
	char tmp[2048] = { 0 };
    Tool::AscToBcd(tmp, response, responseLen * 2);

	if (strlen(tmp) < 4)
	{
		return -1;
	}
	memcpy(output, tmp, strlen(tmp) - 4);
	int result = strtol(tmp + strlen(tmp) - 4, NULL, 16);

	return result;
}


/***********************************************
* ���ܣ� ͨ��APDUָ���뿨Ƭ���н���,���ƶ�ȡtagָ��
* ����Ҫ����ֵ��
************************************************/
int  SendApdu2(const char* cmd)
{
	char output[2048] = { 0 };
	return SendApdu(cmd, output, sizeof(output));
}

/**********************************************
* ���ܣ� ͨ��APUD����ֵ��ȡ���������Ϣ
***********************************************/
char* GetApduError(int status)
{
	switch (status)
	{
	case 0x1000:	return "recive buffer is too small";
	case 0x6281:	return "the relevant data may be corrupted";			
	case 0x6400:	return "Wrong context(for PK Crypto commands)";				
	case 0x6581:	return "Write problem / Memory failure (ISO class byte)";		
	case 0x6700:	return "Incorrect length (Lc or Le)";							
	case 0x6900:	return "Unable to process";										
	case 0x6901:	return "Command not allowed, invalid state";					
	case 0x6981:	return "Command incompatible with file organization";			
	case 0x6982:	return "Security status not satisfied";							
	case 0x6983:	return "Authentication method blocked";							
	case 0x6984:	return "Challenge not present or PIN is blocked";				
	case 0x6985:	return "Conditions of use not satisfied";						//GemGold R3 PBOC EF15�ļ��Ƿ�֧��ED/EP��
	case 0x6986:	return "No current EF selected";								
	case 0x6987:	return "Secure messaging object missing (that is, MAC missing)"; 
	case 0x6988:	return "Secure messaging data object incorrect";				
	case 0x6A80:	return "Incorrect parameters in the data field";				
	case 0x6A81:	return "Function not supported, invalidated file";				
	case 0x6A82:	return "File not found";										
	case 0x6A83:	return "Record not found";										
	case 0x6A84:	return "Not enough memory space in the file";					
	case 0x6A86:	return "Incorrect parameters(P1 & P2)";							
	case 0x6A88:	return "Referenced data not found";								
	case 0x6D00:	return "Unknown instruction code(command is not present)";		
	case 0x6E00:	return "Wrong instruction class given in the command";			
	case 0x6F00:	return "No data available for GET RESPONSE";					
	case 0x9302:	return "Invalid MAC.";											
	case 0x9303:	return "Application permanently blocked.";						
	case 0x9401:	return "Insufficient funds";									
	case 0x9402:	return "Transaction counter overflow";							
	case 0x9403:	return "Key Index not supported";								
	case 0x9406:	return "Requested TAC/MAC not available";						
		}
	return "Unkonw error";
}



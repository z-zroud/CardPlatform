#pragma once
#include "Interface\IPCSC.h"
#include "Interface\IAPDU.h"
#include "Interface\CardBase.h"
#include <vector>
#include <string>
using namespace std;

/***********************************************************
* IPCSC�ӿ�ʵ����
************************************************************/
class PCSC : public IPCSC
{
public:	
	PCSC();
    ~PCSC();

public:
    vector<char*>	GetAllReaders();
	bool			OpenReader(const char* szReaderName);
	void			CloseReader();	
	IAPDU*			GetAPDU() { return m_APDU; }	
	bool			SelectAID(const string &strAID);
	bool			OpenSecureChannel(const string &strKMC);
	bool			SetKMC(const string &strKMC, DIV_METHOD_FLAG flag);
	string			GetATR();
	DIV_METHOD_FLAG	GetDIV() { return m_flag; }
	SECURE_LEVEL	GetSecureLevel();		
	CARD_STATUS		GetCardStatus();	
	CARD_TRANSMISSION_PROTOCOL GetCardTransmissionProtocol();
    string GetKeyVersion() { return m_keyVersion; }
	string GetSessionAuthKey() { return m_sessionAuthKey; }
	string GetSessionMacKey() { return m_sessionMacKey; }
	string GetSessionEncKey() { return m_sessionEncKey; }
	string GetAuthKey() { return m_AuthKey; }
	string GetMacKey() { return m_MacKey; }
	string GetEncKey() { return m_EncKey; }	
	string GetLastError();

protected:
    BOOL EstablishContext();
    void RealseContext();
private:
	BOOL ConnectSmartCard();	
	bool VlidateCardMAC(string szMac, string termRandomNum, string cardRandomNum);//У�鿨ƬMACֵ

private:
	enum { MAX_READER_NAME_LEN = 255 };				//������������Ƴ���	
    enum { MAX_READERS_LEN = 1024 };
    std::vector<char*> m_szReaderNames;	//��д�������б�
	SCARDHANDLE m_scardHandle;						//���ܿ����
	char m_scardReaderName[MAX_READER_NAME_LEN];	//����������
	CARD_TRANSMISSION_PROTOCOL m_dwActiveProtocol;	//����Э��
	SCARDCONTEXT m_scardContextHandle;				//��Դ���������
	IAPDU *m_APDU;									//APDU����
private:
	DIV_METHOD_FLAG m_flag;

private:
    string m_divFactor;     //��ɢ����
	string m_AuthKey;
	string m_MacKey;
	string m_EncKey;
	string m_sessionAuthKey;
	string m_sessionMacKey;
	string m_sessionEncKey;
	string m_KEK_Key;
    string m_keyVersion;
private:
	string m_Error;
};




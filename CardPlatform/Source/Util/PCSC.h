#pragma once
#include "Interface\IPCSC.h"
#include "Interface\IAPDU.h"
#include "Interface\CardBase.h"
#include <vector>
#include <string>
using namespace std;

/***********************************************************
* IPCSC接口实现类
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
	bool VlidateCardMAC(string szMac, string termRandomNum, string cardRandomNum);//校验卡片MAC值

private:
	enum { MAX_READER_NAME_LEN = 255 };				//读卡器最大名称长度	
    enum { MAX_READERS_LEN = 1024 };
    std::vector<char*> m_szReaderNames;	//读写器名称列表
	SCARDHANDLE m_scardHandle;						//智能卡句柄
	char m_scardReaderName[MAX_READER_NAME_LEN];	//读卡器名称
	CARD_TRANSMISSION_PROTOCOL m_dwActiveProtocol;	//传输协议
	SCARDCONTEXT m_scardContextHandle;				//资源管理器句柄
	IAPDU *m_APDU;									//APDU命令
private:
	DIV_METHOD_FLAG m_flag;

private:
    string m_divFactor;     //分散数据
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




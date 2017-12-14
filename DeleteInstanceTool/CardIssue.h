#pragma once
#include "Interface.hpp"


#ifdef PCSC
#include "CardBase.h"
#endif
using namespace std;


/******************************************************************
* 个人化处理类
*******************************************************************/
class CardIssue
{
public:
    CardIssue();
    ~CardIssue();
public:
    bool Init();
	bool ExtAuth();
	bool SendApdu(string& cmd, string& out);
	bool GetApduResult(string data);
	string GetErrorInfo() { return m_ErrorInfo; }
	bool ChangeKmc();
#ifdef PCSC
public:
	DWORD m_card_apdu(char* in, char* out);

	bool SendAPDU(string &strCommand, APDU_RESPONSE &response);
	bool SendAPDU(string strCommand, unsigned char* szResponse, DWORD* pResponseLen);
	bool GetAPDUResponseCommand(unsigned int len, unsigned char* szResponse, DWORD* pResponseLen);

	void CloseReader();
	bool OpenReader(const char* szReaderName);
	BOOL ConnectSmartCard();
	vector<char*> GetAllReaders();
	void RealseContext();
	BOOL EstablishContext();
	void ResetCard(const char* szReaderName);
#endif


private:
#ifndef PCSC
    Func_card_apdu          m_card_apdu;
    Func_card_find          m_card_find;
#endif
	string					m_dekKey;
	string					m_keyData;
	string					m_keyVersion;
    Func_card_verify        m_card_verify;
	Func_generate_en_kmc	m_generate_en_kmc;
	string					m_ErrorInfo;

#ifdef PCSC
	enum { MAX_READER_NAME_LEN = 255 };				//读卡器最大名称长度	
	enum { MAX_READERS_LEN = 1024 };
	std::vector<char*> m_szReaderNames;	//读写器名称列表
	SCARDHANDLE m_scardHandle;						//智能卡句柄
	char m_scardReaderName[MAX_READER_NAME_LEN];	//读卡器名称
	CARD_TRANSMISSION_PROTOCOL m_dwActiveProtocol;	//传输协议
	SCARDCONTEXT m_scardContextHandle;				//资源管理器句柄
#endif
};


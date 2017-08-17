#pragma once
#include "Interface\ICommTransaction.h"
#include "Util\APDU.h"
#include "Util\PCSC.h"
#include "Util\TLVParaser.h"
#include <map>

using namespace std;

class CommTransaction : public ICommTransaction
{
public:
	CommTransaction(IPCSC* reader);
	CommTransaction();
	~CommTransaction();

public:
    //需要实现ICommTransaction接口的基本功能
	virtual bool SelectApplication();
	virtual bool InitilizeApplication();
	virtual bool ReadApplicationData();
	virtual bool OfflineDataAuth();
	virtual bool HandleLimitation();
	virtual bool CardHolderValidation();
	virtual bool TerminalRiskManagement();
	virtual bool CardActionAnalized();
	virtual bool TerminalActionAnalized();
	virtual bool OnlineBussiness();
	virtual bool HandleIusserScript();
	virtual bool EndTransaction();

	virtual void DoTrans(){}
    virtual void Clear();
    virtual void ShowLog() {}

	virtual void SetEncryption(ENCRYPT_TYPE type) { m_encryptType = type; }
	virtual void SetAuthencation(AUTHENCATE_TYPE type) { m_authType = type; }

	virtual void SetUdkAuth(const string &auth) { m_udkAuth = auth; }
	virtual void SetUdkMac(const string &mac) { m_udkMac = mac; }
	virtual void SetUdkEnc(const string &enc) { m_udkEnc = enc; }

	virtual void SetMdkAuth(const string &auth) { m_mdkAuth = auth; }
	virtual void SetMdkMac(const string &mac) { m_mdkMac = mac; }
	virtual void SetMdkEnc(const string &enc) { m_mdkEnc = enc; }


public:

    //子类需要扩展的功能列表
	virtual void SetCommunicationType(COMMUNICATION_TYPE type) = 0;

public:
	string  SelectPSE(APP_TYPE appType);
    bool    SDA(string issuerPublicKey, ENCRYPT_TYPE encryptType);
    bool    DDA(string ICCPublicKey, ENCRYPT_TYPE encryptType);
    string  ReadTagValue(const string &tag);
protected:
	void	PrintTags(PBCD_TLV entities, int num);
	string	GetTagValue(const string tag, PBCD_TLV entities, int num);
	string	GetTagValue(const string tag);
	void	SaveTag(PBCD_TLV entites, int num);
	void	SaveTag(string tag, string value);
	void	ParaseAFL(string strAFL, vector<AFL> &output);
	string	GetResult(unsigned char SW1, unsigned char SW2);

protected:
	string		m_acceptAuthData;
	IPCSC*		m_pReader;
	IAPDU*		m_pAPDU;
	TLVParaser* m_pParaser;
	map<string, string> m_tags;


	ENCRYPT_TYPE m_encryptType;
	AUTHENCATE_TYPE m_authType;
	string		m_mdkAuth;
	string      m_mdkMac;
	string      m_mdkEnc;
	string		m_udkAuth;
	string      m_udkMac;
	string      m_udkEnc;
	bool		m_IsOnlineAuthSucessed;
};
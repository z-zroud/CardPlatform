#pragma once
#include "CardBase.h"

/*******************************************************
* 根据EMV金融交易规范，定义了相应的接口。所有从卡片中获取的
* TAG值，均由实现者提供存储。
********************************************************/
struct ICommTransaction
{
    virtual void DoTrans()                  = 0;

	virtual bool SelectApplication()		= 0;
	virtual bool InitilizeApplication()		= 0;
	virtual bool ReadApplicationData()		= 0;
	virtual bool OfflineDataAuth()			= 0;
	virtual bool HandleLimitation()			= 0;
	virtual bool CardHolderValidation()		= 0;
	virtual bool TerminalRiskManagement()	= 0;
	virtual bool CardActionAnalized()		= 0;
	virtual bool TerminalActionAnalized()	= 0;
	virtual bool OnlineBussiness()			= 0;
	virtual bool HandleIusserScript()		= 0;
	virtual bool EndTransaction()			= 0;

	virtual void SetEncryption(ENCRYPT_TYPE type)		= 0;
	virtual void SetAuthencation(OFFLINE_AUTHENCATE_TYPE type)	= 0;

    virtual void SetUdkAuth(const string &auth) = 0;
    virtual void SetUdkMac(const string &mac) = 0;
    virtual void SetUdkEnc(const string &enc) = 0;

    virtual void SetMdkAuth(const string &auth) = 0;
    virtual void SetMdkMac(const string &mac) = 0;
    virtual void SetMdkEnc(const string &enc) = 0;
    virtual void SetScript(const string& tag, const string& value) = 0;
    virtual void ExecScript(bool bExecScript) = 0;
    virtual void ShowLog() = 0;
};
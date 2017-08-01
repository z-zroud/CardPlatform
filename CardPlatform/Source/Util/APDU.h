#pragma once
#include "Interface\IAPDU.h"


//APDU命令类
class APDU : public IAPDU
{
public:
	APDU(SCARDHANDLE scardHandle, CARD_TRANSMISSION_PROTOCOL protocol);
	virtual bool SelectApplicationCommand(const string &aid, APDU_RESPONSE &response);
	bool SelectPSECommand(APDU_RESPONSE &response);		//应用选择 select PSE命令
	bool SelectPPSECommand(APDU_RESPONSE &response);	//select PPSE
	bool InitializeUpdateCommand(string strDiv, APDU_RESPONSE &response);	//个人化 更新初始化命令
	bool DeleteCommand(string id);	//删除命令
	bool StoreDataCommand(string DGI, string GDIData, STORE_DATA_TYPE dataType, bool bReset);
	bool InstallCommand(string exeLoadFile, string exeModule, string application, string privilege, string installParam, string token = "");
	bool GetTag(const string &tag, APDU_RESPONSE &response);	//取数据命令 获取标签
	bool ExternalAuthCommand(const string cardRandomNum,		//外部认证命令
		const string termRandomNum,
		const string sessionAuthKey,
		const string seesionMacKey,
		SECURE_LEVEL nSecureLevel,
		int nSCP,
		APDU_RESPONSE &strOutputData);
	bool GetApplicationStatusCommand(vector<APP_STATUS> &status);		//获取安全域命令
	bool ExternalAuthcateCommand(const string ARPC, const string authCode, APDU_RESPONSE &response);	//外部认证命令
	bool ReadRecordCommand(const string &strCommand, const string strRecordNumber, APDU_RESPONSE &response);	//读记录 命令
	bool GPOCommand(const string &strCommand, APDU_RESPONSE &response);		//获取处理选项(GPO)命令
	bool InternalAuthCommand(const string &strCommand, APDU_RESPONSE &response);
	bool PutRecordCommand(const string &strCommand, APDU_RESPONSE &response);
    virtual bool PutKeyCommand(const string keyVersion,
        const string authKeyWithKcv,
        const string macKeyWithKcv,
        const string encKeyWithKcv);
	bool UpdateRecordCommand(const string &strCommand, APDU_RESPONSE &response);
	bool GACCommand(GACControlParam p1, const string terminalData, APDU_RESPONSE &response);
	bool WriteDataCommand(const string &strCommand, APDU_RESPONSE &response);
	bool SendAPDU(string &strCommand, APDU_RESPONSE &response);	//发送APDU命令	
	string GetAPDUError();	//获取命令执行失败原因

protected:
	bool GetAPDUResponseCommand(unsigned int len, unsigned char* szResponse, DWORD* pResponseLen);
	void GetAPDUError(unsigned char SW1, unsigned char SW2, char* szError, const int len);
	bool SendAPDU(string strCommand, unsigned char* szResponse, DWORD* dwResponseLen);

private:
	SCARDHANDLE					m_scardHandle;
	CARD_TRANSMISSION_PROTOCOL	m_dwActiveProtocol;
	string						m_Error;
};

#pragma once
#include "Interface\IAPDU.h"


//APDU������
class APDU : public IAPDU
{
public:
	APDU(SCARDHANDLE scardHandle, CARD_TRANSMISSION_PROTOCOL protocol);
	virtual bool SelectApplicationCommand(const string &aid, APDU_RESPONSE &response);
	bool SelectPSECommand(APDU_RESPONSE &response);		//Ӧ��ѡ�� select PSE����
	bool SelectPPSECommand(APDU_RESPONSE &response);	//select PPSE
	bool InitializeUpdateCommand(string strDiv, APDU_RESPONSE &response);	//���˻� ���³�ʼ������
	bool DeleteCommand(string id);	//ɾ������
	bool StoreDataCommand(string DGI, string GDIData, STORE_DATA_TYPE dataType, bool bReset);
    bool StorePSEData(string data, STORE_DATA_TYPE dataType, bool bReset);
	bool InstallCommand(string exeLoadFile, string exeModule, string application, string privilege, string installParam, string token = "");
	bool GetTag(const string &tag, APDU_RESPONSE &response);	//ȡ�������� ��ȡ��ǩ
	bool ExternalAuthCommand(const string cardRandomNum,		//�ⲿ��֤����
		const string termRandomNum,
		const string sessionAuthKey,
		const string seesionMacKey,
		SECURE_LEVEL nSecureLevel,
		int nSCP,
		APDU_RESPONSE &strOutputData);
	bool GetApplicationStatusCommand(vector<APP_STATUS> &status);		//��ȡ��ȫ������
	bool ExternalAuthcateCommand(const string ARPC, const string authCode, APDU_RESPONSE &response);	//�ⲿ��֤����
	bool ReadRecordCommand(const string &strCommand, const string strRecordNumber, APDU_RESPONSE &response);	//����¼ ����
	bool GPOCommand(const string &strCommand, APDU_RESPONSE &response);		//��ȡ����ѡ��(GPO)����
	bool InternalAuthCommand(const string &strCommand, APDU_RESPONSE &response);
	bool PutRecordCommand(const string &strCommand, APDU_RESPONSE &response);
    virtual bool PutKeyCommand(const string keyVersion,
        const string authKeyWithKcv,
        const string macKeyWithKcv,
        const string encKeyWithKcv);
	bool UpdateRecordCommand(const string &strCommand, APDU_RESPONSE &response);
	bool GACCommand(GACControlParam p1, const string terminalData, APDU_RESPONSE &response);
	bool WriteDataCommand(const string &strCommand, APDU_RESPONSE &response);
	bool SendAPDU(string &strCommand, APDU_RESPONSE &response);	//����APDU����	
	string GetAPDUError();	//��ȡ����ִ��ʧ��ԭ��

protected:
	bool GetAPDUResponseCommand(unsigned int len, unsigned char* szResponse, DWORD* pResponseLen);
	void GetAPDUError(unsigned char SW1, unsigned char SW2, char* szError, const int len);
	bool SendAPDU(string strCommand, unsigned char* szResponse, DWORD* dwResponseLen);

private:
	SCARDHANDLE					m_scardHandle;
	CARD_TRANSMISSION_PROTOCOL	m_dwActiveProtocol;
	string						m_Error;
};

#pragma once
#include "Interface\IAPDU.h"

#define PSE_AID		"315041592E5359532E4444463031"
#define PPSE_AID	"325041592E5359532E4444463031"

//APDU������
class APDU : public IAPDU
{
public:
	APDU(SCARDHANDLE scardHandle, CARD_TRANSMISSION_PROTOCOL protocol);
	bool SelectAppCmd(const string &aid, APDU_RESPONSE &response);
	bool InitUpdateCmd(const string& randomNum, APDU_RESPONSE &response);


	bool DeleteAppCmd(const string& aid);	//ɾ������


	bool StoreDataCmd(const string& dgi,
					const string& dgiData,
					STORE_DATA_TYPE type,
					bool reset,
					APDU_RESPONSE& response);


	bool InstallAppCmd(const string& package,
					const string& applet,
					const string& instance,
					const string& privilege,
					const string& installParam,
					const string& token,
					APDU_RESPONSE& response);


	bool ReadTagCmd(const string &tag, APDU_RESPONSE &response);	//ȡ�������� ��ȡ��ǩ
	bool ExternalAuthCommand(const string cardRandomNum,		//�ⲿ��֤����
		const string termRandomNum,
		const string sessionAuthKey,
		const string seesionMacKey,
		SECURE_LEVEL nSecureLevel,
		int nSCP,
		APDU_RESPONSE &strOutputData);
	bool GetAppStatusCmd(vector<APP_STATUS> &status, APDU_RESPONSE& reponse);		//��ȡ��ȫ������
	bool ExternalAuthcateCommand(const string ARPC, const string authCode, APDU_RESPONSE &response);	//�ⲿ��֤����
	bool ReadRecordCommand(const string &strCommand, const string strRecordNumber, APDU_RESPONSE &response);	//����¼ ����
	bool GPOCommand(const string &strCommand, APDU_RESPONSE &response);		//��ȡ����ѡ��(GPO)����
	bool InternalAuthCommand(const string &strCommand, APDU_RESPONSE &response);
	bool PutDataCommand(const string &tag, const string &value, const string &mac);
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

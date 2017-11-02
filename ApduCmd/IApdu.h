#pragma once
#include "CardBase.h"
/****************************************************
* �ýӿڶ���������APDUָ�������PBOC3.0�淶
*****************************************************/
typedef bool(*ApduCmdCallback)(const char* cmd, char* output, int &len);

struct IAPDU
{
	virtual void SetApduCmdCallback(ApduCmdCallback ApduCmdFunc) = 0;
	virtual bool SelectAppCmd(const string& aid, APDU_RESPONSE &response) = 0;
	virtual bool InitUpdateCmd(const string& randomNum, APDU_RESPONSE &response) = 0;	//���˻� ���³�ʼ������
	virtual bool DeleteAppCmd(const string& aid) = 0;	//ɾ������

	virtual bool StoreDataCmd(const string& dgi,
		const string& dgiData,
		STORE_DATA_TYPE type,
		bool reset,
		APDU_RESPONSE& response) = 0;	//��������

	virtual bool InstallAppCmd(const string& package,
		const string& applet,
		const string& instance,
		const string& privilege,
		const string& installParam,
		const string& token,
		APDU_RESPONSE& response) = 0;

	virtual bool ReadTagCmd(const string &tag, APDU_RESPONSE &response) = 0;	//ȡ�������� ��ȡ��ǩ
	virtual bool GetAppStatusCmd(vector<APP_STATUS> &status, APDU_RESPONSE& reponse) = 0;		//��ȡ״̬����



	virtual bool ExternalAuthCommand(const string cardRandomNum,		//�ⲿ��֤����
		const string termRandomNum,
		const string sessionAuthKey,
		const string seesionMacKey,
		SECURE_LEVEL nSecureLevel,
		int nSCP,
		APDU_RESPONSE &strOutputData) = 0;

	virtual bool ExternalAuthcateCommand(const string ARPC, const string authCode, APDU_RESPONSE &response) = 0;	//�ⲿ��֤����
	virtual bool ReadRecordCommand(const string &strCommand, const string strRecordNumber, APDU_RESPONSE &response) = 0;	//����¼ ����
	virtual bool GPOCommand(const string &strCommand, APDU_RESPONSE &response) = 0;		//��ȡ����ѡ��(GPO)����
	virtual bool InternalAuthCommand(const string &strCommand, APDU_RESPONSE &response) = 0;
	virtual bool PutDataCommand(const string &tag, const string &value, const string &mac) = 0;
	virtual bool PutKeyCommand(const string keyVersion,
		const string authKeyWithKcv,
		const string macKeyWithKcv,
		const string encKeyWithKcv) = 0;
	virtual bool UpdateRecordCommand(const string &strCommand, APDU_RESPONSE &response) = 0;
	virtual bool GACCommand(GACControlParam p1, const string terminalData, APDU_RESPONSE &response) = 0;
	virtual bool WriteDataCommand(const string &strCommand, APDU_RESPONSE &response) = 0;
	virtual bool SendAPDU(string &strCommand, APDU_RESPONSE &response) = 0;	//����APDU����	
	virtual string GetAPDUError() = 0;	//��ȡ����ִ��ʧ��ԭ��
};
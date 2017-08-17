#pragma once
#include "CardBase.h"
/****************************************************
* �ýӿڶ���������APDUָ�������PBOC3.0�淶
*****************************************************/
struct IAPDU
{
	virtual bool SelectApplicationCommand(const string &aid, APDU_RESPONSE &response) = 0;
	virtual bool SelectPSECommand(APDU_RESPONSE &response) = 0;
	virtual bool SelectPPSECommand(APDU_RESPONSE &response) = 0;
	virtual bool InitializeUpdateCommand(string strDiv, APDU_RESPONSE &response) = 0;	//���˻� ���³�ʼ������
	virtual bool DeleteCommand(string id) = 0;	//ɾ������
	virtual bool StoreDataCommand(string DGI, string GDIData, STORE_DATA_TYPE dataType, bool bReset) = 0;	//��������
    virtual bool StorePSEData(string data, STORE_DATA_TYPE dataType, bool bReset) = 0;    //�洢PSE��PPSE���˻�����
	virtual bool InstallCommand(string exeLoadFile, 
		string exeModule, 
		string application, 
		string privilege,
		string installParam, 
		string token = "") = 0;
	virtual bool GetTag(const string &tag, APDU_RESPONSE &response) = 0;	//ȡ�������� ��ȡ��ǩ
	virtual bool ExternalAuthCommand(const string cardRandomNum,		//�ⲿ��֤����
		const string termRandomNum,
		const string sessionAuthKey,
		const string seesionMacKey,
		SECURE_LEVEL nSecureLevel,
		int nSCP,
		APDU_RESPONSE &strOutputData) = 0;
	virtual bool GetApplicationStatusCommand(vector<APP_STATUS> &status) = 0;		//��ȡ״̬����
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
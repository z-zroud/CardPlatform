#pragma once
#include <string>

using namespace std;

#ifdef APDUCMD_EXPORTS
#define APDUCMD_API __declspec(dllexport)
#else
#define APDUCMD_API __declspec(dllimport)
#endif

/****************************************************
* �ýӿڶ���������APDUָ�������PBOC3.0�淶
*****************************************************/
#define APDU_OK		0x9000
#define APDU_LEN	2048
#define KEY_LEN		33
#define STORE_DATA_PLANT	0
#define STORE_DATA_ENCRYPT	1
#define STORE_DATA_END		2

#define DIV_NONE	0
#define DIV_CPG202	1
#define DIV_CPG212	2

#define SECURE_NONE	0
#define SECURE_MAC	1



#ifdef __cplusplus
extern "C"
{
#endif
	bool OpenSecureChannel(const string &kmc, int divMethod, int secureLevel = 0);
	bool SetKmc(const string &kmc, int divMethod);
	UINT SelectAppCmd(const string& aid, string& resp);
	UINT DeleteAppCmd(const string& aid);
	UINT StoreDataCmd(const string data, int type, bool reset = false);
	UINT InitializeUpdateCmd(const string& random, string& resp);

	//��ȡ�򿪰�ȫͨ��֮��ĻỰ��Կ������ǰ�������OpenSecureChannel���򿪰�ȫͨ��
	void GetScureChannelSessionAuthKey(char* scureChannelSessionAuthKey, int len = KEY_LEN);
	void GetScureChannelSessionMacKey(char* scureChannelSessionMacKey, int len = KEY_LEN);
	void GetScureChannelSessionEncKey(char* scureChannelSessionEncKey, int len = KEY_LEN);
	void GetScureChannelSessionKekKey(char* scureChannelSessionKekKey, int len = KEY_LEN);

#ifdef __cplusplus
}
#endif


//struct IAPDU
//{
//	virtual void SetApduCmdCallback(ApduCmdCallback ApduCmdFunc) = 0;
//	virtual bool SelectAppCmd(const string& aid, APDU_RESPONSE &response) = 0;
//	virtual bool InitUpdateCmd(const string& randomNum, APDU_RESPONSE &response) = 0;	//���˻� ���³�ʼ������
//	virtual bool DeleteAppCmd(const string& aid) = 0;	//ɾ������
//
//	virtual bool StoreDataCmd(const string& dgi,
//		const string& dgiData,
//		STORE_DATA_TYPE type,
//		bool reset,
//		APDU_RESPONSE& response) = 0;	//��������
//
//	virtual bool InstallAppCmd(const string& package,
//		const string& applet,
//		const string& instance,
//		const string& privilege,
//		const string& installParam,
//		const string& token,
//		APDU_RESPONSE& response) = 0;
//
//	virtual bool ReadTagCmd(const string &tag, APDU_RESPONSE &response) = 0;	//ȡ�������� ��ȡ��ǩ
//	virtual bool GetAppStatusCmd(vector<APP_STATUS> &status, APDU_RESPONSE& reponse) = 0;		//��ȡ״̬����
//
//
//
//	virtual bool ExternalAuthCommand(const string cardRandomNum,		//�ⲿ��֤����
//		const string termRandomNum,
//		const string sessionAuthKey,
//		const string seesionMacKey,
//		SECURE_LEVEL nSecureLevel,
//		int nSCP,
//		APDU_RESPONSE &strOutputData) = 0;
//
//	virtual bool ExternalAuthcateCommand(const string ARPC, const string authCode, APDU_RESPONSE &response) = 0;	//�ⲿ��֤����
//	virtual bool ReadRecordCommand(const string &strCommand, const string strRecordNumber, APDU_RESPONSE &response) = 0;	//����¼ ����
//	virtual bool GPOCommand(const string &strCommand, APDU_RESPONSE &response) = 0;		//��ȡ����ѡ��(GPO)����
//	virtual bool InternalAuthCommand(const string &strCommand, APDU_RESPONSE &response) = 0;
//	virtual bool PutDataCommand(const string &tag, const string &value, const string &mac) = 0;
//	virtual bool PutKeyCommand(const string keyVersion,
//		const string authKeyWithKcv,
//		const string macKeyWithKcv,
//		const string encKeyWithKcv) = 0;
//	virtual bool UpdateRecordCommand(const string &strCommand, APDU_RESPONSE &response) = 0;
//	virtual bool GACCommand(GACControlParam p1, const string terminalData, APDU_RESPONSE &response) = 0;
//	virtual bool WriteDataCommand(const string &strCommand, APDU_RESPONSE &response) = 0;
//	virtual bool SendAPDU(string &strCommand, APDU_RESPONSE &response) = 0;	//����APDU����	
//	virtual string GetAPDUError() = 0;	//��ȡ����ִ��ʧ��ԭ��
//};
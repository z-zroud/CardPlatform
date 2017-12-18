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
#define APDU_LEN	1024
#define RESP_LEN	1024
#define KEY_LEN		33
#define STORE_DATA_PLANT	0
#define STORE_DATA_ENCRYPT	1
#define STORE_DATA_END		2

#define DIV_NONE	0
#define DIV_CPG202	1
#define DIV_CPG212	2

#define SECURE_NONE	0
#define SECURE_MAC	1

#define AAC			0x00
#define ARQC		0x80
#define TC			0x40
#define CDA			0x10


#ifdef __cplusplus
extern "C"
{
#endif
APDUCMD_API bool OpenSecureChannel(const char* kmc, int divMethod, int secureLevel = 0);
APDUCMD_API bool SetKmc(const char* kmc, int divMethod);

/**************************************************
* ͨ��AIDѡ��Ӧ��
***************************************************/
APDUCMD_API	UINT SelectAppCmd(const char* aid, char* resp);

/***********************************************************
* �ն˷���GPO����
* ������ terminalData ��Tag9F38ָ�����ն�����ƴ�Ӷ���
* ������ resp ��Ƭ���ص�������
* ���أ� �ɹ����� 0x9000 ����ֵ��ʾʧ��
************************************************************/
APDUCMD_API UINT GPOCmd(const char* terminalData, char* resp);

/**************************************************
* ��ȡ��¼�ļ���Ϣ
* ������ sfi ��ȡ��¼�ļ��Ķ��ļ���ʶ
* ������ recordNum ��¼��
* ������ resp APDU���ص������򣬲�����SW״̬��
* ���أ� �ɹ����� 0x9000 ����ֵ��ʾʧ��
***************************************************/
APDUCMD_API UINT ReadRecordCmd(int sfi, int recordNum, char* resp);

/**************************************************
* ͨ��AIDɾ��Ӧ��
* ������ aid ��ɾ����Ӧ�õ�AID
***************************************************/
APDUCMD_API	UINT DeleteAppCmd(const char* aid);
APDUCMD_API	UINT StoreDataCmd(const char* data, int type, bool reset = false);
APDUCMD_API	UINT InitializeUpdateCmd(const char* random, char* resp);

/*************************************************************************
* �ڲ���֤����ն˷��͸�����������֤��Ƭ�ĺϷ���
* ������ ddolData ��Tag9F49ָ�����ն�����
* ������ resp ��Ƭ���ɵĶ�̬����ǩ�� ��Tag9F4B
**************************************************************************/
APDUCMD_API UINT InternalAuthencationCmd(const char* ddolData, char* resp);

/*************************************************************************
* �ⲿ��֤����
**************************************************************************/
APDUCMD_API UINT ExternalAuthencationCmd(const char* arpc, const char* authCode, char* resp);
/***************************************************************************
* ����Ӧ�����ģ��ն˴��ͽ���������ݵ�IC���У�IC�����㲢����һ������
* ������terminalCryptogramType �ն�������������� AAC/ARQC/TC
* ������cdolData CDOLָ�����ն�����
* ������resp ���ɵ�Ӧ������
****************************************************************************/
APDUCMD_API UINT GACCmd(int terminalCryptogramType, const char* cdolData, char* resp);

/****************************************************************
* ��Ƭ���ɶ�̬���ݣ��ն�ͨ��������ʹ��IC����Կ
*****************************************************************/
APDUCMD_API void GenDynamicData(const char* ddolData, char* resp);

//��ȡ�򿪰�ȫͨ��֮��ĻỰ��Կ������ǰ�������OpenSecureChannel���򿪰�ȫͨ��
APDUCMD_API	void GetScureChannelSessionAuthKey(char* scureChannelSessionAuthKey);
APDUCMD_API	void GetScureChannelSessionMacKey(char* scureChannelSessionMacKey);
APDUCMD_API	void GetScureChannelSessionEncKey(char* scureChannelSessionEncKey);
APDUCMD_API	void GetScureChannelSessionKekKey(char* scureChannelSessionKekKey);

#ifdef __cplusplus
}
#endif

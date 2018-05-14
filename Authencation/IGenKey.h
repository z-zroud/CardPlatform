#pragma once
#include "CardBase.h"
#include <string>
#include <vector>
using namespace std;

#ifdef GENKEY_EXPORTS
#define GENKEY_API __declspec(dllexport)
#else
#define GENKEY_API __declspec(dllimport)
#endif

const int DES_KEY = 0;
const int SM_KEY = 1;



/********************************************************************************
* ���ܣ� ������Կ��KCVУ��ֵ
* ������ key ��Կ
*********************************************************************************/
extern "C" GENKEY_API int GenDesKcv(const char* key, char* kcv, int kcvLen);

extern "C" GENKEY_API int GenSmKcv(const char* key, char* kcv, int kcvLen);

/********************************************************************************
* ���ܣ������ݿ�(sqlite)�л�ȡCA��Կ
* ������caIndex: ��Կ����
* ������rid ��caIndex ��ȡΨһ��CA��Կ
*********************************************************************************/
extern "C" GENKEY_API int GenCAPublicKey(const char* caIndex, const char* rid, char* caPublicKey);

/********************************************************************************
* ���ܣ� ��ȡ�����й�Կ
* ������ issuerPulicCert �����й�Կ֤��
*		ipkRemainder �����й�Կ����
*		type
*********************************************************************************/
extern "C" GENKEY_API  int GenDesIssuerPublicKey(
	const char* caPublicKey, 
	const char* issuerPublicCert, 
	const char* ipkRemainder, 
	const char* issuerExponent,
	char* issuerPublicKey);

/********************************************************************************
* ���ܣ� ��ȡIC����Կ
* ������ issuerPulicCert �����й�Կ֤��
* ������ iccPublicCert IC����Կ֤��
* ������ iccRemainder IC����Կ����
* ������ sigStaticData ������AFLʱ����Ҫǩ��������
* ������ iccExponent IC����Կָ��
* ���أ� �����й�Կ
*********************************************************************************/
extern "C" GENKEY_API  int GenDesICCPublicKey(
	const char* issuerPublicKey,
	const char* iccPublicCert,
	const char* iccRemainder,
	const char* sigStaticData,
	const char* iccExponent,
	const char* tag82,
	char* iccPublicKey);

/********************************************************************************
* ���ܣ� ��ȡ�����й�Կ
* ������ issuerPulicCert �����й�Կ֤��
*		ipkRemainder �����й�Կ����
*       PAN �˺�
* ���أ�	�����й�Կ
*********************************************************************************/
extern "C" GENKEY_API  int GenSMIssuerPublicKey(
	const char* caPublicKey, 
	const char* issuerPublicCert,
    const char* PAN,
	char* issuerPublicKey);

/********************************************************************************
* ���ܣ� ��ȡ�����й�Կ
* ������ issuerPulicCert �����й�Կ֤��
*		ipkRemainder �����й�Կ����
*		type
*********************************************************************************/
extern "C" GENKEY_API  int GenSMICCPublicKey(
	const char* issuerPublicKey,
	const char* iccPublicCert,
	const char* needAuthStaticData,
    const char* tag82,
    const char* PAN,
	char* iccPublicKey);

/********************************************************************************
* ���ܣ� �ѻ�������֤ SDA
* ������ issuerPublicKey �����й�Կ
* ������ tag93 ǩ���ľ�̬Ӧ������ (�÷�����˽Կǩ�������ݣ���SDA���������ն���֤)
* ������ tag82 AIP(Ӧ�ý�������)
* ���أ� 0 ��ʾSDA��֤�ɹ��������ʾ��֤ʧ��
*********************************************************************************/
extern "C" GENKEY_API int DES_SDA(const char* issuerPublicKey, 
	const char*ipkExponent,  
	const char* tag93, 
	const char* sigStaticData,
	const char* tag82);
extern "C" GENKEY_API int SM_SDA(const char* issuerPublicKey, const char* toBeSignedStaticAppData, const char* tag93, const char* tag82);

extern "C" GENKEY_API int DES_DDA(const char* iccPublicKey, 
	const char*iccExponent,
	const char* tag9F4B,
	const char* dynamicData);
extern "C" GENKEY_API int SM_DDA(const char* iccPublicKey, const char* tag9F4B, const char* dynamicData);

/********************************************************************************
* ͨ��UDK_AC  UDK_MAC  UDK_ENC ���ɶ�Ӧ�ĻỰ��Կ
* ����udkSubKey ���Դ���UDK_AC/UDK_MAC/UDK_ENC ���ɶ�Ӧ��sessionKey
* ����atc Ӧ�ý��׼�����
*********************************************************************************/
extern "C" GENKEY_API void GenUdkSessionKey(const char* udkSubKey, const char* atc, char* udkSessionKey, int keyType=0);

/********************************************************************************
* ͨ����Ӧ��MDK_AC MDK_MAC MDK_ENC ���ɶ�Ӧ��UDK_AC UDK_MAC  UDK_ENC
* ���� mdk ���Դ���MDK_AC/MDK_MAC/MDK_ENC ���ɶ�Ӧ��UDK_AC/UDK_MAC/UDK_ENC
* ���� cardNo ����
* ����cardSequence ��Ƭ���к�
*********************************************************************************/
extern "C" GENKEY_API void GenUdk(const char* mdk,const char* cardNo,const char* cardSequence,char* udk, int keyType = 0);

/********************************************************************************
* ����ARPC
* ������ udkAcSessionKey ΪUDK_AC���ɵĻỰ��Կ
* ������ac Ӧ������
*********************************************************************************/
extern "C" GENKEY_API void GenArpc(const char* udkAuthSessionKey, char* ac, char* authCode, char* arpc, int keyType = 0);

/*******************************************************************************
* ���ɷ����нű�MAC
********************************************************************************/
extern "C" GENKEY_API void GenPBOCMac(const char* udkMacSessionKey, const char* data, char* mac, int keyType = 0);
extern "C" GENKEY_API void GenEMVMac(const char* udkAC, const char* data, char* mac);


extern "C" GENKEY_API void Decrypt_Des3_CBC(const char* key, const char* input, char* output, int outputLen);


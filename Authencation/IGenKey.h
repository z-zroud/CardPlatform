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

extern "C" GENKEY_API int GenDesKcv(const char* key, char* kcv, int kcvLen);

extern "C" GENKEY_API int GenSmKcv(const char* key, char* kcv, int kcvLen);

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
* ���أ�	�����й�Կ
*********************************************************************************/
extern "C" GENKEY_API  int GenSMIssuerPublicKey(
	const char* caPublicKey, 
	const char* issuerPublicCert,
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
	char* iccPublicKey);

/***************************************************************************************************
* ���ܣ� �ѻ�������֤ SDA
* ������ issuerPublicKey �����й�Կ
* ������ tag93 ǩ���ľ�̬Ӧ������ (�÷�����˽Կǩ�������ݣ���SDA���������ն���֤)
* ������ tag82 AIP(Ӧ�ý�������)
* ���أ� 0 ��ʾSDA��֤�ɹ��������ʾ��֤ʧ��
****************************************************************************************************/
extern "C" GENKEY_API int DES_SDA(const char* issuerPublicKey, 
	const char*ipkExponent,  
	const char* tag93, 
	const char* sigStaticData,
	const char* tag82);
extern "C" GENKEY_API int SM_SDA(const char* issuerPublicKey, const char*ipkExponent, const char* sigStaticData, const char* tag93, const char* tag82);

extern "C" GENKEY_API int DES_DDA(const char* iccPublicKey, 
	const char*iccExponent,
	const char* tag9F4B,
	const char* dynamicData);
extern "C" GENKEY_API int SM_DDA(const char* iccPublicKey, const char* dynamicData);

/********************************************************************************************************
* ͨ��UDK_AC  UDK_MAC  UDK_ENC ���ɶ�Ӧ�ĻỰ��Կ
* ����udkSubKey ���Դ���UDK_AC/UDK_MAC/UDK_ENC ���ɶ�Ӧ��sessionKey
* ����atc Ӧ�ý��׼�����
*********************************************************************************************************/
extern "C" GENKEY_API void GenUdkSessionKey(const char* udkSubKey, const char* atc, char* udkSessionKey);

/*******************************************************************************************************
* ͨ����Ӧ��MDK_AC MDK_MAC MDK_ENC ���ɶ�Ӧ��UDK_AC UDK_MAC  UDK_ENC
* ���� mdk ���Դ���MDK_AC/MDK_MAC/MDK_ENC ���ɶ�Ӧ��UDK_AC/UDK_MAC/UDK_ENC
* ���� cardNo ����
* ����cardSequence ��Ƭ���к�
********************************************************************************************************/
extern "C" GENKEY_API void GenUdk(const char* mdk,const char* cardNo,const char* cardSequence,char* udk);

/************************************************************************************************
* ����ARPC
* ������ udkAcSessionKey ΪUDK_AC���ɵĻỰ��Կ
* ������ac Ӧ������
*************************************************************************************************/
extern "C" GENKEY_API void GenArpc(const char* udkAuthSessionKey, char* ac, char* authCode, char* arpc);

/*******************************************************************************************************
* ���ɷ����нű�MAC
********************************************************************************************************/
extern "C" GENKEY_API void GenIssuerScriptMac(const char* udkMacSessionKey, const char* data, char* mac);


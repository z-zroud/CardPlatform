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
extern "C" int GENKEY_API GenDesKcv(const char* key, char* kcv, int kcvLen);

extern "C" int GENKEY_API GenSmKcv(const char* key, char* kcv, int kcvLen);

/********************************************************************************
* ���ܣ������ݿ�(sqlite)�л�ȡCA��Կ
* ������caIndex: ��Կ����
* ������rid ��caIndex ��ȡΨһ��CA��Կ
*********************************************************************************/
extern "C" int GENKEY_API GenCAPublicKey(const char* caIndex, const char* rid, char* caPublicKey);

/********************************************************************************
* ���ܣ� ��ȡ�����й�Կ
* ������ issuerPulicCert �����й�Կ֤��
*		ipkRemainder �����й�Կ����
*		type
*********************************************************************************/
extern "C" int GENKEY_API  GenDesIssuerPublicKey(
	const char* caPublicKey, 
	const char* issuerPublicCert, 
	const char* ipkRemainder, 
	const char* issuerExponent,
    const char* pin,
    const char* tag5F24,
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
extern "C" int GENKEY_API GenDesICCPublicKey(
	const char* issuerPublicKey,
	const char* iccPublicCert,
	const char* iccRemainder,
	const char* sigStaticData,
	const char* iccExponent,
	const char* tag82,
    const char* pin,
    const char* tag5F24,
	char* iccPublicKey);

/********************************************************************************
* ���ܣ� ��ȡ�����й�Կ
* ������ issuerPulicCert �����й�Կ֤��
*		ipkRemainder �����й�Կ����
*       PAN �˺�
* ���أ�	�����й�Կ
*********************************************************************************/
extern "C" int GENKEY_API  GenSMIssuerPublicKey(
	const char* caPublicKey, 
	const char* issuerPublicCert,
    const char* PAN,
    const char* tag5F24,
	char* issuerPublicKey);

/********************************************************************************
* ���ܣ� ��ȡ�����й�Կ
* ������ issuerPulicCert �����й�Կ֤��
*		ipkRemainder �����й�Կ����
*		type
*********************************************************************************/
extern "C" int GENKEY_API   GenSMICCPublicKey(
	const char* issuerPublicKey,
	const char* iccPublicCert,
	const char* needAuthStaticData,
    const char* tag82,
    const char* PAN,
    const char* tag5F24,
	char* iccPublicKey);

/********************************************************************************
* ���ܣ� �ѻ�������֤ SDA
* ������ issuerPublicKey �����й�Կ
* ������ tag93 ǩ���ľ�̬Ӧ������ (�÷�����˽Կǩ�������ݣ���SDA���������ն���֤)
* ������ tag82 AIP(Ӧ�ý�������)
* ���أ� 0 ��ʾSDA��֤�ɹ��������ʾ��֤ʧ��
*********************************************************************************/
extern "C" int GENKEY_API DES_SDA(const char* issuerPublicKey,
	const char*ipkExponent,  
	const char* tag93, 
	const char* sigStaticData,
	const char* tag82);
extern "C" int GENKEY_API SM_SDA(const char* issuerPublicKey, const char* toBeSignedStaticAppData, const char* tag93, const char* tag82);

extern "C" int GENKEY_API DES_DDA(const char* iccPublicKey,
	const char*iccExponent,
	const char* tag9F4B,
	const char* dynamicData);
extern "C" int GENKEY_API SM_DDA(const char* iccPublicKey, const char* tag9F4B, const char* dynamicData);

/********************************************************************************
* ͨ��UDK_AC  UDK_MAC  UDK_ENC ���ɶ�Ӧ�ĻỰ��Կ
* ����udkSubKey ���Դ���UDK_AC/UDK_MAC/UDK_ENC ���ɶ�Ӧ��sessionKey
* ����atc Ӧ�ý��׼�����
*********************************************************************************/
extern "C" void GENKEY_API GenUdkSessionKey(const char* udkSubKey, const char* atc, char* udkSessionKey, int keyType=0);
extern "C" void GENKEY_API GenVisaUdkSessionKey(const char* udkSubKey, const char* atc, char* udkSessionKey);
/********************************************************************************
* ͨ����Ӧ��MDK_AC MDK_MAC MDK_ENC ���ɶ�Ӧ��UDK_AC UDK_MAC  UDK_ENC
* ���� mdk ���Դ���MDK_AC/MDK_MAC/MDK_ENC ���ɶ�Ӧ��UDK_AC/UDK_MAC/UDK_ENC
* ���� cardNo ����
* ����cardSequence ��Ƭ���к�
*********************************************************************************/
extern "C" void GENKEY_API GenUdk(const char* mdk,const char* cardNo,const char* cardSequence,char* udk, int keyType = 0);

/********************************************************************************
* ����ARPC
* ������ udkAcSessionKey ΪUDK_AC���ɵĻỰ��Կ
* ������ac Ӧ������
*********************************************************************************/
extern "C" void GENKEY_API GenArpc(const char* udkAuthSessionKey, char* ac, char* authCode, char* arpc, int keyType = 0);

/*******************************************************************************
* ���ɷ����нű�MAC
********************************************************************************/
extern "C" void GENKEY_API GenPBOCMac(const char* udkMacSessionKey, const char* data, char* mac, int keyType = 0);
extern "C" void GENKEY_API GenEMVMac(const char* udkAC, const char* data, char* mac);


extern "C" void GENKEY_API Decrypt_Des3_CBC(const char* key, const char* input, char* output, int outputLen);

extern "C" int GENKEY_API GenHash(const char* input, char* hash, int len);
extern "C" int GENKEY_API DES_GenRecovery(const char* publicKey, const char* publicKeyExp, const char* encryptionData, char* recoveryData, int len);

extern "C" int GENKEY_API SM2Verify(const char *pPublicKey, const char *pMSG, const char *pSignData);

extern "C" int GENKEY_API  GenSMHash(char *input, char *output);

extern "C" int GENKEY_API GenRSA(int bitLen, char* exp, 
    char* D,
    char* N,
    char* P, 
    char* Q, 
    char* DP, 
    char* DQ, 
    char* Qinv);

extern "C" int GENKEY_API GenIssuerCert(
    const char* ca_D,
    const char* ca_N,
    const char* formatFlag,
    const char* bin,
    const char* expiryDate,
    const char* publicKey,
    const char* publicKeyExp,
    char* publicCert,
    char* publicRemainder
);

extern "C" int GENKEY_API GenIccCert(
    const char* issuer_D,
    const char* issuer_N,
    const char* tag5A,
    const char* expiryDate,
    const char* publicKey,
    const char* publicKeyExp,
    const char* sigData,
    char* publicCert,
    char* publicRemainder
);

extern "C" int GENKEY_API GenSSDA(
    const char* p_D,
    const char* p_N,
    const char* sigData,
    const char* tag82,
    const char* dac,
    char* tag93
);
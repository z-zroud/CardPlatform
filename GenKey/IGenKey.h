#pragma once
#include "CardBase.h"
#include <string>
#include <vector>
using namespace std;

struct IGenKey
{
	/*****************************************************************
	* ���ܣ� ����KMC����Կ
	* ������ divData ��ɢ���ݣ���InitUpdate�����ֵ�ṩ
	*		divFlag ��ɢ����
	*		acKey	K_DEK ���ݼ�����Կ(�����������ݵļ���)
	*		macKey	K_MAC ��ȫͨ����Ϣ��֤����Կ(��ȫͨ��MACУ��)
	*		encKey	K_ENC ��ȫͨ��������Կ(��ȫͨ������֤�ͼ���)
	* ������ K_MAC��K_ENC���������ڰ�ȫͨ����ʼ�����������ɰ�ȫͨ���Ự��Կ
	******************************************************************/
	virtual void GenSubKey(const string kmc,	//��ȡ����KMC����Կ
		string divData,
		DIV_METHOD_FLAG divFlag,
		string &K_DEK,
		string &K_MAC,
		string &K_ENC) = 0;

	virtual string GenK_DEK(const string kmc, string divData, DIV_METHOD_FLAG divFlag) = 0;
	virtual string GenK_MAC(const string kmc, string divData, DIV_METHOD_FLAG divFlag) = 0;
	virtual string GenK_ENC(const string kmc, string divData, DIV_METHOD_FLAG divFlag) = 0;

	/********************************************************************
	* ���ܣ� ��������Կ�Ự��Կ
	*********************************************************************/
	virtual void GenSessionKey(string cardRandom, 
		string termRandom, 
		int scp,
		string K_DEK,
		string K_MAC,
		string K_ENC,
		string &sessionDEK,
		string &sessionMAC,
		string &sessionENC) = 0;

	virtual string GenSessionDEK(string K_DEK, string cardRandom, string termRandom, int scp) = 0;
	virtual string GenSessionMAC(string K_MAC, string cardRandom, string termRandom, int scp) = 0;
	virtual string GenSessionENC(string K_ENC, string cardRandom, string termRandom, int scp) = 0;
	/********************************************************************************
	* ���ܣ����ɻỰ��Կ
	* ������udkAuth: ������Կ
	*		atc:	��Ƭ������
	* ���أ� �Ự��Կ
	*********************************************************************************/
	virtual string GenSUDK_DEK(const string sudkAuth, const string ATC) = 0;		
	
	/********************************************************************************
	*********************************************************************************/																						
	virtual string GenUDK_DEK(const string mdkAuth, const string ATC, const string account, const string cardSeq) = 0;	//��ȡUDK_Auth

	/********************************************************************************
	* ���ܣ���ȡCA��Կ
	* ������index ��Կ����
	*		rid	��ԿID
	* ���أ� ��Կ
	*********************************************************************************/
	virtual string GenCAPublicKey(const string caIndex, string rid) = 0; //��ȡCA��Կ

	virtual void GetCARid(vector<char*> &rid) = 0;
	virtual void GetCAIndex(char* rid, vector<char*>& index) = 0;

	/********************************************************************************
	* ���ܣ� ��ȡ�����й�Կ
	* ������ issuerPulicCert �����й�Կ֤��
	*		ipkRemainder �����й�Կ����
	*		type
	*********************************************************************************/
	virtual string GenDesIssuerPublicKey(const string caPublicKey, const string issuerPublicCert, const string ipkRemainder, const string issuerExponent = "03") = 0;

	/********************************************************************************
	* ���ܣ� ��ȡ�����й�Կ
	* ������ issuerPulicCert �����й�Կ֤��
	*		ipkRemainder �����й�Կ����
	* ���أ� �����й�Կ
	*********************************************************************************/
	virtual string GenDesICCpublicKey(const string issuerPublicKey, const string iccPublicCert, const string iccRemainder, const string signedData, const string issuerExponent = "03") = 0;

	/********************************************************************************
	* ���ܣ� ��ȡ�����й�Կ
	* ������ issuerPulicCert �����й�Կ֤��
	*		ipkRemainder �����й�Կ����
	* ���أ�	�����й�Կ		
	*********************************************************************************/
	virtual string GenSMIssuerPublcKey(const string caPublicKey, const string issuerPublicCert) = 0;

	/********************************************************************************
	* ���ܣ� ��ȡ�����й�Կ
	* ������ issuerPulicCert �����й�Կ֤��
	*		ipkRemainder �����й�Կ����
	*		type
	*********************************************************************************/
	virtual string GenSMICCpublicKey(const string issuerPublicKey, const string iccPublicCert, const string needAuthStaticData) = 0;

	/********************************************************************************
	* ���ܣ� ��ȡ�����й�Կ
	* ������ issuerPulicCert �����й�Կ֤��
	*		ipkRemainder �����й�Կ����
	*		type
	*********************************************************************************/
	virtual string GenARPC1(string udkAuth, string AC, string authCode, string atc) = 0;

	/********************************************************************************
	* ���ܣ� ��ȡ�����й�Կ
	* ������ issuerPulicCert �����й�Կ֤��
	*		ipkRemainder �����й�Կ����
	*		type
	*********************************************************************************/
	virtual string GenARPC2(string mdkAuth, string AC, string authCode, string atc, string cardSeq, string pan) = 0;

	/********************************************************************************
	* ���ܣ� ��ȡ�����й�Կ
	* ������ issuerPulicCert �����й�Կ֤��
	*		ipkRemainder �����й�Կ����
	*		type
	*********************************************************************************/
	virtual string GenScriptMac(string mac, string atc, string data) = 0;
};

#ifdef GENKEY_EXPORTS
#define GENKEY_API __declspec(dllexport)
#else
#define GENKEY_API __declspec(dllimport)
#endif

extern "C" GENKEY_API IGenKey* GetGenKeyInstance();
extern "C" GENKEY_API void DeleteGenKeyInstance(IGenKey* obj);



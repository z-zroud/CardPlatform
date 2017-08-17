#pragma once
#include "CardBase.h"
#include <string>

using namespace std;

struct IKeyGenerator
{
    virtual void GenAllSubKey(const string strKMC,	//��ȡ����KMC����Կ
        string divFactorData,
        DIV_METHOD_FLAG flag,
        string &strAuthKey,
        string &strMacKey,
        string &strEncKey) = 0;
    virtual void GenAllSessionKey(string cardRandomNum, string termRandomNum, int nSCP,	//��ȡSKU�Ự��Կ
        string strAuthKey,
        string strMacKey,
        string strEncKey,
        string &strSessionAuthKey,
        string &strSessionMacKey,
        string &strSessionEncKey) = 0;
    virtual string GenSUDKAuthFromUDKAuth(const string sudkAuth, const string ATC) = 0;		//��ȡSUDK_Auth
    virtual string GenUDKAuthFromMDKAuth(const string mdkAuth, const string ATC, const string account, const string cardSeq) = 0;	//��ȡUDK_Auth
    virtual string GenCAPublicKey(const string caIndex, string rid) = 0; //��ȡCA��Կ
    virtual string GenIssuerPublicKey(const string caPublicKey,             //��ȡ�����й�Կ
        const string issuerPublicCert, 
        const string ipkRemainder, 
        ENCRYPT_TYPE type,
        const string indexNumber = "03") = 0;
    virtual void GetCARid(vector<char*> &rid) = 0;
    virtual void GetCAIndex(char* rid, vector<char*>& index) = 0;
    virtual string GenICCPublicKey(const string issuerPublicKey,        //��ȡIC����Կ
        const string iccPublicCert, 
        const string iccRemainder, 
        const string signedData, 
        ENCRYPT_TYPE type,
        const string indexNumber = "03") = 0;
    virtual string GenDesIssuerPublicKey(const string caPublicKey, const string issuerPublicCert, const string ipkRemainder, const string issuerExponent = "03") = 0;
    virtual string GenDesICCpublicKey(const string issuerPublicKey, const string iccPublicCert, const string iccRemainder, const string signedData, const string issuerExponent = "03") = 0;
    virtual string GenSMIssuerPublcKey(const string caPublicKey, const string issuerPublicCert) = 0;
    virtual string GenSMICCpublicKey(const string issuerPublicKey, const string iccPublicCert, const string needAuthStaticData) = 0;
    virtual string GenARPCByUdkAuth(string udkAuth, string AC, string authCode, string atc) = 0;
    virtual string GenARPCByMdkAuth(string mdkAuth, string AC, string authCode, string atc, string cardSeq, string pan) = 0;
    virtual string GenScriptMac(string mac, string atc, string data) = 0;
};
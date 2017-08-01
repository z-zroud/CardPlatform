#pragma once
#include "Util\PCSC.h"
#include "Util\TLVParaser.h"
#include "..\CommTransaction.h"
#include "Interface\CardBase.h"
#include <map>
#include <vector>

using namespace std;

//����������PBOC�Ľ�������
class EMV : public CommTransaction
{
public:
    EMV();
    EMV(IPCSC* reader);
	~EMV();
	bool SelectApplication(APP_TYPE type, string app = "");     //Ӧ��ѡ��	
	bool InitilizeApplication(ENCRYPT_TYPE type);                                //Ӧ�ó�ʼ��	
	bool ReadApplicationData();                                 //��Ӧ������	
	bool OfflineDataAuth();//�ѻ�������֤
	bool HandleLimitation();                                      //��������	
	bool CardHolderValidation();                                 //�ֿ�����֤	
	bool TerminalRiskManagement();                              //�ն˷��չ���	
	bool CardActionAnalized(TERM_TRANS_TYPE type);              //��Ƭ��Ϊ����	
	bool TerminalActionAnalized();                   //�ն���Ϊ����	
	bool OnlineBussiness();                                     //��������	
	bool DealIusserScript();                                    //�����нű�����	
	bool EndTransaction();                                      //���׽���	
	string GetResult(unsigned char SW1, unsigned char SW2);     //�����ֵ

    void SetUDKAC(const string auth) { m_udkAuth = auth; }      //����UDK��Կ
    void SetUDKMac(const string mac) { m_udkMac = mac; }
    void SetUDKEnc(const string enc) { m_udkEnc = enc; }

    void SetMDKAC(const string auth) { m_mdkAuth = auth; }      //����MDK��Կ
    void SetMDKMac(const string mac) { m_mdkMac = mac; }
    void SetMDKEnc(const string enc) { m_mdkEnc = enc; }

protected:	
	void FormatOutputResponse(PBCD_TLV entities, int num);              //��ʽ�������Ƭ���ؽ��	
	string FindTagValue(const string tag, PBCD_TLV entities, int num);  //����TAG	
	string FindTagValue(const string tag);                              //����Tag	
	void SaveTag(PBCD_TLV entites, int num);                            //������Ӧ�����е�Tag,�������ʹ��
	void SaveTag(string tag, string value);	
	void ParseAFL(string strAFL, vector<AFL> &output);                  //����AFL����	
	string ReadTag(const string tag);                                   //��ȡ��ǩ
	TERM_TRANS_TYPE GetTermAnanlizedResult();                           //��ȡ�ն���Ϊ�������	
	bool CompareDate(string first, string second);                      //�Ƚ����� 170425 �����ַ�������, first >= second ����true 	
	string GetTVR();                                                    //��ȡ�ն�TVR���
	bool CompareIACAndTVR(string IAC, string strTVR);                   //�Ƚ�IAC��TAC���
	void ParseGACResponseData(const string buffer);                     //����GAC��Ӧ����
	bool GetTagAfterGAC();                                              //GAC������֮���ȡ��ǩ
	string GetPAN();                                                    //��ȡӦ�����˺�
	void ParseTransLog(const string buffer);
	string GenARPC(string AC, string authCode);

private:
	string SelectPSE(APP_TYPE appType);		//Ŀ¼ѡ��ʽ
	bool SelectAID(const string &strAID = "A000000333010102");	//AIDѡ��ʽ
	void ShowCVMMethod(int method);
	void ShowCVMCondition(int condition);
	void ShowCardTransType();	//��ʾ��Ƭ��������
	void ShowCardAUC(string AUC);
	string GenTransDate();
	string GenTransTime();
    bool ValidateStaticApplicationData(string issuerPublicKey, ENCRYPT_TYPE encryptType);
    bool ValidateDynamicData(string ICCPublicKey, ENCRYPT_TYPE encryptType);
	
private:
	string m_staticApplicationData;
	IPCSC* m_pReader;
	IAPDU*		m_pAPDU;
	TLVParaser* m_parser;
	map<string,string> m_Tags;
	TVR			m_tvr;		
	string		m_mdkAuth;
    string      m_mdkMac;
    string      m_mdkEnc;
	string		m_udkAuth;
    string      m_udkMac;
    string      m_udkEnc;
	bool		m_IsOnlineAuthSucessed;
};


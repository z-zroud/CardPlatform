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

	bool HandleLimitation();                                      //��������	
	bool CardHolderValidation();                                 //�ֿ�����֤	
	bool TerminalRiskManagement();                              //�ն˷��չ���	
	bool CardActionAnalized(TERM_TRANS_TYPE type);              //��Ƭ��Ϊ����	
	bool TerminalActionAnalized();                   //�ն���Ϊ����	
	bool OnlineBussiness();                                     //��������	
	bool DealIusserScript();                                    //�����нű�����	
	bool EndTransaction();                                      //���׽���	


protected:	
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

	void ShowCVMMethod(int method);
	void ShowCVMCondition(int condition);
	void ShowCardTransType();	//��ʾ��Ƭ��������
	void ShowCardAUC(string AUC);
	
private:
	string m_staticApplicationData;
	IPCSC* m_pReader;
	IAPDU*		m_pAPDU;
	TLVParaser* m_parser;
	TVR			m_tvr;		
	bool		m_IsOnlineAuthSucessed;
};


#pragma once
#include "Util\PCSC.h"
#include "Util\TLVParaser.h"
#include "..\CommTransaction.h"
#include "Interface\CardBase.h"
#include <map>
#include <vector>

using namespace std;

//����������PBOC�Ľ�������
class PBOC : public CommTransaction
{
public:
    PBOC();
    PBOC(COMMUNICATION_TYPE type, IPCSC* reader);
	~PBOC();

    void DoTrans();
	bool HandleLimitation();                                      //��������	
	bool CardHolderValidation();                                 //�ֿ�����֤	
	bool TerminalRiskManagement();                              //�ն˷��չ���	
	bool CardActionAnalized();              //��Ƭ��Ϊ����	
	bool TerminalActionAnalized();                              //�ն���Ϊ����	
	bool OnlineBussiness();                                     //��������	
	bool DealIusserScript();                                    //�����нű�����	
	bool EndTransaction();                                      //���׽���	

    void SetCommunicationType(COMMUNICATION_TYPE type); //ѡ��Ӵ����ǷǽӴ�
protected:	
	TERM_TRANS_TYPE GetTermAnanlizedResult();                           //��ȡ�ն���Ϊ�������	 	
	string          GetTVR();                                                    //��ȡ�ն�TVR���
	bool            CompareIACAndTVR(string IAC, string strTVR);                   //�Ƚ�IAC��TAC���
	void            ParseGACResponseData(const string buffer);                     //����GAC��Ӧ����
	string          GetAccount();                                                    //��ȡӦ�����˺�
	void            ParseTransLog(const string buffer);

private:

	void ShowCVMMethod(int method);
	void ShowCVMCondition(int condition);
	void ShowCardTransType();	//��ʾ��Ƭ��������
	void ShowCardAUC(string AUC);
	
private:
	string m_staticApplicationData;
	TVR			m_tvr;		
	bool		m_IsOnlineAuthSucessed;
    COMMUNICATION_TYPE  m_commType;
    TERM_TRANS_TYPE m_termTransType;
};


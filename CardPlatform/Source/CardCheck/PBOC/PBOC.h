#pragma once
#include "Util\PCSC.h"
#include "Util\TLVParaser.h"
#include "..\CommTransaction.h"
#include "Interface\CardBase.h"
#include <map>
#include <vector>

using namespace std;

//该类描述了PBOC的交易流程
class PBOC : public CommTransaction
{
public:
    PBOC();
    PBOC(COMMUNICATION_TYPE type, IPCSC* reader);
	~PBOC();

    void DoTrans();
	bool HandleLimitation();                                      //处理限制	
	bool CardHolderValidation();                                 //持卡人验证	
	bool TerminalRiskManagement();                              //终端风险管理	
	bool CardActionAnalized();                                  //卡片行为分析	
	bool TerminalActionAnalized();                              //终端行为分析	
	bool OnlineBussiness();                                     //联机处理	
	bool DealIusserScript();                                    //发卡行脚本处理	
	bool EndTransaction();                                      //交易结束	
    
    void SetScript(bool hasScript);
    void ShowLog();
    void SetCommunicationType(COMMUNICATION_TYPE type); //选择接触还是非接触
protected:	
	TERM_TRANS_TYPE GetTermAnanlizedResult();                           //获取终端行为分析结果	 	
	string          GetTVR();                                                    //获取终端TVR结果
	bool            CompareIACAndTVR(string IAC, string strTVR);                   //比较IAC和TAC结果
	void            ParseGACResponseData(const string buffer);                     //分析GAC响应数据
	string          GetAccount();                                                    //获取应用主账号
	void            ParseTransLog(const string buffer);

    void            Clear();

private:

	void ShowCVMMethod(int method);
	void ShowCVMCondition(int condition);
	void ShowCardTransType();	//显示卡片交易类型
	void ShowCardAUC(string AUC);
	
private:
	string m_staticApplicationData;
	TVR			m_tvr;		
	bool		m_IsOnlineAuthSucessed;
    bool        m_bExecScript;          //是否有脚本需要执行
    COMMUNICATION_TYPE  m_commType;
    TERM_TRANS_TYPE m_termTransType;
    vector<pair<string, string>> m_vecECLoadScript;     //不使用map，防止执行脚本区分先后顺序
};


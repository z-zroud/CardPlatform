#pragma once
#include "Util\PCSC.h"
#include "Util\TLVParaser.h"
#include "..\CommTransaction.h"
#include "Interface\CardBase.h"
#include <map>
#include <vector>

using namespace std;

//该类描述了PBOC的交易流程
class EMV : public CommTransaction
{
public:
    EMV();
    EMV(IPCSC* reader);
	~EMV();

	bool HandleLimitation();                                      //处理限制	
	bool CardHolderValidation();                                 //持卡人验证	
	bool TerminalRiskManagement();                              //终端风险管理	
	bool CardActionAnalized(TERM_TRANS_TYPE type);              //卡片行为分析	
	bool TerminalActionAnalized();                   //终端行为分析	
	bool OnlineBussiness();                                     //联机处理	
	bool DealIusserScript();                                    //发卡行脚本处理	
	bool EndTransaction();                                      //交易结束	


protected:	
	string ReadTag(const string tag);                                   //读取标签
	TERM_TRANS_TYPE GetTermAnanlizedResult();                           //获取终端行为分析结果	
	bool CompareDate(string first, string second);                      //比较日期 170425 这类字符串日期, first >= second 返回true 	
	string GetTVR();                                                    //获取终端TVR结果
	bool CompareIACAndTVR(string IAC, string strTVR);                   //比较IAC和TAC结果
	void ParseGACResponseData(const string buffer);                     //分析GAC响应数据
	bool GetTagAfterGAC();                                              //GAC处理完之后获取标签
	string GetPAN();                                                    //获取应用主账号
	void ParseTransLog(const string buffer);
	string GenARPC(string AC, string authCode);

private:

	void ShowCVMMethod(int method);
	void ShowCVMCondition(int condition);
	void ShowCardTransType();	//显示卡片交易类型
	void ShowCardAUC(string AUC);
	
private:
	string m_staticApplicationData;
	IPCSC* m_pReader;
	IAPDU*		m_pAPDU;
	TLVParaser* m_parser;
	TVR			m_tvr;		
	bool		m_IsOnlineAuthSucessed;
};


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
	bool SelectApplication(APP_TYPE type, string app = "");     //应用选择	
	bool InitilizeApplication(ENCRYPT_TYPE type);                                //应用初始化	
	bool ReadApplicationData();                                 //读应用数据	
	bool OfflineDataAuth();//脱机数据认证
	bool HandleLimitation();                                      //处理限制	
	bool CardHolderValidation();                                 //持卡人验证	
	bool TerminalRiskManagement();                              //终端风险管理	
	bool CardActionAnalized(TERM_TRANS_TYPE type);              //卡片行为分析	
	bool TerminalActionAnalized();                   //终端行为分析	
	bool OnlineBussiness();                                     //联机处理	
	bool DealIusserScript();                                    //发卡行脚本处理	
	bool EndTransaction();                                      //交易结束	
	string GetResult(unsigned char SW1, unsigned char SW2);     //命令返回值

    void SetUDKAC(const string auth) { m_udkAuth = auth; }      //设置UDK密钥
    void SetUDKMac(const string mac) { m_udkMac = mac; }
    void SetUDKEnc(const string enc) { m_udkEnc = enc; }

    void SetMDKAC(const string auth) { m_mdkAuth = auth; }      //设置MDK密钥
    void SetMDKMac(const string mac) { m_mdkMac = mac; }
    void SetMDKEnc(const string enc) { m_mdkEnc = enc; }

protected:	
	void FormatOutputResponse(PBCD_TLV entities, int num);              //格式化输出卡片返回结果	
	string FindTagValue(const string tag, PBCD_TLV entities, int num);  //查找TAG	
	string FindTagValue(const string tag);                              //查找Tag	
	void SaveTag(PBCD_TLV entites, int num);                            //保存响应数据中的Tag,方便后续使用
	void SaveTag(string tag, string value);	
	void ParseAFL(string strAFL, vector<AFL> &output);                  //解析AFL数据	
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
	string SelectPSE(APP_TYPE appType);		//目录选择方式
	bool SelectAID(const string &strAID = "A000000333010102");	//AID选择方式
	void ShowCVMMethod(int method);
	void ShowCVMCondition(int condition);
	void ShowCardTransType();	//显示卡片交易类型
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


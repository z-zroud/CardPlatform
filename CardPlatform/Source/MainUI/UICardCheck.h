#pragma once
#include "CardCheck\CommTransaction.h"

/*********************************************************
* 卡片检测主界面
**********************************************************/
class CCardCheckUI : public CContainerUI, public INotifyUI
{
public:
    CCardCheckUI(CPaintManagerUI* pPM);
    ~CCardCheckUI();

    void InitDlg();
    void DoInit();
    void Notify(TNotifyUI& msg); //处理内嵌模块的消息

    void OnBtnDoTransClicked();
    bool HandleScript(ICommTransaction* pCommTrans);

    void DoPBOC(IPCSC* pReader);
    void DoEC(IPCSC* pReader);
    void DoQPBOC(IPCSC* pReader);
protected:
    CPaintManagerUI*	m_pPM;

private:
    CComboUI*		m_pTransType;           //交易类型
    CComboUI*		m_pOfflineAuthType;     //脱机认证类型
    CComboUI*		m_pEncryptType;         //加密类型
    CComboUI*		m_pKeyType;             //密钥类型

    CCheckBoxUI*    m_pSupportOffline;  //支持脱机
    CCheckBoxUI*    m_pTouchTrans;      //接触交易

    CEditUI*		m_pAuth;		//DEK子密钥
    CEditUI*		m_pMac;			//MAC子密钥
    CEditUI*		m_pEnc;			//ENC子密钥
};


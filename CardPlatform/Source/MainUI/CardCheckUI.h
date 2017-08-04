#pragma once

class CCardCheckUI : public CContainerUI, public INotifyUI
{
public:
    CCardCheckUI(CPaintManagerUI* pPM);
    ~CCardCheckUI();

    void InitDlg();
    void DoInit();
    void Notify(TNotifyUI& msg); //处理内嵌模块的消息

protected:
    CPaintManagerUI*	m_pPM;

private:
    CComboUI*   m_pTransType;           //交易类型
    CComboUI*   m_pOfflineAuthType;     //脱机认证类型
    CComboUI*   m_pEncryptType;         //加密类型
    CComboUI*   m_pKeyType;             //密钥类型

    //CEdit*
};


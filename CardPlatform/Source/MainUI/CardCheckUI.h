#pragma once
#include "CardCheck\CommTransaction.h"

class CCardCheckUI : public CContainerUI, public INotifyUI
{
public:
    CCardCheckUI(CPaintManagerUI* pPM);
    ~CCardCheckUI();

    void InitDlg();
    void DoInit();
    void Notify(TNotifyUI& msg); //������Ƕģ�����Ϣ

    void OnBtnDoTransClicked();
    bool HandleScript(ICommTransaction* pCommTrans);

    void DoPBOC(IPCSC* pReader);
    void DoEC(IPCSC* pReader);
    void DoQPBOC(IPCSC* pReader);
protected:
    CPaintManagerUI*	m_pPM;

private:
    CComboUI*   m_pTransType;           //��������
    CComboUI*   m_pOfflineAuthType;     //�ѻ���֤����
    CComboUI*   m_pEncryptType;         //��������
    CComboUI*   m_pKeyType;             //��Կ����

    CCheckBoxUI*    m_pSupportOffline;  //֧���ѻ�
    CCheckBoxUI*    m_pTouchTrans;      //�Ӵ�����

    CEditUI*    m_pAuth;
    CEditUI*    m_pMac;
    CEditUI*    m_pEnc;
};


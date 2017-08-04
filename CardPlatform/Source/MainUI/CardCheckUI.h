#pragma once

class CCardCheckUI : public CContainerUI, public INotifyUI
{
public:
    CCardCheckUI(CPaintManagerUI* pPM);
    ~CCardCheckUI();

    void InitDlg();
    void DoInit();
    void Notify(TNotifyUI& msg); //������Ƕģ�����Ϣ

protected:
    CPaintManagerUI*	m_pPM;

private:
    CComboUI*   m_pTransType;           //��������
    CComboUI*   m_pOfflineAuthType;     //�ѻ���֤����
    CComboUI*   m_pEncryptType;         //��������
    CComboUI*   m_pKeyType;             //��Կ����

    //CEdit*
};


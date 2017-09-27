#pragma once

class CDecryptFileUI : public CContainerUI, public INotifyUI
{
public:
    CDecryptFileUI(CPaintManagerUI* pPM);
    ~CDecryptFileUI();

    void DoInit();
    void InitDlg();	//��ʼ���ؼ�
    void Notify(TNotifyUI& msg); //������Ƕģ�����Ϣ

protected:
    CPaintManagerUI* m_pPM;

    CEditUI*        m_pEncryptFile;
    CEditUI*        m_pKey;
	CComboUI*		m_pDecryptType;
	//CButtonUI*		m_pScanFile;
};


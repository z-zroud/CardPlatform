#pragma once

class CDecryptFileUI : public CContainerUI, public INotifyUI
{
public:
    CDecryptFileUI(CPaintManagerUI* pPM);
    ~CDecryptFileUI();

    void DoInit();
    void InitDlg();	//初始化控件
    void Notify(TNotifyUI& msg); //处理内嵌模块的消息

protected:
    CPaintManagerUI* m_pPM;

    CEditUI*        m_pEncryptFile;
    CEditUI*        m_pKey;
	CComboUI*		m_pDecryptType;
	CButtonUI*		m_pScanFile;
};


#pragma once

class CModifyKmcUI : public CContainerUI, public INotifyUI
{
public:
	CModifyKmcUI(CPaintManagerUI* pPM);
	~CModifyKmcUI();

	void DoInit();
	void InitDlg();	//初始化控件
	void Notify(TNotifyUI& msg); //处理内嵌模块的消息

protected:
	CPaintManagerUI* m_pPM;

	CComboUI*	m_pAid;
	CComboUI*	m_pOldKmc;
	CComboUI*	m_pNewKmc;
	CComboUI*	m_pNewDiv;
};


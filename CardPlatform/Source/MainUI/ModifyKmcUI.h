#pragma once

class CModifyKmcUI : public CContainerUI, public INotifyUI
{
public:
	CModifyKmcUI(CPaintManagerUI* pPM);
	~CModifyKmcUI();

	void DoInit();
	void InitDlg();	//��ʼ���ؼ�
	void Notify(TNotifyUI& msg); //������Ƕģ�����Ϣ

protected:
	CPaintManagerUI* m_pPM;

	CComboUI*	m_pAid;
	CComboUI*	m_pOldKmc;
	CComboUI*	m_pNewKmc;
	CComboUI*	m_pNewDiv;
};


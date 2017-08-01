#pragma once

class CTreeViewNav : public CContainerUI, public INotifyUI
{
public:
    CTreeViewNav(CPaintManagerUI* pPM);
    ~CTreeViewNav();

	void DoInit();
    void Notify(TNotifyUI& msg); //������Ƕģ�����Ϣ

protected:
	CPaintManagerUI*	m_pPM;
	CControlUI*		m_rootParentControl;
};


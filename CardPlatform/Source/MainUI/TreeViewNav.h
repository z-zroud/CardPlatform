#pragma once

class CTreeViewNav : public CContainerUI, public INotifyUI
{
public:
    CTreeViewNav(CPaintManagerUI* pPM);
    ~CTreeViewNav();

	void DoInit();
    void Notify(TNotifyUI& msg); //处理内嵌模块的消息

protected:
	CPaintManagerUI*	m_pPM;
	CControlUI*		m_rootParentControl;
};


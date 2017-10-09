#pragma once
#include <map>
using namespace std;

class CMainFrame : public WindowImplBase
{
public:
    CMainFrame();
    ~CMainFrame();

    CDuiString GetSkinFolder() { return _T("CardPlatformRes"); }
    CDuiString GetSkinFile() { return _T("MainFrame.xml"); }
    LPCTSTR GetWindowClassName(void) const { return _T("CardPlatform"); }

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    CControlUI* CreateControl(LPCTSTR pstrClass);
    void Notify(TNotifyUI& msg);
	void InitWindow();
    LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	void OnShowSysMenu(TNotifyUI& msg);


private:
	map<CDuiString, bool> m_SysFileMenuInfo;
    map<CDuiString, bool> m_terminalMenuInfo;
	map<CDuiString, bool> m_logMenuInfo;
	CTabLayoutUI*	m_tabLayoutMainPanel;
    CTabLayoutUI*   m_tabLayoutOutputPanel;
    CComboUI*       m_pComboReader;
    CRichEditUI*    m_pRichOutput;
};


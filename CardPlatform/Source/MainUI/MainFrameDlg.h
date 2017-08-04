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

    CControlUI* CreateControl(LPCTSTR pstrClass);
    void Notify(TNotifyUI& msg);
	void InitWindow();

private:
	void OnShowSysMenu(TNotifyUI& msg);


private:
	map<CDuiString, bool> m_SysFileMenuInfo;
	CTabLayoutUI*	m_tabLayout;
    CComboUI*       m_pComboReader;
};


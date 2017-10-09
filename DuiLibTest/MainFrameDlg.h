#pragma once
#include <map>

using namespace std;

class CMainFrame : public WindowImplBase
{
public:
    CMainFrame();
    ~CMainFrame();

    CDuiString GetSkinFolder() { return _T("DuiLibTestRes"); }
    CDuiString GetSkinFile() { return _T("MainFrame.xml"); }
    LPCTSTR GetWindowClassName(void) const { return _T("DuiLibTest"); }

    CControlUI* CreateControl(LPCTSTR pstrClass);
    void Notify(TNotifyUI& msg);

private:
	void OnShowSysMenu(TNotifyUI& msg);


private:
	map<CDuiString, bool> m_SysFileMenuInfo;
	map<CDuiString, bool> m_logMenuInfo;
};


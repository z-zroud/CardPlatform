#pragma once
#include <map>

using namespace std;

class CMainFrame : public WindowImplBase
{
public:
	CMainFrame();
	~CMainFrame();

	CDuiString GetSkinFolder() { return _T("ControlAttrRes"); }
	CDuiString GetSkinFile() { return _T("MainFrame.xml"); }
	LPCTSTR GetWindowClassName(void) const { return _T("ListDemo"); }

	void InitWindow();
	void Notify(TNotifyUI& msg);

private:
	CListUI*	pList;
};


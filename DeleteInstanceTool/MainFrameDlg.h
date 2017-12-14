#pragma once
#include <vector>
#include <string>
#include "CardIssue.h"
using namespace std;

class CMainFrame : public WindowImplBase
{
public:
    CMainFrame();
    ~CMainFrame();

    CDuiString GetSkinFolder() { return _T("DeleteInstanceRes"); }
    CDuiString GetSkinFile() { return _T("MainFrame.xml"); }
    LPCTSTR GetWindowClassName(void) const { return _T("DuiLibTest"); }
	void InitWindow();
    CControlUI* CreateControl(LPCTSTR pstrClass);
    void Notify(TNotifyUI& msg);


private:
	void GetInstance();
	void DeleteInstance(string aid);

private:
	CComboUI	*m_comboAids;
	CComboUI	*m_comboReaders;
	CRichEditUI	*m_richEdit;
	vector<string> m_aids;
	CardIssue	m_ci;
};


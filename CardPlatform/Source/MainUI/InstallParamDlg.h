#pragma once
#include <string>

using namespace std;

class CInstallParamDlg : public WindowImplBase
{
public:
    CInstallParamDlg() {};
    ~CInstallParamDlg() {}

    CDuiString GetSkinFolder() { return _T("CardPlatformRes"); }
    CDuiString GetSkinFile() { return _T("InstallParamViewDlg.xml"); }
    LPCTSTR GetWindowClassName(void) const { return _T("InstallDlg"); }

    LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);


    void Notify(TNotifyUI& msg);
    void InitWindow();
    void ShowDlg(HWND hWnd, const string &cfgFilePath);
    CLabelUI* CreateItem(const char* szText, const char* szPos);
    void FillInstallParam(const string &cfgFilePath);
private:

    CHorizontalLayoutUI*    m_pPackagePanel;
    CHorizontalLayoutUI*    m_pAppletPanel;
    CHorizontalLayoutUI*    m_pVarPanel;
    CHorizontalLayoutUI*    m_pPermissionPanel;
    CHorizontalLayoutUI*    m_pInstallParamPanel;
    CHorizontalLayoutUI*    m_pTokenPanel;   
    HWND                    m_hWnd;

};

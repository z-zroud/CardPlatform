#pragma once

#include "Util\IniParaser.h"

class CTerminalDlg : public WindowImplBase
{
public:
    CTerminalDlg() {}
    ~CTerminalDlg() {}

    CDuiString GetSkinFolder() { return _T("CardPlatformRes"); }
    CDuiString GetSkinFile() { return _T("TerminalDlg.xml"); }
    LPCTSTR GetWindowClassName(void) const { return _T("TerminalDlg"); }

    LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    void Notify(TNotifyUI& msg);
    void InitWindow();
    int ShowDlg(HWND hWnd);

    void ShowTags();
    void SaveTags();
private:
    CEditUI*    m_tag5F2A;
    CEditUI*    m_tag95;
    CEditUI*    m_tag9A;
    CEditUI*    m_tag9C;
    CEditUI*    m_tag9F02;
    CEditUI*    m_tag9F03;
    CEditUI*    m_tag9F09;
    CEditUI*    m_tag9F1A;
    CEditUI*    m_tag9F1B;
    CEditUI*    m_tag9F21;
    CEditUI*    m_tag9F37;
    CEditUI*    m_tag9F42;
    CEditUI*    m_tag9F4E;
    CEditUI*    m_tag9F66;
    CEditUI*    m_tag9F7A;
    CEditUI*    m_tagDF60;
    CEditUI*    m_tagDF69;

    INIParser   m_parse;
    int         m_nStatus;
    HWND        m_hWnd;
};

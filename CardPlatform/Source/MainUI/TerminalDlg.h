#pragma once

#define BTN_OK          1
#define BTN_CANCEL      2

#define ICO_INFO        1
#define ICO_WARNING     2
#define ICO_ERROR       3

#define STATUS_OK       0
#define STATUS_CANCEL   1

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

    int         m_nStatus;
    HWND        m_hWnd;
};

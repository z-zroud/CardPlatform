#pragma once

#define BTN_OK          1
#define BTN_CANCEL      2

#define ICO_INFO        1
#define ICO_WARNING     2
#define ICO_ERROR       3

#define STATUS_OK       0
#define STATUS_CANCEL   1

class CTipDlg : public WindowImplBase
{
public:
    CTipDlg() {}
    ~CTipDlg() {}

    CDuiString GetSkinFolder() { return _T("CardPlatformRes"); }
    CDuiString GetSkinFile() { return _T("TipDlg.xml"); }
    LPCTSTR GetWindowClassName(void) const { return _T("TipDlg"); }

    LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    

    void Notify(TNotifyUI& msg);
    void InitWindow();
    int ShowDlg(HWND hWnd, LPCTSTR szTilte, LPCTSTR szContent, int iconType, int btnType);

private:
    CLabelUI*   m_pTitle;
    CLabelUI*   m_pContent;
    CLabelUI*   m_pIcon;

    CButtonUI*  m_pBtnOK;
    CButtonUI*  m_pBtnCancel;

    int         m_nStatus;

    HWND        m_hWnd;
};

#include "StdAfx.h"
#include "TipDlg.h"

LRESULT CTipDlg::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
    ::ScreenToClient(*this, &pt);

    RECT rcClient;
    ::GetClientRect(*this, &rcClient);

    RECT rcCaption = m_PaintManager.GetCaptionRect();
    if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
        && pt.y >= rcCaption.top && pt.y < rcCaption.bottom) {
        CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(pt));
        if (pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 &&
            _tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
            _tcsicmp(pControl->GetClass(), _T("TextUI")) != 0 &&
            _tcsicmp(pControl->GetClass(), _T("SliderUI")) != 0)
            return HTCAPTION;
    }

    return HTCLIENT;
}


void CTipDlg::Notify(TNotifyUI& msg)
{
    if (msg.sType == _T("click"))
    {
        CDuiString name = msg.pSender->GetName();
        if (name == _T("btnOK")) {
            m_nStatus = STATUS_OK;
        }
        else if (name == _T("btnCancel") || name == _T("close")) {
            m_nStatus = STATUS_CANCEL;
        }
        ::DestroyWindow(m_hWnd);
    }
}

void CTipDlg::InitWindow()
{
    m_pTitle        = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("tipTitle")));
    m_pIcon         = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lbIcon")));
    m_pContent      = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lbContent")));

    m_pBtnOK        = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnOK")));
    m_pBtnCancel    = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnCancel")));
}

int CTipDlg::ShowDlg(HWND hWnd, LPCTSTR szTilte, LPCTSTR szContent, int iconType, int btnType)
{
    m_hWnd = this->Create(hWnd, _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0);

    m_pTitle->SetText(szTilte);
    m_pContent->SetText(szContent);

    if (btnType & BTN_CANCEL) {
        m_pBtnCancel->SetVisible(true);
    }

    this->SetIcon(IDI_MAIN_ICON);
    this->CenterWindow();
    this->ShowModal();

    return m_nStatus;
}
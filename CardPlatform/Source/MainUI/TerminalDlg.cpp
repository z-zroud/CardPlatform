#include "StdAfx.h"
#include "TerminalDlg.h"

LRESULT CTerminalDlg::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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


void CTerminalDlg::Notify(TNotifyUI& msg)
{
    if (msg.sType == _T("click"))
    {
        CDuiString name = msg.pSender->GetName();
        if (name == _T("btnTerminalSetting"))
        {

        }
        ::DestroyWindow(m_hWnd);
    }
}

void CTerminalDlg::InitWindow()
{
    m_tag5F2A = static_cast<CEditUI*>(m_PaintManager.FindControl(Tag5F2A));
    m_tag95 = static_cast<CEditUI*>(m_PaintManager.FindControl(Tag95));
    m_tag9A = static_cast<CEditUI*>(m_PaintManager.FindControl(Tag9A));
    m_tag9C = static_cast<CEditUI*>(m_PaintManager.FindControl(Tag9C));
    m_tag9F02 = static_cast<CEditUI*>(m_PaintManager.FindControl(Tag9F02));
    m_tag9F03 = static_cast<CEditUI*>(m_PaintManager.FindControl(Tag9F03));
    m_tag9F09 = static_cast<CEditUI*>(m_PaintManager.FindControl(Tag9F09));
    m_tag9F1A = static_cast<CEditUI*>(m_PaintManager.FindControl(Tag9F1A));
    m_tag9F1B = static_cast<CEditUI*>(m_PaintManager.FindControl(Tag9F1B));
    m_tag9F21 = static_cast<CEditUI*>(m_PaintManager.FindControl(Tag9F21));
    m_tag9F37 = static_cast<CEditUI*>(m_PaintManager.FindControl(Tag9F37));
    m_tag9F42 = static_cast<CEditUI*>(m_PaintManager.FindControl(Tag9F42));
    m_tag9F4E = static_cast<CEditUI*>(m_PaintManager.FindControl(Tag9F4E));
    m_tag9F66 = static_cast<CEditUI*>(m_PaintManager.FindControl(Tag9F66));
    m_tag9F7A = static_cast<CEditUI*>(m_PaintManager.FindControl(Tag9F7A));
    m_tagDF60 = static_cast<CEditUI*>(m_PaintManager.FindControl(TagDF60));
    m_tagDF69 = static_cast<CEditUI*>(m_PaintManager.FindControl(TagDF69));
}

int CTerminalDlg::ShowDlg(HWND hWnd)
{
    m_hWnd = this->Create(hWnd, _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0);

    this->SetIcon(IDI_MAIN_ICON);
    this->CenterWindow();
    this->ShowModal();

    return m_nStatus;
}
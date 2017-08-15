#include "StdAfx.h"
#include "TerminalDlg.h"

#define TERMINAL    _T("TERMINAL")

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
            SaveTags();
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

    CDuiString terminalPath = m_PaintManager.GetInstancePath() + _T("Configuration\\terminal.cfg");
    m_parse.Read(terminalPath.GetData());

    ShowTags();
}

void CTerminalDlg::SaveTags()
{
    m_parse.SetValue(TERMINAL, Tag5F2A, m_tag5F2A->GetText().GetData());
    m_parse.SetValue(TERMINAL, Tag95, m_tag95->GetText().GetData());
    m_parse.SetValue(TERMINAL, Tag9A, m_tag9A->GetText().GetData());
    m_parse.SetValue(TERMINAL, Tag9C, m_tag9C->GetText().GetData());
    m_parse.SetValue(TERMINAL, Tag9F02, m_tag9F02->GetText().GetData());
    m_parse.SetValue(TERMINAL, Tag9F03, m_tag9F03->GetText().GetData());
    m_parse.SetValue(TERMINAL, Tag9F09, m_tag9F09->GetText().GetData());
    m_parse.SetValue(TERMINAL, Tag9F1A, m_tag9F1A->GetText().GetData());
    m_parse.SetValue(TERMINAL, Tag9F1B, m_tag9F1B->GetText().GetData());
    m_parse.SetValue(TERMINAL, Tag9F21, m_tag9F21->GetText().GetData());
    m_parse.SetValue(TERMINAL, Tag9F37, m_tag9F37->GetText().GetData());
    m_parse.SetValue(TERMINAL, Tag9F42, m_tag9F42->GetText().GetData());
    m_parse.SetValue(TERMINAL, Tag9F4E, m_tag9F4E->GetText().GetData());
    m_parse.SetValue(TERMINAL, Tag9F66, m_tag9F66->GetText().GetData());
    m_parse.SetValue(TERMINAL, Tag9F7A, m_tag9F7A->GetText().GetData());
    m_parse.SetValue(TERMINAL, TagDF60, m_tagDF60->GetText().GetData());
    m_parse.SetValue(TERMINAL, TagDF69, m_tagDF69->GetText().GetData());
}

void CTerminalDlg::ShowTags()
{
    m_tag5F2A->SetText(m_parse.GetValue(TERMINAL, Tag5F2A).c_str());
    m_tag95->SetText(m_parse.GetValue(TERMINAL, Tag95).c_str());
    m_tag9A->SetText(m_parse.GetValue(TERMINAL, Tag9A).c_str());
    m_tag9C->SetText(m_parse.GetValue(TERMINAL, Tag9C).c_str());
    m_tag9F02->SetText(m_parse.GetValue(TERMINAL, Tag9F02).c_str());
    m_tag9F03->SetText(m_parse.GetValue(TERMINAL, Tag9F03).c_str());
    m_tag9F09->SetText(m_parse.GetValue(TERMINAL, Tag9F09).c_str());
    m_tag9F1A->SetText(m_parse.GetValue(TERMINAL, Tag9F1A).c_str());
    m_tag9F1B->SetText(m_parse.GetValue(TERMINAL, Tag9F1B).c_str());
    m_tag9F21->SetText(m_parse.GetValue(TERMINAL, Tag9F21).c_str());
    m_tag9F37->SetText(m_parse.GetValue(TERMINAL, Tag9F37).c_str());
    m_tag9F42->SetText(m_parse.GetValue(TERMINAL, Tag9F42).c_str());
    m_tag9F4E->SetText(m_parse.GetValue(TERMINAL, Tag9F4E).c_str());
    m_tag9F66->SetText(m_parse.GetValue(TERMINAL, Tag9F66).c_str());
    m_tag9F7A->SetText(m_parse.GetValue(TERMINAL, Tag9F7A).c_str());
    m_tagDF60->SetText(m_parse.GetValue(TERMINAL, TagDF60).c_str());
    m_tagDF69->SetText(m_parse.GetValue(TERMINAL, TagDF69).c_str());
}

int CTerminalDlg::ShowDlg(HWND hWnd)
{
    m_hWnd = this->Create(hWnd, _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0);

    this->SetIcon(IDI_MAIN_ICON);
    this->CenterWindow();
    this->ShowModal();

    return m_nStatus;
}
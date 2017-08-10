#include "StdAfx.h"
#include "InstallParamDlg.h"
#include "Personalization\InstallCfg.h"



LRESULT CInstallParamDlg::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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


void CInstallParamDlg::Notify(TNotifyUI& msg)
{
    if (msg.sType == _T("click"))
    {
        CDuiString name = msg.pSender->GetName();
        if (name == _T("closebtn"))
        {
            ::DestroyWindow(m_hWnd);
        }          
    }
}

void CInstallParamDlg::InitWindow()
{
    m_pPackagePanel         = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("packagePanel")));
    m_pAppletPanel          = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("appletPanel")));
    m_pPermissionPanel      = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("permissionPanel")));
    m_pInstallParamPanel    = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("paramPanel")));
    m_pTokenPanel           = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("tokenPanel")));
    m_pVarPanel             = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("varPanel")));
}

CLabelUI* CInstallParamDlg::CreateItem(const char* szText, const char* szPos)
{
    CLabelUI* pLabel = new CLabelUI;
    pLabel->SetAttribute(_T("float"), _T("true"));
    pLabel->SetAttribute(_T("pos"), szPos);
    pLabel->SetAttribute(_T("text"), szText);
    pLabel->SetAttribute(_T("align"), _T("center"));

    return pLabel;
}

void CInstallParamDlg::ShowDlg(HWND hWnd, const string &cfgFilePath)
{
    m_hWnd = this->Create(hWnd, _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0);
    FillInstallParam(cfgFilePath);    
    this->SetIcon(IDI_MAIN_ICON);
    this->CenterWindow();
    this->ShowModal();
}

void CInstallParamDlg::FillInstallParam(const string &cfgFilePath)
{
    CInstallCfg cfg(cfgFilePath);
    INSTALL_PARAM pseParam;
    INSTALL_PARAM ppseParam;
    INSTALL_PARAM appParam;

    cfg.GetInstallCfg(INSTALL_PSE, pseParam);
    cfg.GetInstallCfg(INSTALL_PPSE, ppseParam);
    cfg.GetInstallCfg(INSTALL_APP, appParam);

    auto lb1 = CreateItem(pseParam.strExeLoadFileAID.c_str(), _T("100,0,320,30"));
    auto lb2 = CreateItem(pseParam.strExeLoadFileAID.c_str(), _T("320,0,540,30"));
    auto lb3 = CreateItem(pseParam.strExeLoadFileAID.c_str(), _T("540,0,760,30"));
    m_pPackagePanel->Add(lb1);
    m_pPackagePanel->Add(lb2);
    m_pPackagePanel->Add(lb3);

    auto lb4 = CreateItem(pseParam.strExeModuleAID.c_str(), _T("100,0,320,30"));
    auto lb5 = CreateItem(pseParam.strExeModuleAID.c_str(), _T("320,0,540,30"));
    auto lb6 = CreateItem(pseParam.strExeModuleAID.c_str(), _T("540,0,760,30"));
    m_pAppletPanel->Add(lb4);
    m_pAppletPanel->Add(lb5);
    m_pAppletPanel->Add(lb6);

    auto lb7 = CreateItem(pseParam.strApplicationAID.c_str(), _T("100,0,320,30"));
    auto lb8 = CreateItem(pseParam.strApplicationAID.c_str(), _T("320,0,540,30"));
    auto lb9 = CreateItem(pseParam.strApplicationAID.c_str(), _T("540,0,760,30"));
    m_pVarPanel->Add(lb7);
    m_pVarPanel->Add(lb8);
    m_pVarPanel->Add(lb9);

    auto lb10 = CreateItem(pseParam.strPrivilege.c_str(), _T("100,0,320,30"));
    auto lb11 = CreateItem(pseParam.strPrivilege.c_str(), _T("320,0,540,30"));
    auto lb12 = CreateItem(pseParam.strPrivilege.c_str(), _T("540,0,760,30"));
    m_pPermissionPanel->Add(lb10);
    m_pPermissionPanel->Add(lb11);
    m_pPermissionPanel->Add(lb12);

    auto lb13 = CreateItem(pseParam.strInstallParam.c_str(), _T("100,0,320,30"));
    auto lb14 = CreateItem(pseParam.strInstallParam.c_str(), _T("320,0,540,30"));
    auto lb15 = CreateItem(pseParam.strInstallParam.c_str(), _T("540,0,760,30"));
    m_pInstallParamPanel->Add(lb13);
    m_pInstallParamPanel->Add(lb14);
    m_pInstallParamPanel->Add(lb15);

    auto lb16 = CreateItem(pseParam.strToken.c_str(), _T("100,0,320,30"));
    auto lb17 = CreateItem(pseParam.strToken.c_str(), _T("320,0,540,30"));
    auto lb18 = CreateItem(pseParam.strToken.c_str(), _T("540,0,760,30"));
    m_pTokenPanel->Add(lb16);
    m_pTokenPanel->Add(lb17);
    m_pTokenPanel->Add(lb18);

}



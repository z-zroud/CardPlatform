#include "StdAfx.h"
#include "CardCheckUI.h"
#include "IDialogBuilderCallbackEx.h"


CCardCheckUI::CCardCheckUI(CPaintManagerUI* pPM)
{
    m_pPM = pPM;
}

void CCardCheckUI::DoInit()
{
    CDialogBuilder builder;
    CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("CardCheck.xml"), (UINT)0, NULL, m_pPM));
    if (pTreeView) {
        this->Add(pTreeView);
        m_pPM->AddNotifier(this);
    }
    else {
        this->RemoveAll();
        return;
    }

    InitDlg();
}

void CCardCheckUI::InitDlg()
{
    m_pTransType = static_cast<CComboUI*>(m_pManager->FindControl(_T("comboTransType")));
    m_pOfflineAuthType = static_cast<CComboUI*>(m_pManager->FindControl(_T("comboOfflineAuthType")));
    m_pEncryptType = static_cast<CComboUI*>(m_pManager->FindControl(_T("comboEncryptType")));
    m_pKeyType = static_cast<CComboUI*>(m_pManager->FindControl(_T("comboKeyType")));

    //初始化交易类型
    m_pTransType->AddString(_T("PBOC"));
    m_pTransType->AddString(_T("qPBOC"));
    m_pTransType->AddString(_T("电子现金"));
    m_pTransType->SetCurSelected(0);

    //初始化脱机认证类型
    m_pOfflineAuthType->AddString(_T("DDA"));
    m_pOfflineAuthType->AddString(_T("CDA"));
    m_pOfflineAuthType->SetCurSelected(0);

    //初始化加密类型
    m_pEncryptType->AddString(_T("DES"));
    m_pEncryptType->AddString(_T("SM"));
    m_pEncryptType->SetCurSelected(0);

    //初始化密钥类型
    m_pKeyType->AddString(_T("UDK"));
    m_pKeyType->AddString(_T("MDK"));
    m_pKeyType->SetCurSelected(0);
}


CCardCheckUI::~CCardCheckUI()
{
}



void CCardCheckUI::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
    CDuiString name = msg.pSender->GetName();
}

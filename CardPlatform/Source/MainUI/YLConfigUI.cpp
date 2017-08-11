#include "StdAfx.h"
#include "YLConfigUI.h"
#include "IDialogBuilderCallbackEx.h"
#include "Util\FileDlg.h"
#include "DP\YLDP\YLDPConfig.h"

CYLConfigUI::CYLConfigUI(CPaintManagerUI* pPM)
{
    m_pPM = pPM;
}

CYLConfigUI::~CYLConfigUI()
{
}

void CYLConfigUI::DoInit()
{
    static bool bHandled = false;
    CDialogBuilder builder;
    CDialogBuilderCallbackEx cb(m_pPM);
    CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("YLConfig.xml"), (UINT)0, NULL, m_pPM));
    if (pTreeView)
    {
        this->Add(pTreeView);
       // m_pPM->AddNotifier(this); //暂不需要进行事件响应
    }
    else {
        this->RemoveAll();
        return;
    }

    InitDlg();
}

void CYLConfigUI::InitDlg()
{
    m_pEncryptData = static_cast<CEditUI*>(m_pPM->FindControl(_T("ylEncryptData")));
    m_pDecryptKey = static_cast<CEditUI*>(m_pPM->FindControl(_T("ylDecryptKey")));
    m_pValueData = static_cast<CEditUI*>(m_pPM->FindControl(_T("ylValueData")));
    m_pExchangeData = static_cast<CEditUI*>(m_pPM->FindControl(_T("ylExchangeData")));

    //初始化数据
    CYLDPConfig cfg;
    string filePath = m_pManager->GetInstancePath() + _T("Configuration\\YLConfig.cfg");
    if (cfg.Read(filePath))
    {
        m_pEncryptData->SetText(cfg.GetEncryptData().c_str());
        m_pDecryptKey->SetText(cfg.GetDecryptKey().c_str());
        m_pValueData->SetText(cfg.GetValueData().c_str());
        m_pExchangeData->SetText(cfg.GetExchangeData().c_str());
    }   
}




#include "StdAfx.h"
#include "YLConfigUI.h"
#include "IDialogBuilderCallbackEx.h"
#include "Util\FileDlg.h"


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
    m_pConvertFile = static_cast<CEditUI*>(m_pPM->FindControl(_T("ylDoConvert")));
}




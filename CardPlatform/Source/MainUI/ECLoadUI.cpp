#include "StdAfx.h"
#include "ECLoadUI.h"
#include "IDialogBuilderCallbackEx.h"


CECLoadUI::CECLoadUI(CPaintManagerUI* pPM)
{
    m_pPM = pPM;
}

CECLoadUI::~CECLoadUI()
{
}

void CECLoadUI::DoInit()
{
    CDialogBuilder builder;
    CDialogBuilderCallbackEx cb(m_pPM);
    CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("ECLoadScript.xml"), (UINT)0, NULL, m_pPM));
    if (pTreeView)
    {
        this->Add(pTreeView);
        m_pPM->AddNotifier(this);
    }
    else {
        this->RemoveAll();
        return;
    }

    InitDlg();
}

void CECLoadUI::InitDlg()
{

}


void CECLoadUI::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{

    if (msg.sType == _T("click"))
    {
        CDuiString name = msg.pSender->GetName();

    }
}



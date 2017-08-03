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
}


CCardCheckUI::~CCardCheckUI()
{
}



void CCardCheckUI::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
    CDuiString name = msg.pSender->GetName();
}

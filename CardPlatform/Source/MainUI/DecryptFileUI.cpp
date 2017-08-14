#include "StdAfx.h"
#include "DecryptFileUI.h"
#include "IDialogBuilderCallbackEx.h"
#include "Util\SqliteDB.h"


CDecryptFileUI::CDecryptFileUI(CPaintManagerUI* pPM)
{
    m_pPM = pPM;
}

CDecryptFileUI::~CDecryptFileUI()
{
}

void CDecryptFileUI::DoInit()
{
    CDialogBuilder builder;
    CDialogBuilderCallbackEx cb(m_pPM);
    CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("FileDecrypt.xml"), (UINT)0, NULL, m_pPM));
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

void CDecryptFileUI::InitDlg()
{
    m_pEncryptFile = static_cast<CEditUI*>(m_pManager->FindControl(_T("editEncryptFile")));
    m_pKey = static_cast<CEditUI*>(m_pManager->FindControl(_T("editKey")));
}



void CDecryptFileUI::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
    if (msg.sType == _T("click"))
    {
        CDuiString name = msg.pSender->GetName();
        if (name == _T("btnModify"))
        {

        }
    }
}

#include "StdAfx.h"
#include "DecryptFileUI.h"
#include "IDialogBuilderCallbackEx.h"
#include "Util\FileDlg.h"
#include "Util\DpDecrypt.h"


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
    m_pEncryptFile	= static_cast<CEditUI*>(m_pManager->FindControl(_T("editEncryptFile")));
	m_pScanFile		= static_cast<CButtonUI*>(m_pManager->FindControl(_T("btnEncryptFile")));
	m_pDecryptType	= static_cast<CComboUI*>(m_pManager->FindControl(_T("comboEncryptType")));
    m_pKey			= static_cast<CEditUI*>(m_pManager->FindControl(_T("editKey")));

	m_pDecryptType->AddAt(_T("DES3_ECB"), DES3_ECB);
	m_pDecryptType->SetCurSelected(DES3_ECB);
}



void CDecryptFileUI::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
    if (msg.sType == _T("click"))
    {
        CDuiString name = msg.pSender->GetName();
        if (name == _T("btnEncryptFile"))
        {
			CFileDlg fileDlg;
			string filePath = fileDlg.OpenFileDlg();
			m_pScanFile->SetText(filePath.c_str());
		}
		else if (name == _T("btnDoDecrypt"))
		{
			string file = m_pEncryptFile->GetText().GetData();
			string key = m_pKey->GetText().GetData();
			DECRYPT_TYPE type = static_cast<DECRYPT_TYPE>(m_pDecryptType->GetCurSel());
			DpDecrypt::Decrypt(key, file, type);
		}
    }
}

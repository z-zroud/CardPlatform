#include "StdAfx.h"
#include "YLConverterUI.h"
#include "IDialogBuilderCallbackEx.h"
#include "Util\FileDlg.h"


CYLConverterUI::CYLConverterUI(CPaintManagerUI* pPM)
{
    m_pPM = pPM;
}

CYLConverterUI::~CYLConverterUI()
{
}

void CYLConverterUI::DoInit()
{
    static bool bHandled = false;
    CDialogBuilder builder;
    CDialogBuilderCallbackEx cb(m_pPM);
    CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("YLConverter.xml"), (UINT)0, NULL, m_pPM));
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

void CYLConverterUI::InitDlg()
{
    m_pConvertFile = static_cast<CEditUI*>(m_pPM->FindControl(_T("ylDoConvert")));
}


void CYLConverterUI::Notify(TNotifyUI& msg) //������Ƕģ�����Ϣ
{
    
    if (msg.sType == _T("click"))
    {
        CDuiString name = msg.pSender->GetName();
        string editText;
        if (name == _T("ylBtnDoConvert"))
        {
            editText = m_pConvertFile->GetText();
        }
        else if (name == _T("ylBtnScanFile"))
        {            
			CFileDlg fileDlg;
			string filePath = fileDlg.OpenFileDlg();
			m_pConvertFile->SetText(filePath.c_str());
        }
    }
}

void CYLConverterUI::DoConvert()
{
    CEditUI* m_pEncryptData = static_cast<CEditUI*>(m_pPM->FindControl(_T("ylEncryptData")));
    CEditUI* m_pDecryptKey = static_cast<CEditUI*>(m_pPM->FindControl(_T("ylDecryptKey")));
    CEditUI* m_pValueData = static_cast<CEditUI*>(m_pPM->FindControl(_T("ylValueData")));
    CEditUI* m_pExchangeData = static_cast<CEditUI*>(m_pPM->FindControl(_T("ylExchangeData")));
}



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
    m_pConvertFile = static_cast<CEditUI*>(m_pPM->FindControl(_T("editDoConvert")));
}


void CYLConverterUI::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
    
    if (msg.sType == _T("click"))
    {
        CDuiString name = msg.pSender->GetName();
        string editText;
        if (name == _T("btnDoConvert"))
        {
            editText = m_pConvertFile->GetText();
        }
        else if (name == _T("btnScanFile"))
        {            
			CFileDlg fileDlg;
			string filePath = fileDlg.OpenFileDlg();
			m_pConvertFile->SetText(filePath.c_str());
        }
    }
}



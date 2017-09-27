#include "StdAfx.h"
#include "UIGEConverter.h"
#include "IDialogBuilderCallbackEx.h"
#include "Util\FileDlg.h"


CGEConverterUI::CGEConverterUI(CPaintManagerUI* pPM)
{
	m_pPM = pPM;
}

CGEConverterUI::~CGEConverterUI()
{
}

void CGEConverterUI::DoInit()
{
	CDialogBuilder builder;
	CDialogBuilderCallbackEx cb(m_pPM);
	CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("GEConverter.xml"), (UINT)0, NULL, m_pPM));
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

void CGEConverterUI::InitDlg()
{
	m_pConvertFile = static_cast<CEditUI*>(m_pPM->FindControl(_T("geDoConvert")));
}


void CGEConverterUI::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{

	if (msg.sType == _T("click"))
	{
		CDuiString name = msg.pSender->GetName();
		string editText;
		if (name == _T("geBtnDoConvert"))
		{
			editText = m_pConvertFile->GetText();
		}
		else if (name == _T("geBtnScanFile"))
		{
			CFileDlg fileDlg;
			string filePath = fileDlg.OpenFileDlg();
			m_pConvertFile->SetText(filePath.c_str());
		}
	}
}



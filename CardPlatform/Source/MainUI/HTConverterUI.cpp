#include "StdAfx.h"
#include "HTConverterUI.h"
#include "IDialogBuilderCallbackEx.h"
#include "Util\FileDlg.h"


CHTConverterUI::CHTConverterUI(CPaintManagerUI* pPM)
{
	m_pPM = pPM;
}

CHTConverterUI::~CHTConverterUI()
{
}

void CHTConverterUI::DoInit()
{
	static bool bHandled = false;
	CDialogBuilder builder;
	CDialogBuilderCallbackEx cb(m_pPM);
	CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("HTConverter.xml"), (UINT)0, NULL, m_pPM));
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

void CHTConverterUI::InitDlg()
{
	m_pConvertFile = static_cast<CEditUI*>(m_pPM->FindControl(_T("htDoConvert")));
}


void CHTConverterUI::Notify(TNotifyUI& msg) //������Ƕģ�����Ϣ
{

	if (msg.sType == _T("click"))
	{
		CDuiString name = msg.pSender->GetName();
		string editText;
		if (name == _T("htBtnDoConvert"))
		{
			editText = m_pConvertFile->GetText();
		}
		else if (name == _T("htBtnScanFile"))
		{
			CFileDlg fileDlg;
			string filePath = fileDlg.OpenFileDlg();
			m_pConvertFile->SetText(filePath.c_str());
		}
	}
}



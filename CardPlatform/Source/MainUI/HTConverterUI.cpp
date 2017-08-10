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
	CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("Converter.xml"), (UINT)0, NULL, m_pPM));
	if (pTreeView) {
		this->Add(pTreeView);
		if (!bHandled) {    //对于复用的组件，仅添加一次通知，避免多次重复事件的发生
			m_pPM->AddNotifier(this);
			bHandled = true;
		}

	}
	else {
		this->RemoveAll();
		return;
	}

	InitDlg();
}

void CHTConverterUI::InitDlg()
{
	m_pConvertFile = static_cast<CEditUI*>(m_pPM->FindControl(_T("editDoConvert")));
}


void CHTConverterUI::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{

	if (msg.sType == _T("click"))
	{
		CDuiString name = msg.pSender->GetName();
		string editText;
		if (name == _T("btnDoConvert"))
		{
			editText = m_pConvertFile->GetText();
		}
		else if (name == _T("btnScanFile1"))
		{
			CFileDlg fileDlg;
			string filePath = fileDlg.OpenFileDlg();
			m_pConvertFile->SetText(filePath.c_str());
		}
	}
}



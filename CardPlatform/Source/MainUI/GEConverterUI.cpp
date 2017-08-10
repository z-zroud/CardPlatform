#include "StdAfx.h"
#include "GEConverterUI.h"
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
	static bool bHandled = false;
	CDialogBuilder builder;
	CDialogBuilderCallbackEx cb(m_pPM);
	CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("Converter.xml"), (UINT)0, NULL, m_pPM));
	if (pTreeView) {
		this->Add(pTreeView);
		if (!bHandled) {    //���ڸ��õ�����������һ��֪ͨ���������ظ��¼��ķ���
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

void CGEConverterUI::InitDlg()
{
	m_pConvertFile = static_cast<CEditUI*>(m_pPM->FindControl(_T("editDoConvert")));
}


void CGEConverterUI::Notify(TNotifyUI& msg) //������Ƕģ�����Ϣ
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



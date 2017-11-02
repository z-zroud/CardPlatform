#include "StdAfx.h"
#include "UIEpScriptCheck.h"
#include "IDialogBuilderCallbackEx.h"


CEpScriptCheckUI::CEpScriptCheckUI(CPaintManagerUI* pPM)
{
	m_pPM = pPM;
}

CEpScriptCheckUI::~CEpScriptCheckUI()
{
}

void CEpScriptCheckUI::DoInit()
{
	CDialogBuilder builder;
	CDialogBuilderCallbackEx cb(m_pPM);
	CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("EpCheck.xml"), (UINT)0, NULL, m_pPM));
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

void CEpScriptCheckUI::InitDlg()
{

}


void CEpScriptCheckUI::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{

	if (msg.sType == _T("click"))
	{
		CDuiString name = msg.pSender->GetName();

	}
}



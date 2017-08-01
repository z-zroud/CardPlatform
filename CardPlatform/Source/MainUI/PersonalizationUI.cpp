#include "StdAfx.h"
#include "PersonaliztionUI.h"
#include "IDialogBuilderCallbackEx.h"


CPersonalizationUI::CPersonalizationUI(CPaintManagerUI* pPM)
{
	m_pPM = pPM;
}

void CPersonalizationUI::DoInit()
{
	CDialogBuilder builder;
	CDialogBuilderCallbackEx cb(m_pPM);
	CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("Personalization.xml"), (UINT)0, NULL, m_pPM));
	if (pTreeView) {
		this->Add(pTreeView);
		m_pPM->AddNotifier(this);
	}
	else {
		this->RemoveAll();
		return;
	}
}


CPersonalizationUI::~CPersonalizationUI()
{
}



void CPersonalizationUI::Notify(TNotifyUI& msg) //������Ƕģ�����Ϣ
{
	CDuiString name = msg.pSender->GetName();
	if (msg.sType == _T("click"))
	{
		if (name == _T("TestBtn"))
		{
			MessageBox(NULL, _T(":Sub:������˲��԰�ť"), _T("��ť����"), MB_OK);
		}
	}
	else if (msg.sType == _T("selectchanged"))
	{
		if (name == _T("TestBtn"))
		{
			MessageBox(NULL, _T(":Sub:������˲��԰�ť"), _T("��ť����"), MB_OK);
		}
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMDBCLICK)
	{
		if (name == _T("TestBtn"))
		{
			MessageBox(NULL, _T(":Sub:������˲��԰�ť"), _T("��ť����"), MB_OK);
		}
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMCLICK)
	{
		if (name == _T("TestBtn"))
		{
			MessageBox(NULL, _T(":Sub:������˲��԰�ť"), _T("��ť����"), MB_OK);
		}
	}
}

#include "StdAfx.h"
#include "MenuNav.h"



CMenuContainerUI::CMenuContainerUI(CPaintManagerUI* pPM)
{
	CDialogBuilder builder;
	CContainerUI* pSysMenu = static_cast<CContainerUI*>(builder.Create(_T("MenuNav.xml"), (UINT)0, NULL, pPM));
	if (pSysMenu) {
		this->Add(pSysMenu);
		pPM->AddNotifier(this);
	}
	else {
		this->RemoveAll();
		return;
	}
}


CMenuContainerUI::~CMenuContainerUI()
{
}



void CMenuContainerUI::Notify(TNotifyUI& msg) //������Ƕģ�����Ϣ
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

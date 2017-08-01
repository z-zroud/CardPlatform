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



void CMenuContainerUI::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
	CDuiString name = msg.pSender->GetName();
	if (msg.sType == _T("click"))
	{
		if (name == _T("TestBtn"))
		{
			MessageBox(NULL, _T(":Sub:您点击了测试按钮"), _T("按钮例子"), MB_OK);
		}
	}
	else if (msg.sType == _T("selectchanged"))
	{
		if (name == _T("TestBtn"))
		{
			MessageBox(NULL, _T(":Sub:您点击了测试按钮"), _T("按钮例子"), MB_OK);
		}
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMDBCLICK)
	{
		if (name == _T("TestBtn"))
		{
			MessageBox(NULL, _T(":Sub:您点击了测试按钮"), _T("按钮例子"), MB_OK);
		}
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMCLICK)
	{
		if (name == _T("TestBtn"))
		{
			MessageBox(NULL, _T(":Sub:您点击了测试按钮"), _T("按钮例子"), MB_OK);
		}
	}
}

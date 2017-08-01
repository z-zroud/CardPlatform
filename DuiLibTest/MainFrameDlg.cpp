#include "StdAfx.h"
#include "MainFrameDlg.h"



CMainFrame::CMainFrame()
{
	m_SysFileMenuInfo.insert(map<CDuiString, bool>::value_type(_T("file_New"), false));
	m_SysFileMenuInfo.insert(map<CDuiString, bool>::value_type(_T("file_Open"), false));
}


CMainFrame::~CMainFrame()
{
}

CControlUI* CMainFrame::CreateControl(LPCTSTR pstrClass)
{
    //if (_tcscmp(pstrClass, TREE_VIEW_NAV_UI) == 0) return new CTreeViewNav(&m_PaintManager);
    return NULL;
}

void CMainFrame::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
    CDuiString name = msg.pSender->GetName();
	OnShowSysMenu(msg);
    if (msg.sType == _T("click"))
    {
        if (name == _T("TestBtn"))
        {
            MessageBox(NULL, _T(":Sub:您点击了测试按钮"), _T("按钮例子"), MB_OK);
        }
        else {
            WindowImplBase::Notify(msg);
        }
	}
	else if (msg.sType == _T("valuechanged"))
	{
		WindowImplBase::Notify(msg);
	}
}

void CMainFrame::OnShowSysMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("btnFile"))
	{
		CMenuWnd *pMenu = new CMenuWnd();
		CPoint point(0, 0);
		GetCursorPos(&point);
		point.x = msg.pSender->GetX();
		point.y = msg.pSender->GetY() + msg.pSender->GetHeight();
		ClientToScreen(m_hWnd, &point);
		pMenu->CreateMenu(NULL, _T("FileMenu.xml"), point, &m_PaintManager, &m_SysFileMenuInfo);

	}
	else if (msg.pSender->GetName() == _T("btnEdit"))
	{

	}
	else if (msg.pSender->GetName() == _T("btnView"))
	{

	}
	else if (msg.pSender->GetName() == _T("btnProject"))
	{

	}
}


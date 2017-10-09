#include "StdAfx.h"
#include "MainFrameDlg.h"



CMainFrame::CMainFrame()
{

}


CMainFrame::~CMainFrame()
{
}


void CMainFrame::InitWindow()
{
	pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("list_data")));

}


void CMainFrame::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
	CDuiString name = msg.pSender->GetName();
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

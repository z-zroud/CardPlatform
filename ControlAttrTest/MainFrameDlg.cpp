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


void CMainFrame::Notify(TNotifyUI& msg) //������Ƕģ�����Ϣ
{
	CDuiString name = msg.pSender->GetName();
	if (msg.sType == _T("click"))
	{
		if (name == _T("TestBtn"))
		{
			MessageBox(NULL, _T(":Sub:������˲��԰�ť"), _T("��ť����"), MB_OK);
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

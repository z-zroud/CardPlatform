#include "StdAfx.h"
#include "MainFrameDlg.h"
#include "IDialogBuilderCallbackEx.h"




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
	CDialogBuilderCallbackEx cb(&m_PaintManager);
	CControlUI* pControl = cb.CreateControl(pstrClass);

    return pControl;
}

void CMainFrame::InitWindow()
{
	m_tabLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("mainPanelContainer")));
}

/************************************************************
* �������¼���Ӧ����
*************************************************************/
void CMainFrame::Notify(TNotifyUI& msg) //������Ƕģ�����Ϣ
{
    CDuiString name = msg.pSender->GetName();
    
    if (msg.sType == _T("click"))
    {
        if (name == _T("treeViewNav"))
        {
			if (m_tabLayout)
			{
				m_tabLayout->SelectItem(msg.wParam);
			}
        }
        else {
            WindowImplBase::Notify(msg);
			OnShowSysMenu(msg);
        }
    }
}

/**********************************************************
* ��Ӧ�˵���ť�¼�
***********************************************************/
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


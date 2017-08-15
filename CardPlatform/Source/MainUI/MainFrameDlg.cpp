#include "StdAfx.h"
#include "MainFrameDlg.h"
#include "IDialogBuilderCallbackEx.h"
#include "Interface\InterfaceInstance.h"
#include "TerminalDlg.h"


CMainFrame::CMainFrame()
{
	m_SysFileMenuInfo.insert(map<CDuiString, bool>::value_type(_T("file_New"), false));
	m_SysFileMenuInfo.insert(map<CDuiString, bool>::value_type(_T("file_Open"), false));

    m_terminalMenuInfo.insert(map<CDuiString, bool>::value_type(_T("terminalSettting"), false));
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

LRESULT CMainFrame::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = m_PaintManager.GetCaptionRect();
	if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom) {
		CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(pt));
		if (pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 &&
			_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
			_tcsicmp(pControl->GetClass(), _T("TextUI")) != 0 &&
			_tcsicmp(pControl->GetClass(), _T("SliderUI")) != 0)
			return HTCAPTION;
	}

	return HTCLIENT;
}

void CMainFrame::InitWindow()
{
    m_tabLayoutMainPanel = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("mainPanelContainer")));
    m_tabLayoutOutputPanel = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tabOutputPanel")));
    m_pComboReader = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("comboReaderList")));

    IPCSC* pPCSC = GetPCSCInterface();
    auto readers = pPCSC->GetAllReaders();
    for (auto reader : readers)
    {
        m_pComboReader->AddString(reader);
    }
    m_pComboReader->SetCurSelected(0);
    
    delete pPCSC;
}

/************************************************************
* 主界面事件响应函数
*************************************************************/
void CMainFrame::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
    CDuiString name = msg.pSender->GetName();
    
    if (msg.sType == _T("click"))
    {
        if (name == _T("treeViewNav"))
        {
			if (m_tabLayoutMainPanel)
			{
                m_tabLayoutMainPanel->SelectItem(msg.wParam);
			}
        }
        else if (name == _T("optOutput")) {
            m_tabLayoutOutputPanel->SelectItem(0);
        }
        else if (name == _T("optError")) {
            m_tabLayoutOutputPanel->SelectItem(1);
        }
        else {
            WindowImplBase::Notify(msg);
			OnShowSysMenu(msg);
        }
    }
}

LRESULT CMainFrame::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (uMsg == WM_MENUCLICK)
    {
        CDuiString strMenuName = CMenuWnd::GetClickedMenuName();
        if (strMenuName == _T("terminalSetting"))
        {
            CTerminalDlg termDlg;
            termDlg.ShowDlg(m_hWnd);
        }
    }

    return 0;
}

/**********************************************************
* 响应菜单按钮事件
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
	else if (msg.pSender->GetName() == _T("btnTerminal"))
	{
        CMenuWnd *pMenu = new CMenuWnd();
        CPoint point(0, 0);
        GetCursorPos(&point);
        point.x = msg.pSender->GetX();
        point.y = msg.pSender->GetY() + msg.pSender->GetHeight();
        ClientToScreen(m_hWnd, &point);
        pMenu->CreateMenu(NULL, _T("TerminalMenu.xml"), point, &m_PaintManager, &m_terminalMenuInfo);
	}
	else if (msg.pSender->GetName() == _T("btnView"))
	{

	}
	else if (msg.pSender->GetName() == _T("btnProject"))
	{

	}
}


#include "StdAfx.h"
#include "TreeViewNav.h"
#include "IDialogBuilderCallbackEx.h"

#define TAB_MODIFY_KMC		    0
#define TAB_DO_PERSONALIZATION	1
#define TAB_CHARD_CHECK         2

CTreeViewNav::CTreeViewNav(CPaintManagerUI* pPM)
{
	m_pPM = pPM;
}

void CTreeViewNav::DoInit()
{
	CDialogBuilder builder;
	CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("TreeViewNav.xml"), (UINT)0, NULL, m_pPM));
	if (pTreeView) {
		this->Add(pTreeView);
		m_pPM->AddNotifier(this);
	}
	else {
		this->RemoveAll();
		return;
	}

	CControlUI* m_rootControl = m_pManager->FindControl(_T("treeViewMenu"));
	if (m_rootControl)
	{
		m_rootParentControl = m_rootControl->GetParent();
	}
}


CTreeViewNav::~CTreeViewNav()
{
}



void CTreeViewNav::Notify(TNotifyUI& msg) //������Ƕģ�����Ϣ
{
    CDuiString name = msg.pSender->GetName();
	if (msg.sType == DUI_MSGTYPE_ITEMCLICK)
    {
        if (name == _T("ylDoConvertNode") ||
			name == _T("geDoConvertNode") ||
			name == _T("htDoConvertNode"))
		{
			m_pManager->SendNotify(m_rootParentControl, DUI_MSGTYPE_CLICK, (WPARAM)TAB_DO_PERSONALIZATION);
		}
		else if (name == _T("kmcNode")) {
			m_pManager->SendNotify(m_rootParentControl, DUI_MSGTYPE_CLICK, (WPARAM)TAB_MODIFY_KMC);
		}
        else if (name == _T("ccCardCheckNode")) {
            m_pManager->SendNotify(m_rootParentControl, DUI_MSGTYPE_CLICK, (WPARAM)TAB_CHARD_CHECK);
        }
    }
}

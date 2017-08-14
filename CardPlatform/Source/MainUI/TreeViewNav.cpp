#include "StdAfx.h"
#include "TreeViewNav.h"
#include "IDialogBuilderCallbackEx.h"

#define TAB_YL_CONFIG           0
#define TAB_DO_YL_CONVERTER     1
#define TAB_DO_HT_CONVERTER     2
#define TAB_DO_GE_CONVERTER     3
#define TAB_PERSO_CONFIG        4
#define TAB_DO_PERSONALIZATION	5
#define TAB_CHARD_CHECK         6
#define TAB_MODIFY_KMC		    7
#define TAB_FILE_DECRYPT        8

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
        if (name == _T("ylDoConvertNode")){
            m_pManager->SendNotify(m_rootParentControl, DUI_MSGTYPE_CLICK, (WPARAM)TAB_DO_YL_CONVERTER);
        }
        else if (name == _T("htDoConvertNode")) {
            m_pManager->SendNotify(m_rootParentControl, DUI_MSGTYPE_CLICK, (WPARAM)TAB_DO_HT_CONVERTER);
        }
        else if (name == _T("geDoConvertNode")) {
            m_pManager->SendNotify(m_rootParentControl, DUI_MSGTYPE_CLICK, (WPARAM)TAB_DO_GE_CONVERTER);
        }
        else if (name == _T("doPersonalizationNode")) {
            m_pManager->SendNotify(m_rootParentControl, DUI_MSGTYPE_CLICK, (WPARAM)TAB_DO_PERSONALIZATION);
        }
		else if (name == _T("kmcNode")) {
			m_pManager->SendNotify(m_rootParentControl, DUI_MSGTYPE_CLICK, (WPARAM)TAB_MODIFY_KMC);
		}
        else if (name == _T("ccCardCheckNode")) {
            m_pManager->SendNotify(m_rootParentControl, DUI_MSGTYPE_CLICK, (WPARAM)TAB_CHARD_CHECK);
        }
        else if (name == _T("ylOptionSettingNode")) {
            m_pManager->SendNotify(m_rootParentControl, DUI_MSGTYPE_CLICK, (WPARAM)TAB_YL_CONFIG);
        }
        else if (name == _T("personOptionSettingNode")) {
            m_pManager->SendNotify(m_rootParentControl, DUI_MSGTYPE_CLICK, (WPARAM)TAB_PERSO_CONFIG);
        }
        else if (name == _T("fileDecryptNode")) {
            m_pManager->SendNotify(m_rootParentControl, DUI_MSGTYPE_CLICK, (WPARAM)TAB_FILE_DECRYPT);
        }
    }
}

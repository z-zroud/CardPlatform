#include "StdAfx.h"
#include "ModifyKmcUI.h"
#include "IDialogBuilderCallbackEx.h"
#include "Util\SqliteDB.h"

CModifyKmcUI::CModifyKmcUI(CPaintManagerUI* pPM)
{
	m_pPM = pPM;
}

void CModifyKmcUI::DoInit()
{
	CDialogBuilder builder;
	CDialogBuilderCallbackEx cb(m_pPM);
	CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("ModifyKMCTool.xml"), (UINT)0, NULL, m_pPM));
	if (pTreeView) {
		this->Add(pTreeView);
		m_pPM->AddNotifier(this);
	}
	else {
		this->RemoveAll();
		return;
	}

	InitDlg();
}

void CModifyKmcUI::InitDlg()
{
	m_pAid = static_cast<CComboUI*>(m_pManager->FindControl(_T("comboAID")));
	m_pOldKmc = static_cast<CComboUI*>(m_pManager->FindControl(_T("comboOldKmc")));
	m_pNewKmc = static_cast<CComboUI*>(m_pManager->FindControl(_T("comboNewKmc")));
	m_pNewDiv = static_cast<CComboUI*>(m_pManager->FindControl(_T("comboNewDiv")));

    //初始化DIV flag
	m_pNewDiv->AddString("Not div");
	m_pNewDiv->AddString("CPG2.0.2");
    m_pNewDiv->AddString("CPG2.1.2");
    m_pNewDiv->SetCurSelected(0);

    //初始化新KMC、旧KMC
    DB_KMC kmcTable;
    vector<string> vecKmc;
    kmcTable.GetKmc(vecKmc);
    for (auto v : vecKmc)
    {
        m_pOldKmc->AddString(v.c_str());
        m_pNewKmc->AddString(v.c_str());
    }
    m_pOldKmc->SetCurSelected(0);
    m_pNewKmc->SetCurSelected(0);

    //初始化AID
    DB_AID aidTable;
    vector<string> vecAid;
    aidTable.GetAid(vecAid);
    for (auto v : vecAid)
    {
        m_pAid->AddString(v.c_str());
    }
    m_pAid->SetCurSelected(0);
}


CModifyKmcUI::~CModifyKmcUI()
{
}



void CModifyKmcUI::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
	CDuiString name = msg.pSender->GetName();
	CDuiString xxx;
	if (msg.sType == _T("click"))
	{
		if (name == _T("btnModify"))
		{
			//xxx = m_pNewDiv->GetCurItemString();
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

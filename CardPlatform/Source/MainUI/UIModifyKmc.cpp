#include "StdAfx.h"
#include "UIModifyKmc.h"
#include "IDialogBuilderCallbackEx.h"
#include "Util\SqliteDB.h"
#include "Tool\ModifyKmc.h"


DIV_METHOD_FLAG GetDivFlag(string flag)
{
    if (flag == NOT_DIV)  return DIV_METHOD_FLAG::NO_DIV;
    if (flag == CPG202)  return DIV_METHOD_FLAG::DIV_CPG202;
    if (flag == CPG212)  return DIV_METHOD_FLAG::DIV_CPG212;

    return DIV_METHOD_FLAG::NO_DIV;
}


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
	m_pNewDiv->AddString(NOT_DIV);
	m_pNewDiv->AddString(CPG202);
    m_pNewDiv->AddString(CPG212);
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
	if (msg.sType == _T("click"))
	{
        CDuiString name = msg.pSender->GetName();
		if (name == _T("btnModify"))
		{
            CComboUI* pComboReader = static_cast<CComboUI*>(m_pManager->FindControl(_T("comboReaderList")));
            CDuiString readerName = pComboReader->GetCurItemString();
            
            string aid      = m_pAid->GetCurItemString().GetData();
            string oldKmc   = m_pOldKmc->GetCurItemString().GetData();
            string newKmc   = m_pNewKmc->GetCurItemString().GetData();
            string newDiv   = m_pNewDiv->GetCurItemString().GetData();

            DIV_METHOD_FLAG flag    = GetDivFlag(newDiv);
            CModifyKmc* pModifyKmc  = new CModifyKmc(readerName.GetData());

            pModifyKmc->StartModifyKMC(aid, oldKmc, newKmc, flag);
		}
	}
}

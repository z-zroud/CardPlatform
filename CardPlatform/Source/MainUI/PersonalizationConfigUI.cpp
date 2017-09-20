#include "StdAfx.h"
#include "PersonalizationConfigUI.h"
#include "IDialogBuilderCallbackEx.h"
#include "Util\IniParaser.h"

#define PERSO_ROOT  _T("Perso")
#define PERSO_ENCRYPT_DATA  _T("ENCRYPT_DATA")

CPersonalizationConfigUI::CPersonalizationConfigUI(CPaintManagerUI* pPM)
{
    m_pPM = pPM;
}

CPersonalizationConfigUI::~CPersonalizationConfigUI()
{
}

void CPersonalizationConfigUI::DoInit()
{
    static bool bHandled = false;
    CDialogBuilder builder;
    CDialogBuilderCallbackEx cb(m_pPM);
    CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("PersonalizationConfig.xml"), (UINT)0, NULL, m_pPM));
    if (pTreeView)
    {
        this->Add(pTreeView);
        // m_pPM->AddNotifier(this); //�ݲ���Ҫ�����¼���Ӧ
    }
    else {
        this->RemoveAll();
        return;
    }

    InitDlg();
}

void CPersonalizationConfigUI::InitDlg()
{
    m_pEncryptData = static_cast<CEditUI*>(m_pPM->FindControl(_T("persoEncryptData")));

    //��ʼ������
    IniParser cfg;
    string filePath = m_pManager->GetInstancePath() + _T("Configuration\\Personalization.cfg");
    if (cfg.Read(filePath))
    {
        m_pEncryptData->SetText(cfg.GetValue(PERSO_ROOT, PERSO_ENCRYPT_DATA).c_str());
    }
}




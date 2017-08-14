#include "StdAfx.h"
#include "PersonaliztionUI.h"
#include "IDialogBuilderCallbackEx.h"
#include "MainUI\TipDlg.h"
#include "MainUI\InstallParamDlg.h"
#include "Interface\CardBase.h"
#include "Util\SqliteDB.h"
#include "Util\FileDlg.h"
#include <cstdio>
#include <io.h>

CPersonalizationUI::CPersonalizationUI(CPaintManagerUI* pPM)
{
	m_pPM = pPM;
}

CPersonalizationUI::~CPersonalizationUI()
{
}


void CPersonalizationUI::DoInit()
{
	CDialogBuilder builder;
	CDialogBuilderCallbackEx cb(m_pPM);
	CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("Personalization.xml"), (UINT)0, NULL, m_pPM));
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

void CPersonalizationUI::InitDlg()
{
    m_pAid = static_cast<CComboUI*>(m_pPM->FindControl(_T("comboAID")));
    m_pKmc = static_cast<CComboUI*>(m_pPM->FindControl(_T("comboKmc")));
    m_pDiv = static_cast<CComboUI*>(m_pPM->FindControl(_T("comboDiv")));
    m_pSecLevel = static_cast<CComboUI*>(m_pPM->FindControl(_T("comboSecLevel")));
    m_pCfgFile  = static_cast<CComboUI*>(m_pPM->FindControl(_T("comboConfig")));
    m_pCpsFile = static_cast<CEditUI*>(m_pPM->FindControl(_T("editCpsFile")));

    //��ʼ��AID
    DB_AID aidTable;
    vector<string> vecAid;
    aidTable.GetAid(vecAid);
    for (auto v : vecAid)
    {
        m_pAid->AddString(v.c_str());
    }
    m_pAid->SetCurSelected(0);

    //��ʼ����KMC����KMC
    DB_KMC kmcTable;
    vector<string> vecKmc;
    kmcTable.GetKmc(vecKmc);
    for (auto v : vecKmc)
    {
        m_pKmc->AddString(v.c_str());
    }
    m_pKmc->SetCurSelected(0);

    //��ʼ��DIV flag
    m_pDiv->AddString(NOT_DIV);
    m_pDiv->AddString(CPG202);
    m_pDiv->AddString(CPG212);
    m_pDiv->SetCurSelected(0);

    //��ʼ��Secure Level
    m_pSecLevel->AddString(NO_SECURE);
    m_pSecLevel->AddString(MAC);
    m_pSecLevel->SetCurSelected(0);

    //��ʼ���ļ��б�
    vector<string> vecFiles;
    GetFiles(_T(".\\Configuration\\InstallParams"), vecFiles);
    for (auto v : vecFiles)
    {
        m_pCfgFile->AddString(v.c_str());
    }
    if (m_pCfgFile->GetCount() > 0)
        m_pCfgFile->SetCurSelected(0);
}



void CPersonalizationUI::Notify(TNotifyUI& msg) //������Ƕģ�����Ϣ
{
	CDuiString name = msg.pSender->GetName();
	if (msg.sType == _T("click"))
	{
		if (name == _T("btnDoPerso"))   //��ʼ���˻�
		{
            CTipDlg* pTip = new CTipDlg;
            pTip->ShowDlg(m_pPM->GetPaintWindow(), _T("��ʾ"), _T("Test"), ICO_INFO, BTN_OK);
        }
        else if (name == _T("btnInstCfgView")) {   //�鿴��װ����
            string file = m_pCfgFile->GetCurItemString().GetData();
            string filePath = m_pPM->GetInstancePath().GetData() + string("\\Configuration\\InstallParams\\") +  file;
            CInstallParamDlg* pInstParamDlg = new CInstallParamDlg;

            pInstParamDlg->ShowDlg(m_pPM->GetPaintWindow(), filePath);
        }
        else if (name == _T("btnScanCpsFile"))
        {
            CFileDlg fileDlg;
            string filePath = fileDlg.OpenFileDlg();
            m_pCpsFile->SetText(filePath.c_str());
        }
    }
}

void CPersonalizationUI::GetFiles(string path, vector<string>& files)
{
    //�ļ����  
    long   hFile = 0;
    //�ļ���Ϣ  
    struct _finddata_t fileinfo;
    string p = path + _T("\\*");
    if ((hFile = _findfirst(p.c_str(), &fileinfo)) != -1)
    {
        do
        {
            //�����Ŀ¼,����֮  
            //�������,�����б�  
            if ((fileinfo.attrib &  _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    GetFiles(p.assign(path).append("\\").append(fileinfo.name), files);
            }
            else
            {
                //files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                files.push_back(fileinfo.name);
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}

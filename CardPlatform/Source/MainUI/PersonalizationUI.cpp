#include "StdAfx.h"
#include "PersonaliztionUI.h"
#include "IDialogBuilderCallbackEx.h"
#include "MainUI\TipDlg.h"
#include "MainUI\InstallParamDlg.h"
#include "Interface\CardBase.h"
#include "Util\SqliteDB.h"
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

    //初始化AID
    DB_AID aidTable;
    vector<string> vecAid;
    aidTable.GetAid(vecAid);
    for (auto v : vecAid)
    {
        m_pAid->AddString(v.c_str());
    }
    m_pAid->SetCurSelected(0);

    //初始化新KMC、旧KMC
    DB_KMC kmcTable;
    vector<string> vecKmc;
    kmcTable.GetKmc(vecKmc);
    for (auto v : vecKmc)
    {
        m_pKmc->AddString(v.c_str());
    }
    m_pKmc->SetCurSelected(0);

    //初始化DIV flag
    m_pDiv->AddString(NOT_DIV);
    m_pDiv->AddString(CPG202);
    m_pDiv->AddString(CPG212);
    m_pDiv->SetCurSelected(0);

    //初始化Secure Level
    m_pSecLevel->AddString(NO_SECURE);
    m_pSecLevel->AddString(MAC);
    m_pSecLevel->SetCurSelected(0);

    //初始化文件列表
    vector<string> vecFiles;
    GetFiles(_T(".\\Configuration\\InstallParams"), vecFiles);
    for (auto v : vecFiles)
    {
        m_pCfgFile->AddString(v.c_str());
    }
    if (m_pCfgFile->GetCount() > 0)
        m_pCfgFile->SetCurSelected(0);
}



void CPersonalizationUI::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
	CDuiString name = msg.pSender->GetName();
	if (msg.sType == _T("click"))
	{
		if (name == _T("btnDoPerso"))   //开始个人化
		{
            CTipDlg* pTip = new CTipDlg;
            pTip->ShowDlg(m_pPM->GetPaintWindow(), _T("提示"), _T("Test"), ICO_INFO, BTN_OK);
        }
        else if (name == _T("btnView")) {   //查看安装参数
            string file = m_pCfgFile->GetCurItemString().GetData();
            string filePath = m_pPM->GetInstancePath().GetData() + string("\\Configuration\\InstallParams\\") +  file;
            CInstallParamDlg* pInstParamDlg = new CInstallParamDlg;

            pInstParamDlg->ShowDlg(m_pPM->GetPaintWindow(), filePath);
        }
    }
}

void CPersonalizationUI::GetFiles(string path, vector<string>& files)
{
    //文件句柄  
    long   hFile = 0;
    //文件信息  
    struct _finddata_t fileinfo;
    string p = path + _T("\\*");
    if ((hFile = _findfirst(p.c_str(), &fileinfo)) != -1)
    {
        do
        {
            //如果是目录,迭代之  
            //如果不是,加入列表  
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

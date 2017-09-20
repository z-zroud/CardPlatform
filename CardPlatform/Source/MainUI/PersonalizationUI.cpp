#include "StdAfx.h"
#include "PersonaliztionUI.h"
#include "IDialogBuilderCallbackEx.h"
#include "MainUI\TipDlg.h"
#include "MainUI\InstallParamDlg.h"
#include "Interface\CardBase.h"
#include "Util\SqliteDB.h"
#include "Util\FileDlg.h"
#include "Util\Des0.h"
#include "Util\Base.h"
#include "Util\IniParaser.h"
#include "Interface\InterfaceInstance.h"
#include "Personalization\InstallCfg.h"
#include "Util\FileDlg.h"

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
    CFileDlg::GetFiles(_T(".\\Configuration\\InstallParams"), vecFiles);
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
            DoPersonaliztion();
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

bool CPersonalizationUI::SetSelectedApplication(string aid)
{
    CDuiString kmc = m_pKmc->GetText();
    if (m_pPCSC == NULL)
    {
        return false;     //�޷���ȡAPDU�ӿ�
    }
    bool bRet = m_pPCSC->SelectAID(aid);
    if (!bRet)
    {
        return false;     //ѡ��Ӧ��ʧ��
    }
    bRet = m_pPCSC->OpenSecureChannel(kmc.GetData());
    if (!bRet)
    {
        return false;     //�򿪰�ȫͨ��ʧ��
    }

    return true;
}

void CPersonalizationUI::DoPersonaliztion()
{
    CDuiString aid = m_pAid->GetText();
    CDuiString kmc = m_pKmc->GetText();
    CDuiString div = m_pDiv->GetText();
    CDuiString secLevel = m_pSecLevel->GetText();
    CDuiString instCfg  = m_pCfgFile->GetText();
    CDuiString cpfFile  = m_pCpsFile->GetText();

    //���Ӷ�����
    CComboUI* pComboReader = static_cast<CComboUI*>(m_pManager->FindControl(_T("comboReaderList")));
    CDuiString readerName = pComboReader->GetCurItemString();

    m_pPCSC = GetPCSCInterface(readerName.GetData());

    if (m_pPCSC == NULL)
    {
        return;     //�޷����Ӷ�����
    }
    IAPDU *pAPDU = m_pPCSC->GetAPDU();
    vector<APP_STATUS> status;
	APDU_RESPONSE reponse;
    if (pAPDU)
    {
        bool bResult = SetSelectedApplication(aid.GetData());
        if (!bResult)
            return;

        //ɾ����ǰӦ��
        pAPDU->GetAppStatusCmd(status, reponse);
        for (auto v : status)
        {
            pAPDU->DeleteAppCmd(v.strAID);
        }

        //���°�װӦ��
        CDuiString instCfgPath = m_pPM->GetInstancePath() + _T("Configuration\\InstallParams\\") + instCfg;
        CInstallCfg cfg(instCfgPath.GetData());
        for (int i = INSTALL_APP; i < INSTALL_MAX; i++)
        {
            INSTALL_PARAM param;
            cfg.GetInstallCfg((INSTALL_TYPE)i, param);
            pAPDU->InstallAppCmd(param.strExeLoadFileAID,
                param.strExeModuleAID,
                param.strApplicationAID,
                param.strPrivilege,
                param.strInstallParam,
                param.strToken,
				reponse);
        }
        INIParser ini;
        if (!ini.Read(cpfFile.GetData()))
        {
            return;     //��ȡCPS�ļ�ʧ��
        }

        //���˻�PSE
        string pseAid = cfg.GetApplicationAID(INSTALL_PSE);      
        if (!SetSelectedApplication(pseAid))
            return;
        string pse1 = ini.GetValue("Store_PSE_1", "Store_PSE_1");       
        pse1 = "0101" + Base::GetDataHexLen(pse1) + pse1;

        string pse2 = ini.GetValue("Store_PSE_2", "Store_PSE_2");
        string pse2Len = Base::GetDataHexLen(pse2);
        pse2 = _T("9102") + Base::Increase(pse2Len, 5) + _T("A5") + Base::Increase(pse2Len,3) + _T("88") +_T("0101") + pse2;
        //if (!pAPDU->StorePSEData(pse1, STORE_DATA_COMMON, true))
        //{
        //    return;     //���˻�PSEʧ��
        //}
        //if (!pAPDU->StorePSEData(pse2, STORE_DATA_LAST, false))
        //{
        //    return;     //���˻�PSEʧ��
        //}

        //���˻�PPSE
        string ppseAid = cfg.GetApplicationAID(INSTALL_PPSE);
        if (!SetSelectedApplication(ppseAid))
            return;
        string ppse = ini.GetValue("Store_PPSE", "Store_PPSE");
        string ppseLen = Base::GetDataHexLen(ppse);
        ppse = _T("9102") + Base::Increase(ppseLen, 5) + _T("A5") + Base::Increase(ppseLen, 3) + _T("BF0C") + ppseLen + ppse;
        //if (!pAPDU->StorePSEData(ppse, STORE_DATA_LAST, true))
        //{
        //    return;     // ���˻�PPSEʧ��
        //}

        //���˻�PBOC
        string pbocAid = cfg.GetApplicationAID(INSTALL_APP);
        if (!SetSelectedApplication(pbocAid))
            return;      
        auto vec = ConcatNodeWithSameSection(ini);
        int nVecCount = 0;
        for (auto v : vec)
        {
            nVecCount++;
            if (v.first == "Store_PSE_1" || v.first == "Store_PSE_2" || v.first == "Store_PPSE")
            {
                continue;
            }
            else {

                string sDataLen;
                if (v.second.length() > 0xFE)
                {
                    sDataLen = _T("81") + Base::GetDataHexLen(v.second);
                }
                else {
                    sDataLen = Base::GetDataHexLen(v.second);
                }

                int nDGI = stoi(v.first, 0, 16);
                if (nDGI <= 0x0501) //С��0x0501��GDI������Ҫ���70ģ��
                {
                    v.second = _T("70") + sDataLen + v.second;
                }

                STORE_DATA_TYPE type;
                if (vec.size() - 2 == nVecCount)    //PSE��PPSE���ļ�β����֮��
                {
                    type = STORE_DATA_LAST;
                }
                else {
                    type = GetStoreDataType(v.first);
                }
                
                if (type == STORE_DATA_ENCRYPT)
                {   //������������
                    string encKey = m_pPCSC->GetSessionEncKey(); //m_pPCSC->GetEncKey();
                    char szEncryptData[1024] = { 0 };
                    if (v.first != "8000")
                    {
                        v.second += "8000000000000000";
                    }
                    Des3_ECB(szEncryptData, (char*)encKey.c_str(), (char*)v.second.c_str(), v.second.length());
                    v.second = szEncryptData;
                }
				APDU_RESPONSE response;
                //�洢���˻�����
                if (!pAPDU->StoreDataCmd(v.first, v.second, type, false,response))
                {
                    return;     // ���˻�PBOCʧ��
                }
            }
        }
        //���˻�����ɹ�
    }
}

STORE_DATA_TYPE CPersonalizationUI::GetStoreDataType(string tag)
{
    string encryptTag[] = { "8000","8201","8202","8203","8204","8205" };
    string lastTag = "9207";
    if (tag == lastTag)
    {
        return STORE_DATA_LAST;
    }
    for (auto s : encryptTag)
    {
        if (tag == s)
            return STORE_DATA_ENCRYPT;
    }

    return STORE_DATA_COMMON;
}

vector<pair<string, string>> CPersonalizationUI::ConcatNodeWithSameSection(INIParser ini)
{
    vector<pair<string, string>> vecResult;
    auto nodes = ini.GetAllNodes();
    if (nodes.size() == 0)
    {
        return vecResult;
    }


    for (auto v : nodes)
    {
        string value;
        for (auto nodeIter : v.second.m_Dict)
        {
            value += nodeIter.second;
        }
        vecResult.push_back(pair<string, string>(v.first, value));
    }

    return vecResult;
}

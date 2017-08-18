#include "StdAfx.h"
#include "CardCheckUI.h"
#include "IDialogBuilderCallbackEx.h"
#include "Interface\CardBase.h"
#include "Interface\InterfaceInstance.h"
#include "CardCheck\EC\EC.h"
#include "CardCheck\PBOC\PBOC.h"
#include "CardCheck\qPBOC\QPBOC.h"
#include "TipDlg.h"


CCardCheckUI::CCardCheckUI(CPaintManagerUI* pPM)
{
    m_pPM = pPM;
}

void CCardCheckUI::DoInit()
{
    CDialogBuilder builder;
    CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("CardCheck.xml"), (UINT)0, NULL, m_pPM));
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

void CCardCheckUI::InitDlg()
{
    m_pTransType        = static_cast<CComboUI*>(m_pManager->FindControl(_T("comboTransType")));
    m_pOfflineAuthType  = static_cast<CComboUI*>(m_pManager->FindControl(_T("comboOfflineAuthType")));
    m_pEncryptType      = static_cast<CComboUI*>(m_pManager->FindControl(_T("comboEncryptType")));
    m_pKeyType          = static_cast<CComboUI*>(m_pManager->FindControl(_T("comboKeyType")));
    m_pAuth             = static_cast<CEditUI*>(m_pManager->FindControl(_T("editAC")));
    m_pMac              = static_cast<CEditUI*>(m_pManager->FindControl(_T("editMAC")));
    m_pEnc              = static_cast<CEditUI*>(m_pManager->FindControl(_T("editENC")));
    m_pSupportOffline   = static_cast<CCheckBoxUI*>(m_pManager->FindControl(_T("ckSupportOffline")));
    m_pTouchTrans       = static_cast<CCheckBoxUI*>(m_pManager->FindControl(_T("ckTouchTrans")));

    //初始化交易类型
    m_pTransType->AddAt(_T("PBOC"), TRANS_TYPE::TRANS_PBOC);
    m_pTransType->AddAt(_T("qPBOC"), TRANS_TYPE::TRANS_QPBOC);
    m_pTransType->AddAt(_T("EC"), TRANS_TYPE::TRANS_EC);
    m_pTransType->SetCurSelected(TRANS_TYPE::TRANS_PBOC);

    //初始化脱机认证类型
    m_pOfflineAuthType->AddAt(_T("DDA"), AUTHENCATE_TYPE::AUTH_DDA);
    m_pOfflineAuthType->AddAt(_T("CDA"), AUTHENCATE_TYPE::AUTH_CDA);
    m_pOfflineAuthType->SetCurSelected(AUTHENCATE_TYPE::AUTH_DDA);

    //初始化加密类型
    m_pEncryptType->AddAt(_T("DES"), ENCRYPT_TYPE::ENCRYPT_DES);
    m_pEncryptType->AddAt(_T("SM"), ENCRYPT_TYPE::ENCRYPT_SM);
    m_pEncryptType->SetCurSelected(ENCRYPT_TYPE::ENCRYPT_DES);

    //初始化密钥类型
    m_pKeyType->AddAt(_T("UDK"), KEY_TYPE::KEY_UDK);
    m_pKeyType->AddAt(_T("MDK"), KEY_TYPE::KEY_MDK);
    m_pKeyType->SetCurSelected(KEY_TYPE::KEY_UDK);
}


CCardCheckUI::~CCardCheckUI()
{
}

/*********************************************************************
* 功能：响应开始交易事件
**********************************************************************/
void CCardCheckUI::OnBtnDoTransClicked()
{
    CComboUI* pComboReader = static_cast<CComboUI*>(m_pManager->FindControl(_T("comboReaderList")));
    CDuiString readerName = pComboReader->GetCurItemString();
    IPCSC* pReader = GetPCSCInterface(readerName.GetData());

    ICommTransaction* pTrans = NULL;
    TRANS_TYPE transType = static_cast<TRANS_TYPE>(m_pTransType->GetCurSel());    
    switch (transType)
    {
    case TRANS_PBOC:
    {
        COMMUNICATION_TYPE commType = m_pTouchTrans->GetCheck() ? COMM_TOUCH : COMM_UNTOUCH;
        pTrans = new PBOC(commType, pReader);      
    }
    break;
    case TRANS_QPBOC:
        pTrans = new QPBOC(pReader);
        break;
    case TRANS_EC:
        pTrans = new EC(pReader);
        break;
    }

    AUTHENCATE_TYPE authType    = static_cast<AUTHENCATE_TYPE>(m_pOfflineAuthType->GetCurSel());
    ENCRYPT_TYPE encType        = static_cast<ENCRYPT_TYPE>(m_pEncryptType->GetCurSel());
    KEY_TYPE keyType            = static_cast<KEY_TYPE>(m_pKeyType->GetCurSel());
    string auth                 = m_pAuth->GetText().GetData();
    string enc                  = m_pEnc->GetText().GetData();
    string mac                  = m_pMac->GetText().GetData();

    if (pTrans)
    {
        pTrans->SetAuthencation(authType);
        pTrans->SetEncryption(encType);

        if (keyType == KEY_UDK) {
            pTrans->SetUdkAuth(auth);
            pTrans->SetUdkEnc(enc);
            pTrans->SetUdkMac(mac);
        }
        else if (keyType == KEY_MDK) {
            pTrans->SetMdkAuth(auth);
            pTrans->SetMdkEnc(enc);
            pTrans->SetMdkMac(mac);
        }
        
        bool bECLoadChecked = HandleScript(pTrans);
        pTrans->ExecScript(bECLoadChecked);

        pTrans->DoTrans();  //开始交易流程
    }
}

bool CCardCheckUI::HandleScript(ICommTransaction* pCommTrans)
{
    //处理 电子现金脚本
    CCheckBoxUI* pECLoadScript = static_cast<CCheckBoxUI*>(m_pPM->FindControl(_T("ckExeECLoadScript")));
    bool bExecECLoadScript = pECLoadScript->GetCheck();
    if (bExecECLoadScript)
    {
        CEditUI* pTag9F77 = static_cast<CEditUI*>(m_pPM->FindControl(_T("edit9F77")));
        CEditUI* pTag9F78 = static_cast<CEditUI*>(m_pPM->FindControl(_T("edit9F78")));
        CEditUI* pTag9F79 = static_cast<CEditUI*>(m_pPM->FindControl(_T("edit9F79")));
        CEditUI* pTagDF77 = static_cast<CEditUI*>(m_pPM->FindControl(_T("editDF77")));
        CEditUI* pTagDF78 = static_cast<CEditUI*>(m_pPM->FindControl(_T("editDF78")));
        CEditUI* pTagDF79 = static_cast<CEditUI*>(m_pPM->FindControl(_T("editDF79")));
        CEditUI* pTagDF62 = static_cast<CEditUI*>(m_pPM->FindControl(_T("editDF62")));

        //格式化tag 9F79
        string tag9F79Value = string(pTag9F79->GetText().GetData()) + _T("00");
        int prefix = 12 - tag9F79Value.length();
        if (prefix < 0)
        {
            CTipDlg tip;
            tip.ShowDlg(m_pPM->GetPaintWindow(), _T("Error"), _T("电子现金金额太大"), ICO_ERROR, BTN_OK);
        }
        tag9F79Value = string(_T("")).append(prefix, '0') + tag9F79Value;
        pCommTrans->SetScript(Tag9F77, pTag9F77->GetText().GetData());
        pCommTrans->SetScript(Tag9F78, pTag9F78->GetText().GetData());
        pCommTrans->SetScript(Tag9F79, tag9F79Value);
        pCommTrans->SetScript(TagDF77, pTagDF77->GetText().GetData());
        pCommTrans->SetScript(TagDF78, pTagDF78->GetText().GetData());
        pCommTrans->SetScript(TagDF79, pTagDF79->GetText().GetData());
        pCommTrans->SetScript(TagDF62, pTagDF62->GetText().GetData());
    }

    return bExecECLoadScript;
}

void CCardCheckUI::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
    if (msg.sType == _T("click"))
    {
        CDuiString name = msg.pSender->GetName();
        if (name == _T("btnCheckCard"))
        {
            OnBtnDoTransClicked();
        }
    }    
}

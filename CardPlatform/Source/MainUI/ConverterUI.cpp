#include "StdAfx.h"
#include "ConverterUI.h"
#include "IDialogBuilderCallbackEx.h"
#include "Util\FileDlg.h"


CConverterUI::CConverterUI(CPaintManagerUI* pPM)
{
    m_pPM = pPM;
}

CConverterUI::~CConverterUI()
{
}

void CConverterUI::DoInit()
{
    static bool bHandled = false;
    CDialogBuilder builder;
    CDialogBuilderCallbackEx cb(m_pPM);
    CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("Converter.xml"), (UINT)0, NULL, m_pPM));
    if (pTreeView) {
        this->Add(pTreeView);
        if (!bHandled) {    //���ڸ��õ�����������һ��֪ͨ���������ظ��¼��ķ���
            m_pPM->AddNotifier(this);
            bHandled = true;
        }
       
    }
    else {
        this->RemoveAll();
        return;
    }

    InitDlg();
}

void CConverterUI::InitDlg()
{
    m_pConvertFile = static_cast<CEditUI*>(m_pPM->FindControl(_T("editDoConvert")));
}


void CConverterUI::Notify(TNotifyUI& msg) //������Ƕģ�����Ϣ
{
    
    if (msg.sType == _T("click"))
    {
        CDuiString name = msg.pSender->GetName();
        string editText;
        if (name == _T("btnDoConvert"))
        {
            editText = m_pConvertFile->GetText();
        }
        else if (name == _T("btnScanFile1"))
        {            
            CControlUI* pControl = msg.pSender->GetParent();
            int i = 0;
            while (pControl && i < 2)
            {
                pControl = pControl->GetParent();
                i++;
            }
            if (pControl == NULL)    return;
            if (pControl->GetName() == _T("ylConverterPanel"))      //��������DP��ʽ
            {
                CFileDlg fileDlg;
                string filePath = fileDlg.OpenFileDlg();
                m_pConvertFile->SetText(filePath.c_str());
            }
            else if (pControl->GetName() == _T("htConverterPanel")) {   //������DP��ʽ
                CFileDlg fileDlg;
                string filePath = fileDlg.OpenFileDlg();
                m_pConvertFile->SetText(filePath.c_str());
            }
            else if (pControl->GetName() == _T("geConverterPanel")) {   //������DP��ʽ
                CFileDlg fileDlg;
                string filePath = fileDlg.OpenFileDlg();
                m_pConvertFile->SetText(filePath.c_str());
            }

        }
    }
}



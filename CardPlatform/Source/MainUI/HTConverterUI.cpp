#include "StdAfx.h"
#include "HTConverterUI.h"
#include "IDialogBuilderCallbackEx.h"
#include "Util\FileDlg.h"
#include "DP\HTDP\HTDPParser.h"


CHTConverterUI::CHTConverterUI(CPaintManagerUI* pPM)
{
	m_pPM = pPM;
}

CHTConverterUI::~CHTConverterUI()
{
}

void CHTConverterUI::DoInit()
{
	static bool bHandled = false;
	CDialogBuilder builder;
	CDialogBuilderCallbackEx cb(m_pPM);
	CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("HTConverter.xml"), (UINT)0, NULL, m_pPM));
	if (pTreeView)
	{
		this->Add(pTreeView);
		m_pPM->AddNotifier(this);
	}
	else {
		this->RemoveAll();
		return;
	}

	InitDlg();
}

void CHTConverterUI::InitDlg()
{
    m_pConvertFolder = static_cast<CEditUI*>(m_pPM->FindControl(_T("htDoConvert")));
}


void CHTConverterUI::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{

	if (msg.sType == _T("click"))
	{
		CDuiString name = msg.pSender->GetName();
		string htDpFolder;
		if (name == _T("htBtnDoConvert"))
		{
            vector<string> files;
            HTDPParaser parser;

            htDpFolder = m_pConvertFolder->GetText();            
            CFileDlg::GetFiles(htDpFolder, files);
            for (auto file : files)
            {
                parser.Read(file);
            }
		}
		else if (name == _T("htBtnScanFolder"))
		{
			CFileDlg fileDlg;
            string filePath = fileDlg.OpenFolderDlg();
            m_pConvertFolder->SetText(filePath.c_str());
		}
	}
}



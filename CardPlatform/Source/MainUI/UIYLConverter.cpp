#include "StdAfx.h"
#include "UIYLConverter.h"
#include "IDialogBuilderCallbackEx.h"
#include "Util\FileDlg.h"
#include "Util\StringParaser.h"
#include "DP\YLDP\YLDPParaser.h"


CYLConverterUI::CYLConverterUI(CPaintManagerUI* pPM)
{
    m_pPM = pPM;
}

CYLConverterUI::~CYLConverterUI()
{
}

void CYLConverterUI::DoInit()
{
    static bool bHandled = false;
    CDialogBuilder builder;
    CDialogBuilderCallbackEx cb(m_pPM);
    CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("YLConverter.xml"), (UINT)0, NULL, m_pPM));
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

void CYLConverterUI::InitDlg()
{
    m_pConvertFile = static_cast<CEditUI*>(m_pPM->FindControl(_T("ylDoConvert")));
}


void CYLConverterUI::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
    
    if (msg.sType == _T("click"))
    {
        CDuiString name = msg.pSender->GetName();
        if (name == _T("ylBtnDoConvert"))
        {
            DoConvert();
        }
        else if (name == _T("ylBtnScanFile"))
        {            
			CFileDlg fileDlg;
			string filePath = fileDlg.OpenFileDlg();
			m_pConvertFile->SetText(filePath.c_str());
        }
    }
}

void CYLConverterUI::DoConvert()
{
    CEditUI* m_pEncryptData = static_cast<CEditUI*>(m_pPM->FindControl(_T("ylEncryptData")));
    CEditUI* m_pDecryptKey = static_cast<CEditUI*>(m_pPM->FindControl(_T("ylDecryptKey")));
    CEditUI* m_pValueData = static_cast<CEditUI*>(m_pPM->FindControl(_T("ylValueData")));
    CEditUI* m_pExchangeData = static_cast<CEditUI*>(m_pPM->FindControl(_T("ylExchangeData")));

    vector<string> vecEncryptData;
    vector<string> vecValueData;
    vector<string> vecExchangeData;

    Tool::Stringparser::SplitString(m_pEncryptData->GetText().GetData(), vecEncryptData, _T(","));
    Tool::Stringparser::SplitString(m_pValueData->GetText().GetData(), vecValueData, _T(","));
    Tool::Stringparser::SplitString(m_pExchangeData->GetText().GetData(), vecExchangeData, _T("("), _T(")"));

    string decryptKey = m_pDecryptKey->GetText().GetData();

    YLDpParser* pDP = new YLDpParser;
    pDP->SetDecryptKey(decryptKey);
    pDP->SetEncryptTag(vecEncryptData);

    map<string, string> mapExchangeDGI;
    for (auto v : vecExchangeData)
    {
        vector<string> temp;
        Tool::Stringparser::SplitString(v, temp, _T(","));
        if (temp.size() != 2)
        {
            return;
        }
        mapExchangeDGI.insert(make_pair<string, string>(string(temp[0]), string(temp[1])));
    }
    pDP->SetExchangeDGI(mapExchangeDGI);

    vector<unsigned short> vecValueDGI;
    for (auto v : vecValueData)
    {
        int result = stoi(v, 0, 16);
        vecValueDGI.push_back(static_cast<unsigned int>(result));
    }
    pDP->SetOnlyValueOfDGI(vecValueDGI);
    string filePath = m_pConvertFile->GetText().GetData();
    pDP->Read(filePath);

    string result = filePath;
    int index = result.find_last_of('\\');
    result = result.substr(0, index + 1) + "conv";
    pDP->Save(result);
}



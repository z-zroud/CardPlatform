#include "StdAfx.h"
#include "MainFrameDlg.h"
#include "Tool.h"


CMainFrame::CMainFrame()
{

}


CMainFrame::~CMainFrame()
{
}

CControlUI* CMainFrame::CreateControl(LPCTSTR pstrClass)
{
    //if (_tcscmp(pstrClass, TREE_VIEW_NAV_UI) == 0) return new CTreeViewNav(&m_PaintManager);
    return NULL;
}

void CMainFrame::InitWindow()
{
	m_comboAids = static_cast<CComboUI*>(m_PaintManager.FindControl("comboInstanceAid"));
	m_comboReaders = static_cast<CComboUI*>(m_PaintManager.FindControl("comboReaders"));
	m_richEdit = static_cast<CRichEditUI*>(m_PaintManager.FindControl("rtLog"));

	if (!m_ci.Init())
	{
		m_richEdit->AppendText("load des_rsa_jmj.dll failed.\n");
	}
	vector<char*> readers = m_ci.GetAllReaders();
	for (auto item : readers)
	{
		m_comboReaders->AddString(item);
	}
	if (readers.size() > 0)
	{
		m_comboReaders->SetCurSelected(0);
	}
}



void CMainFrame::GetInstance()
{
	string cmd = "80F24000 02 4F00";
	string resp;
	m_aids.clear();
	if (m_ci.SendApdu(cmd,resp))
	{
		unsigned int i = 0;
		while (i < resp.length())
		{

			int len = 2 * stoi(resp.substr(i, 2), 0, 16);
			string aid = resp.substr(i + 2, len);
			i += 6 + len;

			m_aids.push_back(aid);
		}
	}
}

void CMainFrame::DeleteInstance(string aid)
{
	string aidLen = Tool::GetStringLen(aid);
	string totalDataLen = Tool::IncStringLenStep(aidLen, 2);
	string cmd = "80E40000" + totalDataLen + "4F" + aidLen + aid;
	string resp;
	if (!m_ci.SendApdu(cmd, resp))
	{
		string info = m_ci.GetErrorInfo() + "\n";
		m_richEdit->AppendText(info.c_str());
		return;
	}
	string info = "Delete instance " + aid + " sucess\n";
	m_richEdit->AppendText(info.c_str());
}

void CMainFrame::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
    CDuiString name = msg.pSender->GetName();

    if (msg.sType == _T("click"))
    {
        if (name == _T("btnGetAid"))
        {
			m_ci.OpenReader(m_comboReaders->GetCurItemString().GetData());
			string result;
			string selectISDCmd = "00A40400 00";
			if (!m_ci.SendApdu(selectISDCmd, result))
			{
				m_richEdit->AppendText("Select ISD failed.");
			}
			if (!m_ci.ExtAuth())
			{
				m_richEdit->AppendText("do external auth failed.\n");
			}
			GetInstance();
			for (auto item : m_aids)
			{
				m_comboAids->AddString(item.c_str());
			}	
			if (m_aids.size() > 0)
			{
				m_comboAids->SetCurSelected(0);
				m_richEdit->AppendText("Get Instance aid sucess.\n");
			}
        }
		else if (name == _T("btnDeleteAll"))
		{
			for (auto iter = m_aids.rbegin(); iter != m_aids.rend(); iter++)
			{
				DeleteInstance(*iter);
			}			
			m_comboAids->RemoveAll();
		}
		else if (name == _T("btnModifyKmc"))
		{
			if (m_ci.ChangeKmc()) {
				m_richEdit->AppendText("change KMC sucess.\n");
			}
			else {
				string info = m_ci.GetErrorInfo();
				m_richEdit->AppendText(info.c_str());
				m_richEdit->AppendText("change KMC failed.\n");
			}
		}
		else if (name == "btnClearLog")
		{
			m_richEdit->SetText("");
		}
        else {
            WindowImplBase::Notify(msg);
        }
	}
}




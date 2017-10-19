#include "StdAfx.h"
#include "MainFrameDlg.h"
#include <process.h>

CMainFrame::CMainFrame()
{

}


CMainFrame::~CMainFrame()
{
}


unsigned int WINAPI DoProgress(LPVOID param)
{
	CProgressUI* pProgress = static_cast<CProgressUI*>(param);
	int value = pProgress->GetValue();
	int maxValue = pProgress->GetMaxValue();
	int minValue = pProgress->GetMinValue();
	while (value <= maxValue)
	{
		value += 1;
		pProgress->SetValue(value);
		Sleep(100);
		if (value == maxValue) {
			value = 0;
		}
	}

	return 0;
}

unsigned int WINAPI DoCircleProgress(LPVOID param)
{
	CCircleProgressUI* pProgress = static_cast<CCircleProgressUI*>(param);
	int value = pProgress->GetValue();
	int maxValue = pProgress->GetMaxValue();
	int minValue = pProgress->GetMinValue();
	while (value <= maxValue)
	{
		value += 1;
		pProgress->SetValue(value);
		Sleep(100);
		if (value == maxValue) {
			value = 0;
		}
	}

	return 0;
}

unsigned int WINAPI DoChangeTextLen(LPVOID param)
{
	CTextUI* pTextCtl = static_cast<CTextUI*>(param);
	int i = 0; 
	while (i < 5)
	{
		CDuiString value = pTextCtl->GetText();
		value.Append("Lable");
		pTextCtl->SetText(value.GetData());
		pTextCtl->Invalidate();
	}

	return 0;
}

void CMainFrame::InitWindow()
{
	pProgress = static_cast<CProgressUI*>(m_PaintManager.FindControl(_T("progress")));
	pCircleProgress = static_cast<CCircleProgressUI*>(m_PaintManager.FindControl(_T("circleProgress")));
	m_pTextCtl = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("text1")));
	_beginthreadex(NULL, 0, DoProgress, (void*)pProgress, 0, NULL);
	_beginthreadex(NULL, 0, DoCircleProgress, (void*)pCircleProgress, 0, NULL);
	_beginthreadex(NULL, 0, DoChangeTextLen, (void*)m_pTextCtl, 0, NULL);
}


void CMainFrame::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
	CDuiString name = msg.pSender->GetName();
	if (msg.sType == _T("click"))
	{
		if (name == _T("TestBtn"))
		{
			MessageBox(NULL, _T(":Sub:您点击了测试按钮"), _T("按钮例子"), MB_OK);
		}
		else {
			WindowImplBase::Notify(msg);
		}
	}
	else if (msg.sType == _T("valuechanged"))
	{
		WindowImplBase::Notify(msg);
	}
}

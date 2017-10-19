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

void CMainFrame::InitWindow()
{
	pProgress = static_cast<CProgressUI*>(m_PaintManager.FindControl(_T("progress")));
	pCircleProgress = static_cast<CCircleProgressUI*>(m_PaintManager.FindControl(_T("circleProgress")));
	_beginthreadex(NULL, 0, DoProgress, (void*)pProgress, 0, NULL);
	_beginthreadex(NULL, 0, DoCircleProgress, (void*)pCircleProgress, 0, NULL);
}


void CMainFrame::Notify(TNotifyUI& msg) //������Ƕģ�����Ϣ
{
	CDuiString name = msg.pSender->GetName();
	if (msg.sType == _T("click"))
	{
		if (name == _T("TestBtn"))
		{
			MessageBox(NULL, _T(":Sub:������˲��԰�ť"), _T("��ť����"), MB_OK);
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

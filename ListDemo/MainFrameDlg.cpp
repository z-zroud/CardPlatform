#include "StdAfx.h"
#include "MainFrameDlg.h"



CMainFrame::CMainFrame()
{

}


CMainFrame::~CMainFrame()
{
}


void CMainFrame::InitWindow()
{
	pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("list_data")));

	if (pList)
	{
		CListTextElementUI* pTextItem = new CListTextElementUI();

		pList->Add(pTextItem);
		pTextItem->SetAttribute(_T("align"), _T("left"));
		pTextItem->SetText(0, _T("0"));
		pTextItem->SetText(2, _T("TT"));
		pTextItem->SetText(1, _T("Desc"));

		CListLabelElementUI* pLabelItem = new CListLabelElementUI();
		pList->Add(pLabelItem);
		pLabelItem->SetAlign(DT_LEFT);
		pLabelItem->SetText(_T("Desc1"));

		
		for (int i = 0; i < 20; i++)
		{
			CDialogBuilder builder;
			CListContainerElementUI* pLine = (CListContainerElementUI*)(builder.Create(_T("sigle_list_item_column.xml"), (UINT)0));
			if (pLine != NULL)
			{
				pList->Add(pLine);
				DeleteObject(pLine);
				pLine = NULL;
			}
		}
	}
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

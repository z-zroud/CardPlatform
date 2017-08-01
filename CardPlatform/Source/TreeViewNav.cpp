#include "StdAfx.h"
#include "MainUI/TreeViewNav.h"



CTreeViewNav::CTreeViewNav(CPaintManagerUI* pPM)
{
    CDialogBuilder builder;
    CContainerUI* pTreeView = static_cast<CContainerUI*>(builder.Create(_T("TreeViewNav.xml"), (UINT)0, NULL, pPM));
    if (pTreeView) {
        this->Add(pTreeView);
        pPM->AddNotifier(this);
    }
    else {
        this->RemoveAll();
        return;
    }
}


CTreeViewNav::~CTreeViewNav()
{
}



void CTreeViewNav::Notify(TNotifyUI& msg) //������Ƕģ�����Ϣ
{
    CDuiString name = msg.pSender->GetName();
    if (msg.sType == _T("click"))
    {
        if (name == _T("TestBtn"))
        {
            MessageBox(NULL, _T(":Sub:������˲��԰�ť"), _T("��ť����"), MB_OK);
        }
    }
    else if (msg.sType == _T("selectchanged"))
    {
        if (name == _T("TestBtn"))
        {
            MessageBox(NULL, _T(":Sub:������˲��԰�ť"), _T("��ť����"), MB_OK);
        }
    }
    else if (msg.sType == DUI_MSGTYPE_ITEMDBCLICK)
    {
        if (name == _T("TestBtn"))
        {
            MessageBox(NULL, _T(":Sub:������˲��԰�ť"), _T("��ť����"), MB_OK);
        }
    }
    else if (msg.sType == DUI_MSGTYPE_ITEMCLICK)
    {
        if (name == _T("TestBtn"))
        {
            MessageBox(NULL, _T(":Sub:������˲��԰�ť"), _T("��ť����"), MB_OK);
        }
    }
}

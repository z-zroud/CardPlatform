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



void CTreeViewNav::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
    CDuiString name = msg.pSender->GetName();
    if (msg.sType == _T("click"))
    {
        if (name == _T("TestBtn"))
        {
            MessageBox(NULL, _T(":Sub:您点击了测试按钮"), _T("按钮例子"), MB_OK);
        }
    }
    else if (msg.sType == _T("selectchanged"))
    {
        if (name == _T("TestBtn"))
        {
            MessageBox(NULL, _T(":Sub:您点击了测试按钮"), _T("按钮例子"), MB_OK);
        }
    }
    else if (msg.sType == DUI_MSGTYPE_ITEMDBCLICK)
    {
        if (name == _T("TestBtn"))
        {
            MessageBox(NULL, _T(":Sub:您点击了测试按钮"), _T("按钮例子"), MB_OK);
        }
    }
    else if (msg.sType == DUI_MSGTYPE_ITEMCLICK)
    {
        if (name == _T("TestBtn"))
        {
            MessageBox(NULL, _T(":Sub:您点击了测试按钮"), _T("按钮例子"), MB_OK);
        }
    }
}

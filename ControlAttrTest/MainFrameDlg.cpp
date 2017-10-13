#include "StdAfx.h"
#include "MainFrameDlg.h"
#include <TextServ.h>
#include <RichOle.h>
#include "../AXCtl/AXCtlidl.h"

CMainFrame::CMainFrame()
{

}


CMainFrame::~CMainFrame()
{
}


void CMainFrame::InitWindow()
{
	LPSTORAGE lpStorage = NULL;
	LPOLEOBJECT	lpObject = NULL;
	LPLOCKBYTES lpLockBytes = NULL;
	LPOLECLIENTSITE lpClientSite = NULL;
	pRichEdit = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("chatPanel")));
	IRichEditOle *pRichEditOle = pRichEdit->GetRichEditOle();
	if (pRichEdit)
	{
		pRichEdit->AppendText(_T("我的世界, My World"));
	}
	_DAXCtl* pAxCtl = NULL;
	HRESULT hr = ::CoCreateInstance(CLSID_AXCtl, NULL, CLSCTX_INPROC, DIID__DAXCtl, (LPVOID*)&pAxCtl);
	if (NULL == pAxCtl || FAILED(hr))
	{
		return;
	}
	
	//Create lockbytes
	hr = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
	if (FAILED(hr))
	{
		return;
	}
	//use lockbytes to create storage
	SCODE sc = ::StgCreateDocfileOnILockBytes(lpLockBytes, STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &lpStorage);
	if (sc != S_OK)
	{
		return;
	}
	// retrieve OLE interface for richedit   and  Get site
	pRichEditOle->GetClientSite(&lpClientSite);
	pAxCtl->QueryInterface(IID_IOleObject, (void**)&lpObject);
	//Set it to be inserted
	OleSetContainedObject(lpObject, TRUE);
	//to insert into richedit, you need a struct of REOBJECT
	REOBJECT reobject;
	ZeroMemory(&reobject, sizeof(REOBJECT));
	reobject.cbStruct = sizeof(REOBJECT);
	CLSID clsid;
	hr = lpObject->GetUserClassID(&clsid);
	//set clsid
	reobject.clsid = clsid;
	//can be selected
	reobject.cp = REO_CP_SELECTION;
	//content, but not static
	reobject.dvaspect = DVASPECT_CONTENT;
	//goes in the same line of text line
	reobject.dwFlags = REO_BELOWBASELINE;
	//the very object
	reobject.poleobj = lpObject;
	//client site contain the object
	reobject.polesite = lpClientSite;
	//the storage 
	reobject.pstg = lpStorage;
	SIZEL sizel = { 0 };
	reobject.sizel = sizel;

	//这里附加自定义数据，做预加载处理
	//reobject.dwUser = (DWORD)pPreloadData;

	LPOLECLIENTSITE lpObjectClientSite = NULL;
	hr = lpObject->GetClientSite(&lpObjectClientSite);
	if (FAILED(hr) || lpObjectClientSite == NULL)
		lpObject->SetClientSite(lpClientSite);
	pRichEditOle->InsertObject(&reobject);
	//redraw the window to show animation
	HWND hwnd = (HWND)((long)m_PaintManager.GetPaintWindow());
	::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
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

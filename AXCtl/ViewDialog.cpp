// ViewDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AXCtl.h"
#include "ViewDialog.h"
#include "afxdialogex.h"


// CViewDialog dialog

IMPLEMENT_DYNAMIC(CViewDialog, CDialogEx)

CViewDialog::CViewDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MAIN_DIALOG, pParent)
{

}

CViewDialog::~CViewDialog()
{
}

void CViewDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CViewDialog, CDialogEx)
END_MESSAGE_MAP()



#pragma once


// CViewDialog dialog

class CViewDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CViewDialog)

public:
	CViewDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CViewDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};

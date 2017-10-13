#pragma once

// AXCtlPropPage.h : Declaration of the CAXCtlPropPage property page class.


// CAXCtlPropPage : See AXCtlPropPage.cpp for implementation.

class CAXCtlPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CAXCtlPropPage)
	DECLARE_OLECREATE_EX(CAXCtlPropPage)

// Constructor
public:
	CAXCtlPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_AXCTL };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};


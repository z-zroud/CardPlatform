#pragma once
#include "ViewDialog.h"
// AXCtlCtrl.h : Declaration of the CAXCtlCtrl ActiveX Control class.


// CAXCtlCtrl : See AXCtlCtrl.cpp for implementation.

class CAXCtlCtrl : public COleControl
{
	DECLARE_DYNCREATE(CAXCtlCtrl)

// Constructor
public:
	CAXCtlCtrl();
	CViewDialog m_VideoDlg;
// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// Implementation
protected:
	~CAXCtlCtrl();

	DECLARE_OLECREATE_EX(CAXCtlCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CAXCtlCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CAXCtlCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CAXCtlCtrl)		// Type name and misc status

// Message maps
	DECLARE_MESSAGE_MAP()
	afx_msg void Fuck2(void);
// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
		dispidFuck2 = 1L
	};

	
	afx_msg LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


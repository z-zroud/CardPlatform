// AXCtlCtrl.cpp : Implementation of the CAXCtlCtrl ActiveX Control class.

#include "stdafx.h"
#include "AXCtl.h"
#include "AXCtlCtrl.h"
#include "AXCtlPropPage.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CAXCtlCtrl, COleControl)

// Message map

BEGIN_MESSAGE_MAP(CAXCtlCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_MESSAGE(WM_CREATE, OnCreate)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// Dispatch map

BEGIN_DISPATCH_MAP(CAXCtlCtrl, COleControl)
	DISP_FUNCTION(CAXCtlCtrl, "Fuck2", Fuck2, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

// Event map

BEGIN_EVENT_MAP(CAXCtlCtrl, COleControl)	
END_EVENT_MAP()

// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CAXCtlCtrl, 1)
	PROPPAGEID(CAXCtlPropPage::guid)
END_PROPPAGEIDS(CAXCtlCtrl)

// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CAXCtlCtrl, "AXCTL.AXCtlCtrl.1",
	0x14038829, 0xea6f, 0x41d6, 0x86, 0x2c, 0x6d, 0x14, 0x6d, 0x7a, 0xc3, 0xa0)

// Type library ID and version

IMPLEMENT_OLETYPELIB(CAXCtlCtrl, _tlid, _wVerMajor, _wVerMinor)

// Interface IDs

const IID IID_DAXCtl = { 0x1A614CEA, 0x184, 0x4015, { 0x9F, 0x97, 0x13, 0x19, 0x2C, 0xD0, 0xE6, 0x56 } };
const IID IID_DAXCtlEvents = { 0x8ADCF049, 0xFD89, 0x4449, { 0x84, 0x51, 0x0, 0xD7, 0xF0, 0x35, 0x65, 0x43 } };

// Control type information

static const DWORD _dwAXCtlOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CAXCtlCtrl, IDS_AXCTL, _dwAXCtlOleMisc)

// CAXCtlCtrl::CAXCtlCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CAXCtlCtrl

BOOL CAXCtlCtrl::CAXCtlCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_AXCTL,
			IDB_AXCTL,
			afxRegApartmentThreading,
			_dwAXCtlOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


// CAXCtlCtrl::CAXCtlCtrl - Constructor

CAXCtlCtrl::CAXCtlCtrl()
{
	InitializeIIDs(&IID_DAXCtl, &IID_DAXCtlEvents);
	// TODO: Initialize your control's instance data here.
	//m_pVideoDlg = new CViewDialog;
}

// CAXCtlCtrl::~CAXCtlCtrl - Destructor

CAXCtlCtrl::~CAXCtlCtrl()
{
	// TODO: Cleanup your control's instance data here.
}

// CAXCtlCtrl::OnDraw - Drawing function

void CAXCtlCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& /* rcInvalid */)
{
	if (!pdc)
		return;

	// TODO: Replace the following code with your own drawing code.
	//pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	//pdc->Ellipse(rcBounds);
	//static bool inited = false;
	//if (m_pVideoDlg && !inited)
	//{
	//	inited = true;
	//	m_pVideoDlg->MoveWindow(rcBounds);
	//}
		
}

// CAXCtlCtrl::DoPropExchange - Persistence support

void CAXCtlCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}


// CAXCtlCtrl::OnResetState - Reset control to default state

void CAXCtlCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}

void CAXCtlCtrl::Fuck2(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

// CAXCtlCtrl message handlers
DWORD CAXCtlCtrl::GetControlFlags()
{
	DWORD flags = COleControl::GetControlFlags();

	return flags;
}

LRESULT CAXCtlCtrl::OnCreate(WPARAM wParam, LPARAM lParam)
{


	//// TODO:  Add your specialized creation code here
	m_VideoDlg.Create(IDD_MAIN_DIALOG, this);
	m_VideoDlg.ShowWindow(SW_SHOW);
	return 0;
}


void CAXCtlCtrl::OnSize(UINT nType, int cx, int cy)
{
	COleControl::OnSize(nType, cx, cy);

	//// TODO: Add your message handler code here
	RECT activeXRect;
	GetClientRect(&activeXRect);
	m_VideoDlg.MoveWindow(&activeXRect);
}


void CAXCtlCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	COleControl::OnMouseMove(nFlags, point);
}


void CAXCtlCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	COleControl::OnLButtonDblClk(nFlags, point);
}

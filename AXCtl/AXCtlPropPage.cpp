// AXCtlPropPage.cpp : Implementation of the CAXCtlPropPage property page class.

#include "stdafx.h"
#include "AXCtl.h"
#include "AXCtlPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CAXCtlPropPage, COlePropertyPage)

// Message map

BEGIN_MESSAGE_MAP(CAXCtlPropPage, COlePropertyPage)
END_MESSAGE_MAP()

// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CAXCtlPropPage, "AXCTL.AXCtlPropPage.1",
	0xb00329d0, 0x6069, 0x4980, 0x87, 0xf2, 0xa0, 0x3c, 0x1b, 0x37, 0x83, 0x32)

// CAXCtlPropPage::CAXCtlPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CAXCtlPropPage

BOOL CAXCtlPropPage::CAXCtlPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_AXCTL_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}

// CAXCtlPropPage::CAXCtlPropPage - Constructor

CAXCtlPropPage::CAXCtlPropPage() :
	COlePropertyPage(IDD, IDS_AXCTL_PPG_CAPTION)
{
}

// CAXCtlPropPage::DoDataExchange - Moves data between page and properties

void CAXCtlPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}

// CAXCtlPropPage message handlers

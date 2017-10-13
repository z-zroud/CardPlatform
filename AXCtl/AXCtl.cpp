// AXCtl.cpp : Implementation of CAXCtlApp and DLL registration.

#include "stdafx.h"
#include "AXCtl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CAXCtlApp theApp;

const GUID CDECL _tlid = { 0x7292663B, 0x606A, 0x487A, { 0x8F, 0xD8, 0xCE, 0xB2, 0x3D, 0x37, 0xEC, 0xAD } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CAXCtlApp::InitInstance - DLL initialization

BOOL CAXCtlApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}



// CAXCtlApp::ExitInstance - DLL termination

int CAXCtlApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}

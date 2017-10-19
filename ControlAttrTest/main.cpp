#include "StdAfx.h"
#include "MainFrameDlg.h"
#include <string>
using namespace std;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{


	CPaintManagerUI::SetInstance(hInstance);


	CMainFrame* pMainDlg = new CMainFrame();
	if (pMainDlg == NULL) return 0;


	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + pMainDlg->GetSkinFolder());


	pMainDlg->CreateDuiWindow(NULL, _T("DuiLit²âÊÔ"), UI_WNDSTYLE_FRAME, 0);

	pMainDlg->CenterWindow();
	::ShowWindow(*pMainDlg, SW_SHOW);

	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::Term();



	return 0;
}
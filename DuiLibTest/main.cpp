#include "StdAfx.h"
#include "MainFrameDlg.h"
#include <string>
using namespace std;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{

	string str = "ABCDEFG";
	string test;
	test = str.substr(4, 10);

    CPaintManagerUI::SetInstance(hInstance);


    CMainFrame* pMainDlg = new CMainFrame();
    if (pMainDlg == NULL) return 0;


	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + pMainDlg->GetSkinFolder());


    pMainDlg->Create(NULL, _T("DuiLit²âÊÔ"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);

    pMainDlg->CenterWindow();
    ::ShowWindow(*pMainDlg, SW_SHOW);

    CPaintManagerUI::MessageLoop();
    CPaintManagerUI::Term();



    return 0;
}
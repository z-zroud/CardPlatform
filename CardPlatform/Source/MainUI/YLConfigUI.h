#pragma once
#include <string>

using namespace std;

class CYLConfigUI : public CContainerUI
{
public:
    CYLConfigUI(CPaintManagerUI* pPM);
    ~CYLConfigUI();

    void DoInit();
    void InitDlg();	//³õÊ¼»¯¿Ø¼þ

protected:
    CPaintManagerUI* m_pPM;

    CEditUI*    m_pConvertFile;
};


#pragma once
#include <string>

using namespace std;

class CPersonalizationConfigUI : public CContainerUI
{
public:
    CPersonalizationConfigUI(CPaintManagerUI* pPM);
    ~CPersonalizationConfigUI();

    void DoInit();
    void InitDlg();	//³õÊ¼»¯¿Ø¼þ

protected:
    CPaintManagerUI* m_pPM;

    CEditUI*    m_pEncryptData;
};


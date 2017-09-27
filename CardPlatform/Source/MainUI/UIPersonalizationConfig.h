#pragma once
#include <string>

using namespace std;

class CPersonalizationConfigUI : public CContainerUI
{
public:
    CPersonalizationConfigUI(CPaintManagerUI* pPM);
    ~CPersonalizationConfigUI();

    void DoInit();
    void InitDlg();	//��ʼ���ؼ�

protected:
    CPaintManagerUI* m_pPM;

    CEditUI*    m_pEncryptData;
};


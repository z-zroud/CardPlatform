#pragma once
#include <string>

using namespace std;

class CYLConfigUI : public CContainerUI
{
public:
    CYLConfigUI(CPaintManagerUI* pPM);
    ~CYLConfigUI();

    void DoInit();
    void InitDlg();	//��ʼ���ؼ�

protected:
    CPaintManagerUI* m_pPM;

    CEditUI*    m_pEncryptData;
    CEditUI*    m_pDecryptKey;
    CEditUI*    m_pValueData;
    CEditUI*    m_pExchangeData;
};


#pragma once
#include <string>

using namespace std;

class CYLConverterUI : public CContainerUI, public INotifyUI
{
public:
    CYLConverterUI(CPaintManagerUI* pPM);
    ~CYLConverterUI();

    void DoInit();
    void InitDlg();	//��ʼ���ؼ�
    void Notify(TNotifyUI& msg); //������Ƕģ�����Ϣ

    void DoConvert();
protected:
    CPaintManagerUI* m_pPM;

    CEditUI*    m_pConvertFile;
};


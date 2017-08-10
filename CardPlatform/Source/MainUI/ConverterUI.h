#pragma once
#include <string>

using namespace std;

class CConverterUI : public CContainerUI, public INotifyUI
{
public:
    CConverterUI(CPaintManagerUI* pPM);
    ~CConverterUI();

    void DoInit();
    void InitDlg();	//��ʼ���ؼ�
    void Notify(TNotifyUI& msg); //������Ƕģ�����Ϣ
protected:
    CPaintManagerUI* m_pPM;

    CEditUI*    m_pConvertFile;
};


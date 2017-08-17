#pragma once
#include <string>

using namespace std;

class CECLoadUI : public CContainerUI, public INotifyUI
{
public:
    CECLoadUI(CPaintManagerUI* pPM);
    ~CECLoadUI();

    void DoInit();
    void InitDlg();	//��ʼ���ؼ�
    void Notify(TNotifyUI& msg); //������Ƕģ�����Ϣ
protected:
    CPaintManagerUI* m_pPM;

    CEditUI*    m_pConvertFile;
};


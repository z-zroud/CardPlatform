#pragma once
#include <string>

using namespace std;

class CECLoadUI : public CContainerUI, public INotifyUI
{
public:
    CECLoadUI(CPaintManagerUI* pPM);
    ~CECLoadUI();

    void DoInit();
    void InitDlg();	//初始化控件
    void Notify(TNotifyUI& msg); //处理内嵌模块的消息
protected:
    CPaintManagerUI* m_pPM;

    CEditUI*    m_pConvertFile;
};


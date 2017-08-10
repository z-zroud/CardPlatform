#pragma once
#include <string>

using namespace std;

class CConverterUI : public CContainerUI, public INotifyUI
{
public:
    CConverterUI(CPaintManagerUI* pPM);
    ~CConverterUI();

    void DoInit();
    void InitDlg();	//初始化控件
    void Notify(TNotifyUI& msg); //处理内嵌模块的消息
protected:
    CPaintManagerUI* m_pPM;

    CEditUI*    m_pConvertFile;
};


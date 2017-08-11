#pragma once
#include <string>

using namespace std;

class CYLConverterUI : public CContainerUI, public INotifyUI
{
public:
    CYLConverterUI(CPaintManagerUI* pPM);
    ~CYLConverterUI();

    void DoInit();
    void InitDlg();	//初始化控件
    void Notify(TNotifyUI& msg); //处理内嵌模块的消息

    void DoConvert();
protected:
    CPaintManagerUI* m_pPM;

    CEditUI*    m_pConvertFile;
};


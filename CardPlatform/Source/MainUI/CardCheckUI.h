#pragma once

class CCardCheckUI : public CContainerUI, public INotifyUI
{
public:
    CCardCheckUI(CPaintManagerUI* pPM);
    ~CCardCheckUI();

    void DoInit();
    void Notify(TNotifyUI& msg); //处理内嵌模块的消息

protected:
    CPaintManagerUI*	m_pPM;
};


#pragma once

class CCardCheckUI : public CContainerUI, public INotifyUI
{
public:
    CCardCheckUI(CPaintManagerUI* pPM);
    ~CCardCheckUI();

    void DoInit();
    void Notify(TNotifyUI& msg); //������Ƕģ�����Ϣ

protected:
    CPaintManagerUI*	m_pPM;
};


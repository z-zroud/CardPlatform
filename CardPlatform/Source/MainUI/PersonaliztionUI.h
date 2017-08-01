#pragma once

class CPersonalizationUI : public CContainerUI, public INotifyUI
{
public:
	CPersonalizationUI(CPaintManagerUI* pPM);
	~CPersonalizationUI();

	void DoInit();
	void Notify(TNotifyUI& msg); //������Ƕģ�����Ϣ

protected:
	CPaintManagerUI* m_pPM;

};



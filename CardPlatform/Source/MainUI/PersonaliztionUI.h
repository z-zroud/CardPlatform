#pragma once

class CPersonalizationUI : public CContainerUI, public INotifyUI
{
public:
	CPersonalizationUI(CPaintManagerUI* pPM);
	~CPersonalizationUI();

	void DoInit();
	void Notify(TNotifyUI& msg); //处理内嵌模块的消息

protected:
	CPaintManagerUI* m_pPM;

};



#pragma once
class CMenuContainerUI : public CContainerUI, public INotifyUI
{
public:
	CMenuContainerUI(CPaintManagerUI* pPM);
	~CMenuContainerUI();

	void Notify(TNotifyUI& msg); //������Ƕģ�����Ϣ

};

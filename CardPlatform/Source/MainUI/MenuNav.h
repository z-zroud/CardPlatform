#pragma once
class CMenuContainerUI : public CContainerUI, public INotifyUI
{
public:
	CMenuContainerUI(CPaintManagerUI* pPM);
	~CMenuContainerUI();

	void Notify(TNotifyUI& msg); //处理内嵌模块的消息

};

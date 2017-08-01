#pragma once
class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CDialogBuilderCallbackEx(CPaintManagerUI *pPM);
	CControlUI* CreateControl(LPCTSTR pstrClass);


protected:
	CPaintManagerUI *m_pPM;
};
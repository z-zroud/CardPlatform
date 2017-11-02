#pragma once
#include <string>

using namespace std;

class CEpScriptCheckUI : public CContainerUI, public INotifyUI
{
public:
	CEpScriptCheckUI(CPaintManagerUI* pPM);
	~CEpScriptCheckUI();

	void DoInit();
	void InitDlg();	//��ʼ���ؼ�
	void Notify(TNotifyUI& msg); //������Ƕģ�����Ϣ
protected:
	CPaintManagerUI* m_pPM;

	CEditUI*    m_pConvertFile;
};


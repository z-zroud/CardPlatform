#pragma once
#include <string>

using namespace std;

class CHTConverterUI : public CContainerUI, public INotifyUI
{
public:
	CHTConverterUI(CPaintManagerUI* pPM);
	~CHTConverterUI();

	void DoInit();
	void InitDlg();	//��ʼ���ؼ�
	void Notify(TNotifyUI& msg); //������Ƕģ�����Ϣ
protected:
	CPaintManagerUI* m_pPM;

	CEditUI*    m_pConvertFolder;
};


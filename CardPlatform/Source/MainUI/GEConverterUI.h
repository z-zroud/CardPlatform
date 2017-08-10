#pragma once
#include <string>

using namespace std;

class CGEConverterUI : public CContainerUI, public INotifyUI
{
public:
	CGEConverterUI(CPaintManagerUI* pPM);
	~CGEConverterUI();

	void DoInit();
	void InitDlg();	//��ʼ���ؼ�
	void Notify(TNotifyUI& msg); //������Ƕģ�����Ϣ
protected:
	CPaintManagerUI* m_pPM;

	CEditUI*    m_pConvertFile;
};


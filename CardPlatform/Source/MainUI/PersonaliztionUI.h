#pragma once
#include <vector>
using namespace std;

class CPersonalizationUI : public CContainerUI, public INotifyUI
{
public:
	CPersonalizationUI(CPaintManagerUI* pPM);
	~CPersonalizationUI();

	void DoInit();
    void InitDlg();
	void Notify(TNotifyUI& msg); //������Ƕģ�����Ϣ
    void CPersonalizationUI::GetFiles(string path, vector<string>& files);
protected:
	CPaintManagerUI* m_pPM;

private:
    CComboUI*   m_pAid;
    CComboUI*   m_pKmc;
    CComboUI*   m_pDiv;
    CComboUI*   m_pSecLevel;
    CComboUI*   m_pCfgFile;
    CEditUI*    m_pCpsFile;
};



#pragma once
#include <vector>
#include "Interface\IPCSC.h"
#include "Util\IniParaser.h"

using namespace std;

class CPersonalizationUI : public CContainerUI, public INotifyUI
{
public:
	CPersonalizationUI(CPaintManagerUI* pPM);
	~CPersonalizationUI();

	void DoInit();
    void InitDlg();
	void Notify(TNotifyUI& msg); //处理内嵌模块的消息
    void GetFiles(string path, vector<string>& files);
    void DoPersonaliztion();
    bool SetSelectedApplication(string aid);
    vector< pair<string, string>> ConcatNodeWithSameSection(INIParser ini);
    STORE_DATA_TYPE GetStoreDataType(string tag);
protected:
	CPaintManagerUI* m_pPM;
    IPCSC*      m_pPCSC;
private:
    CComboUI*   m_pAid;
    CComboUI*   m_pKmc;
    CComboUI*   m_pDiv;
    CComboUI*   m_pSecLevel;
    CComboUI*   m_pCfgFile;
    CEditUI*    m_pCpsFile;
};



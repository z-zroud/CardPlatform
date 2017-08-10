#pragma once
#include <string>
#include "Interface\CardBase.h"
#include "Util\IniParaser.h"
using namespace std;

class CInstallCfg
{
public:
    CInstallCfg(string cfgPath);
	string GetExeLoadFileAID(INSTALL_TYPE type);
	string GetExeModuleAID(INSTALL_TYPE type);
	string GetApplicationAID(INSTALL_TYPE type);
	string GetPrivilege(INSTALL_TYPE type);
	string GetInstallParam(INSTALL_TYPE type);
	string GetToken(INSTALL_TYPE type);
	void GetInstallCfg(INSTALL_TYPE type, INSTALL_PARAM &cfg);

protected:
	string GetRoot(INSTALL_TYPE type);
private:
	INIParser m_parser;
};

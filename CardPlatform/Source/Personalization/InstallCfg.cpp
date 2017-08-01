#include "stdafx.h"
#include "InstallCfg.h"


InstallCfg::InstallCfg(string cfgPath)
{
	m_parser.Read(cfgPath);
}

string InstallCfg::GetExeLoadFileAID(INSTALL_TYPE type)
{
	return m_parser.GetValue(GetRoot(type), "ExecuableLoadFileAID");
}

string InstallCfg::GetExeModuleAID(INSTALL_TYPE type)
{
	return m_parser.GetValue(GetRoot(type), "ExecuableModuleAID");
}

string InstallCfg::GetApplicationAID(INSTALL_TYPE type)
{
	return m_parser.GetValue(GetRoot(type), "ApplicationAID");
}

string InstallCfg::GetPrivilege(INSTALL_TYPE type)
{
	return m_parser.GetValue(GetRoot(type), "Privilege");
}

string InstallCfg::GetInstallParam(INSTALL_TYPE type)
{
	return m_parser.GetValue(GetRoot(type), "InstallParam");
}

string InstallCfg::GetToken(INSTALL_TYPE type)
{
	return m_parser.GetValue(GetRoot(type), "Token");
}

void InstallCfg::GetInstallCfg(INSTALL_TYPE type, INSTALL_CFG &cfg)
{
	cfg.strExeLoadFileAID	= GetExeLoadFileAID(type);
	cfg.strExeModuleAID		= GetExeModuleAID(type);
	cfg.strApplicationAID	= GetApplicationAID(type);
	cfg.strInstallParam		= GetInstallParam(type);
	cfg.strPrivilege		= GetPrivilege(type);
	cfg.strToken			= GetToken(type);
}

//根据安装应用类型返回匹配的字符串形式
string InstallCfg::GetRoot(INSTALL_TYPE type)
{
	switch (type)
	{
	case INSTALL_APP:	return "APP";
	case INSTALL_PSE:	return "PSE";
	case INSTALL_PPSE:	return "PPSE";
	}

	return "";
}
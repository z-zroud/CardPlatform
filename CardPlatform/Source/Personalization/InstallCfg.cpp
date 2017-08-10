#include "stdafx.h"
#include "InstallCfg.h"


CInstallCfg::CInstallCfg(string cfgPath)
{
	m_parser.Read(cfgPath);
}

string CInstallCfg::GetExeLoadFileAID(INSTALL_TYPE type)
{
	return m_parser.GetValue(GetRoot(type), "ExecuableLoadFileAID");
}

string CInstallCfg::GetExeModuleAID(INSTALL_TYPE type)
{
	return m_parser.GetValue(GetRoot(type), "ExecuableModuleAID");
}

string CInstallCfg::GetApplicationAID(INSTALL_TYPE type)
{
	return m_parser.GetValue(GetRoot(type), "ApplicationAID");
}

string CInstallCfg::GetPrivilege(INSTALL_TYPE type)
{
	return m_parser.GetValue(GetRoot(type), "Privilege");
}

string CInstallCfg::GetInstallParam(INSTALL_TYPE type)
{
	return m_parser.GetValue(GetRoot(type), "InstallParam");
}

string CInstallCfg::GetToken(INSTALL_TYPE type)
{
	return m_parser.GetValue(GetRoot(type), "Token");
}

void CInstallCfg::GetInstallCfg(INSTALL_TYPE type, INSTALL_PARAM &cfg)
{
	cfg.strExeLoadFileAID	= GetExeLoadFileAID(type);
	cfg.strExeModuleAID		= GetExeModuleAID(type);
	cfg.strApplicationAID	= GetApplicationAID(type);
	cfg.strInstallParam		= GetInstallParam(type);
	cfg.strPrivilege		= GetPrivilege(type);
	cfg.strToken			= GetToken(type);
}

//根据安装应用类型返回匹配的字符串形式
string CInstallCfg::GetRoot(INSTALL_TYPE type)
{
	switch (type)
	{
	case INSTALL_APP:	return "APP";
	case INSTALL_PSE:	return "PSE";
	case INSTALL_PPSE:	return "PPSE";
	}

	return "";
}
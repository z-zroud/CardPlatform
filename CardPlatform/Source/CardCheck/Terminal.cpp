#include "StdAfx.h"
#include "Terminal.h"
#include "Interface\Tag.h"
#include "Util\Base.h"

string CTerminal::m_5F2A = _T("");
string CTerminal::m_95 = _T("");
string CTerminal::m_9A = _T("");
string CTerminal::m_9C = _T("");
string CTerminal::m_9F02 = _T("");
string CTerminal::m_9F03 = _T("");
string CTerminal::m_9F09 = _T("");
string CTerminal::m_9F1A = _T("");
string CTerminal::m_9F1B = _T("");
string CTerminal::m_9F21 = _T("");
string CTerminal::m_9F37 = _T("");
string CTerminal::m_9F42 = _T("");
string CTerminal::m_9F4E = _T("");
string CTerminal::m_9F66 = _T("");
string CTerminal::m_9F7A = _T("");
string CTerminal::m_DF60 = _T("");
string CTerminal::m_DF69 = _T("");
bool CTerminal::bLoaded = false;
INIParser CTerminal::m_parse;

const string root = _T("TERMINAL");

void CTerminal::LoadData(const string &filePath)
{	
	if (INI_OK == m_parse.Read(filePath))
	{
		Set5F2A(m_parse.GetValue(root, _T("Tag5F2A")));
		Set95(m_parse.GetValue(root, _T("Tag95")));
		Set9A(m_parse.GetValue(root, _T("Tag9A")));
		Set9C(m_parse.GetValue(root, _T("Tag9C")));
		Set9F02(m_parse.GetValue(root, _T("Tag9F02")));
		Set9F03(m_parse.GetValue(root, _T("Tag9F03")));
		Set9F09(m_parse.GetValue(root, _T("Tag9F09")));
		Set9F1A(m_parse.GetValue(root, _T("Tag9F1A")));
		Set9F1B(m_parse.GetValue(root, _T("Tag9F1B")));
		Set9F21(m_parse.GetValue(root, _T("Tag9F21")));
		Set9F37(m_parse.GetValue(root, _T("Tag9F37")));
		Set9F42(m_parse.GetValue(root, _T("Tag9F42")));
		Set9F4E(m_parse.GetValue(root, _T("Tag9F4E")));
		Set9F66(m_parse.GetValue(root, _T("Tag9F66")));
		Set9F7A(m_parse.GetValue(root, _T("Tag9F7A")));
		SetDF60(m_parse.GetValue(root, _T("TagDF60")));
		SetDF69(m_parse.GetValue(root, _T("TagDF69")));
	}
}

void CTerminal::SetTerminalData(const string &tag, const string &value)
{
	if (tag == _T("Tag5F2A"))	Set5F2A(value);
	else if (tag == _T("Tag95"))	Set95(value);
	else if (tag == _T("Tag9A"))	Set9A(value);
	else if (tag == _T("Tag9C"))	Set9C(value);
	else if (tag == _T("Tag9F02"))	Set9F02(value);
	else if (tag == _T("Tag9F03"))	Set9F03(value);
	else if (tag == _T("Tag9F09"))	Set9F09(value);
	else if (tag == _T("Tag9F1A"))	Set9F1A(value);
	else if (tag == _T("Tag9F1B"))	Set9F1B(value);
	else if (tag == _T("Tag9F21"))	Set9F21(value);
	else if (tag == _T("Tag9F37"))	Set9F37(value);
	else if (tag == _T("Tag9F42"))	Set9F42(value);
	else if (tag == _T("Tag9F4E"))	Set9F4E(value);
	else if (tag == _T("Tag9F66"))	Set9F66(value);
	else if (tag == _T("Tag9F7A"))	Set9F7A(value);
	else if (tag == _T("TagDF60"))	SetDF60(value);
	else if (tag == _T("TagDF69"))	SetDF69(value);

	m_parse.SetValue(root, tag, value);
	m_parse.Save();
}

string CTerminal::GetTerminalData(const string &tag)
{
	if (!bLoaded)
	{
		TCHAR tszModule[MAX_PATH + 1] = { 0 };
		::GetModuleFileName(NULL, tszModule, MAX_PATH);
		CDuiString sInstancePath = tszModule;
		int pos = sInstancePath.ReverseFind(_T('\\'));
		if (pos >= 0) sInstancePath = sInstancePath.Left(pos + 1);

		string path = string(sInstancePath.GetData()) + _T("Configuration\\terminal.cfg");
		LoadData(path);

		bLoaded = true;
	}
	if (tag == _T("Tag5F2A"))	return Get5F2A();
	else if (tag == _T("Tag95"))	return Get95();
	else if (tag == _T("Tag9A"))	return Get9A();
	else if (tag == _T("Tag9C"))	return Get9C();
	else if (tag == _T("Tag9F02"))	return Get9F02();
	else if (tag == _T("Tag9F03"))	return Get9F03();
	else if (tag == _T("Tag9F09"))	return Get9F09();
	else if (tag == _T("Tag9F1A"))	return Get9F1A();
	else if (tag == _T("Tag9F1B"))	return Get9F1B();
	else if (tag == _T("Tag9F21"))	return Get9F21();
	else if (tag == _T("Tag9F37"))	return Get9F37();
	else if (tag == _T("Tag9F42"))	return Get9F42();
	else if (tag == _T("Tag9F4E"))	return Get9F4E();
	else if (tag == _T("Tag9F66"))	return Get9F66();
	else if (tag == _T("Tag9F7A"))	return Get9F7A();
	else if (tag == _T("TagDF60"))	return GetDF60();
	else if (tag == _T("TagDF69"))	return GetDF69();

	return _T("");
}

string CTerminal::Get5F2A()
{
	if (m_5F2A.empty())
		m_5F2A = _T("0156");
	return m_5F2A;
}
string CTerminal::Get95()
{
	return m_95;
}
string CTerminal::Get9A()
{
	if (m_9A.empty())
	{
        m_9A = Base::GenTransDate();
	}
	return m_9A;
}
string CTerminal::Get9C()
{
	if (m_9C.empty())
		m_9C = _T("00");
	return m_9C;
}
string CTerminal::Get9F02()
{
	if (m_9F02.empty())
		m_9F02 = _T("000000010000");
	return m_9F02;
}
string CTerminal::Get9F03()
{
	if (m_9F03.empty())
		m_9F03 = _T("000000000000");
	return m_9F03;
}
string CTerminal::Get9F09()
{
	return m_9F09;
}
string CTerminal::Get9F1A()
{
	return m_9F1A;
}
string CTerminal::Get9F1B()
{
	return m_9F1B;
}
string CTerminal::Get9F21()
{
	if (m_9F21.empty())
	{
        m_9F21 = Base::GenTransTime();
	}
	return m_9F21;
}
string CTerminal::Get9F37()
{
	return m_9F37;
}
string CTerminal::Get9F42()
{
	return m_9F42;
}
string CTerminal::Get9F4E()
{
	return m_9F4E;
}
string CTerminal::Get9F66()
{
	return m_9F66;
}
string CTerminal::Get9F7A()
{
	return m_9F7A;
}
string CTerminal::GetDF60()
{
	return m_DF60;
}
string CTerminal::GetDF69()
{
	return m_DF69;
}


void CTerminal::Set5F2A(string value)
{
	m_5F2A = value;
}
void CTerminal::Set95(string value)
{
	m_95 = value;
}
void CTerminal::Set9A(string value)
{
	m_9A = value;
}
void CTerminal::Set9C(string value)
{
	m_9C = value;
}
void CTerminal::Set9F02(string value)
{
	m_9F02 = value;
}
void CTerminal::Set9F03(string value)
{
	m_9F03 = value;
}
void CTerminal::Set9F09(string value)
{
	m_9F09 = value;
}
void CTerminal::Set9F1A(string value)
{
	m_9F1A = value;
}
void CTerminal::Set9F1B(string value)
{
	m_9F1B = value;
}
void CTerminal::Set9F21(string value)
{
	m_9F21 = value;
}
void CTerminal::Set9F37(string value)
{
	m_9F37 = value;
}
void CTerminal::Set9F42(string value)
{
	m_9F42 = value;
}
void CTerminal::Set9F4E(string value)
{
	m_9F4E = value;
}
void CTerminal::Set9F66(string value)
{
	m_9F66 = value;
}
void CTerminal::Set9F7A(string value)
{
	m_9F7A = value;
}
void CTerminal::SetDF60(string value)
{
	m_DF60 = value;
}
void CTerminal::SetDF69(string value)
{
	m_DF69 = value;
}
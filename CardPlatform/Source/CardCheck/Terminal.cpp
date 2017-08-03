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

string CTerminal::GetTerminalData(const string &tag)
{
	if (tag == Tag5F2A)	return Get5F2A();
	else if (tag == Tag95)	return Get95();
	else if (tag == Tag9A)	return Get9A();
	else if (tag == Tag9C)	return Get9C();
	else if (tag == Tag9F02)	return Get9F02();
	else if (tag == Tag9F03)	return Get9F03();
	else if (tag == Tag9F09)	return Get9F09();
	else if (tag == Tag9F1A)	return Get9F1A();
	else if (tag == Tag9F1B)	return Get9F1B();
	else if (tag == Tag9F21)	return Get9F21();
	else if (tag == Tag9F37)	return Get9F37();
	else if (tag == Tag9F42)	return Get9F42();
	else if (tag == Tag9F4E)	return Get9F4E();
	else if (tag == Tag9F66)	return Get9F66();
	else if (tag == Tag9F7A)	return Get9F7A();
	else if (tag == TagDF60)	return GetDF60();
	else if (tag == TagDF69)	return GetDF69();

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
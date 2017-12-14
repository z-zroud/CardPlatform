#include "stdafx.h"
#include "Tool.h"

//分割字符串
void Tool::SplitString(const string& s, std::vector<std::string>& v, const string& c)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

//删除字符串中的空格
string Tool::DeleteSpace(string s)
{
	string strResult;
	int len = s.length();
	for (auto c : s)
	{
		if (c != ' ')
		{
			strResult.push_back(c);
		}
	}
	return strResult;
	int index = 0;
	if (!s.empty())
	{
		while ((index = s.find(' ', index)) != string::npos)
		{
			s.erase(index, 1);
		}
	}
	return strResult;
}


void Tool::AscToBcd(byte *bcd, byte *asc, long asc_len)
{
	long j;
	unsigned char flag;
	unsigned char is_high, by;

	is_high = !(asc_len % 2);
	flag = (asc_len % 2);
	bcd[0] = 0x00;

	for (j = 0; j<asc_len; j++)
	{
		by = asc[j];
		if (by == ' ') by = 0;
		if (!(by & 0x10) && by) by += 9;

		if (is_high)  bcd[(j + flag) / 2] = by << 4;
		else
		{
			by &= 0x0f;
			bcd[(j + flag) / 2] |= by;
		}
		is_high = !is_high;
	}
	return;/*BCDLen(j);*/
}

void Tool::BcdToAsc(char *asc, char *bcd, long asc_len)
{
	long j;
	unsigned char   is_first;
	unsigned char  by = 0;

	is_first = 1;
	for (j = 0; j<asc_len; j++)
	{
		if (is_first)  by = (bcd[j / 2] & 0xf0) >> 4;
		else
		{
			by = bcd[j / 2] & 0x0F;
		}
		by += (by >= 0x0a) ? 0x37 : '0';       /* 0x37 = 'A' - 0x0a*/
		asc[j] = by;
		is_first = !is_first;
	}
	by = by;
	return;/*return BCDLen(j)*/
}

//获取文件大小
long Tool::GetFileSize(FILE* file)
{
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	return fileSize;
}

//获取字符串长度
string Tool::GetStringLen(string data)
{
	int nLen = data.length() / 2;
	char szLen[5] = { 0 };
	sprintf_s(szLen, 5, "%02X", nLen);

	return string(szLen);
}

//增加长度
string Tool::IncStringLenStep(string currentLen, int step)
{
	int nCurrentLen = stoi(currentLen, 0, 16);
	nCurrentLen += step;

	char szLen[5] = { 0 };
	sprintf_s(szLen, 5, "%02X", nCurrentLen);

	return string(szLen);
}

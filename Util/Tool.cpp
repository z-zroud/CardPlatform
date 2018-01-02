#include "stdafx.h"
#include "Tool.h"
#include <cstring>
#include <string>
#include <locale>
#include <cmath>

using namespace std;

namespace  Tool 
{
	bool SubStr(const char* src, int start, int len, char* dst)
	{
		if (strlen(src) < len ||
			start < 0 || 
			start > strlen(src))
		{
			return false;
		}
		strncpy_s(dst, len + 1, src + start, len);

		return true;
	}

	void BcdToAsc(unsigned char *asc, unsigned char *bcd, long bcd_len)
	//void BcdToAsc(byte *bcd, byte *asc, long asc_len)
	{
		long j;
		unsigned char flag;
		unsigned char is_high, by;

		is_high = !(bcd_len % 2);
		flag = (bcd_len % 2);
		asc[0] = 0x00;

		for (j = 0; j<bcd_len; j++)
		{
			by = bcd[j];
			if (by == ' ') by = 0;
			if (!(by & 0x10) && by) by += 9;

			if (is_high)  asc[(j + flag) / 2] = by << 4;
			else
			{
				by &= 0x0f;
				asc[(j + flag) / 2] |= by;
			}
			is_high = !is_high;
		}
		return;/*BCDLen(j);*/
	}

	long AscToBin(char *ptr, int len)
	{
		long result;
		result = 0;
		while (len-- >0) {
			result = result * 10 + (*ptr - '0');
			ptr++;
		}
		return result;
	}

	void AscToBcd(char *bcd, char *asc, long asc_len)
	{
		long j;
		unsigned char   is_first;
		unsigned char  by = 0;

		is_first = 1;
		for (j = 0; j<asc_len; j++)
		{
			if (is_first)  by = (asc[j / 2] & 0xf0) >> 4;
			else
			{
				by = asc[j / 2] & 0x0F;
			}
			by += (by >= 0x0a) ? 0x37 : '0';       /* 0x37 = 'A' - 0x0a*/
			bcd[j] = by;
			is_first = !is_first;
		}
		by = by;
		return;/*return BCDLen(j)*/
	}

	static void Char2Hex(unsigned char ch, char *szHex)
	{
		int i;
		unsigned char byte[2];
		byte[0] = ch / 16;
		byte[1] = ch % 16;
		for (i = 0; i<2; i++)
		{
			if (byte[i] >= 0 && byte[i] <= 9)
				szHex[i] = '0' + byte[i];
			else
				szHex[i] = 'a' + byte[i] - 10;
		}
		szHex[2] = 0;
	}

	void StrToHex(unsigned char *pDest, unsigned char *pSrc, int nLen)
	{
		char h1, h2;
		unsigned char s1, s2;
		int i;
		for (i = 0; i < nLen; i++)
		{
			h1 = pSrc[2 * i];
			h2 = pSrc[2 * i + 1];

			s1 = toupper(h1) - 0x30;
			if (s1 > 9)
				s1 -= 7;
			s2 = toupper(h2) - 0x30;
			if (s2 > 9)
				s2 -= 7;
			pDest[i] = s1 * 16 + s2;
		}
	}

    void DeleteSpace(const char* src, char* dest, int len)
    {
        int srcLen = strlen(src);
        int j = 0;
        for (int i = 0; i < srcLen; i++)
        {
            if (src[i] != ' ' && j < len)
            {
                dest[j] = src[i];
                j++;
            }
        }
    }

    //删除字符串中的空格
    string Trim(string str)
    {
        string result;
        int len = str.length();
        for (auto c : str)
        {
            if (c != ' ')
            {
                result.push_back(c);
            }
        }
        return result;
        int index = 0;
        if (!str.empty())
        {
            while ((index = str.find(' ', index)) != string::npos)
            {
                str.erase(index, 1);
            }
        }
        return result;
    }

    /************************************************************
    * 只考虑长度在0-0xFFFF之间
    *************************************************************/
	void GetBcdDataLen(const char* data, char* output, int len)
	{
		memset(output, 0, len);
        if (strlen(data) > 0xFF * 2) {
            sprintf_s(output, len, "%04X", strlen(data) / 2);
        }
        else {
            sprintf_s(output, len, "%02X", strlen(data) / 2);
        }		
	}

	void IntToStr(int value, char* hexStr, Hex_Len hexLen)
	{
		if (hexLen == Two_byte) {
			sprintf_s(hexStr, 3, "%02X", value);
		}
		else {
			sprintf_s(hexStr, 5, "%04X", value);
		}
	}

	void IncreaseStep(const char* strLen, int step, char* output, int len)
	{
		memset(output, 0, len);
		int incLen = stoi(strLen, 0, 16) + step;
		sprintf_s(output, len, "%02X", incLen);
	}

    //将字符串型十六进制转为整形
    int ctoi(unsigned char c)
    {
        switch (toupper(c))
        {
        case '0':		return 0;
        case '1':		return 1;
        case '2':		return 2;
        case '3':		return 3;
        case '4':		return 4;
        case '5':		return 5;
        case '6':		return 6;
        case '7':		return 7;
        case '8':		return 8;
        case '9':		return 9;
        case 'A':		return 10;
        case 'B':		return 11;
        case 'C':		return 12;
        case 'D':		return 13;
        case 'E':		return 14;
        case 'F':		return 15;
        default:		return -1;
        }

        return -1;
    }
}


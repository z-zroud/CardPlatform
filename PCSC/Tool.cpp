#include "stdafx.h"
#include "Tool.h"
#include <cstring>
#include <string>
#include <locale>
#include <cmath>

using namespace std;

namespace  Tool 
{
	void BcdToAsc(byte *bcd, byte *asc, long asc_len)
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

	void AscToBcd(char *asc, char *bcd, long bcd_len)
	{
		long j;
		unsigned char   is_first;
		unsigned char  by = 0;

		is_first = 1;
		for (j = 0; j<bcd_len; j++)
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

	void StrToHex(byte *pDest, byte *pSrc, int nLen)
	{
		char h1, h2;
		byte s1, s2;
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
}


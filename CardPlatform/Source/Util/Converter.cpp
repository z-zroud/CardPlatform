#include "stdafx.h"
#include "Converter.h"
#include <cstring>
#include <string>
#include <locale>
#include <cmath>

using namespace std;

namespace  Tool 
{
	namespace Converter
	{
		void AscToBcd(byte *bcd, byte *asc, long asc_len)
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

		void BcdToAsc(char *asc, char *bcd, long asc_len)
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

		string StrToHex(const char* strBin, int len, bool bIsUpper)
		{
			char *pCopy = new char[len + 1];
			memset(pCopy, 0, len + 1);
			memcpy(pCopy, strBin, len);
			string strHex;
			//strHex.resize(strBin.size() * 2);
			strHex.resize(len * 2);
			for (int i = 0; i < len; i++)
			{
				uint8_t cTemp = pCopy[i];
				for (size_t j = 0; j < 2; j++)
				{
					uint8_t cCur = (cTemp & 0x0f);
					if (cCur < 10)
					{
						cCur += '0';
					}
					else
					{
						cCur += ((bIsUpper ? 'A' : 'a') - 10);
					}
					strHex[2 * i + 1 - j] = cCur;
					cTemp >>= 4;
				}
			}

			return strHex;
		}

		void HexToStr(byte *pDest, byte *pSrc, int nLen)
		{

		}

		string StrUpper(const char* str)
		{
			int len = strlen(str);
			char *result = new char[len + 1];
			memset(result, 0, len + 1);
			for (int i = 0; i < len; i++)
			{
				char c = toupper(str[i]);
				char temp[2] = { 0 };
				sprintf_s(temp, "%c", c);
				strcat_s(result, len + 1, temp);
			}
			string strResult(result);

			delete result;
			return strResult;
		}

		int HexToInt(char c)
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

		int HexStrToInt(const char* strHex, int len)
		{
			int nResult = 0;
			for (int i = 0; i < len; i++)
			{
				nResult += HexToInt(strHex[i]) * (int)pow(16, len - i - 1);
			}

			return nResult;
		}
	}
}


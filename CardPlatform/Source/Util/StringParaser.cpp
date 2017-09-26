#include "stdafx.h"
#include "StringParaser.h"

using namespace std;

namespace Tool
{
	namespace Stringparser
	{
		string GetStringLen(const string& str)
		{
			char len[5] = { 0 };
			sprintf_s(len, 5, "%02X", str.length() / 2);

			return len;
		}

		string IncStringLenStep(const string& len, int step)
		{
			int incLen = stoi(len, 0, 16) + step;
			char sLen[3] = { 0 };
			sprintf_s(sLen, 3, "%02X", incLen);

			return sLen;
		}

		//�ָ��ַ���
		void SplitString(const string& s, std::vector<std::string>& v, const string& c)
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

        void SplitString(const string &s, std::vector<string> &vec, const string& start, const string& end)
        {
            string::size_type startPos = 0;
            string::size_type endPos = 0;
            
            while (string::npos != startPos)
            {
                startPos = s.find_first_of(start, startPos);
                if (startPos == string::npos)
                {
                    break;
                }
                endPos = s.find_first_of(end, startPos);
                if (endPos == string::npos)
                {
                    break;
                }
                vec.push_back(s.substr(startPos + start.length(), endPos - startPos - start.length()));
                startPos = endPos;
            }
        }

		//ɾ���ַ����еĿո�
		string DeleteSpace(string s)
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

		//���ַ�����ʮ��������ʽ���
		string StrToHexStr(const char* str, int len, bool bIsUpper)
		{
			string strHex;
			strHex.resize(len * 2 + 1);
			for (int i = 0; i < len; i++)
			{
				unsigned char cTemp = str[i];
				for (size_t j = 0; j < 2; j++)
				{
					unsigned char cCur = (cTemp & 0x0f);	//ȡ�ֽڵĵ�4λ
					if (cCur >= 10)
					{
						cCur += ((bIsUpper ? 'A' : 'a') - 10);
					}
					else {
						cCur += '0';	//��������ֽڴ�С + '0' = '0' + ƫ�� => [0-9]
					}
					strHex[2 * i + 1 - j] = cCur;
					cTemp >>= 4;	//ȡ�ֽڵĸ�4λ
				}
			}

			return strHex;
		}

		//��ʮ�������ַ��� ת��Ϊascii�ַ���
		string HexStrToStr(const char* str)
		{
			string strResult;
			if (strlen(str) % 2 != 0)
				return "";
			for (size_t i = 0; i < strlen(str); i += 2)
			{
				char temp[3] = { 0 };
				memcpy(temp, str + i, 2);
				char c = static_cast<char>(stoi(temp, 0, 16));

				strResult.push_back(c);
			}

			return strResult;
		}

		//��������ת��Ϊʮ�����Ƶ��ַ�����ʽ
		string IntToHexStr(const int n)
		{
			char szResult[64] = { 0 };
			_itoa_s(n, szResult, 16);

			return string(szResult);
		}

		//��ʮ�����Ƶ��ַ��� ת��Ϊ����
		int HexStrToInt(const string hexStr)
		{
			return stoi(hexStr, 0, 16);
		}

		//���ַ�����д
		string ToUpper(string& str)
		{
			for (auto &s : str)
			{
				s = toupper(s);
			}

			return str;
		}
	}
}
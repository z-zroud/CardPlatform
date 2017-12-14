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

		//·Ö¸î×Ö·û´®
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

		//É¾³ý×Ö·û´®ÖÐµÄ¿Õ¸ñ
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
	}
}
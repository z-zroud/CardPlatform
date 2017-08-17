#include "StdAfx.h"
#include "Base.h"

namespace Base
{
    string GenTransDate()
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        char szTransDate[32] = { 0 };	//交易日期
        sprintf_s(szTransDate, 32, "%02d%02d%02d", st.wYear, st.wMonth, st.wDay);

        return string(szTransDate).substr(2);
    }

    string GenTransTime()
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        char szTransDate[32] = { 0 };	//交易日期
        sprintf_s(szTransDate, 32, "%02d%02d%02d", st.wHour, st.wMinute, st.wSecond);

        return string(szTransDate);
    }

    string GetDataHexLen(string data)
    {
        int nLen = data.length() / 2;
        char szLen[5] = { 0 };
        sprintf_s(szLen, 5, _T("%02X"), nLen);

        return string(szLen);
    }

    string Increase(string currentLen, int step)
    {
        int nCurrentLen = stoi(currentLen, 0, 16);
        nCurrentLen += step;

        char szLen[5] = { 0 };
        sprintf_s(szLen, 5, _T("%02X"), nCurrentLen);

        return string(szLen);
    }

    //日期比较
    bool CompareDate(string grateDate, string lessDate)
    {
        if (grateDate.length() != 6 || lessDate.length() != 6)
        {
            return false;
        }
        int nFirst = stoi(grateDate, 0);
        int nSecond = stoi(lessDate, 0);

        return nFirst >= nSecond;
    }
}
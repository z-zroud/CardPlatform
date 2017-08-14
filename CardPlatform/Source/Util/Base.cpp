#include "StdAfx.h"
#include "Base.h"

namespace Base
{
    string GenTransDate()
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        char szTransDate[32] = { 0 };	//��������
        sprintf_s(szTransDate, 32, "%02d%02d%02d", st.wYear, st.wMonth, st.wDay);

        return string(szTransDate).substr(2);
    }

    string GenTransTime()
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        char szTransDate[32] = { 0 };	//��������
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

    string Increase(string current, int step)
    {
        int nCurrent = stoi(current, 0, 16);
        nCurrent += step;

        char szLen[5] = { 0 };
        sprintf_s(szLen, 5, _T("%02X"), nCurrent);

        return string(szLen);
    }

    //���ڱȽ�
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
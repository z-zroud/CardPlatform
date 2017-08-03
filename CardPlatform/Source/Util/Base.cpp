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
}
using System;
using CardPlatform.Config;

namespace CardPlatform.Cases
{
    public interface IExcuteCase
    {
        void ExcuteCase(Object srcData);
        void TraceInfo(CaseLevel level, string caseNo, string format, params object[] args);
    }
}

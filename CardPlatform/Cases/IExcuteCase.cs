using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Business;
namespace CardPlatform.Cases
{

    public enum CaseLevel
    {
        CaseSucess = 0,
        CaseWarn = 1,
        CaseFailed = 2
    }

    public interface IExcuteCase
    {
        void ExcuteCase(Object srcData, CardRespDataType type);
        void ShowInfo(string caseNo, string description, CaseLevel level);
        void ShowComparedResult(string tag);
    }
}

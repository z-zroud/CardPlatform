using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Business;
namespace CardPlatform.Cases
{
    public interface IExcuteCase
    {
        void ExcuteCase(Object srcData, CardRespDataType type);
    }
}

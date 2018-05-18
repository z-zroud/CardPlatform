using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Business;

namespace CardPlatform.Cases
{
    public class CaseFactory
    {
        public static IExcuteCase GetCaseInstance(string app, string step)
        {
            switch(step)
            {
                case Constant.STEP_SELECT_PSE:   return new PSECases() { CurrentApp = app, Step = step };
                case Constant.STEP_SELECT_APP:   return new SelectAppCase() { CurrentApp = app, Step = step };
                case Constant.STEP_READ_RECORD:  return new ReadRecordCase() { CurrentApp = app, Step = step };
            }
            return new CaseBase() { CurrentApp = app, Step = step };
        }
    }
}

using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using CardPlatform.Business;
using CardPlatform.Config;

namespace CardPlatform.Cases
{
    public class PSECases : CaseBase
    {
        public PSECases()
        {
        }

        protected override void Load()
        {
            Step = "SelectPSE";
            base.Load();           
        }

        public void PBOC_sPSE_SJHGX_001_01()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (response.Response.Length < 2 || response.Response.Substring(0,2) != "6F")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(CaseLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        public void PBOC_sPSE_SJHGX_003_01()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

        }
    }
}

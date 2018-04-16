using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using CardPlatform.Business;

namespace CardPlatform.Cases
{
    public class PSECases : CaseBase
    {
        private List<TLV> arrTLV;
        private ApduResponse response;

        public PSECases()
        {
            response = new ApduResponse();
            arrTLV = new List<TLV>();
        }

        public override void ExcuteCase(Object srcData, CardRespDataType type)
        {
            response = (ApduResponse)srcData;
            arrTLV = DataParse.ParseTLV(response.Response);

            switch(type)
            {
                case CardRespDataType.SelectPSE:
                    PBOC_sPSE_SJHGX_001_01();
                    break;
                case CardRespDataType.ReadPSERecord:
                    break;
            }
        }

        protected void PBOC_sPSE_SJHGX_001_01()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseDesc = GetDescription(caseNo);
            if (response.Response.Length < 2 || response.Response.Substring(0,2) != "6F")
            {
                ShowInfo(caseNo, caseDesc, CaseLevel.CaseFailed);
            }
            else
            {
                ShowInfo(caseNo, caseDesc, CaseLevel.CaseSucess);
            }
        }

        protected void PBOC_sPSE_SJHGX_003_01()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseDesc = GetDescription(caseNo);
            
        }
    }
}

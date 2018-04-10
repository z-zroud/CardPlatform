﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Business;
using CplusplusDll;

namespace CardPlatform.Cases
{
    public class GPOCase : CaseBase
    {
        private List<TLV> arrTLV;
        private ApduResponse response;

        public GPOCase()
        {
            arrTLV = new List<TLV>();
            response = new ApduResponse();
        }

        public override void ExcuteCase(object srcData, CardRespDataType type)
        {
            response = (ApduResponse)srcData;
            arrTLV = DataParse.ParseTLV(response.Response);

            PBOC_GPO_SJHGX_001();
        }

        protected void PBOC_GPO_SJHGX_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseDesc = GetDescription(caseNo);
            if (response.Response.Length < 2 || response.Response.Substring(0, 2) != "80")
            {
                ShowInfo(caseNo, caseDesc, CaseLevel.CaseFailed);
            }
            else
            {
                ShowInfo(caseNo, caseDesc, CaseLevel.CaseSucess);
            }
        }
    }
}

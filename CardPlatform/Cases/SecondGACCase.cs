using System;
using System.Collections.Generic;
using CardPlatform.Common;
using CplusplusDll;

namespace CardPlatform.Cases
{
    public class SecondGACCase : CaseBase
    {
        private ApduResponse response;
        private List<TLV> TLVs;

        public SecondGACCase()
        {
            response = new ApduResponse();
            TLVs = new List<TLV>();
        }

        protected override void Load()
        {
            base.Load();
        }

        public override void ExcuteCase(TransactionStep step, object srcData)
        {
            response = (ApduResponse)srcData;
            TLVs = DataParse.ParseTLV(response.Response);
            CheckTemplateTag(TLVs);
            base.ExcuteCase(step, srcData);
        }
    }
}

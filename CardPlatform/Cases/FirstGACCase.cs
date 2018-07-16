using System;
using System.Collections.Generic;
using CardPlatform.Common;
using CplusplusDll;


namespace CardPlatform.Cases
{
    public class FirstGACCase : CaseBase
    {
        private ApduResponse response;
        private List<TLV> TLVs;

        public FirstGACCase()
        {
            response = new ApduResponse();
            TLVs = new List<TLV>();
        }

        protected override void Load()
        {
            base.Load();
        }

        public override void Excute(int batchNo, TransactionApp app, TransactionStep step, object srcData)
        {
            response = (ApduResponse)srcData;
            //TLVs = DataParse.ParseTLV(response.Response);
            Excute(batchNo,app,step, srcData);
            CheckTemplateTag(TLVs);           
        }
    }
}

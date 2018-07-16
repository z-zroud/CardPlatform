using CardPlatform.Cases;
using CplusplusDll;
using System;
using System.Collections.Generic;
using CardPlatform.Common;

namespace CardPlatform.Business
{
    public class BusinessPPSE : BusinessBase
    {
        public List<string> SelectPPSE()
        {
            ApduResponse response = new ApduResponse();
            response = APDU.SelectCmd(Constant.PPSE);
            if (response.SW != 0x9000)
            {
                return new List<string>();
            }
            List<TLV> arrTLV = DataParse.ParseTLV(response.Response);
            //TagDict tagDict = TagDict.GetInstance();
            //tagDict.SetTags(arrTLV);

            IExcuteCase ppseCase = new PPSECases();
            ppseCase.Excute(BatchNo, CurrentApp,TransactionStep.SelectPPSE,arrTLV);

            List<string> Aids = new List<string>();
            foreach(var tlv in arrTLV)
            {
                if(tlv.Tag == "4F")
                {
                    Aids.Add(tlv.Value);
                }
            }
            return Aids;
        }
    }
}

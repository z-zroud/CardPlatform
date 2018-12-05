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
            var tlvs = DataParse.ParseTLV(response.Response);
            businessUtil.ShowTlvLog(tlvs);
            businessUtil.SaveTags(TransactionStep.SelectPPSE, tlvs);

            IExcuteCase ppseCase = new PPSECases();
            ppseCase.Excute(BatchNo, TransactionConfig.GetInstance().CurrentApp,TransactionStep.SelectPPSE, response);

            List<string> Aids = new List<string>();
            foreach(var tlv in tlvs)
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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Business;
using CplusplusDll;

namespace CardPlatform.Cases
{
    public class ReadRecordCase : CaseBase
    {
        private List<TLV> arrTLV;
        private List<ApduResponse> responses;

        public ReadRecordCase()
        {
            arrTLV = new List<TLV>();
            responses = new List<ApduResponse>();
        }

        public override void ExcuteCase(object srcData, CardRespDataType type)
        {
            responses = (List<ApduResponse>)srcData;
            foreach(var resp in responses)
            {
                var tlvs = DataParse.ParseTLV(resp.Response);
                arrTLV.AddRange(tlvs);
            }
            
        }
    }
}

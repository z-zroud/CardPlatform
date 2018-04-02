using CardPlatform.Cases;
using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Business
{
    public class BusinessPPSE
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
            TagDict tagDict = TagDict.GetInstance();
            tagDict.SetTags(arrTLV);

            IExcuteCase cases = new PPSECases();
            cases.ExcuteCase(arrTLV, CardRespDataType.SelectPPSE);

            List<string> Aids = new List<string>();

            return Aids;
        }
    }
}

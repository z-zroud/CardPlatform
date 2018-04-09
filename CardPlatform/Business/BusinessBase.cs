using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UtilLib;

namespace CardPlatform.Business
{
    public class BusinessBase
    {
        public List<TLV> ParseAndSave(string response)
        {
            List<TLV> arrTLV = DataParse.ParseTLV(response);
            TagDict tagDict = TagDict.GetInstance();
            tagDict.SetTags(arrTLV);

            return arrTLV;
        }

        public ApduResponse SelectAid(string aid)
        {
            return APDU.SelectCmd(aid);
        }

        public ApduResponse GPO(string pdol)
        {
            return APDU.GPOCmd(pdol);
        }

        public List<ApduResponse> ReadRecords(List<AFL> afls)
        {
            var responses = new List<ApduResponse>();
            foreach(var afl in afls)
            {
                var resp = APDU.ReadRecordCmd(afl.SFI, afl.RecordNo);
                responses.Add(resp);
            }
            return responses;
        }


    }
}

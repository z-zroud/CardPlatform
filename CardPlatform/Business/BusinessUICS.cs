using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CplusplusDll;
using CardPlatform.Cases;

namespace CardPlatform.Business
{
    public class BusinessUICS : BusinessBase
    {
        public override void DoTrans(string aid, bool doDesTrans, bool doSMTrans)
        {
            SelectAid(aid);
        }

        protected override ApduResponse SelectAid(string aid)
        {
            ApduResponse response = base.SelectAid(aid);
            ParseAndSave(response.Response);
            IExcuteCase excuteCase = new SelectAidCase();
            excuteCase.ExcuteCase(response, CardRespDataType.SelectAid);

            return response;
        }

        protected override ApduResponse GPO(string pdol, List<AFL> AFLs)
        {
            ApduResponse response = base.GPO(pdol, AFLs);

            IExcuteCase excuteCase = new GPOCase();
            excuteCase.ExcuteCase(response, CardRespDataType.GPO);

            return response;
        }

        protected override List<ApduResponse> ReadRecords(List<AFL> afls)
        {
            var resps = base.ReadRecords(afls);

            foreach(var resp in resps)
            {
                ParseAndSave(resp.Response);
            }

            IExcuteCase excuteCase = new ReadRecordCase();
            excuteCase.ExcuteCase(resps, CardRespDataType.ReadPSERecord);

            return resps;
        }
    }
}

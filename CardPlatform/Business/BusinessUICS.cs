using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CplusplusDll;
using CardPlatform.Cases;
using CardPlatform.ViewModel;

namespace CardPlatform.Business
{
    public class BusinessUICS : BusinessBase
    {
        private TagDict tagDict = TagDict.GetInstance();

        public override void DoTrans(string aid, bool doDesTrans, bool doSMTrans)
        {
            ViewModelLocator locator = new ViewModelLocator();
            locator.Terminal.TermianlSettings.Tag9F7A = "00";
            
            if(doDesTrans)  // do des uics transaction
            {
                locator.Terminal.TermianlSettings.TagDF69 = "00";
                SelectAid(aid);
                string tag9F38 = tagDict.GetTag("9F38");
                var tls = DataParse.ParseTL(tag9F38);
                string pdol = string.Empty;
                foreach(var tl in tls)
                {
                    pdol += locator.Terminal.TermianlSettings.GetTag(tl.Tag);
                }
                var AFLs = GPOEx(pdol);
                ReadRecords(AFLs);
            }
            if(doSMTrans)   // do sm uics transaction
            {
                locator.Terminal.TermianlSettings.TagDF69 = "01";
                SelectAid(aid);
            }

        }



        protected override ApduResponse SelectAid(string aid)
        {
            ApduResponse response = base.SelectAid(aid);
            ParseAndSave(response.Response);
            IExcuteCase excuteCase = new SelectAidCase();
            excuteCase.ExcuteCase(response, CardRespDataType.SelectAid);

            return response;
        }

        protected List<AFL> GPOEx(string pdol)
        {
            ApduResponse response = base.GPO(pdol);
            var tlvs = DataParse.ParseTLV(response.Response);
            if(tlvs.Count == 1 &&
                tlvs[0].Value.Length > 4)
            {
                
                tagDict.SetTag("82", tlvs[0].Value.Substring(0, 4));
                tagDict.SetTag("94", tlvs[0].Value.Substring(4));
            }

            var AFLs = DataParse.ParseAFL(tagDict.GetTag("94"));

            IExcuteCase excuteCase = new GPOCase();
            excuteCase.ExcuteCase(response, CardRespDataType.GPO);

            return AFLs;
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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Business;
using CplusplusDll;

namespace CardPlatform.Cases
{
    public class ReadRecordCase : CaseBase
    {
        private List<ApduResponse> resps;
        private List<TLV> TLVs;
        private Dictionary<string, TLV> readRecordTags;
        protected override void Load()
        {
            TLVs = new List<TLV>();
            readRecordTags = new Dictionary<string, TLV>();
            Step = "ReadRecord";
            base.Load();
        }

        public override void ExcuteCase(object srcData)
        {
            resps = (List<ApduResponse>)srcData;
            List<string> tags = new List<string> { "9F12", "9F79", "9F51", "9F52" };
            foreach(var resp in resps)
            {
                TLVs = DataParse.ParseTLV(resp.Response);
                foreach(var tlv in TLVs)
                {
                    if(tlv.Len == 0)
                    {
                        TraceInfo(Config.TipLevel.Failed, "ReadRecord", "tag[{0}] 长度为0", tlv.Tag);
                    }
                    if(tags.Contains(tlv.Tag))
                    {
                        TraceInfo(Config.TipLevel.Failed, "ReadRecord", "读记录不应出现tag{0}", tlv.Tag);
                    }
                    if(!tlv.IsTemplate)
                    {
                        if(readRecordTags.ContainsKey(tlv.Tag))
                        {
                            TraceInfo(Config.TipLevel.Failed, "ReadRecord", "读记录中出现重复tag{0},请检查", tlv.Tag);
                        }
                        else
                        {
                            readRecordTags.Add(tlv.Tag, tlv);
                        }
                    }
                }
                CheckTemplateTag(TLVs);
            }

            base.ExcuteCase(srcData);
        }


        /// <summary>
        /// 是否每条记录都以70开始
        /// </summary>
        public void POBC_rDATA_GFFHX_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var resp in resps)
            {
                if(!CaseUtil.RespStartWith(resp.Response,"70"))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }

        public void POBC_rDATA_GFFHX_007()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if(!readRecordTags.ContainsKey("57"))
            {
                TraceInfo(Config.TipLevel.Failed, caseNo, "读记录缺少tag57");
            }
            var tag57 = readRecordTags["57"];
            if(!CaseUtil.RespStartWith(tag57.Value,"62"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            if(tag57.Value.Contains("F"))
            {
                int index = tag57.Value.
            }
        }

    }
}

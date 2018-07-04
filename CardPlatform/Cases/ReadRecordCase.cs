using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using CardPlatform.Common;
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
            base.Load();
        }

        public override void ExcuteCase(TransactionStep step, object srcData)
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

            base.ExcuteCase(step, srcData);
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

        /// <summary>
        /// Tag57的合规性检查（卡号不以62开头则警告）
        /// </summary>
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
        }

        /// <summary>
        /// tag45是否为合法的格式，包括填充F，账号位数，D的位置，失效月份检查，服务码检查等
        /// </summary>
        public void POBC_rDATA_GFFHX_008()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (readRecordTags.ContainsKey("57"))
            {
                var tag57 = readRecordTags["57"];
                if(!CaseUtil.IsCorrectTag57Format(tag57.Value))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }

        /// <summary>
        /// 长度检查，至少10字节，2字节的倍数
        /// </summary>
        public void POBC_rDATA_GFFHX_015()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!readRecordTags.ContainsKey("8E"))
            {
                TraceInfo(Config.TipLevel.Failed, caseNo, "读记录缺少tag8E");
            }
            var tag8E = readRecordTags["8E"];
            if(tag8E.Len < 10 || tag8E.Len % 2 != 0)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 分析前8字节是否为全0，如不是则提示
        /// </summary>
        public void POBC_rDATA_GFFHX_016()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (readRecordTags.ContainsKey("8E"))
            {
                var tag8E = readRecordTags["8E"];
                if(tag8E.Len > 10)
                {
                    if(tag8E.Value.Substring(0,8) != "00000000")
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                }
            }         
        }

        /// <summary>
        /// Tag5F20和tag9F0B不能同时存在
        /// </summary>
        public void POBC_rDATA_GFFHX_019()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if(readRecordTags.ContainsKey("5F20") &&
                readRecordTags.ContainsKey("9F0B"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 建议为202F
        /// </summary>
        public void POBC_rDATA_GFFHX_020()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("5F20"))
            {
                var tag5F20 = readRecordTags["5F20"];
                if(tag5F20.Value != "202F")
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }

        /// <summary>
        /// 转码后不能为乱码,中文
        /// </summary>
        public void POBC_rDATA_GFFHX_021()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("5F20"))
            {
                var tag5F20 = readRecordTags["5F20"];
                string value = UtilLib.Utils.BcdToStr(tag5F20.Value);
                if(!CaseUtil.IsAlpha(value))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
                if(tag5F20.Len < 2 || tag5F20.Len > 26)
                {
                    TraceInfo(Config.TipLevel.Warn, caseNo, "tag5F20长度需要在2-26字节之间");
                }
            }
        }

        /// <summary>
        /// 转码后不能为乱码,中文
        /// </summary>
        public void POBC_rDATA_GFFHX_022()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("9F0B"))
            {
                var tag9F0B = readRecordTags["9F0B"];
                string value = UtilLib.Utils.BcdToStr(tag9F0B.Value);
                if (!CaseUtil.IsAlpha(value))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
                if (tag9F0B.Len < 27 || tag9F0B.Len > 45)
                {
                    TraceInfo(Config.TipLevel.Warn, caseNo, "tag9F0B长度需要在27-45字节之间");
                }
            }
        }

        /// <summary>
        /// 服务码必须是02或06开头
        /// </summary>
        public void POBC_rDATA_GFFHX_024()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("5F30"))
            {
                TraceInfo(Config.TipLevel.Failed, caseNo, "缺少tag5F30");
            }
            else
            {
                var tag5F30 = readRecordTags["5F30"];
                if(tag5F30.Len != 2)
                {
                    TraceInfo(Config.TipLevel.Failed, caseNo, "tag5F30服务码长度错误");
                }
                if(tag5F30.Value.Substring(0,2) != "02" || tag5F30.Value.Substring(0,2) != "06")
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }

        /// <summary>
        /// 检查tag5F30是否为规范值
        /// </summary>
        public void POBC_rDATA_GFFHX_025()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if(readRecordTags.ContainsKey("5F30"))
            {
                var tag5F30 = readRecordTags["5F30"];
                var standardTag = new List<string> { "0220", "0620", "0223", "0623", "0201", "0601",
                    "0202", "0602", "0206", "0606", "0220", "0620", "0221","0621","0222","0622"};
                if(!standardTag.Contains(tag5F30.Value))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }

        /// <summary>
        /// 值只能是00～05，显示出对应的证件类型
        /// </summary>
        public void POBC_rDATA_GFFHX_028()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("9F62"))
            {
                var tag9F62 = readRecordTags["9F62"];
                if (tag9F62.Len != 1)
                {
                    TraceInfo(Config.TipLevel.Failed, caseNo, "tag9F62证件类型长度错误,长度需为1");
                }
                var standardTag = new List<string> { "00", "01", "02", "03", "04", "05" };
                if (!standardTag.Contains(tag9F62.Value))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }

        /// <summary>
        /// Tag9F63（长度必须是16字节）
        /// </summary>
        public void POBC_rDATA_GFFHX_033()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("9F63"))
            {
                var tag9F63 = readRecordTags["9F63"];
                if(tag9F63.Len != 16)
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }

        /// <summary>
        /// Tag9F0D/9F0E/9F0F（长度必须是5个字节
        /// </summary>
        public void POBC_rDATA_GFFHX_034()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F0D") || 
                !readRecordTags.ContainsKey("9F0E") ||
                !readRecordTags.ContainsKey("9F0F"))
            {
                TraceInfo(Config.TipLevel.Failed, caseNo, "读记录中缺少tag9F0D/tag9F0E/tag9F0F");
            }
            else
            {
                var tag9F0D = readRecordTags["9F0D"];
                var tag9F0E = readRecordTags["9F0E"];
                var tag9F0F = readRecordTags["9F0F"];

                if(tag9F0D.Len != 5 || tag9F0E.Len != 5 || tag9F0F.Len != 5)
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }

        /// <summary>
        /// Tag8C/8D能否分解
        /// </summary>
        public void POBC_rDATA_GFFHX_035()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if(!readRecordTags.ContainsKey("8C") || !readRecordTags.ContainsKey("8D"))
            {
                TraceInfo(Config.TipLevel.Failed, caseNo, "读记录中缺少tag8C/tag8D");
            }
            else
            {
                var tag8C = readRecordTags["8C"];
                var tag8D = readRecordTags["8D"];
                var tls8C = DataParse.ParseTL(tag8C.Value);
                var tls8D = DataParse.ParseTL(tag8D.Value);
                if(tls8C.Count == 0 || tls8D.Count == 0)
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }

        public void POBC_rDATA_GFFHX_036()
        {

        }

    }
}

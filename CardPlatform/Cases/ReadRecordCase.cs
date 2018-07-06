using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using CardPlatform.Common;
using CardPlatform.Config;
using CplusplusDll;
using CardPlatform.Helper;

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
                TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag57");
                return;
            }
            var tag57 = readRecordTags["57"];
            if(!CaseUtil.RespStartWith(tag57.Value,"62"))
            {
                TraceInfo(TipLevel.Warn, caseNo, "中国地区银联卡号一般以62开头");
            }
            if(!CaseUtil.IsCorrectTag57Format(tag57.Value))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
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
                TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag8E");
            }
            var tag8E = readRecordTags["8E"];
            if(tag8E.Len < 10 || tag8E.Len % 2 != 0)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            if (tag8E.Value.Substring(0, 8) != "00000000")
            {
                TraceInfo(TipLevel.Warn, caseNo, "8E前8字节一般为0");
            }
        }


        /// <summary>
        /// Tag5F20和tag9F0B只能存在一个
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
            if(!readRecordTags.ContainsKey("5F20") &&
                !readRecordTags.ContainsKey("9F0B"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测5F20的规范性
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
                    TraceInfo(TipLevel.Warn, caseNo, "5F20一般建议值为202F");
                }
                string value = UtilLib.Utils.BcdToStr(tag5F20.Value);
                if (!CaseUtil.IsAlpha(value) || value.Contains("/"))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
                if (tag5F20.Len < 2 || tag5F20.Len > 26)
                {
                    TraceInfo(Config.TipLevel.Warn, caseNo, "tag5F20长度需要在2-26字节之间");
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
        /// 服务码检测
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
                    return;
                }
                if(tag5F30.Value.Substring(0,2) != "02" || tag5F30.Value.Substring(0,2) != "06")
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
                var debitServiceCode = new List<string> { "120","220","520","620","123","223","523","623"};
                var creditServiceCode = new List<string> { "101","201","501","601","102",
                    "202","502","602","106","206","506","606","120","220","520","620",
                    "121","221","521","621","122","222","522","622","123","223","523","623"};

                var aid = TransactionTag.GetInstance().GetTag("84");
                if(aid == Constant.CreditAid)
                {
                    if(creditServiceCode.Contains(tag5F30.Value.Substring(1)))
                    {
                        TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                    else
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                }else if(aid == Constant.DebitAid)
                {
                    if (debitServiceCode.Contains(tag5F30.Value.Substring(1)))
                    {
                        TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                    else
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
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
                    TraceInfo(TipLevel.Failed, caseNo, "tag9F62证件类型长度错误,长度需为1");
                    return;
                }
                var standardTag = new List<string> { "00", "01", "02", "03", "04", "05" };
                if (!standardTag.Contains(tag9F62.Value))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测tag9F61的合规性
        /// </summary>
        public void POBC_rDATA_GFFHX_030()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("9F63"))
            {
                var tag9F63 = readRecordTags["9F63"];
                var value = UtilLib.Utils.BcdToStr(tag9F63.Value);
                if (!CaseUtil.IsAplhaDigit(value))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
                if(!readRecordTags.ContainsKey("9F62"))
                {
                    TraceInfo(TipLevel.Failed, caseNo, "tag9F63存在的情况下，tag9F62必须存在");
                    return;
                }
                else
                {
                    var tag9F62 = readRecordTags["9F61"];
                    if(tag9F62.Value == "00" && tag9F63.Value.Length != 18)
                    {
                        TraceInfo(TipLevel.Failed, caseNo, "如果是身份证类型，则tag9F62应为18字节");
                        return;
                    }
                }
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
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
        /// Tag9F0D/9F0E/9F0F（长度必须是5个字节)
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
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag8C/tag8D");
                return;
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
                    return;
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测5A的规范性
        /// </summary>
        public void POBC_rDATA_GFFHX_036()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if(!readRecordTags.ContainsKey("5A"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag5A");
                return;
            }
            Tag5AHelper tag5AHelper = new Tag5AHelper(readRecordTags["5A"].Value);
            if(!tag5AHelper.IsCorrectFormat())
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag5F24/tag5F25的规范性
        /// </summary>
        public void PBOC_rDATA_GFFHX_037()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("5F24") || !readRecordTags.ContainsKey("5F25"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag5F24/tag5F25");
                return;
            }
            else
            {
                var tag5F24 = readRecordTags["5F24"];
                var tag5F25 = readRecordTags["5F25"];
                if(tag5F24.Len != 3 || tag5F25.Len != 3)    //校验长度
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
                var month1 = Convert.ToInt16(tag5F24.Value.Substring(2, 2));
                var month2 = Convert.ToInt16(tag5F25.Value.Substring(2, 2));
                var day1 = Convert.ToInt16(tag5F24.Value.Substring(4, 2));
                var day2 = Convert.ToInt16(tag5F25.Value.Substring(4, 2));
                if(month1 < 1 || month1 > 12 || month2 < 1 || month2 > 12)  //月份检测
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
                if(day1 < 1 || day1 > 31 || day2 < 1 || day2 > 31)  //天数
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
                if((month1 == 2 && day1 > 28) ||(month2 == 2 && day2 > 28)) //2月份检测
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
                var intTag5F24 = Convert.ToInt32(tag5F24.Value);
                var intTag5F25 = Convert.ToInt32(tag5F25.Value);
                if(intTag5F24 < intTag5F25) //关系检测
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// tag5F34序列号检测
        /// </summary>
        public void POBC_rDATA_GFFHX_043()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("5F34"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag5F34");
                return;
            }
            if(readRecordTags["5F34"].Len != 1)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测9F4A
        /// </summary>
        public void POBC_rDATA_GFFHX_044()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F4A"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag9F4A");
                return;
            }
            if (readRecordTags["9F4A"].Value != "82")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F49的值
        /// </summary>
        public void POBC_rDATA_GFFHX_046()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F49"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag9F49");
                return;
            }
            if (readRecordTags["9F49"].Value != "9F3704")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag9F47/Tag9F32值
        /// </summary>
        public void POBC_rDATA_GFFHX_047()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F47") ||
                !readRecordTags.ContainsKey("9F32"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag9F47/tag9F32");
                return;
            }
            if ((readRecordTags["9F47"].Value != "03" && readRecordTags["9F47"].Value != "010001") ||
                (readRecordTags["9F32"].Value != "03" && readRecordTags["9F32"].Value != "010001"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F1F
        /// </summary>
        public void POBC_rDATA_GFFHX_049()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("9F1F"))
            {
                var tag9F63 = readRecordTags["9F1F"];
                var value = UtilLib.Utils.BcdToStr(tag9F63.Value);
                if (!CaseUtil.IsAlpha(value))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag8F的长度
        /// </summary>
        public void POBC_rDATA_GFFHX_050()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("8F"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag8F");
                return;
            }
            var tag8F = readRecordTags["8F"];
            if(tag8F.Len != 1)
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag8F");
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag5F28
        /// </summary>
        public void POBC_rDATA_GFFHX_051()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("5F28"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag5F28");
                return;
            }
            var tag5F28 = readRecordTags["5F28"];
            if (tag5F28.Len != 3)
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag5F28");
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F14
        /// </summary>
        public void POBC_rDATA_GFFHX_052()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F14"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag9F14");
                return;
            }
            var tag9F14 = readRecordTags["9F14"];
            if (tag9F14.Len != 2)
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag9F14");
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F23
        /// </summary>
        public void POBC_rDATA_GFFHX_053()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F23"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag9F23");
                return;
            }
            var tag9F23 = readRecordTags["9F23"];
            if (tag9F23.Len != 2)
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag9F23");
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F08
        /// </summary>
        public void POBC_rDATA_GFFHX_054()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F08"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag9F08");
                return;
            }
            if (readRecordTags["9F08"].Value != "0030")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 读记录中不能存在tag9F69,9F79
        /// </summary>
        public void POBC_rDATA_GFFHX_058()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("9F69") || readRecordTags.ContainsKey("9F79"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }


    }
}

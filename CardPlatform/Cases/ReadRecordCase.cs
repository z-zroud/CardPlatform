using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using CardPlatform.Common;
using CardPlatform.Config;
using CplusplusDll;
using CardPlatform.Helper;
using CardPlatform.ViewModel;
using CardPlatform.Models;

namespace CardPlatform.Cases
{
    public class ReadRecordCase : CaseBase
    {
        private List<ApduResponse> resps;
        private List<TLV> tlvs = new List<TLV>();
        private Dictionary<string, TLV> readRecordTags = new Dictionary<string, TLV>();

        public override void Excute(int batchNo, AppType app, TransactionStep step, object srcData)
        {
            resps = (List<ApduResponse>)srcData;
            List<string> tags = new List<string> { "9F12", "9F79", "9F51", "9F52" };
            foreach (var resp in resps)
            {
                tlvs = DataParse.ParseTLV(resp.Response);
                foreach (var tlv in tlvs)
                {
                    if (tlv.Len == 0)
                    {
                        TraceInfo(Config.TipLevel.Failed, "ReadRecord", "tag[{0}] 长度为0,银联,Visa,MC都建议空值无需个人化", tlv.Tag);
                    }
                    if (tags.Contains(tlv.Tag))
                    {
                        TraceInfo(Config.TipLevel.Failed, "ReadRecord", "读记录不应出现tag{0}", tlv.Tag);
                    }
                    if (!tlv.IsTemplate)
                    {
                        if (readRecordTags.ContainsKey(tlv.Tag))
                        {
                            TraceInfo(Config.TipLevel.Failed, "ReadRecord", "读记录中出现重复tag{0},请检查", tlv.Tag);
                        }
                        else
                        {
                            readRecordTags.Add(tlv.Tag, tlv);
                        }
                    }
                }
            }

            base.Excute(batchNo,app,step, srcData);
        }

        #region 通用case
        /// <summary>
        /// 检测读记录是否以70模板开头
        /// </summary>
        public TipLevel ReadRecord_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var resp in resps)
            {
                log.TraceLog("响应数据:【{0}】", resp.Response);
                if (!CaseUtil.RespStartWith(resp.Response, "70"))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[响应数据为:" + resp.Response + "]");
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// Tag57的合规性检查
        /// </summary>
        public TipLevel ReadRecord_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag57 =TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "57");
            if (!readRecordTags.ContainsKey("57") &&
                string.IsNullOrEmpty(tag57))
            {
                return TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag57");
            }
            if(string.IsNullOrEmpty(tag57))
            {
                tag57 = readRecordTags["57"].Value;
            }
            
            caseItem.Description += "tag57=" + tag57;
            log.TraceLog("tag57={0}", tag57);
            if (!CaseUtil.IsCorrectTag57Format(tag57))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag8E的合规性(格式、长度等)
        /// </summary>
        public TipLevel ReadRecord_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!readRecordTags.ContainsKey("8E"))
            {
                return TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag8E");
            }
            var tag8E = readRecordTags["8E"];
            caseItem.Description += "【tag8E=" + tag8E.Value + "】";
            if (tag8E.Len < 10 || tag8E.Len % 2 != 0)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[8E长度不符合规范]");
            }
            if (tag8E.Value.Substring(0, 16) != "0000000000000000")
            {
                return TraceInfo(TipLevel.Warn, caseNo, "8E前8字节一般为0");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag5F20和tag9F0B的互斥性(不能同时存在，只能存在一个)
        /// </summary>
        public TipLevel ReadRecord_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("5F20"))
            {
                caseItem.Description += "【tag5F20=" + readRecordTags["5F20"].Value + "】";
                log.TraceLog("tag5F20=【{0}】", readRecordTags["5F20"].Value);
            }
            else
            {
                log.TraceLog("tag5F20=【null】");
            }
            if (readRecordTags.ContainsKey("9F0B"))
            {
                caseItem.Description += "【tag9F0B=" + readRecordTags["9F0B"].Value + "】";
                log.TraceLog("tag9F0B=【{0}】", readRecordTags["9F0B"].Value);
            }
            else
            {
                log.TraceLog("tag9F0B=【null】");
            }
            
            if (readRecordTags.ContainsKey("5F20") &&
                readRecordTags.ContainsKey("9F0B"))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }

            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag5F20的规范性(英文字符、包含/,建议值、长度等)
        /// </summary>
        public TipLevel ReadRecord_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (readRecordTags.ContainsKey("5F20"))
            {
                var tag5F20 = readRecordTags["5F20"];
                string value = UtilLib.Utils.BcdToStr(tag5F20.Value);
                caseItem.Description += "tag5F20" + "转为字符串显示:" + value;
                log.TraceLog("转码为:{0}", value);
                if(!CaseUtil.IsContainSlash(value))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
                log.TraceLog("长度为:{0}", tag5F20.Len);
                if (tag5F20.Len < 2 || tag5F20.Len > 26)
                {
                    TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "tag5F20长度需要在2-26字节之间");
                }
                if (tag5F20.Value != "202F")
                {
                    return TraceInfo(TipLevel.Tip, caseNo, caseItem.Description + "[5F20一般建议值为202F]");
                }
                else
                {
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
            }
            else
            {
                if(transConfig.CurrentApp == AppType.qVSDC_offline ||
                    transConfig.CurrentApp == AppType.qVSDC_online ||
                    transConfig.CurrentApp == AppType.qVSDC_online_without_ODA)
                    return TraceInfo(TipLevel.Tip, caseNo, caseItem.Description + "[tag5F20不存在]");
                else
                    return TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "[tag5F20不存在]");
            }
        }

        /// <summary>
        /// 检测tag9F0B的规范性(英文字符、长度等)
        /// </summary>
        public TipLevel ReadRecord_006()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("9F0B"))
            {
                var tag9F0B = readRecordTags["9F0B"];
                string value = UtilLib.Utils.BcdToStr(tag9F0B.Value);
                log.TraceLog("转码为:【{0}】", value);
                if (!CaseUtil.IsContainSlash(value))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
                log.TraceLog("长度为:【{0}】", tag9F0B.Len);
                if (tag9F0B.Len < 27 || tag9F0B.Len > 45)
                {
                    return TraceInfo(Config.TipLevel.Warn, caseNo, "tag9F0B长度需要在27-45字节之间");
                }
            }
            return TipLevel.Unknown;
        }

        /// <summary>
        /// 检测Tag9F0D/9F0E/9F0F（长度必须是5个字节)
        /// </summary>
        public TipLevel ReadRecord_011()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F0D") ||
                !readRecordTags.ContainsKey("9F0E") ||
                !readRecordTags.ContainsKey("9F0F"))
            {
                return TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "【读记录中缺少tag9F0D/tag9F0E/tag9F0F】");
            }
            else
            {
                var tag9F0D = readRecordTags["9F0D"];
                var tag9F0E = readRecordTags["9F0E"];
                var tag9F0F = readRecordTags["9F0F"];
                caseItem.Description += "【tag9F0D=" + tag9F0D.Value + "】";
                caseItem.Description += "【tag9F0E=" + tag9F0E.Value + "】";
                caseItem.Description += "【tag9F0F=" + tag9F0F.Value + "】";
                log.TraceLog("tag9F0D=【" + tag9F0D.Value + "】 长度为:" + tag9F0D.Len);
                log.TraceLog("tag9F0E=【" + tag9F0E.Value + "】 长度为:" + tag9F0E.Len);
                log.TraceLog("tag9F0F=【" + tag9F0F.Value + "】 长度为:" + tag9F0F.Len);
                if (tag9F0D.Len != 5 || tag9F0E.Len != 5 || tag9F0F.Len != 5)
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "【请检查tag9F0D/tag9F0E/tag9F0F长度是否为5个字节】");
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag8D是否符合规范，能否分解，长度是否符合规范
        /// </summary>
        public TipLevel ReadRecord_036()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("8D"))
            {
                return TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag8C。借贷记交易，卡片风险管理数据对象列表必须存在");
            }
            else
            {
                var tag8D = readRecordTags["8D"];
                var tls8D = DataParse.ParseTL(tag8D.Value);
                //caseItem.Description += "【tag8D=" + tag8D.Value + "】";
                if (tls8D.Count == 0)
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag8D无法TL分解]");
                }
                foreach (var tl in tls8D)
                {
                    var terminalData = locator.Terminal.GetTag(tl.Tag);
                    if (string.IsNullOrEmpty(terminalData))
                    {
                        return TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "终端数据[tag{0}]不存在，请联系开发人员添加该tag", tl.Tag);
                    }
                    if (tl.Len * 2 != terminalData.Length)
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "【tag" + tl.Tag + "长度为" + tl.Len + "终端规范长度为:" + terminalData.Length / 2 + "】");
                    }
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag8C是否符合规范，能否分解，长度是否符合规范
        /// </summary>
        public TipLevel ReadRecord_012()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("8C"))
            {
                return TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag8C。借贷记交易，卡片风险管理数据对象列表必须存在");
            }
            else
            {
                var tag8C = readRecordTags["8C"];
                var tls8C = DataParse.ParseTL(tag8C.Value);
                //caseItem.Description += "【tag8C" + tag8C.Value + "】";
                if (tls8C.Count == 0)
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag8C无法TL分解]");
                }
                foreach (var tl in tls8C)
                {
                    var terminalData = locator.Terminal.GetTag(tl.Tag);
                    if (string.IsNullOrEmpty(terminalData))
                    {
                        return TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "终端数据[tag{0}]不存在，请联系开发人员添加该tag", tl.Tag);
                    }
                    if (tl.Len * 2 != terminalData.Length)
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "【tag" + tl.Tag + "长度为" + tl.Len + "终端规范长度为:" + terminalData.Length / 2 + "】");
                    }
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5A的规范性(格式、长度在16~19位之间等)
        /// </summary>
        public TipLevel ReadRecord_013()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!readRecordTags.ContainsKey("5A"))
            {
                return TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag5A");
            }
            caseItem.Description += "【tag5A=" + readRecordTags["5A"].Value + "】";
            Tag5AHelper tag5AHelper = new Tag5AHelper(readRecordTags["5A"].Value);
            if (!tag5AHelper.IsCorrectFormat())
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            if(!readRecordTags["5A"].Value.StartsWith("4"))
                return TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "[港澳及海外项目，一般卡号以4开头，请确认卡号的正确性]");
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        bool CheckDate(string date, ref string desc)
        {
            if (date.Length / 2 != 3)    //校验长度
            {
                desc = "[长度校验错误,实际长度为" + (date.Length / 2).ToString() + "]";
                return false;
            }
            var month = Convert.ToInt16(date.Substring(2, 2));
            var day = Convert.ToInt16(date.Substring(4, 2));
            if (month < 1 || month > 12)  //月份检测
            {
                desc = "[月份校验错误,数据中月份为" + month.ToString() + "]";
                return false;
            }
            if (day < 1 || day > 31)  //天数
            {
                date = "[天数校验错误,数据中天数为" + day.ToString() + "]";
                return false;
            }
            if ((month == 2 && day > 28) || (month == 2 && day > 28)) //2月份检测
            {
                desc = date = "[二月份天数校验错误,数据中天数为" + day.ToString() + "]";
                return false;
            }
            return true;
        }
        /// <summary>
        /// 检测Tag5F24/5F25的规范性(必须存在，格式、长度、先后顺序等)
        /// </summary>
        public TipLevel ReadRecord_014()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!readRecordTags.ContainsKey("5F24"))
            {
                return TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag5F24");
            }
            var tag5F24 = readRecordTags["5F24"].Value;
            string result = string.Empty;
            if (!CheckDate(tag5F24, ref result))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + result);
            }
            if (readRecordTags.ContainsKey("5F25"))
            {
                var tag5F25 = readRecordTags["5F25"].Value;
                if (!CheckDate(tag5F25, ref result))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + result);
                }
                var intTag5F24 = Convert.ToInt32(tag5F24);
                var intTag5F25 = Convert.ToInt32(tag5F25);
                if (intTag5F24 < intTag5F25) //关系检测
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "失效日期早于生效日期");
                }

            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5F34的长度
        /// </summary>
        public TipLevel ReadRecord_015()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("5F34"))
            {
                return TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag5F34");
            }
            caseItem.Description += "【tag5F34=" + readRecordTags["5F34"].Value + "】";
            if (readRecordTags["5F34"].Len != 1)
            {

                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag9F4A值是否为82
        /// </summary>
        public TipLevel ReadRecord_016()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if(transConfig.CurrentApp == AppType.UICS ||
                transConfig.CurrentApp == AppType.VISA ||
                transConfig.CurrentApp == AppType.MC)
            {
                var tag82 = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "82");
                var aipHelper = new AipHelper(tag82);
                if (aipHelper.IsSupportDDA() || aipHelper.IsSupportCDA())
                {
                    if (!readRecordTags.ContainsKey("9F4A"))
                    {
                        return TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag9F4A");
                    }
                    caseItem.Description += "【tag5F4A=" + readRecordTags["9F4A"].Value + "】";
                    if (readRecordTags["9F4A"].Value != "82")
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
            }
            
            return TipLevel.Sucess;
        }

        /// <summary>
        /// 检测Tag9F49值是否为9F3704
        /// </summary>
        public TipLevel ReadRecord_017()
        {

            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag82 = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(tag82);
            if (aipHelper.IsSupportCDA() || aipHelper.IsSupportDDA() || aipHelper.IsSupportSDA())
            {
                if (!readRecordTags.ContainsKey("9F49"))
                {
                    return TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag9F49");
                }
                caseItem.Description += "【tag9F49=" + readRecordTags["9F49"].Value + "】";
                if (readRecordTags["9F49"].Value != "9F3704")
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            return TipLevel.Sucess;
        }

        /// <summary>
        /// 检测Tag9F47/9F32值是否为03或010001
        /// </summary>
        public TipLevel ReadRecord_018()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag82 = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(tag82);
            if (aipHelper.IsSupportCDA() || aipHelper.IsSupportDDA() || aipHelper.IsSupportSDA())
            {
                if (!readRecordTags.ContainsKey("9F47") ||
                !readRecordTags.ContainsKey("9F32"))
                {
                    return TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag9F47/tag9F32");
                }
                caseItem.Description += "【tag9F47=" + readRecordTags["9F47"].Value + "】";
                caseItem.Description += "【tag9F32=" + readRecordTags["9F32"].Value + "】";
                if ((readRecordTags["9F47"].Value != "03" && readRecordTags["9F47"].Value != "010001") ||
                    (readRecordTags["9F32"].Value != "03" && readRecordTags["9F32"].Value != "010001"))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            return TipLevel.Sucess;
        }

        /// <summary>
        /// 检测Tag9F1F(转换为字符串)
        /// </summary>
        public TipLevel ReadRecord_019()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("9F1F"))
            {
                var tag9F1F = readRecordTags["9F1F"];
                var value = UtilLib.Utils.BcdToStr(tag9F1F.Value);
                caseItem.Description += "tag9F1F" + "转为字符串显示:" + value;
                if (!CaseUtil.IsAplhaDigit(value))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            else
            {
                return TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "[读数据中缺少tag9F1F]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag8F长度等于1
        /// </summary>
        public TipLevel ReadRecord_020()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag82 = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(tag82);
            if (aipHelper.IsSupportCDA() || aipHelper.IsSupportDDA() || aipHelper.IsSupportSDA())
            {
                if (!readRecordTags.ContainsKey("8F"))
                {
                    return TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag8F");
                }
                var tag8F = readRecordTags["8F"];
                caseItem.Description += "【tag8F=" + tag8F.Value + "】";
                if (tag8F.Len != 1)
                {
                    return TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "[长度有误，规范长度为1字节]");
                }
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            return TipLevel.Sucess;
        }

        /// <summary>
        /// 检测Tag5F28的规范性
        /// </summary>
        public TipLevel ReadRecord_021()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("5F28"))
            {
                if (string.IsNullOrEmpty(TransactionTag.GetInstance().GetTag("9F07")))
                    return TipLevel.Sucess;
                return TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag5F28，终端将无法做应用用途控制检查");
            }
            var tag5F28 = readRecordTags["5F28"];
            caseItem.Description += "【tag5F28=" + tag5F28.Value + "】";
            if (tag5F28.Len != 2)
            {
                return TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "长度有误，规范长度为2字节");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag14的规范性
        /// </summary>
        public TipLevel ReadRecord_022()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F14"))
            {
                return TraceInfo(TipLevel.Warn, caseNo, "读记录缺少tag9F14");
            }
            var tag9F14 = readRecordTags["9F14"];
            caseItem.Description += "【tag9F14=" + tag9F14.Value + "】";
            if (tag9F14.Len != 1)
            {
                return TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "数据长度有误，规范长度为1字节");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag23的规范性
        /// </summary>
        public TipLevel ReadRecord_023()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F23"))
            {
                return TraceInfo(TipLevel.Warn, caseNo, "读记录缺少tag9F23");
            }
            var tag9F23 = readRecordTags["9F23"];
            caseItem.Description += "【tag9F23=" + tag9F23.Value + "】";
            if (tag9F23.Len != 1)
            {
                return TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "长度有误，规范长度为1字节");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag9F08的规范性
        /// </summary>
        public TipLevel ReadRecord_024()
        {
            var transCfg = TransactionConfig.GetInstance();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F08"))
            {
                return TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "[读记录中缺少tag9F08]");
            }
            caseItem.Description += "[tag9F08=" + readRecordTags["9F08"].Value + "]";
            if (transCfg.CurrentApp == AppType.UICS)
            {
                if (readRecordTags["9F08"].Value != "0030")
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            else if (transCfg.CurrentApp == AppType.VISA)
            {
                if (readRecordTags["9F08"].Value != "00A0")
                {
                    if(readRecordTags["9F08"].Value == "0096")
                        return TraceInfo(TipLevel.Tip, caseNo, caseItem.Description + "==>[VISA版本为1.5，请确认是否正确]");
                    else
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            else if(transCfg.CurrentApp == AppType.MC ||
                transCfg.CurrentApp == AppType.Paypass)
            {
                if (readRecordTags["9F08"].Value != "0002")
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            else
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[未知的应用类型]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag9F69,9F79，不能出现在读记录中
        /// </summary>
        public TipLevel ReadRecord_025()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("9F69"))
            {
                caseItem.Description += "【tag9F69=" + readRecordTags["9F69"].Value + "】";
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            if (readRecordTags.ContainsKey("9F79"))
            {
                caseItem.Description += "【tag9F79=" + readRecordTags["9F79"].Value + "】";
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5F30中的服务码与57中的一致性,是否以02或06开头，长度为2字节
        /// </summary>
        public TipLevel ReadRecord_026()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!readRecordTags.ContainsKey("5F30"))
            {
                if(transConfig.CurrentApp == AppType.qVSDC_online_without_ODA ||
                    transConfig.CurrentApp == AppType.qVSDC_offline ||
                    transConfig.CurrentApp == AppType.qVSDC_online)
                    return TraceInfo(TipLevel.Tip, caseNo, "读数据中缺少服务码tag5F30,qVSDC无需该tag值");
                else
                    return TraceInfo(TipLevel.Warn, caseNo, "读数据中缺少服务码tag5F30");
            }
            if (!readRecordTags.ContainsKey("57"))
            {
                return TraceInfo(caseItem.Level, caseNo, "读数据中缺少tag57");
            }
            if (readRecordTags["5F30"].Value.Length != 4)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[长度错误]");
            }
            if(readRecordTags["5F30"].Value.Substring(0,2) != "02" &&
                readRecordTags["5F30"].Value.Substring(0,2) != "06")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            Tag57Helper tag57Helper = new Tag57Helper(readRecordTags["57"].Value);
            caseItem.Description += "【tag5F30=" + readRecordTags["5F30"].Value + "】";
            if (readRecordTags["5F30"].Value != tag57Helper.GetServiceCode())
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[57中服务码为{0}]", tag57Helper.GetServiceCode());
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5F24与57中的失效期的年月一致性
        /// </summary>
        public TipLevel ReadRecord_027()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag57 = TransactionTag.GetInstance().GetTag("57");
            if (!readRecordTags.ContainsKey("5F24") || string.IsNullOrEmpty(tag57))
            {
                return TraceInfo(caseItem.Level, caseNo, "读数据中缺少tag5F24或者tag57");
            }
            Tag57Helper tag57Helper = new Tag57Helper(tag57);
            caseItem.Description += "[tag5F24=" + readRecordTags["5F24"].Value + "]";
            if (readRecordTags["5F24"].Value.Substring(0, 4) != tag57Helper.GetExpiredDate())
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag57中的失效日期为:{0}]", tag57Helper.GetExpiredDate());
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5A与57中卡号的一致性
        /// </summary>
        public TipLevel ReadRecord_028()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);


            var tag57 = TransactionTag.GetInstance().GetTag("57");

            if (!readRecordTags.ContainsKey("5A") || string.IsNullOrEmpty(tag57))
            {
                return TraceInfo(caseItem.Level, caseNo, "读数据中缺少tag5A或者tag57");
            }
            Tag57Helper tag57Helper = new Tag57Helper(tag57);
            Tag5AHelper tag5AHelper = new Tag5AHelper(readRecordTags["5A"].Value);
            caseItem.Description += "【5A=" + readRecordTags["5A"].Value + "】";
            if (tag5AHelper.GetAccount() != tag57Helper.GetAccount())
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "tag57账号为:{0}", tag57Helper.GetAccount());
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag9F42应用货币代码，如果CVM中要求金额检查(X,Y金额不为零)，该数据必须存在
        /// </summary>
        public TipLevel ReadRecord_035()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag9F42 = TransactionTag.GetInstance().GetTag("9F42");
            if (!string.IsNullOrEmpty(tag9F42))
            {
                caseItem.Description += "[tag9F42=" + tag9F42 + "]";
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            var tag8E = TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "8E");
            if (!string.IsNullOrEmpty(tag8E))
            {
                if (tag8E.Substring(0, 16) == "0000000000000000")
                {
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description + "[tag9F42不存在]");
                }
                else
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[XY金额为:{0}],缺少tag9F42", tag8E.Substring(0, 16));
                }
            }
            return TipLevel.Unknown;
        }

        #endregion
        #region UICS
        /// <summary>
        /// 检测tag5F30的规范性(借记/贷记使用的服务码需在规范范围内)
        /// </summary>
        public TipLevel ReadRecord_007()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("5F30"))
            {
                return TraceInfo(Config.TipLevel.Failed, caseNo, "缺少tag5F30");
            }
            else
            {
                var tag5F30 = readRecordTags["5F30"];
                if (tag5F30.Len != 2)
                {
                    return TraceInfo(Config.TipLevel.Failed, caseNo, "tag5F30服务码长度错误");
                }
                if (tag5F30.Value.Substring(0, 2) != "02" && tag5F30.Value.Substring(0, 2) != "06")
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
                var debitServiceCode = new List<string> { "120", "220", "520", "620", "123", "223", "523", "623" };
                var creditServiceCode = new List<string> { "101","201","501","601","102",
                    "202","502","602","106","206","506","606","120","220","520","620",
                    "121","221","521","621","122","222","522","622","123","223","523","623"};

                var aid = TransactionTag.GetInstance().GetTag("84");
                if (aid == Constant.CreditAid)
                {
                    if (creditServiceCode.Contains(tag5F30.Value.Substring(1)))
                    {
                        return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                    else
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                }
                else if (aid == Constant.DebitAid)
                {
                    if (debitServiceCode.Contains(tag5F30.Value.Substring(1)))
                    {
                        return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                    else
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                }
            }
            return TipLevel.Unknown;
        }

        /// <summary>
        /// 检测tag9F62证件类型的规范性(取值范围、长度等)
        /// </summary>
        public TipLevel ReadRecord_008()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("9F62"))
            {
                var tag9F62 = readRecordTags["9F62"];
                if (tag9F62.Len != 1)
                {
                    return TraceInfo(TipLevel.Failed, caseNo, "tag9F62证件类型长度错误,长度必须为固定1字节");
                }
                var standardTag = new List<string> { "00", "01", "02", "03", "04", "05" };
                if (!standardTag.Contains(tag9F62.Value))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[取值范围不符合规范]");
                }
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            return TipLevel.Unknown;
        }

        /// <summary>
        /// 检测tag9F61证件号的规范性(转字符串、证件类型对应的证件长度、规范长度等)
        /// </summary>
        public TipLevel ReadRecord_009()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if ((!readRecordTags.ContainsKey("9F61") && readRecordTags.ContainsKey("9F62")) ||
                readRecordTags.ContainsKey("9F61") && !readRecordTags.ContainsKey("9F62"))
            {
                return TraceInfo(caseItem.Level, caseNo, "检测Tag9F61和Tag9F62是否同时存在，必须同时存在");
            }
            if (readRecordTags.ContainsKey("9F61"))
            {
                var tag9F61 = readRecordTags["9F61"];
                var value = UtilLib.Utils.BcdToStr(tag9F61.Value);
                if (!CaseUtil.IsAplhaDigit(value))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }

                var tag9F62 = readRecordTags["9F62"];
                if (tag9F62.Value == "00" && tag9F61.Value.Length != 18)
                {
                    return TraceInfo(TipLevel.Failed, caseNo, "如果是身份证类型，则tag9F61应为18字节");
                }
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            return TipLevel.Unknown;
        }

        #endregion

        /// <summary>
        /// 检测QPBOC/QUICS交易中，tag9F74是否正确，且作为最后一个读记录命令中返回
        /// </summary>
        public TipLevel ReadRecord_031()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F66 = locator.Terminal.GetTag("9F66");
            if (tag9F66.Length != 8)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "结果: tag9F66为空或长度不对");
            }
            int lastByte = Convert.ToInt16(tag9F66.Substring(6), 16);

            int count = resps.Count;
            var tlvs = DataParse.ParseTLV(resps[count - 1].Response);
            if ((lastByte & 0x80) == 0x80 && tlvs.Count != 3)   //支持fDDA01
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "结果:fDDA01版本，卡片最后一条记录只能返回tag9F69和9F74");
            }
            if ((lastByte & 0x80) != 0x80 && tlvs.Count != 2)   //支持fDDA00
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "结果:fDDA00版本，卡片最后一条记录只能返回tag9F74");
            }

            foreach (var tl in tlvs)
            {
                if (tl.Tag == "9F74" && tl.Value == "454343303031")
                {
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
            }

            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测fDDA01版本，卡片最后一个读记录应返回tag9F69
        /// </summary>
        public TipLevel ReadRecord_032()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F66 = locator.Terminal.GetTag("9F66");
            if(tag9F66.Length != 8)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "结果: tag9F66为空或长度不对");
            }
            int lastByte = Convert.ToInt16(tag9F66.Substring(6), 16);
            if((lastByte & 0x80) == 0x80)   //支持fDDA01
            {
                int count = resps.Count;
                var tlvs = DataParse.ParseTLV(resps[count - 1].Response);
                foreach (var tl in tlvs)
                {
                    if (tl.Tag == "9F69" &&
                        tl.Len >= 8 &&
                        tl.Len <= 16)
                    {
                        return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                }
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TipLevel.Unknown;
        }

        /// <summary>
        /// 检测fDDA01下tag9F69与DF61和9F6C的一致性
        /// </summary>
        public TipLevel ReadRecord_033()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F66 = locator.Terminal.GetTag("9F66");
            if (tag9F66.Length != 8)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "结果: tag9F66为空或长度不对");
            }
            int lastByte = Convert.ToInt16(tag9F66.Substring(6), 16);
            if ((lastByte & 0x80) == 0x80)   //支持fDDA01
            {
                int count = resps.Count;
                var tlvs = DataParse.ParseTLV(resps[count - 1].Response);
                foreach (var tl in tlvs)
                {
                    if (tl.Tag == "9F69" && tl.Len >= 8 && tl.Len <= 16)
                    {
                        var tagDF61 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectApp,"DF61");
                        var tag9F6C = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "9F6C");
                        if(tl.Value.Substring(tl.Value.Length - 2, 2) != tagDF61 ||
                            tl.Value.Substring(10,4) != tag9F6C)
                        {
                            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        }
                        return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                }
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TipLevel.Unknown;
        }

        /// <summary>
        /// 检测fDDA00版本下，卡片不应该返回tag9F69
        /// </summary>
        public TipLevel ReadRecord_034()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F66 = locator.Terminal.GetTag("9F66");
            if (tag9F66.Length != 8)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "结果: tag9F66为空或长度不对");
            }
            int lastByte = Convert.ToInt16(tag9F66.Substring(6), 16);
            if ((lastByte & 0x80) != 0x80)   //支持fDDA01
            {
                int count = resps.Count;
                var tlvs = DataParse.ParseTLV(resps[count - 1].Response);
                foreach (var tl in tlvs)
                {
                    if (tl.Tag == "9F69")
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                }
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            return TipLevel.Unknown;
        }

        /// <summary>
        /// 检测qVSDC 读记录最后一条数据必须是tag9F69
        /// </summary>
        public TipLevel ReadRecord_037()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            int count = resps.Count;
            var tlvs = DataParse.ParseTLV(resps[count - 1].Response);

            var lastTag = tlvs[tlvs.Count - 1];
            if(lastTag.Tag != "9F69")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description += "最后一条记录为:tag{0}", lastTag.Tag);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测qVSDC 必须在GPO或读记录中返回的数据是否存在
        /// </summary>
        /// <returns></returns>
        public TipLevel ReadRecord_038()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var gpoTags = TransactionTag.GetInstance().GetTags(TransactionStep.GPO);
            var recordTags = TransactionTag.GetInstance().GetTags(TransactionStep.ReadRecord);
            if(transConfig.CurrentApp == AppType.qVSDC_offline ||
                transConfig.CurrentApp == AppType.qVSDC_online)
            {
                var tagsNeedReturn = new List<string> { "57", "9F4B", "9F6E" };
                foreach(var tag in tagsNeedReturn)
                {
                    if(!CaseUtil.HasTag(tag,gpoTags) && !CaseUtil.HasTag(tag,recordTags))
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[qVSDC缺少必备tag{0}]", tag);
                    }
                }
            }else if(transConfig.CurrentApp == AppType.qVSDC_online_without_ODA)
            {
                var tagsNeedReturn = new List<string> { "57", "9F6E" };
                foreach (var tag in tagsNeedReturn)
                {
                    if (!CaseUtil.HasTag(tag, gpoTags) && !CaseUtil.HasTag(tag, recordTags))
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[qVSDC缺少必备tag{0}]", tag);
                    }
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测qVSDC交易，tag9F07是否存在,若存在，则qVSDC所有路径都需返回该值(联机ODA,脱机ODA,联机without ODA)
        /// </summary>
        /// <returns></returns>
        public TipLevel ReadRecord_039()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var gpoTags = TransactionTag.GetInstance().GetTags(TransactionStep.GPO);
            var recordTags = TransactionTag.GetInstance().GetTags(TransactionStep.ReadRecord);
            if (transConfig.CurrentApp == AppType.qVSDC_offline ||
                transConfig.CurrentApp == AppType.qVSDC_online ||
                transConfig.CurrentApp == AppType.qVSDC_online_without_ODA)
            {
                if(CaseUtil.HasTag("9F07",gpoTags))
                {
                    return TraceInfo(TipLevel.Warn, caseNo, caseItem.Description);
                }
            }
            return TipLevel.Sucess; //不在界面中显示
        }
    }
}

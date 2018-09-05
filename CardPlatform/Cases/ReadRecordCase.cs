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
        private List<TLV> tlvs;
        private Dictionary<string, TLV> readRecordTags = new Dictionary<string, TLV>();

        protected override void Load()
        {
            tlvs = new List<TLV>();
            base.Load();
        }

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
                        TraceInfo(Config.TipLevel.Failed, "ReadRecord", "tag[{0}] 长度为0", tlv.Tag);
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
                CheckTemplateTag(tlvs);
            }

            base.Excute(batchNo,app,step, srcData);
        }

        #region 通用case
        /// <summary>
        /// 检测读记录是否以70模板开头
        /// </summary>
        public void ReadRecord_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var resp in resps)
            {
                if (!CaseUtil.RespStartWith(resp.Response, "70"))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[响应数据为:" + resp.Response + "]");
                    return;
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// Tag57的合规性检查
        /// </summary>
        public void ReadRecord_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!readRecordTags.ContainsKey("57"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag57");
                return;
            }
            var tag57 = readRecordTags["57"];
            caseItem.Description += "【tag57=" + tag57.Value + "】";
            //if (!CaseUtil.RespStartWith(tag57.Value, "62"))
            //{
            //    TraceInfo(TipLevel.Warn, caseNo, "中国地区银联卡号一般以62开头");
            //}
            if (!CaseUtil.IsCorrectTag57Format(tag57.Value))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag8E的合规性(格式、长度等)
        /// </summary>
        public void ReadRecord_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!readRecordTags.ContainsKey("8E"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag8E");
                return;
            }
            var tag8E = readRecordTags["8E"];
            caseItem.Description += "【tag8E=" + tag8E.Value + "】";
            if (tag8E.Len < 10 || tag8E.Len % 2 != 0)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[8E长度不符合规范]");
                return;
            }
            if (tag8E.Value.Substring(0, 16) != "0000000000000000")
            {
                TraceInfo(TipLevel.Warn, caseNo, "8E前8字节一般为0");
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag5F20和tag9F0B的互斥性(不能同时存在，只能存在一个)
        /// </summary>
        public void ReadRecord_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("5F20"))
            {
                caseItem.Description += "【tag5F20=" + readRecordTags["5F20"].Value + "】";
            }
            if (readRecordTags.ContainsKey("9F0B"))
            {
                caseItem.Description += "【tag9F0B=" + readRecordTags["9F0B"].Value + "】";
            }
            if (readRecordTags.ContainsKey("5F20") &&
                readRecordTags.ContainsKey("9F0B"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            if (!readRecordTags.ContainsKey("5F20") &&
                !readRecordTags.ContainsKey("9F0B"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag5F20的规范性(英文字符、包含/,建议值、长度等)
        /// </summary>
        public void ReadRecord_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (readRecordTags.ContainsKey("5F20"))
            {
                var tag5F20 = readRecordTags["5F20"];
                string value = UtilLib.Utils.BcdToStr(tag5F20.Value);
                caseItem.Description += "【tag5F20=" + tag5F20.Value + "】转为字符串显示:" + value;
                if (tag5F20.Value != "202F")
                {
                    TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "【tag5F20="+ tag5F20.Value + "】5F20一般建议值为202F");
                }
                else
                {
                    TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    return;
                }
                if (!CaseUtil.IsAlpha(value) || value.Contains("/"))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
                if (tag5F20.Len < 2 || tag5F20.Len > 26)
                {
                    TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "tag5F20长度需要在2-26字节之间");
                }
            }
            else
            {
                TraceInfo(TipLevel.Warn, caseNo, "tag5F20不存在");
            }
        }

        /// <summary>
        /// 检测tag9F0B的规范性(英文字符、长度等)
        /// </summary>
        public void ReadRecord_006()
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
        /// 检测Tag9F0D/9F0E/9F0F（长度必须是5个字节)
        /// </summary>
        public void ReadRecord_011()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F0D") ||
                !readRecordTags.ContainsKey("9F0E") ||
                !readRecordTags.ContainsKey("9F0F"))
            {
                TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "【读记录中缺少tag9F0D/tag9F0E/tag9F0F】");
                return;
            }
            else
            {
                var tag9F0D = readRecordTags["9F0D"];
                var tag9F0E = readRecordTags["9F0E"];
                var tag9F0F = readRecordTags["9F0F"];
                caseItem.Description += "【tag9F0D=" + tag9F0D.Value + "】";
                caseItem.Description += "【tag9F0E=" + tag9F0E.Value + "】";
                caseItem.Description += "【tag9F0F=" + tag9F0F.Value + "】";
                if (tag9F0D.Len != 5 || tag9F0E.Len != 5 || tag9F0F.Len != 5)
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description + "【请检查tag9F0D/tag9F0E/tag9F0F长度是否为5个字节】");
                    return;
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag8D是否符合规范，能否分解，长度是否符合规范
        /// </summary>
        public void ReadRecord_036()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("8D"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag8C。借贷记交易，卡片风险管理数据对象列表必须存在");
                return;
            }
            else
            {
                var tag8D = readRecordTags["8D"];
                var tls8D = DataParse.ParseTL(tag8D.Value);
                caseItem.Description += "【tag8D" + tag8D.Value + "】";
                if (tls8D.Count == 0)
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag8D无法TL分解]");
                    return;
                }
                foreach (var tl in tls8D)
                {
                    var terminalData = locator.Terminal.GetTag(tl.Tag);
                    if (string.IsNullOrEmpty(terminalData))
                    {
                        TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "终端数据[tag{0}]不存在，请联系开发人员添加该tag", tl.Tag);
                        return;
                    }
                    if (tl.Len * 2 != terminalData.Length)
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description + "【tag" + tl.Tag + "长度为" + tl.Len + "终端规范长度为:" + terminalData.Length / 2 + "】");
                        return;
                    }
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag8C是否符合规范，能否分解，长度是否符合规范
        /// </summary>
        public void ReadRecord_012()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("8C"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag8C。借贷记交易，卡片风险管理数据对象列表必须存在");
                return;
            }
            else
            {
                var tag8C = readRecordTags["8C"];
                var tls8C = DataParse.ParseTL(tag8C.Value);
                caseItem.Description += "【tag8C" + tag8C.Value + "】";
                if (tls8C.Count == 0)
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag8C无法TL分解]");
                    return;
                }
                foreach (var tl in tls8C)
                {
                    var terminalData = locator.Terminal.GetTag(tl.Tag);
                    if (string.IsNullOrEmpty(terminalData))
                    {
                        TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "终端数据[tag{0}]不存在，请联系开发人员添加该tag", tl.Tag);
                        return;
                    }
                    if (tl.Len * 2 != terminalData.Length)
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description + "【tag" + tl.Tag + "长度为" + tl.Len + "终端规范长度为:" + terminalData.Length / 2 + "】");
                        return;
                    }
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5A的规范性(格式、长度等)
        /// </summary>
        public void ReadRecord_013()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!readRecordTags.ContainsKey("5A"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag5A");
                return;
            }
            caseItem.Description += "【tag5A=" + readRecordTags["5A"].Value + "】";
            Tag5AHelper tag5AHelper = new Tag5AHelper(readRecordTags["5A"].Value);
            if (!tag5AHelper.IsCorrectFormat())
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5F24/5F25的规范性(必须存在，格式、长度、先后顺序等)
        /// </summary>
        public void ReadRecord_014()
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
                caseItem.Description += "【tag5F24=" + tag5F24.Value + "】";
                caseItem.Description += "【tag5F25=" + tag5F25.Value + "】";
                if (tag5F24.Len != 3 || tag5F25.Len != 3)    //校验长度
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[长度校验错误]");
                    return;
                }
                var month1 = Convert.ToInt16(tag5F24.Value.Substring(2, 2));
                var month2 = Convert.ToInt16(tag5F25.Value.Substring(2, 2));
                var day1 = Convert.ToInt16(tag5F24.Value.Substring(4, 2));
                var day2 = Convert.ToInt16(tag5F25.Value.Substring(4, 2));
                if (month1 < 1 || month1 > 12 || month2 < 1 || month2 > 12)  //月份检测
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[月份校验错误]");
                    return;
                }
                if (day1 < 1 || day1 > 31 || day2 < 1 || day2 > 31)  //天数
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
                if ((month1 == 2 && day1 > 28) || (month2 == 2 && day2 > 28)) //2月份检测
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description + "月份校验错误");
                    return;
                }
                var intTag5F24 = Convert.ToInt32(tag5F24.Value);
                var intTag5F25 = Convert.ToInt32(tag5F25.Value);
                if (intTag5F24 < intTag5F25) //关系检测
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description + "失效日期早于生效日期");
                    return;
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5F34的长度
        /// </summary>
        public void ReadRecord_015()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("5F34"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag5F34");
                return;
            }
            caseItem.Description += "【tag5F34=" + readRecordTags["5F34"].Value + "】";
            if (readRecordTags["5F34"].Len != 1)
            {
                
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag9F4A值是否为82
        /// </summary>
        public void ReadRecord_016()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F4A"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag9F4A");
                return;
            }
            caseItem.Description += "【tag5F34=" + readRecordTags["9F4A"].Value + "】";
            if (readRecordTags["9F4A"].Value != "82")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag9F49值是否为9F3704
        /// </summary>
        public void ReadRecord_017()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F49"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag9F49");
                return;
            }
            caseItem.Description += "【tag9F49=" + readRecordTags["9F49"].Value + "】";
            if (readRecordTags["9F49"].Value != "9F3704")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag9F47/9F32值是否为03或010001
        /// </summary>
        public void ReadRecord_018()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F47") ||
                !readRecordTags.ContainsKey("9F32"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag9F47/tag9F32");
                return;
            }
            caseItem.Description += "【tag9F47=" + readRecordTags["9F47"].Value + "】";
            caseItem.Description += "【tag9F32=" + readRecordTags["9F32"].Value + "】";
            if ((readRecordTags["9F47"].Value != "03" && readRecordTags["9F47"].Value != "010001") ||
                (readRecordTags["9F32"].Value != "03" && readRecordTags["9F32"].Value != "010001"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag9F1F(转换为字符串)
        /// </summary>
        public void ReadRecord_019()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("9F1F"))
            {
                var tag9F1F = readRecordTags["9F1F"];
                var value = UtilLib.Utils.BcdToStr(tag9F1F.Value);
                caseItem.Description += "【tag9F1F=" + tag9F1F.Value + "】转为字符串显示:" + value;
                if (!CaseUtil.IsAplhaDigit(value))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
            }
            else
            {
                TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "[读数据中缺少tag9F1F]");
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag8F长度等于1
        /// </summary>
        public void ReadRecord_020()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("8F"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag8F");
                return;
            }
            var tag8F = readRecordTags["8F"];
            caseItem.Description += "【tag8F=" + tag8F.Value + "】";
            if (tag8F.Len != 1)
            {
                TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "[长度有误，规范长度为1字节]");
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5F28的规范性
        /// </summary>
        public void ReadRecord_021()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("5F28"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag5F28，终端将无法做应用用途控制检查");
                return;
            }
            var tag5F28 = readRecordTags["5F28"];
            caseItem.Description += "【tag5F28=" + tag5F28.Value + "】";
            if (tag5F28.Len != 2)
            {
                TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "长度有误，规范长度为2字节");
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag14的规范性
        /// </summary>
        public void ReadRecord_022()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F14"))
            {
                TraceInfo(TipLevel.Warn, caseNo, "读记录缺少tag9F14");
                return;
            }
            var tag9F14 = readRecordTags["9F14"];
            caseItem.Description += "【tag9F14=" + tag9F14.Value + "】";
            if (tag9F14.Len != 1)
            {
                TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "数据长度有误，规范长度为1字节");
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag23的规范性
        /// </summary>
        public void ReadRecord_023()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F23"))
            {
                TraceInfo(TipLevel.Warn, caseNo, "读记录缺少tag9F23");
                return;
            }
            var tag9F23 = readRecordTags["9F23"];
            caseItem.Description += "【tag9F23=" + tag9F23.Value + "】";
            if (tag9F23.Len != 1)
            {
                TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "长度有误，规范长度为1字节");
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag9F08的规范性
        /// </summary>
        public void ReadRecord_024()
        {
            var transCfg = TransactionConfig.GetInstance();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (!readRecordTags.ContainsKey("9F08"))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag9F08");
                return;
            }
            caseItem.Description += "【tag9F08=" + readRecordTags["9F08"].Value + "】";
            if (transCfg.CurrentApp == AppType.UICS)
            {
                if (readRecordTags["9F08"].Value != "0030")
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
            }
            else if (transCfg.CurrentApp == AppType.VISA)
            {
                if (readRecordTags["9F08"].Value != "00A0")
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
            }
            else
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[未知的应用类型]");
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag9F69,9F79，不能出现在读记录中
        /// </summary>
        public void ReadRecord_025()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("9F69") || readRecordTags.ContainsKey("9F79"))
            {
                caseItem.Description += "【tag9F69=" + readRecordTags["9F69"].Value + "】";
                caseItem.Description += "【tag9F79=" + readRecordTags["9F79"].Value + "】";
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5F30中的服务码与57中的一致性
        /// </summary>
        public void ReadRecord_026()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!readRecordTags.ContainsKey("5F30") || !readRecordTags.ContainsKey("57"))
            {
                TraceInfo(caseItem.Level, caseNo, "读数据中缺少tag5F30或者tag57");
                return;
            }
            Tag57Helper tag57Helper = new Tag57Helper(readRecordTags["57"].Value);
            caseItem.Description += "【tag5F30=" + readRecordTags["5F30"].Value + "】";
            if (readRecordTags["5F30"].Value != tag57Helper.GetServiceCode())
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[57中服务码为{0}]", tag57Helper.GetServiceCode());
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5F24与57中的失效期的年月一致性
        /// </summary>
        public void ReadRecord_027()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag57 = TransactionTag.GetInstance().GetTag("57");
            if (!readRecordTags.ContainsKey("5F24") || string.IsNullOrEmpty(tag57))
            {
                TraceInfo(caseItem.Level, caseNo, "读数据中缺少tag5F24或者tag57");
                return;
            }
            Tag57Helper tag57Helper = new Tag57Helper(tag57);
            caseItem.Description += "【tag5F24=" + readRecordTags["5F24"].Value + "】";
            if (readRecordTags["5F24"].Value.Substring(0, 4) != tag57Helper.GetExpiredDate())
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag57中的失效日期为:{0}]", tag57Helper.GetExpiredDate());
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5A与57中卡号的一致性
        /// </summary>
        public void ReadRecord_028()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);


            var tag57 = TransactionTag.GetInstance().GetTag("57");

            if (!readRecordTags.ContainsKey("5A") || string.IsNullOrEmpty(tag57))
            {
                TraceInfo(caseItem.Level, caseNo, "读数据中缺少tag5A或者tag57");
                return;
            }
            Tag57Helper tag57Helper = new Tag57Helper(tag57);
            Tag5AHelper tag5AHelper = new Tag5AHelper(readRecordTags["5A"].Value);
            caseItem.Description += "【5A=" + readRecordTags["5A"].Value + "】";
            if (tag5AHelper.GetAccount() != tag57Helper.GetAccount())
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "tag57账号为:{0}", tag57Helper.GetAccount());
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag9F42应用货币代码，如果CVM中要求金额检查(X,Y金额不为零)，该数据必须存在
        /// </summary>
        public void ReadRecord_035()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag9F42 = TransactionTag.GetInstance().GetTag("9F42");
            if (!string.IsNullOrEmpty(tag9F42))
            {
                caseItem.Description += "【tag9F42=" + tag9F42 + "】";
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                return;
            }
            var tag8E = TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "8E");
            if (!string.IsNullOrEmpty(tag8E))
            {
                if (tag8E.Substring(0, 16) == "0000000000000000")
                {
                    TraceInfo(TipLevel.Warn, caseNo, caseItem.Description);
                    return;
                }
                else
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[XY金额为:{0}],缺少tag9F42", tag8E.Substring(0, 16));
                    return;
                }
            } 
        }

        #endregion
        #region UICS
        /// <summary>
        /// 检测tag5F30的规范性(借记/贷记使用的服务码需在规范范围内)
        /// </summary>
        public void ReadRecord_007()
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
                if (tag5F30.Len != 2)
                {
                    TraceInfo(Config.TipLevel.Failed, caseNo, "tag5F30服务码长度错误");
                    return;
                }
                if (tag5F30.Value.Substring(0, 2) != "02" && tag5F30.Value.Substring(0, 2) != "06")
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
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
                        TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                    else
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                }
                else if (aid == Constant.DebitAid)
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
        /// 检测tag9F62证件类型的规范性(取值范围、长度等)
        /// </summary>
        public void ReadRecord_008()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("9F62"))
            {
                var tag9F62 = readRecordTags["9F62"];
                if (tag9F62.Len != 1)
                {
                    TraceInfo(TipLevel.Failed, caseNo, "tag9F62证件类型长度错误,长度必须为固定1字节");
                    return;
                }
                var standardTag = new List<string> { "00", "01", "02", "03", "04", "05" };
                if (!standardTag.Contains(tag9F62.Value))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[取值范围不符合规范]");
                    return;
                }
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测tag9F61证件号的规范性(转字符串、证件类型对应的证件长度、规范长度等)
        /// </summary>
        public void ReadRecord_009()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if ((!readRecordTags.ContainsKey("9F61") && readRecordTags.ContainsKey("9F62")) ||
                readRecordTags.ContainsKey("9F61") && !readRecordTags.ContainsKey("9F62"))
            {
                TraceInfo(caseItem.Level, caseNo, "检测Tag9F61和Tag9F62是否同时存在，必须同时存在");
                return;
            }
            if (readRecordTags.ContainsKey("9F61"))
            {
                var tag9F61 = readRecordTags["9F61"];
                var value = UtilLib.Utils.BcdToStr(tag9F61.Value);
                if (!CaseUtil.IsAplhaDigit(value))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }

                var tag9F62 = readRecordTags["9F62"];
                if (tag9F62.Value == "00" && tag9F61.Value.Length != 18)
                {
                    TraceInfo(TipLevel.Failed, caseNo, "如果是身份证类型，则tag9F61应为18字节");
                    return;
                }
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测tag9F63长度是否等于16字节
        /// </summary>
        public void ReadRecord_010()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (readRecordTags.ContainsKey("9F63"))
            {
                var tag9F63 = readRecordTags["9F63"];
                if (tag9F63.Len != 16)
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }
        #endregion

        /// <summary>
        /// 检测QPBOC/QUICS交易中，tag9F74是否正确，且作为最后一个读记录命令中返回
        /// </summary>
        public void ReadRecord_031()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F66 = locator.Terminal.GetTag("9F66");
            if (tag9F66.Length != 8)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "结果: tag9F66为空或长度不对");
                return;
            }
            int lastByte = Convert.ToInt16(tag9F66.Substring(6), 16);

            int count = resps.Count;
            var tlvs = DataParse.ParseTLV(resps[count - 1].Response);
            if ((lastByte & 0x80) == 0x80 && tlvs.Count != 3)   //支持fDDA01
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "结果:fDDA01版本，卡片最后一条记录只能返回tag9F69和9F74");
                return;
            }
            if ((lastByte & 0x80) != 0x80 && tlvs.Count != 2)   //支持fDDA00
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "结果:fDDA00版本，卡片最后一条记录只能返回tag9F74");
                return;
            }

            foreach (var tl in tlvs)
            {
                if (tl.Tag == "9F74" && tl.Value == "454343303031")
                {
                    TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    return;
                }
            }

            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测fDDA01版本，卡片最后一个读记录应返回tag9F69
        /// </summary>
        public void ReadRecord_032()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F66 = locator.Terminal.GetTag("9F66");
            if(tag9F66.Length != 8)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "结果: tag9F66为空或长度不对");
                return;
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
                        TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                        return;
                    }
                }
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
        }

        /// <summary>
        /// 检测fDDA01下tag9F69与DF61和9F6C的一致性
        /// </summary>
        public void ReadRecord_033()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F66 = locator.Terminal.GetTag("9F66");
            if (tag9F66.Length != 8)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "结果: tag9F66为空或长度不对");
                return;
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
                            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                            return;
                        }
                        TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                        return;
                    }
                }
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
        }

        /// <summary>
        /// 检测fDDA00版本下，卡片不应该返回tag9F69
        /// </summary>
        public void ReadRecord_034()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F66 = locator.Terminal.GetTag("9F66");
            if (tag9F66.Length != 8)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "结果: tag9F66为空或长度不对");
                return;
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
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        return;
                    }
                }
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                return;
            }
        }
    }
}

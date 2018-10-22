﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using CardPlatform.Common;
using CardPlatform.Business;
using CardPlatform.Config;
using CplusplusDll;
using CardPlatform.ViewModel;
using CardPlatform.Models;

namespace CardPlatform.Cases
{
    public class SelectAppCase : CaseBase
    {
        private ApduResponse response;
        private List<TLV> tlvs;

        public SelectAppCase()
        {
            response = new ApduResponse();
            tlvs = new List<TLV>();
        }

        public override void Excute(int batchNo, AppType app, TransactionStep step, object srcData)
        {
            response = (ApduResponse)srcData;
            tlvs = DataParse.ParseTLV(response.Response);
            base.Excute(batchNo, app, step,srcData);
        }
        #region 通用case

        /// <summary>
        /// 检测响应数据是否以6F模板开头
        /// </summary>
        public TipLevel SelectAid_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            caseItem.Description += "响应数据:【" + response.Response + "】";
            log.TraceLog("响应数据为:【{0}】", response.Response);
            if (!CaseUtil.RespStartWith(response.Response, "6F"))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测6F模板下包含且只包含Tag84和A5模板，顺序不能颠倒
        /// </summary>
        public TipLevel SelectAid_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var template6F = new List<TLV>();
            foreach (var item in tlvs)
            {
                if (item.Level == 1)
                {
                    template6F.Add(item);
                }
            }
            caseItem.Description += "【A5模板依次包含以下tag及模板";
            string tags = "";
            foreach(var subTag in template6F)
            {
                caseItem.Description += "[" + subTag.Tag + "]";
                tags += "【" + subTag.Tag + "】";
            }
            log.TraceLog("A5模板依次包含以下tag及模板" + tags);
            if (template6F.Count != 2 || template6F[0].Tag != "84" || template6F[1].Tag != "A5")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 84是否存在且长度是否正确且在5～16字节之间
        /// </summary>
        public TipLevel SelectAid_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag84 = tlvs.FirstOrDefault(tlv => tlv.Tag == "84");
            if (tag84 != null)
            {
                caseItem.Description += "【tag84=" + tag84.Value + "】";
                string len = UtilLib.Utils.GetBcdLen(tag84.Value);
                log.TraceLog("tag84 = 【" + tag84.Value + "】" + "长度为: 【" + len + "】");
                if (CaseUtil.IsExpectedLen(tag84.Value, 10, 32))
                {
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
                else
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            else
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag84不存在]");
            }
        }

        /// <summary>
        /// 检测tag50是否符合规范(必须存在于A5模板下，长度1-16字节，ASCII字符)
        /// </summary>
        public TipLevel SelectAid_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var subTags = CaseUtil.GetSubTags("A5", tlvs);
            bool hasTag50 = false;
            foreach (var item in subTags)
            {
                if (item.Tag == "50")
                {
                    caseItem.Description += "【tag50=" + item.Value + "】";
                    hasTag50 = true;
                    log.TraceLog("tag50=【" + item.Value + "】");
                    log.TraceLog("长度为: 【{0}】", item.Len);
                    if (item.Len < 1 || item.Len > 16)
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "【tag50长度要求在1-16字节之间】");
                    }
                    string value = UtilLib.Utils.BcdToStr(item.Value);
                    log.TraceLog("转码为: 【{0}】", value);
                    if (!CaseUtil.IsAlpha(value))
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag50无法转换为合法的ANSII字符]");
                    }
                    caseItem.Description += "字符串显示为:" + value;
                }
            }
            if (!hasTag50)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag50应用标签不存在(Visa规范此Tag必须存在)]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// A5模板下可包含的tag包括87，9F38,5F2D,9F11，9F12，BF0C
        /// </summary>
        public TipLevel SelectAid_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var items = new List<string> { "50", "87", "9F38", "5F2D", "9F11", "9F12", "BF0C" };
            var subTlvs = CaseUtil.GetSubTags("A5", tlvs);

            caseItem.Description += "【tagA5依次包含以下值:";
            string tags = "";
            foreach(var tlv in subTlvs)
            {
                caseItem.Description += "[" + tlv.Tag + "]";
                tags += "【" + tlv.Tag + "】";
            }
            caseItem.Description += "】";
            log.TraceLog("tagA5依次包含以下值:" + tags);
            foreach (var tlv in subTlvs)
            {
                if (!items.Contains(tlv.Tag))
                {
                    log.TraceLog("包含其他不符合规定Tag:{0}", tlv.Tag);
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测87的合规性(长度是否为1字节)
        /// </summary>
        public TipLevel SelectAid_006()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var subTags = CaseUtil.GetSubTags("A5", tlvs);

            var tag87 = subTags.FirstOrDefault(tlv => tlv.Tag == "87");
            if (tag87 != null)
            {
                caseItem.Description += "tag87=【" + tag87.Value + "】";
                log.TraceLog("tag87=【" + tag87.Value + "】");
                log.TraceLog("长度为:【{0}】", tag87.Len);
                if (tag87.Value.Length == 2)
                {
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
                else
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            else
            {
                //这里应判断若卡片包含多个应用，87必须存在
            }
            return TipLevel.Unknown;
        }

        /// <summary>
        /// 检测5F2D是否符合规范(长度必须是2字节的倍数，2～8字节之间,能转可读小写字符串)
        /// </summary>
        public TipLevel SelectAid_007()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var subTags = CaseUtil.GetSubTags("A5", tlvs);
            var tag5F2D = subTags.FirstOrDefault(tlv => tlv.Tag == "5F2D");
            if (tag5F2D == null)
            {
                return TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "[tag5F2D不存在]");
            }
            string value = UtilLib.Utils.BcdToStr(tag5F2D.Value);
            caseItem.Description += "【tag5F2D=" + tag5F2D.Value + "】字符串显示:" + value;
            log.TraceLog("tag5F2D:【{0}】", tag5F2D.Value);
            log.TraceLog("转码为:【{0}】", value);
            log.TraceLog("长度为:【{0}】", tag5F2D.Len);
            if (tag5F2D.Value.Length % 2 == 0 &&
                tag5F2D.Value.Length >= 4 &&
                tag5F2D.Value.Length <= 16 &&
                CaseUtil.IsLowerCaseAlpha(value))
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            else
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 9F11的长度必须是1字节，在A5模板下值在01～10之间,如果tag9F12存在，则tag9F11必须存在
        /// </summary>
        public TipLevel SelectAid_008()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var subTlvs = CaseUtil.GetSubTags("A5", tlvs);
            var tag9F12 = subTlvs.FirstOrDefault(tlv => tlv.Tag == "9F12");
            var tag9F11 = subTlvs.FirstOrDefault(tlv => tlv.Tag == "9F11");
            if (tag9F12 != null && tag9F11 == null) //如果tag9F12存在，则tag9F11必须存在
            {
                log.TraceLog("tag9F12=" + tag9F12.Value);
                caseItem.Description += "【如果tag9F12存在，则tag9F11必须存在】";
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            if (tag9F11 == null)
            {
                log.TraceLog("tag9F11不存在");
                return TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "[tag9F11不存在]");
            }
            caseItem.Description += "【tag9F11=" + tag9F11.Value + "】";
            log.TraceLog("tag9F11=【" + tag9F11.Value + "】");
            log.TraceLog("长度为:【{0}】", tag9F11.Len);
            var tag9F11Value = Convert.ToInt16(tag9F11.Value, 16);
            log.TraceLog("值为:【{0}】", tag9F11Value);
            if (tag9F11.Len == 1 &&
                tag9F11Value >= 1 &&
                tag9F11Value <= 10)
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            else
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 9F12在A5模板下长度必须在1～16之间,Tag9F12转成BCD显示
        /// </summary>
        public TipLevel SelectAid_009()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var subTags = CaseUtil.GetSubTags("A5", tlvs);
            var tag9F12 = subTags.FirstOrDefault(tlv => tlv.Tag == "9F12");
            if(tag9F12 == null)
            {
                log.TraceLog("tag9F12不存在");
                return TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "[tag9F12不存在]");
            }
            string value = UtilLib.Utils.BcdToStr(tag9F12.Value);
            caseItem.Description += "【tag9F12=" + tag9F12.Value + "】转为字符串显示:" + value;
            log.TraceLog("tag9F12:【{0}】", tag9F12.Value);
            log.TraceLog("转码为:【{0}】", value);
            log.TraceLog("长度为:【{0}】", tag9F12.Len);
            if (tag9F12.Value.Length >= 2 &&
                tag9F12.Value.Length <= 32 &&
                CaseUtil.IsAlpha(value))
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            else
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测Select Application响应数据tag的唯一性
        /// </summary>
        public TipLevel SelectAid_012()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var dupTags = new List<string>();
            var tlvItems = DataParse.ParseTLV(response.Response);
            string tags = "";
            foreach(var tlv in tlvItems)
            {
                tags += "【" + tlv.Tag + "】";
            }
            log.TraceLog("FCI响应数据Tag依次为:{0}", tags);
            if (CaseUtil.IsUniqTag(response.Response, out dupTags))
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            else
            {
                caseItem.Description += "【包含重复的tag";
                foreach (var dupTag in dupTags)
                {
                    caseItem.Description += "[" + dupTag + "]";
                }
                caseItem.Description += "】";
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测tag9F38是否存在,是否能正常分解,长度是否符合规范
        /// </summary>
        public TipLevel SelectAid_013()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag9F38 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectApp, "9F38");
            if (string.IsNullOrEmpty(tag9F38))
            {
                return TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "缺少tag9F38");
            }
            var tls = DataParse.ParseTL(tag9F38);
            caseItem.Description += "【tag9F38=" + tag9F38 + "】";
            if (tls.Count == 0)  //分解不正确
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "无法分解tag9F38");
            }
            string tlList = "";
            foreach(var tl in tls)
            {
                tlList += "【tag" + tl.Tag + " 长度为:" + tl.Len + "】";
            }
            log.TraceLog("tag9F38分解为:{0}", tlList);
            foreach (var tl in tls)
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
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        #endregion
        #region UICS 应用
        /// <summary>
        /// 检测BF0C模板下是否包含9F4D借贷记交易日志入口
        /// </summary>
        public TipLevel SelectAid_010()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tags = CaseUtil.GetSubTags("BF0C", tlvs);
            var tag9F4D = tags.FirstOrDefault(tag => tag.Tag == "9F4D");
            if(tag9F4D != null)
            {
                caseItem.Description += "【tag9F4D=" + tag9F4D.Value + "】";
                log.TraceLog("tag9F4D=【" + tag9F4D.Value + "】");
                log.TraceLog("长度为:【{0}】", tag9F4D.Len);
                if (tag9F4D.Len == 2 && tag9F4D.Value == "0B0A")
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                else
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                return TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "[缺少tag9F4D, 没有记录交易日志入口]");
            }
        }

        /// <summary>
        /// 检测BF0C模板下是否包含DF4D电子现金交易日志入口
        /// </summary>
        public TipLevel SelectAid_011()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tags = CaseUtil.GetSubTags("BF0C", tlvs);
            var tagDF4D = tags.FirstOrDefault(tag => tag.Tag == "DF4D");
            if (tagDF4D != null)
            {
                caseItem.Description += "【tagDF4D=" + tagDF4D.Value + "】";
                log.TraceLog("tagDF4D=【" + tagDF4D.Value + "】");
                log.TraceLog("长度为:【{0}】", tagDF4D.Len);
                if (tagDF4D.Len == 2 && tagDF4D.Value == "0C0A")
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                else
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                return TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "[缺少tagDF4D, 没有ECC/qUICS交易日志入口]");
            }
        }

        /// <summary>
        /// 检测国密交易下，tag9F38需包含DF69算法支持指示器，且长度为01
        /// </summary>
        public TipLevel SelectAid_015()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag9F38 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectApp, "9F38");
            var tls = DataParse.ParseTL(tag9F38);
            string tlList = "";
            foreach(var tl in tls)
            {
                tlList += "【" + tl.Tag + "】";
            }
            log.TraceLog("tag9F38包含如下tag{}", tlList);
            foreach (var tl in tls)
            {
                if (tl.Tag == "DF69" && tl.Len == 1)
                {
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
            }
            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        #endregion
        #region Visa 特有case
        /// <summary>
        /// 检测Select Application BF0C模板是否存在9F4D,Visa应用已废弃日志的使用
        /// </summary>
        public TipLevel SelectAid_021()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tags = CaseUtil.GetSubTags("BF0C", tlvs);
            var tag9F4D = tags.FirstOrDefault(tag => tag.Tag == "9F4D");
            if(tag9F4D == null)
            {
                log.TraceLog("tag9F4D=【null】");
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            log.TraceLog("tag9F4D=【" + tag9F4D.Value + "】");
            return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "【tag9F4D=" + tag9F4D.Value + "】");
        }

        /// <summary>
        /// 检测Select Application BF0C模板是否存在DF4D,Visa应用已废弃日志的使用
        /// </summary>
        public TipLevel SelectAid_022()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tags = CaseUtil.GetSubTags("BF0C", tlvs);
            var tagDF4D = tags.FirstOrDefault(tag => tag.Tag == "DF4D");
            if (tagDF4D == null)
            {
                log.TraceLog("tagDF4D=【null】");
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            log.TraceLog("tagDF4D=【" + tagDF4D.Value + "】");
            return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "【tagDF4D=" + tagDF4D.Value + "】");
        }
        #endregion
        #region 非接特有case
        /// <summary>
        /// 检测PPSE返回的tag50和tag87是否和选择AID应用时的FCI模板中的tag50和tag87一致
        /// </summary>
        public TipLevel SelectAid_017()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var ppseTag50 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectPPSE, "50");
            var ppseTag87 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectPPSE, "87");
            var tag50 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectApp, "50");
            var tag87 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectApp, "87");
            log.TraceLog("PPSE中Tag50为:【{0}】", ppseTag50);
            log.TraceLog("当前应用中Tag50为:【{0}】", tag50);
            log.TraceLog("PPSE中Tag87为:【{0}】", ppseTag87);
            log.TraceLog("当前应用中Tag50为:【{0}】", tag87);
            if (ppseTag50 != tag50 ||
                ppseTag87 != tag87)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测AID返回的FCI模板中tag84的值和PPSE FCI中的4F是否一致
        /// </summary>
        public TipLevel SelectAid_018()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var ppseTag4F = TransactionTag.GetInstance().GetTag(TransactionStep.SelectPPSE, "4F");
            var tag84 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectApp, "84");
            log.TraceLog("PPSE中tag4F为:【{0}】", ppseTag4F);
            log.TraceLog("当前应用中tag4F为:【{0}】", tag84);
            if (ppseTag4F != tag84)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F38中是否包含tag9F66(必须包含)
        /// </summary>
        public TipLevel SelectAid_020()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag9F38 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectApp, "9F38");
            if (string.IsNullOrEmpty(tag9F38))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "【tag9F38不存在】");
            }
            var tls = DataParse.ParseTL(tag9F38);
            string tlList = "";
            foreach(var tl in tlvs)
            {
                tlList += "【" + tl.Tag + "】";
            }
            log.TraceLog("tag9F38包括如下Tag{0}", tlList);
            foreach (var tl in tls)
            {
                if (tl.Tag == "9F66")
                {
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
            }
            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }
        #endregion
        #region q交易特有case
        #endregion
        #region qVSDC特有case
        /// <summary>
        /// 检测BF0C模板下tag9F5A的规范性
        /// </summary>
        public TipLevel SelectAid_019()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tags = CaseUtil.GetSubTags("BF0C", tlvs);
            var tag9F5A = tags.FirstOrDefault(tlv => tlv.Tag == "9F5A");
            if (tag9F5A == null)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "【tag9F5A不存在】");
            }

            caseItem.Description += "【tag9F5A=" + tag9F5A.Value + "】";
            if (tag9F5A.Len < 5 || tag9F5A.Len > 16)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            if (tag9F5A.Value.Substring(0, 2) != "40")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[亚洲地区区域代码必须是40，请确认该卡是否为亚洲地区发卡]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
        #endregion
        #region qPBOC特有case
        /// <summary>
        /// 检测采用密文版本01的QPBOC最基本的PDOL包含的标签是否存在(tag9F66/9F02/9F03/9F1A/95/5F2A/9A/9C/9F37)
        /// </summary>
        public TipLevel SelectAid_016()
        {
            List<Tuple<string, int>> tags = new List<Tuple<string, int>>
            {
                new Tuple<string, int>("9F66",4),
                new Tuple<string, int>("9F02",6),
                new Tuple<string, int>("9F03",6),
                new Tuple<string, int>("9F1A",2),
                new Tuple<string, int>("5F2A",2),
                new Tuple<string, int>("9A",3),
                new Tuple<string, int>("9C",1),
                new Tuple<string, int>("9F37",4),
                new Tuple<string, int>("95",5)
            };
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag9F38 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectApp, "9F38");
            var tls = DataParse.ParseTL(tag9F38);
            bool bFind = false;
            foreach (var tag in tags)
            {
                bFind = false;
                foreach (var tl in tls)
                {
                    if (tl.Tag == tag.Item1 && tl.Len == tag.Item2)
                    {
                        bFind = true;
                        break;
                    }
                }
                if (!bFind)
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
        #endregion
    }
}

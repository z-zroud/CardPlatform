using System;
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
            caseItem.Description += "[响应数据:" + response.Response.Substring(0,20) + "...]";
            log.TraceLog("响应数据为:{0}", response.Response);
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
        /// 检测选择应用响应数据6F模板下依次、仅包含Tag84和A5模板
        /// </summary>
        public TipLevel SelectAid_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var template6F = CaseUtil.GetSubTags("6F", tlvs);
            if (template6F.Count != 2 ||
                template6F[0].Tag != "84" ||
                template6F[1].Tag != "A5")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测选择应用响应数据tag6F下的tag84长度是否符合规范
        /// </summary>
        public TipLevel SelectAid_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag84 = CaseUtil.GetTag("84", tlvs);
            if(string.IsNullOrEmpty(tag84))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag84不存在]");
            }
            caseItem.Description += "tag84=" + tag84;
            log.TraceLog("tag84 = " + tag84 + "长度为: " + tag84.Length.ToString());
            if (!CaseUtil.IsExpectedLen(tag84, 5, 16))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测选择应用A5模板下的必备的tag50长度、转字符串是否规范
        /// </summary>
        public TipLevel SelectAid_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var templateA5 = CaseUtil.GetSubTags("A5", tlvs);
            var tag50 = CaseUtil.GetTag("50", templateA5);
            if(string.IsNullOrEmpty(tag50))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[A5模板下缺少必备的tag50]");
            }
            caseItem.Description += "[tag50=" + tag50 + "]";
            log.TraceLog("tag50=" + tag50);
            log.TraceLog("长度为: {0}", tag50.Length / 2);
            if (tag50.Length / 2 < 1 || tag50.Length / 2 > 16)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag50长度要求在1-16字节之间]");
            }
            string value = UtilLib.Utils.BcdToStr(tag50);
            log.TraceLog("转码为: {0}", value);
            if (!CaseUtil.IsAlphaAndBlank(value))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag50无法转换为合法的ANSII字符,卡片中值为{0}]",value);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description + "字符串显示为:{0}" , value);
        }

        /// <summary>
        /// A5模板下可包含的tag包括87，9F38,5F2D,9F11，9F12，BF0C
        /// </summary>
        public TipLevel SelectAid_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var items = new List<string> { "50", "87", "9F38", "5F2D", "9F11", "9F12", "BF0C" };
            var templateA5 = CaseUtil.GetSubTags("A5", tlvs);

            caseItem.Description += "tagA5依次包含以下值:";
            string tags = "";
            foreach(var tlv in templateA5)
            {
                caseItem.Description += "[" + tlv.Tag + "]";
                tags += "[" + tlv.Tag + "]";
            }
            caseItem.Description += "]";
            log.TraceLog("tagA5依次包含以下值:" + tags);
            foreach (var tlv in templateA5)
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
        /// 检测选择应用A5模板下的tag84在多个应用下是否存在、长度是否为1字节
        /// </summary>
        public TipLevel SelectAid_006()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var templateA5 = CaseUtil.GetSubTags("A5", tlvs);

            var transConfig = TransactionConfig.GetInstance();
            if (transConfig.Aids.Count > 1)
            {
                if(!CaseUtil.HasTag("87",templateA5))
                {
                    return TraceInfo(caseItem.Level, caseNo, "如果存在多个应用，tag87必须存在");
                }
            }
            var tag87 = CaseUtil.GetTag("87", templateA5);
            if(!string.IsNullOrEmpty(tag87))
            {
                caseItem.Description += "tag87=" + tag87;
                log.TraceLog("tag87=" + tag87);
                log.TraceLog("长度为:{0}", tag87.Length);
                if (tag87.Length != 2)
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测选择应用A5模板下5F2D长度为2～8字节之间,能转英文小写字符串
        /// </summary>
        public TipLevel SelectAid_007()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var templateA5 = CaseUtil.GetSubTags("A5", tlvs);
            var tag5F2D = CaseUtil.GetTag("5F2D", templateA5);
            if (string.IsNullOrEmpty(tag5F2D))
            {
                return TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "[tag5F2D不存在,建议该tag写入卡中]");
            }
            string value = UtilLib.Utils.BcdToStr(tag5F2D);
            caseItem.Description += "tag5F2D=" + tag5F2D + "字符串显示:" + value;
            log.TraceLog("tag5F2D:{0}", tag5F2D);
            log.TraceLog("转码为:{0}", value);
            log.TraceLog("长度为:{0}", tag5F2D.Length / 2);
            if (tag5F2D.Length % 2 == 0 &&
                tag5F2D.Length / 2 >= 2 &&
                tag5F2D.Length / 2 <= 8 &&
                CaseUtil.IsLowerCaseAlpha(value))
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测选择应用A5模板下9F11是否符合规范，如果tag9F12存在，那么tag9F11必须存在
        /// </summary>
        public TipLevel SelectAid_008()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var templateA5 = CaseUtil.GetSubTags("A5", tlvs);
            var tag9F12 = CaseUtil.GetTag("9F12", templateA5);
            var tag9F11 = CaseUtil.GetTag("9F11", templateA5);
            if (!string.IsNullOrEmpty(tag9F12) && string.IsNullOrEmpty(tag9F11)) //如果tag9F12存在，则tag9F11必须存在
            {
                log.TraceLog("tag9F12=" + tag9F12);
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag9F12存在，但tag9F11不存在]");
            }
            if (string.IsNullOrEmpty(tag9F11))
            {
                log.TraceLog("tag9F11不存在");
                return TraceInfo(TipLevel.Tip, caseNo, caseItem.Description + "[tag9F11不存在]");
            }
            caseItem.Description += "tag9F11=" + tag9F11;
            log.TraceLog("tag9F11=" + tag9F11);
            log.TraceLog("长度为:{0}", tag9F11.Length / 2);
            var value = Convert.ToInt16(tag9F11, 16);
            log.TraceLog("值为:{0}", value);
            if (tag9F11.Length / 2 == 1 && value >= 1 && value <= 0x10)
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测选择应用A5模板下9F12长度为1～16之间,ASCII字符
        /// </summary>
        public TipLevel SelectAid_009()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var templateA5 = CaseUtil.GetSubTags("A5", tlvs);
            var tag9F12 = CaseUtil.GetTag("9F12", templateA5);
            if(string.IsNullOrEmpty(tag9F12))
            {
                log.TraceLog("tag9F12不存在");
                return TraceInfo(TipLevel.Tip, caseNo, caseItem.Description + "[tag9F12不存在]");
            }
            string value = UtilLib.Utils.BcdToStr(tag9F12);

            string tag50 = CaseUtil.GetTag("50", templateA5);
            if(tag50 != tag9F12)
            {
                TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "[推荐tag50和tag9F12值一致]");
            }
            caseItem.Description += "[tag9F12=" + tag9F12 + "[转为字符串显示:" + value;
            log.TraceLog("tag9F12:{0}", tag9F12);
            log.TraceLog("转码为:{0}", value);
            log.TraceLog("长度为:{0}", tag9F12.Length / 2);
            if (tag9F12.Length / 2 >= 1 &&
                tag9F12.Length / 2 <= 16 &&
                CaseUtil.IsAlphaAndBlank(value))
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测选择应用响应数据是否包含有重复的tag
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
                tags += "[" + tlv.Tag + "]";
            }
            log.TraceLog("FCI响应数据Tag依次为:{0}", tags);
            if (CaseUtil.IsUniqTag(response.Response, out dupTags))
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            else
            {
                caseItem.Description += "[包含重复的tag";
                foreach (var dupTag in dupTags)
                {
                    caseItem.Description += "[" + dupTag + "]";
                }
                caseItem.Description += "[";
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测选择应用响应数据tag9F38是否存在,是否能正常分解,分解的tag长度是否符合规范
        /// </summary>
        public TipLevel SelectAid_013()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag9F38 = CaseUtil.GetTag("9F38",tlvs);
            if (string.IsNullOrEmpty(tag9F38))
            {
                if(transConfig.CurrentApp == AppType.qPBOC ||
                    transConfig.CurrentApp == AppType.qUICS ||
                    transConfig.CurrentApp == AppType.qVSDC_offline ||
                    transConfig.CurrentApp == AppType.qVSDC_online ||
                    transConfig.CurrentApp == AppType.Paypass)
                {
                    return TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "[非接触式交易条件下tag9F38必备]");
                }
                else
                {
                    return TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "[tag9F38不存在]");
                }
                
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
                tlList += "tag" + tl.Tag + " 长度为:" + tl.Len;
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
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "tag" + tl.Tag + "长度为" + tl.Len + "终端规范长度为:" + terminalData.Length / 2);
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
                caseItem.Description += "tag9F4D=" + tag9F4D.Value + "";
                log.TraceLog("tag9F4D=" + tag9F4D.Value + "");
                log.TraceLog("长度为:{0}", tag9F4D.Len);
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
            log.TraceLog("tag9F4D=" + tag9F4D.Value);
            return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "tag9F4D=" + tag9F4D.Value);
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
            log.TraceLog("tagDF4D=" + tagDF4D.Value);
            return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "【tagDF4D=" + tagDF4D.Value + "】");
        }

        /// <summary>
        /// 检测选择应用BF0C模板下是否仅包含可选的tag9F0A,5F55,9F5A，其中visa应用应包含9F5A
        /// </summary>
        /// <returns></returns>
        public TipLevel SelectAid_023()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var items = new List<string> { "9F0A","5F55","9F5A"};
            var templateBF0C = CaseUtil.GetSubTags("BF0C", tlvs);

            caseItem.Description += "模板BF0C依次包含以下值:";
            string tags = "";
            foreach (var tlv in templateBF0C)
            {
                caseItem.Description += "[" + tlv.Tag + "]";
                tags += "[" + tlv.Tag + "]";
            }
            caseItem.Description += "]";
            log.TraceLog("模板BF0C依次包含以下值:" + tags);
            if(transConfig.CurrentApp == AppType.qVSDC_offline ||
                transConfig.CurrentApp == AppType.qVSDC_online)
            {
                if (!CaseUtil.HasTag("9F5A", templateBF0C))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[visa应用下，选择应用的BF0C模板必须包含9F5A]");
                }
            }
            
            foreach (var tlv in templateBF0C)
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
        /// 检测tag9F5A的长度是否为5字节,是否为亚洲区域发卡
        /// </summary>
        /// <returns></returns>
        public TipLevel SelectAid_024()
        {
            if(transConfig.CurrentApp == AppType.qVSDC_offline ||
                transConfig.CurrentApp == AppType.qVSDC_online ||
                transConfig.CurrentApp == AppType.qVSDC_online_without_ODA)
            {
                var caseNo = MethodBase.GetCurrentMethod().Name;
                var caseItem = GetCaseItem(caseNo);

                var templateBF0C = CaseUtil.GetSubTags("BF0C", tlvs);
                var tag9F5A = templateBF0C.FirstOrDefault(tlv => tlv.Tag == "9F5A");
                if (tag9F5A == null)
                {
                    return TraceInfo(TipLevel.Failed, caseNo, "BF0C模板缺少tag9F5A,VCPS2.1及以上版本必须个人化该tag");
                }
                if (tag9F5A.Len != 5)
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
                if (tag9F5A.Value.Substring(0, 2) != "40")
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[亚洲地区区域代码必须是40，请确认该卡是否为亚洲地区发卡]");
                }
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            return TipLevel.Sucess;
        }

        /// <summary>
        /// 检测visa VSDC tag5F55长度是否为2字节，并能转BCD显示
        /// </summary>
        /// <returns></returns>
        public TipLevel SelectAid_025()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var templateBF0C = CaseUtil.GetSubTags("BF0C", tlvs);
            var tag5F55 = templateBF0C.FirstOrDefault(tlv => tlv.Tag == "5F55");
            if (tag5F55 == null)
            {
                return TraceInfo(TipLevel.Warn, caseNo, "BF0C模板缺少tag5F55");
            }
            if (tag5F55.Len != 2)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            string value = UtilLib.Utils.BcdToStr(tag5F55.Value);
            if(!CaseUtil.IsAlpha(value))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[不识别的ASCII码]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description + "[tag5F55值为:{0}]",value);
        }

        /// <summary>
        /// 检测AID FCI中的tag9F11和PSE FCI中的9F11是否相同
        /// </summary>
        /// <returns></returns>
        public TipLevel SelectAid_026()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F11 = CaseUtil.GetTag("9F11", tlvs);
            if(string.IsNullOrEmpty(tag9F11))
            {
                return TraceInfo(TipLevel.Warn, caseNo, "选择应用A5模板下缺少9F11");
            }
            var pse9F11 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectPSE, "9F11");
            if(string.IsNullOrEmpty(pse9F11))
            {
                return TraceInfo(TipLevel.Warn, caseNo, "PSE FCI中缺少9F11");
            }
            if(tag9F11 != pse9F11)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
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
            log.TraceLog("PPSE中Tag50为:{0}", ppseTag50);
            log.TraceLog("当前应用中Tag50为:{0}", tag50);
            log.TraceLog("PPSE中Tag87为:{0}", ppseTag87);
            log.TraceLog("当前应用中Tag50为:{0}", tag87);
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
            log.TraceLog("PPSE中tag4F为:[{0}]", ppseTag4F);
            log.TraceLog("当前应用中tag4F为:[{0}]", tag84);
            if (ppseTag4F != tag84)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F38中必须包含如下tags:9F66,9F02,9F03,9F1A,5F2A,9A,9C,9F37,95
        /// </summary>
        public TipLevel SelectAid_020()
        {
            List<Tuple<string, int>> tags = new List<Tuple<string, int>>
            {
                new Tuple<string, int>("9F66",4),
                new Tuple<string, int>("9F02",6),
                new Tuple<string, int>("9F03",6),
                new Tuple<string, int>("9F1A",2),
                new Tuple<string, int>("95",5),
                new Tuple<string, int>("5F2A",2),
                new Tuple<string, int>("9A",3),
                new Tuple<string, int>("9C",1),
                new Tuple<string, int>("9F37",4),
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
        #region q交易特有case
        #endregion
        #region qVSDC特有case

        #endregion
    }
}

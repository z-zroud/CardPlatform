﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using CardPlatform.Common;
using CardPlatform.Business;
using CardPlatform.Config;
using CplusplusDll;
using CardPlatform.ViewModel;

namespace CardPlatform.Cases
{
    public class SelectAppCase : CaseBase
    {
        private ApduResponse response;
        private List<TLV> TLVs;

        public SelectAppCase()
        {
            response = new ApduResponse();
            TLVs = new List<TLV>();
        }

        protected override void Load()
        {
            base.Load();
        }

        public override void Excute(int batchNo, TransactionApp app, TransactionStep step, object srcData)
        {
            response = (ApduResponse)srcData;
            TLVs = DataParse.ParseTLV(response.Response);
            base.Excute(batchNo, app, step,srcData);
        }

        /// <summary>
        /// 是否以6F开头
        /// </summary>
        public void SelectAid_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!CaseUtil.RespStartWith(response.Response,"6F"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 6F模板下只能并且包含Tag84和A5模板，顺序不能颠倒
        /// </summary>
        public void SelectAid_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var template6F = new List<TLV>();
            foreach (var item in TLVs)
            {
                if (item.Level == 1)
                {
                    template6F.Add(item);
                }
            }
            if (template6F.Count != 2 ||
                template6F[0].Tag != "84" ||
                template6F[1].Tag != "A5")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 84是否存在且长度是否正确且在5～16字节之间
        /// </summary>
        public void SelectAid_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag84 = from tlv in TLVs where tlv.Tag == "84" select tlv;
            if (tag84.First() != null)
            {
                if (CaseUtil.IsExpectedLen(tag84.First().Value, 10, 32))
                {
                    TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
                else
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            else
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// A5模板下必须包含tag50
        /// </summary>
        public void SelectAid_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            bool hasTag50 = false;
            bool isCorrectLen = false;
            foreach (var item in TLVs)
            {
                if (item.Tag == "50")
                {
                    hasTag50 = true;
                    if (item.Len >= 1 && item.Len <= 16)
                    {
                        isCorrectLen = true;
                    }
                    else
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        return;
                    }
                }
            }
            if (hasTag50 && isCorrectLen)
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// A5模板下可包含的tag包括87，9F38,5F2D,9F11，9F12，BF0C
        /// </summary>
        public void SelectAid_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var items = new List<string>() { "50", "87", "9F38", "5F2D", "9F11", "9F12", "BF0C" };
            var tlvs = CaseUtil.GetSubTags("A5", TLVs);

            bool hasOtherTag = false;
            foreach(var tlv in tlvs)
            {
                if(!items.Contains(tlv.Tag))
                {
                    hasOtherTag = true;
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    break;
                }
            }
            if (!hasOtherTag)
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }



        /// <summary>
        /// 87的长度是否为1字节
        /// </summary>
        public void SelectAid_006()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag87 = from tlv in TLVs where tlv.Tag == "87" select tlv;
            if (tag87.First() != null)
            {
                if (tag87.First().Value.Length == 2)
                {
                    TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
                else
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }

        /// <summary>
        /// 检测5F2D是否符合规范(长度必须是2字节的倍数，2～8字节之间,能转可读字符串)
        /// </summary>
        public void SelectAid_007()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in TLVs)
            {
                if (item.Tag == "5F2D")
                {
                    string value = UtilLib.Utils.BcdToStr(item.Value);
                    if (item.Len % 2 == 0 &&
                        item.Len >= 2 &&
                        item.Len <= 8 &&
                        CaseUtil.IsAlpha(value))
                    {
                        TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                    else
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                    break;
                }
            }
        }

        /// <summary>
        /// 9F11的长度必须是1字节，值在01～10之间
        /// </summary>
        public void SelectAid_008()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in TLVs)
            {
                if (item.Tag == "9F11")
                {
                    var tag9F11Value = Convert.ToInt16(item.Value, 16);
                    if (item.Len == 1 &&
                        tag9F11Value >= 1 &&
                        tag9F11Value <= 10)
                    {
                        var pse9F11 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectPSE, "9F11");
                        if(!string.IsNullOrEmpty(pse9F11))
                        {
                            if (pse9F11 == item.Value)
                                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                            else
                                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        }
                    }
                    else
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                    break;
                }
            }
        }

        /// <summary>
        /// 9F12的长度必须在1～16之间,Tag9F12转成BCD显示
        /// </summary>
        public void SelectAid_009()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in TLVs)
            {
                if (item.Tag == "9F12")
                {
                    string value = UtilLib.Utils.BcdToStr(item.Value);
                    if (item.Len >= 1 &&
                        item.Len <= 16 &&
                        CaseUtil.IsAlpha(value))
                    {
                        TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                    else
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                    break;
                }
            }
        }

        /// <summary>
        /// 9F4D和DF4D的长度是否为2字节
        /// </summary>
        public void SelectAid_010()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in TLVs)
            {
                if (item.Tag == "9F4D")
                {
                    if(item.Len == 2)
                        TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    else
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
                if (item.Tag == "DF4D")
                {
                    if (item.Len == 2)
                        TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    else
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }

        /// <summary>
        /// PSE的FCI中有9F11的话在PSE的DIR文件中是否存在9F12
        /// </summary>
        public void SelectAid_011()
        {
            var tag9F11 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectPSE,"9F11");
            if(tag9F11.Length >0)
            {

            }
        }

        /// <summary>
        /// 每个Tag只能存在一个，包括6F，A5，BF0C模板
        /// </summary>
        public void SelectAid_012()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var dupTags = new List<string>();
            if(CaseUtil.IsUniqTag(response.Response, out dupTags))
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测tag9F38是否存在,是否能正常分解
        /// </summary>
        public void SelectAid_013()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag9F38 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectApp, "9F38");
            if (string.IsNullOrEmpty(tag9F38))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            var tls = DataParse.ParseTL(tag9F38);
            if (tls.Count == 0)  //分解不正确
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F38每个tag长度是否符合规范
        /// </summary>
        public void SelectAid_014()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag9F38 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectApp, "9F38");
            var locator = new ViewModelLocator();
            if (string.IsNullOrEmpty(tag9F38))
            {
                return;
            }
            var tls = DataParse.ParseTL(tag9F38);
            foreach (var tl in tls)
            {
                var terminalData = locator.Terminal.TermianlSettings.GetTag(tl.Tag);
                if (string.IsNullOrEmpty(terminalData))
                {
                    TraceInfo(TipLevel.Failed, caseNo, "终端数据[tag{0}]不存在，请联系开发人员添加该tag", tl.Tag);
                    return;
                }
                if (tl.Len * 2 != terminalData.Length)
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测国密交易下，tag9F38需包含DF69算法支持指示器，且长度为01
        /// </summary>
        public void SelectAid_015()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag9F38 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectApp, "9F38");
            var tls = DataParse.ParseTL(tag9F38);
            foreach(var tl in tls)
            {
                if(tl.Tag == "DF69" && tl.Len == 1)
                {
                    TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    return;
                }
            }
            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测采用密文版本01的QPBOC最基本的PDOL包含的标签是否存在(tag9F66/9F02/9F03/9F1A/95/5F2A/9A/9C/9F37)
        /// </summary>
        public void SelectAid_016()
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
            foreach(var tag in tags)
            {
                bFind = false;
                foreach(var tl in tls)
                {
                    if(tl.Tag == tag.Item1 && tl.Len == tag.Item2)
                    {
                        bFind = true;
                        break;
                    }
                }
                if (!bFind)
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测PPSE返回的tag50和tag87是否和选择AID应用时的FCI模板中的tag50和tag87一致
        /// </summary>
        public void SelectAid_017()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var ppseTag50 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectPPSE, "50");
            var ppseTag87 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectPPSE, "87");
            var tag50 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectApp, "50");
            var tag87 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectApp, "87");
            if(ppseTag50 != tag50 ||
                ppseTag87 != tag87)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测AID返回的FCI模板中tag84的值和PPSE FCI中的4F是否一致
        /// </summary>
        public void SelectAid_018()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var ppseTag4F = TransactionTag.GetInstance().GetTag(TransactionStep.SelectPPSE, "4F");
            var tag84 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectApp, "84");
            if (ppseTag4F != tag84)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
    }
}

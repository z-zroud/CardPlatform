using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using CardPlatform.Common;
using CardPlatform.Business;
using CardPlatform.Config;
using CplusplusDll;

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

        public override void ExcuteCase(TransactionStep step, object srcData)
        {
            response = (ApduResponse)srcData;
            TLVs = DataParse.ParseTLV(response.Response);
            base.ExcuteCase(step,srcData);
        }

        /// <summary>
        /// 是否以6F开头
        /// </summary>
        public void PBOC_sAID_SJHGX_001()
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
        public void PBOC_sAID_SJHGX_003()
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
        /// 84的长度是否正确且在5～16字节之间
        /// </summary>
        public void PBOC_sAID_SJHGX_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag84 = from tlv in TLVs where tlv.Tag == "84" select tlv;
            if (tag84.First() != null)
            {
                if (CaseUtil.IsExpectedLen(tag84.First().Value, 5, 16))
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
        /// A5模板下必须包含tag50
        /// </summary>
        public void PBOC_sAID_SJHGX_006()
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
        public void PBOC_sAID_SJHGX_007()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var items = new List<string>() { "50", "87", "9F38", "5F2D", "9F11", "9F12", "BF0C" };
            var tlvs = from tlv in TLVs where tlv.Level == 1 select tlv;

            bool hasOtherTag = false;
            foreach(var tlv in tlvs)
            {
                if(!items.Contains(tlv.Tag))
                {
                    hasOtherTag = true;
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
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
        public void PBOC_sAID_SJHGX_008()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag87 = from tlv in TLVs where tlv.Tag == "87" select tlv;
            if (tag87.First() != null)
            {
                if (tag87.First().Value.Length == 1)
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
        public void PBOC_sAID_SJHGX_010()
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
        public void PBOC_sAID_SJHGX_012()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in TLVs)
            {
                if (item.Tag == "9F11")
                {
                    var tag9F11Value = Convert.ToInt16(item.Value);
                    if (item.Len == 1 &&
                        tag9F11Value >= 1 &&
                        tag9F11Value <= 10)
                    {
                        var pse9F11 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectPSE, "9F11");
                        if(pse9F11 == item.Value)
                            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                        else
                            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
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
        public void PBOC_sAID_SJHGX_013()
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
        public void PBOC_sAID_SJHGX_015()
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
        public void PBOC_sPSE_GLX_001()
        {
            var tag9F11 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectPSE,"9F11");
            if(tag9F11.Length >0)
            {

            }
        }

        /// <summary>
        /// 每个Tag只能存在一个，包括6F，A5，BF0C模板
        /// </summary>
        public void PBOC_sPSE_CFX_001()
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
    }
}

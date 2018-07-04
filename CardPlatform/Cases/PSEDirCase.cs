﻿using CardPlatform.Config;
using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using UtilLib;
using CardPlatform.Common;

namespace CardPlatform.Cases
{
    public class PSEDirCase : CaseBase
    {
        private ApduResponse response;
        private List<TLV> TLVs;

        public PSEDirCase()
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
            CheckTemplateTag(TLVs);
            base.ExcuteCase(step, srcData);
        }

        /// <summary>
        /// 是否以70开头
        /// </summary>
        public void PBOC_rPSEDIR_SJHGX_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (response.Response.Length < 2 || response.Response.Substring(0, 2) != "70")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 70后是否仅包含61模板
        /// </summary>
        public void PBOC_rPSEDIR_SJHGX_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach(var item in TLVs)
            {
                if(item.Level == 1)
                {
                    if(item.Tag != "61")
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                    else
                    {
                        TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                }
            }
        }

        /// <summary>
        /// 61中是否包含4F（必选包含）4F的长度是否在5～16字节之间
        /// </summary>
        public void PBOC_rPSEDIR_SJHGX_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var item = from tlv in TLVs where tlv.Tag == "4F" select tlv;
            var item1 = from tlv in TLVs where tlv.Level == 2 select tlv;

            var result = item.Union(item1);

            if(result.FirstOrDefault() == null ||
                result.Count() != 1 || 
                result.First().Len < 5 ||
                result.First().Len > 16)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 50必须包含在61模板内，50的长度是否在1～16字节之间
        /// </summary>
        public void PBOC_rPSEDIR_SJHGX_007()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var item = from tlv in TLVs where tlv.Tag == "50" select tlv;
            var item1 = from tlv in TLVs where tlv.Level == 3 select tlv;

            var result = item.Union(item1);

            if (result.FirstOrDefault() == null ||
                result.Count() != 1 ||
                result.First().Len < 1 ||
                result.First().Len > 16)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 61中可包含的其他Tag有 50.9F12、87、73，其他Tag不能出现
        /// </summary>
        public void PBOC_rPSEDIR_SJHGX_008()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag61SubItems = new List<string>() { "50", "9F12", "87", "73" };
            var items = from tlv in TLVs where tlv.Level == 3 select tlv;

            bool hasOtherTag = false;
            foreach(var item in items)
            {
                if(!tag61SubItems.Contains(item.Tag))
                {
                    hasOtherTag = true;
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            if(!hasOtherTag)
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 9F12的长度应在1～16字节之间,BCD码显示
        /// </summary>
        public void PBOC_rPSEDIR_SJHGX_009()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var item = from tlv in TLVs where tlv.Tag == "9F12" select tlv;
            if(item.FirstOrDefault() != null )
            {
                var tag9F12 = item.First();
                var str9F12 = Utils.BcdToStr(tag9F12.Value);
                if(tag9F12.Len < 1 ||
                    tag9F12.Len > 16 ||
                    !CaseUtil.IsAlpha(str9F12))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
                else
                {
                    TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
            }
        }

        /// <summary>
        /// 87的长度是否为1字节
        /// </summary>
        public void PBOC_rPSEDIR_SJHGX_010()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var item = from tlv in TLVs where tlv.Tag == "87" select tlv;
            if (item.FirstOrDefault() != null)
            {
                if (item.First().Len != 1)
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
                else
                {
                    TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
            }
        }


    }
}

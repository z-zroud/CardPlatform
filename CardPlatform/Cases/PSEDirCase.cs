using CardPlatform.Config;
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

        public override void Excute(int batchNo, TransactionApp app, TransactionStep step, object srcData)
        {
            response = (ApduResponse)srcData;
            TLVs = DataParse.ParseTLV(response.Response);
            Excute(batchNo,app,step, srcData);           
            CheckTemplateTag(TLVs);
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
            bool hasTag4F = false;
            bool isCorrectLen = false;
            foreach(var item in TLVs)
            {
                if(item.Tag == "4F")
                {
                    hasTag4F = true;
                    if(item.Len >= 5 && item.Len <= 16)
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
            if(hasTag4F && isCorrectLen)
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 50必须包含在61模板内，50的长度是否在1～16字节之间
        /// </summary>
        public void PBOC_rPSEDIR_SJHGX_007()
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
        /// 61中可包含的其他Tag有 50.9F12、87、73，其他Tag不能出现
        /// </summary>
        public void PBOC_rPSEDIR_SJHGX_008()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag61SubTags = CaseUtil.GetSubTags("61", TLVs);
            List<string> tags = new List<string>() { "4F","50", "9F12", "87", "73" };
            foreach (var item in tag61SubTags)
            {
                if (!tags.Contains(item.Tag))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 9F12的长度应在1～16字节之间,BCD码显示
        /// </summary>
        public void PBOC_rPSEDIR_SJHGX_009()
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
        /// 87的长度是否为1字节,值的合规性检测
        /// </summary>
        public void PBOC_rPSEDIR_SJHGX_010()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach(var item in TLVs)
            {
                if(item.Tag == "87")
                {
                    var tag87Value = Convert.ToInt16(item.Value, 16);
                    if (item.Len == 1 &&
                        ((tag87Value >= 0 && tag87Value <= 0xF) ||
                        (tag87Value > 0x80 && tag87Value <= 0x8F))
                        )
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
        /// PSE的FCI中有9F11的话在PSE的DIR文件中是否存在9F12
        /// </summary>
        public void PBOC_rPSEDIR_SJHGX_011()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach(var item in TLVs)
            {
                if(item.Tag == "9F11")
                {
                    string tag9F12 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectPSE, "9F12");
                    if (string.IsNullOrEmpty(tag9F12))
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        return;
                    }
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测4F和50的一致性
        /// </summary>
        public void PBOC_sPSEDIR_GLX_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            List<Tuple<string, string>> tuples = new List<Tuple<string, string>>()
             {
                 new Tuple<string, string>("A000000333010101","556E696F6E506179204465626974"),
                 new Tuple<string, string>("A000000333010102","556E696F6E50617920437265646974"),
             };

            var tag4F = CaseUtil.GetTag("4F", TLVs);
            var tag50 = CaseUtil.GetTag("50", TLVs);

            foreach(var item in tuples)
            {
                if(item.Item1 == tag4F)
                {
                    if(item.Item2 == tag50)
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
        /// PSE DIR tag重复性检测
        /// </summary>
        public void PBOC_rPSEDIR_CFX_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            List<string> duplexTags = new List<string>();
            if(CaseUtil.IsUniqTag(response.Response,out duplexTags))
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

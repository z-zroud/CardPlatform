using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using CardPlatform.Common;
using CardPlatform.Config;

namespace CardPlatform.Cases
{
    public class PSECases : CaseBase
    {
        private ApduResponse response;
        private List<TLV> TLVs;

        public PSECases()
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
            base.ExcuteCase(step, srcData);
            CheckTemplateTag(TLVs);
        }

        /// <summary>
        /// 数据是否以6F开头
        /// </summary>
        public void PBOC_sPSE_SJHGX_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!CaseUtil.RespStartWith(response.Response, "6F"))
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
        public void PBOC_sPSE_SJHGX_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var template6F = new List<TLV>();
            foreach(var item in TLVs)
            {
                if(item.Level == 1)
                {
                    template6F.Add(item);
                }
            }
            if(template6F.Count != 2 ||
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
        /// 84是否为银联规范规定的值
        /// </summary>
        public void PBOC_sPSE_SJHGX_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach(var item in TLVs)
            {
                if(item.Tag == "84")
                {
                    if(item.Value != Constant.PSE)
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                    else
                    {
                        TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                    break;
                }
            }
        }

        /// <summary>
        /// A5模板下只能包含必选数据tag88和可选数据5F2D、9F11和BF0C，其他Tag不能存在。
        /// </summary>
        public void PBOC_sPSE_SJHGX_006()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            List<string> tags = new List<string>() { "88", "5F2D", "9F11", "BF0C" };
            foreach(var item in TLVs)
            {
                if(item.Level == 2) //A5模板的数据
                {
                    if(!tags.Contains(item.Tag))
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        return;
                    }
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测A5模板下tag88是否符合规范(必须存在，长度01，值1-1F)
        /// </summary>
        public void PBOC_sPSE_SJHGX_007()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in TLVs)
            {
                if (item.Level == 2) //A5模板的数据
                {
                    if (item.Tag == "88" && item.Len == 1)
                    {
                        var tag88Value = Convert.ToInt16(item.Value);
                        if (tag88Value > 1 && tag88Value < 1F)
                        {
                            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                        }
                    }
                }
            }
            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测5F2D是否符合规范(长度必须是2字节的倍数，2～8字节之间,能转可读字符串)
        /// </summary>
        public void PBOC_sPSE_SJHGX_009()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in TLVs)
            {
                if(item.Tag == "5F2D")
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
        public void PBOC_sPSE_SJHGX_011()
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
        /// 每个Tag只能存在一个，包括6F，A5，BF0C模板
        /// </summary>
        public void PBOC_sPSE_CFX_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            Dictionary<string, TLV> tags = new Dictionary<string, TLV>();
            var templates = new List<string>() { "6F", "A5", "BF0C" };
            foreach(var item in TLVs)
            {
                if(templates.Contains(item.Tag))
                {
                    if (tags.ContainsKey(item.Tag))
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        return;
                    }
                    else
                    {
                        tags.Add(item.Tag, item);
                    }
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
    }
}

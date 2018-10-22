using System;
using System.Collections.Generic;
using System.Reflection;
using CardPlatform.Common;
using CardPlatform.Config;
using CardPlatform.Models;
using CplusplusDll;


namespace CardPlatform.Cases
{
    public class TerminalActionAnalyzeCase : CaseBase
    {
        private ApduResponse response;
        private List<TLV> TLVs;

        public TerminalActionAnalyzeCase()
        {
            response = new ApduResponse();
            TLVs = new List<TLV>();
        }

        public override void Excute(int batchNo, AppType app, TransactionStep step, object srcData)
        {
            response = (ApduResponse)srcData;
            base.Excute(batchNo,app,step, srcData);
            CheckTemplateTag(TLVs);           
        }

        /// <summary>
        /// 检测第一个GAC卡片返回结果是否为80
        /// </summary>
        public TipLevel TerminalActionAnalyze_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!CaseUtil.RespStartWith(response.Response, "80"))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }

            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F10中MAC的正确性
        /// </summary>
        public TipLevel TerminalActionAnalyze_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (Check9F10Mac())
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F26的值与终端计算的mac一致性
        /// </summary>
        public TipLevel TerminalActionAnalyze_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (TransactionConfig.GetInstance().CurrentApp == AppType.VISA)
            {
                if (CheckVisaAc())
                {
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
            }
            else if (TransactionConfig.GetInstance().CurrentApp == AppType.MC)
            {
                if (CheckMcAc())
                {
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
            }
            else
            {
                if (CheckPbocAc())
                {
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
            }
            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        public TipLevel TerminalActionAnalyze_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            string tag9F4C = locator.Terminal.GetTag("9F4C");
            string key = transConfig.TransIdnKey;
            if(string.IsNullOrEmpty(key))
            {

                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "IDN key不存在");
            }
            if(string.IsNullOrEmpty(tag9F4C))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "无法获取IC卡动态数据，请确认DDA是否成功?");
            }
            string atc = TransactionTag.GetInstance().GetTag(TransactionStep.TerminalActionAnalyze,"9F36");
            if(string.IsNullOrEmpty(atc))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "无法获取 ATC");
            }
            var calcIccDynamicData = Algorithm.Des3Encrypt(key, atc + "000000000000");
            if(calcIccDynamicData != tag9F4C)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "tag9F4C:{0},计算得到的IC卡动态数据为:{1}",tag9F4C,calcIccDynamicData);
            }
            return TipLevel.Sucess;
        }
    }
}

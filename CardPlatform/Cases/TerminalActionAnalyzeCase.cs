using System;
using System.Collections.Generic;
using System.Reflection;
using CardPlatform.Common;
using CardPlatform.Config;
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

        protected override void Load()
        {
            base.Load();
        }

        public override void Excute(int batchNo, TransactionApp app, TransactionStep step, object srcData)
        {
            response = (ApduResponse)srcData;
            base.Excute(batchNo,app,step, srcData);
            CheckTemplateTag(TLVs);           
        }

        /// <summary>
        /// 检测第一个GAC卡片返回结果是否为80
        /// </summary>
        public void TerminalActionAnalyze_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!CaseUtil.RespStartWith(response.Response, "80"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }

            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F10中MAC的正确性
        /// </summary>
        public void TerminalActionAnalyze_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (Check9F10Mac())
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F26的值与终端计算的mac一致性
        /// </summary>
        public void TerminalActionAnalyze_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (CheckAc())
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }
    }
}

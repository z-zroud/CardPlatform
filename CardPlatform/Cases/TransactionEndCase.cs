using CardPlatform.Common;
using CardPlatform.Config;
using CardPlatform.Models;
using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Cases
{
    public class TransactionEndCase : CaseBase
    {
        private ApduResponse response;
        private List<TLV> TLVs;

        public TransactionEndCase()
        {
            response = new ApduResponse();
            TLVs = new List<TLV>();
        }

        public override void Excute(int batchNo, AppType app, TransactionStep step, object srcData)
        {
            response = (ApduResponse)srcData;
            base.Excute(batchNo, app, step, srcData);
            CheckTemplateTag(TLVs);
        }

        /// <summary>
        /// 检测第一个GAC卡片返回结果是否为80
        /// </summary>
        public void TransactionEnd_001()
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
        public void TransactionEnd_002()
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
        public void TransactionEnd_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (TransactionConfig.GetInstance().CurrentApp == AppType.VISA)
            {
                if (CheckVisaAc())
                {
                    TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    return;
                }
            }
            else
            {
                if (CheckPbocAc())
                {
                    TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    return;
                }
            }
            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }
    }
}

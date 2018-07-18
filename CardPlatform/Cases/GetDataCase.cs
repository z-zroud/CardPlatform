using CardPlatform.Common;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Config;

namespace CardPlatform.Cases
{
    public class GetDataCase : CaseBase
    {
        public GetDataCase()
        {

        }

        /// <summary>
        /// 检测Tag9F79小于等于tag9F77, tag9F78小于等于tag9F77
        /// </summary>
        public void GetData_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F79 = TransactionTag.GetInstance().GetTag(TransactionStep.GetData, "9F79");
            var tag9F78 = TransactionTag.GetInstance().GetTag(TransactionStep.GetData, "9F78");
            var tag9F77 = TransactionTag.GetInstance().GetTag(TransactionStep.GetData, "9F77");
            int nTag9F79 = Convert.ToInt32(tag9F79);
            int nTag9F78 = Convert.ToInt32(tag9F78);
            int nTag9F77 = Convert.ToInt32(tag9F77);

            if(nTag9F79 > nTag9F77 || nTag9F78 > nTag9F77)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F6D设置的合理性，数值不宜过大，一般默认为0
        /// </summary>
        public void GetData_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F6D = TransactionTag.GetInstance().GetTag(TransactionStep.GetData, "9F6D");
            if(tag9F6D != "000000000000")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F68卡片附加处理选项的规范性
        /// </summary>
        public void GetData_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F68 = TransactionTag.GetInstance().GetTag(TransactionStep.GetData, "9F68");
            if (string.IsNullOrEmpty(tag9F68))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
    }
}

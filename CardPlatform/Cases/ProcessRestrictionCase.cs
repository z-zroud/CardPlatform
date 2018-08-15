using CardPlatform.Common;
using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Config;

namespace CardPlatform.Cases
{
    public class ProcessRestrictionCase : CaseBase
    {
        public ProcessRestrictionCase()
        {

        }
        /// <summary>
        /// 检测Tag5F24应用失效日期,应用是否失效
        /// </summary>
        public void HandleLimitation_001()
        {
            int expiryDate;
            int currentDate;
            int.TryParse(TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "5F24"), out expiryDate);
            int.TryParse(DateTime.Now.ToString("yyMMdd"), out currentDate);
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (expiryDate < currentDate)    //应用已失效
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5F25应用生效日期，应用是否生效
        /// </summary>
        public void HandleLimitation_002()
        {
            int effectiveDate;
            int currentDate;
            int.TryParse(TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "5F25"), out effectiveDate);
            int.TryParse(DateTime.Now.ToString("yyMMdd"), out currentDate);
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (effectiveDate >= currentDate) // 应用未生效
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);

        }

        /// <summary>
        /// 检测Tag5F24和Tag5F25时间顺序关系是否正确
        /// </summary>
        public void HandleLimitation_003()
        {
            int expiryDate;
            int effectiveDate;
            int currentDate;
            int.TryParse(TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "5F24"), out expiryDate);
            int.TryParse(TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "5F25"), out effectiveDate);
            int.TryParse(DateTime.Now.ToString("yyMMdd"), out currentDate);
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (expiryDate <= effectiveDate) //应用失效日期 大于生效日期
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
    }
}

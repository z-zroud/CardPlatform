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
        public TipLevel GetData_001()
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
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F6D设置的合理性，数值不宜过大，一般默认为0
        /// </summary>
        public TipLevel GetData_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F6D = TransactionTag.GetInstance().GetTag(TransactionStep.GetData, "9F6D");
            if(tag9F6D != "000000000000")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F68卡片附加处理选项的规范性
        /// </summary>
        public TipLevel GetData_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F68 = TransactionTag.GetInstance().GetTag(TransactionStep.GetData, "9F68");
            if (string.IsNullOrEmpty(tag9F68))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5F28与9F57发卡行国家代码的一致性
        /// </summary>
        public TipLevel GetData_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag5F28 = TransactionTag.GetInstance().GetTag("5F28");
            var tag9F57 = TransactionTag.GetInstance().GetTag(TransactionStep.GetData, "9F57");
            if (string.IsNullOrEmpty(tag9F57))
            {
                return TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "卡片缺少tag9F57，如果支持卡片频度检测，该数据必须存在");
            }
            if (tag5F28 != tag9F57)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag9F42与9F51应用货币代码的一致性
        /// </summary>
        public TipLevel GetData_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F42 = TransactionTag.GetInstance().GetTag("9F42");
            var tag9F51 = TransactionTag.GetInstance().GetTag(TransactionStep.GetData, "9F51");
            if (string.IsNullOrEmpty(tag9F42))
            {
                return TraceInfo(TipLevel.Warn, caseNo, "读数据中缺少tag9F42,如果8E中X,Y金额不为0，该数据必须存在");
            }
            
            if (string.IsNullOrEmpty(tag9F51))
            {
                return TraceInfo(caseItem.Level, caseNo, "卡片缺少tag9F51,如果只选频度检测，该数据必须存在");
            }
            caseItem.Description += "【tag9F42=" + tag9F42 + "】";
            caseItem.Description += "【tag9F51=" + tag9F51 + "】";
            if (tag9F42 != tag9F51)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
    }
}

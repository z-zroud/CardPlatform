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
    public class TerminalRiskManagementCase : CaseBase
    {
        public TerminalRiskManagementCase()
        {

        }

        /// <summary>
        /// 检测发卡行支持终端频度检测必要的数据(tag9F14,tag9F23)
        /// </summary>
        public TipLevel TerminalRiskManagement_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            string aip = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(aip);

            if (aipHelper.IsSupportTerminalRiskManagement())
            {
                var tag9F14 = TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "9F14");
                var tag9F23 = TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "9F23");
                if (string.IsNullOrEmpty(tag9F14) || string.IsNullOrEmpty(tag9F23))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description + "tag82={0}不支持终端风险管理",aip);
        }

        /// <summary>
        /// 检测tag9F13的规范性
        /// </summary>
        public TipLevel TerminalRiskManagement_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F13 = TransactionTag.GetInstance().GetTag("9F13");
            if(string.IsNullOrEmpty(tag9F13))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "【缺少tag9F13,终端将无法执行新卡检查】");
            }
            caseItem.Description += "【Tag9F13=" + tag9F13 + "】";
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
    }
}

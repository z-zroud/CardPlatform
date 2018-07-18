﻿using CardPlatform.Common;
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
        public void TerminalRiskManagement_001()
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
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
    }
}

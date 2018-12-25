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
    public class CardHolderVerifyCase : CaseBase
    {
        public CardHolderVerifyCase()
        {

        }

        /// <summary>
        /// 检测8E格式是否符合规范
        /// </summary>
        public TipLevel CardHolderVerfiy_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag8E = TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "8E");
            if (tag8E.Length < 20 || (tag8E.Length - 16) % 4 != 0)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测X，Y金额是否全0
        /// </summary>
        public TipLevel CardHolderVerfiy_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag8E = TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "8E");
            if (tag8E.Length < 20)
            {
                return TraceInfo(caseItem.Level, caseNo, "8E长度有误");
            }
            if(tag8E.Substring(0,16) != "0000000000000000")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }


        /// <summary>
        /// 检测UICS数据中8E是否符合联机PIN + 签名
        /// </summary>
        public TipLevel CardHolderVerfiy_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag8E = TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "8E");
            if (tag8E.Length != 28 || tag8E.Substring(16) != "42031E031F00")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "8E={0}",tag8E);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }


        /// <summary>
        /// 检测8E持卡人认证方法列表
        /// </summary>
        public TipLevel CardHolderVerfiy_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag8E = TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "8E");
            caseItem.Description += "tag8E=" + tag8E.Substring(16);
            tag8E = tag8E.Substring(16, tag8E.Length - 20);
            string methodList = string.Empty;
            for (int i = 0; i < tag8E.Length; i += 4)
            {
                int method = Convert.ToInt32(tag8E.Substring(i, 2), 16);
                int condition = Convert.ToInt32(tag8E.Substring(i + 2, 2), 16);
                string conditionDesc = string.Empty;
                switch (condition)
                {
                    case 0x01: conditionDesc = "[如果是ATM现金交易]"; break;
                    case 0x02: conditionDesc = "[如果不是ATM现金交易或返现交易]"; break;
                    case 0x03: conditionDesc = "[如果终端支持这个CVM]"; break;
                    case 0x04: conditionDesc = "[如果是人工值守现金交易]"; break;
                    case 0x05: conditionDesc = "[如果是返现交易]"; break;
                    default: conditionDesc = "[未知的使用条件]"; break;
                }
                int methodSixBit = method & 0x3F;
                string methodDesc = string.Empty;
                switch(methodSixBit)
                {
                    case 0x01: methodDesc = "[卡片执行明文PIN核对]"; break;
                    case 0x02: methodDesc = "[联机PIN验证]"; break;
                    case 0x03: methodDesc = "[明文PIN+签名]"; break;
                    case 0x1E: methodDesc = "[签名]"; break;
                    default: methodDesc = "[未知的方法]"; break;
                }

                int nextAction = method & 0x40;
                string nextActionDesc = string.Empty;
                if(nextAction == 1)
                {
                    nextActionDesc = "[如果此CVM失败，应用后续的]";
                }
                else
                {
                    nextActionDesc = "[如果此CVM失败，则持卡人验证失败]";
                }
                TraceInfo(TipLevel.Tip, caseNo, "CVM:[" + tag8E.Substring(i,4) + "]" + conditionDesc + methodDesc + nextActionDesc);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description + methodList);
        }
    }
}

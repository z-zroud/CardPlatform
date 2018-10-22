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
        /// 检测X，Y金额是否全0
        /// </summary>
        public TipLevel CardHolderVerfiy_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag8E = TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "8E");
            if (tag8E.Length != 28 || tag8E.Substring(16) != "42031E031F00")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }


        /// <summary>
        /// 检测8E是否存在脱机PIN入口，若有是否有相应的tag存在卡片中
        /// </summary>
        public TipLevel CardHolderVerfiy_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag8E = TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "8E");
            for (int i = 16; i < tag8E.Length - 2; i += 4)
            {
                int method = Convert.ToInt32(tag8E.Substring(i, 2), 16);
                int condition = Convert.ToInt32(tag8E.Substring(i + 2, 2), 16);
                int methodSixBit = method & 0x3F;
                if (methodSixBit == 1)   //存在脱机PIN入口，需要判断是否有脱机PIN相关tag
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
    }
}

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
        public TipLevel ProcessRestriction_001()
        {
            int expiryDate;
            int currentDate;
            string tag5F24 = TransactionTag.GetInstance().GetTag("5F24");
            int.TryParse(tag5F24, out expiryDate);
            int.TryParse(DateTime.Now.ToString("yyMMdd"), out currentDate);
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if(string.IsNullOrEmpty(tag5F24))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法获取tag5F24]"); 
            }
            caseItem.Description += "【tag5F24=" + tag5F24 + "】";
            log.TraceLog("tag5F24失效日期为:【{0}】", tag5F24);
            log.TraceLog("当前日期为:【{0}】", DateTime.Now.ToString("yyMMdd"));
            if (expiryDate < currentDate)    //应用已失效
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5F25应用生效日期，应用是否生效
        /// </summary>
        public TipLevel ProcessRestriction_002()
        {
            int effectiveDate;
            int currentDate;
            string tag5F25 = TransactionTag.GetInstance().GetTag("5F25");
            int.TryParse(tag5F25, out effectiveDate);
            int.TryParse(DateTime.Now.ToString("yyMMdd"), out currentDate);
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if(string.IsNullOrEmpty(tag5F25))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法获取tag5F25]");
            }
            log.TraceLog("tag5F25生效日期为:【{0}】", tag5F25);
            log.TraceLog("当前日期为:【{0}】", DateTime.Now.ToString("yyMMdd"));
            caseItem.Description += "【tag5F25=" + tag5F25 + "】";
            if (effectiveDate >= currentDate) // 应用未生效
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);

        }

        /// <summary>
        /// 检测Tag5F24和Tag5F25时间顺序关系是否正确
        /// </summary>
        public TipLevel ProcessRestriction_003()
        {
            int expiryDate;
            int effectiveDate;
            int currentDate;
            int.TryParse(TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "5F24"), out expiryDate);
            int.TryParse(TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "5F25"), out effectiveDate);
            int.TryParse(DateTime.Now.ToString("yyMMdd"), out currentDate);
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            log.TraceLog("tag5F24失效日期为:【{0}】", TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "5F24"));
            log.TraceLog("tag5F25生效日期为:【{0}】", TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "5F25"));
            if (expiryDate <= effectiveDate) //应用失效日期 大于生效日期
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F07是否符合规范
        /// </summary>
        public TipLevel ProcessRestriction_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            string tag9F07 = TransactionTag.GetInstance().GetTag("9F07");
            if (string.IsNullOrEmpty(tag9F07))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法获取tag9F07]");
            }
            caseItem.Description += "【tag9F07" + tag9F07 + "】";
            log.TraceLog("tag9F07=【{0}】", tag9F07);
            if (tag9F07 == "FF00")
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            else
            {
                int auc = Convert.ToInt32(tag9F07, 16);
                if ((auc & 0x8000) == 0) caseItem.Description += "[国内现金交易有效检测失败]";
                if ((auc & 0x4000) == 0) caseItem.Description += "[国际现金交易有效检测失败]";
                if ((auc & 0x2000) == 0) caseItem.Description += "[国内商品有效检测失败]";
                if ((auc & 0x1000) == 0) caseItem.Description += "[国际商品有效效检测失败]";
                if ((auc & 0x0800) == 0) caseItem.Description += "[国内服务有效检测失败]";
                if ((auc & 0x0400) == 0) caseItem.Description += "[国际服务有效检测失败]";
                if ((auc & 0x0200) == 0) caseItem.Description += "[ATM有效检测失败]";
                if ((auc & 0x0100) == 0) caseItem.Description += "[除ATM外的终端有效检测失败]";
                //if ((auc & 0x0080) == 0) caseItem.Description += "[允许国内返现检测失败]";
                //if ((auc & 0x0040) == 0) caseItem.Description += "[允许国际返现检测失败]";
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 应用版本信息比较
        /// </summary>
        /// <returns></returns>
        public TipLevel ProcessRestriction_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            string tag9F08 = TransactionTag.GetInstance().GetTag("9F08");
            if (string.IsNullOrEmpty(tag9F08))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法获取tag9F08]");
            }
            var tag9F09 = locator.Terminal.GetTag("9F09");
            caseItem.Description += "【tag9F08=" + tag9F08 + "】";
            log.TraceLog("卡片版本号:tag9F08=【{0}】", tag9F08);
            log.TraceLog("终端版本号:【{0}】", tag9F09);
            if(tag9F09 != tag9F08)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
    }
}

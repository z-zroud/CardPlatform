using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Business;
using CardPlatform.Common;
using CardPlatform.Config;
using CplusplusDll;

namespace CardPlatform.Cases
{
    public class GPOCase : CaseBase
    {
        private ApduResponse response;
        private List<TLV> tlvs;

        public GPOCase()
        {
            response    = new ApduResponse();
            tlvs        = new List<TLV>();
        }

        protected override void Load()
        {
            base.Load();
        }

        public override void Excute(int batchNo, TransactionApp app, TransactionStep step, object srcData)
        {
            response = (ApduResponse)srcData;
            tlvs = DataParse.ParseTLV(response.Response);
            base.Excute(batchNo, app, step, srcData);
            CheckTemplateTag(tlvs);            
        }

        /// <summary>
        /// 检测GPO响应数据是否以80开头,格式是否正确
        /// </summary>
        public void GPO_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!CaseUtil.RespStartWith(response.Response, "80"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }   
            var tlvs = DataParse.ParseTLV(response.Response);
            if (tlvs.Count != 1 || tlvs[0].Value.Length <= 4)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }


        /// <summary>
        /// 检测tag94是否为4字节的倍数
        /// </summary>
        public void GPO_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag94 = TransactionTag.GetInstance().GetTag(TransactionStep.GPO,"94");
            if(tag94.Length % 4 != 0)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测GPO相应数据中没有重复的AFL记录
        /// </summary>
        public void GPO_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            bool hasDuplex = false;
            var tag94 = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "94");
            var tls = DataParse.ParseAFL(tag94);
            for(int i = 0; i < tls.Count; i++)
            {
                for(int j = i + 1; j < tls.Count; j++)
                {
                    if(tls[i].SFI == tls[j].SFI &&
                        tls[i].RecordNo == tls[j].RecordNo)
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        hasDuplex = true;
                    }
                }
            }
            if(!hasDuplex)
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测tag94的合规性(按4字节分组后的AFL，第2个字节应该小于第3个字节)
        /// </summary>
        public void GPO_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag94 = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "94");
            int count = tag94.Length / 8;
            for(int i = 0; i < count; i++)
            {
                var firstRecord = Convert.ToInt16(tag94.Substring(2 + i * 8, 2));
                var secondRecord = Convert.ToInt16(tag94.Substring(4 + i * 8, 2));

                if (firstRecord > secondRecord)
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            
        }


        /// <summary>
        /// 检测卡片是否支持SDA功能
        /// </summary>
        public void GPO_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var aip = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(aip);
            if(aipHelper.IsSupportSDA())
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测卡片是否支持DDA功能
        /// </summary>
        public void GPO_006()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var aip = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(aip);
            if (aipHelper.IsSupportDDA())
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测卡片是否支持持卡人认证功能
        /// </summary>
        public void GPO_007()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var aip = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(aip);
            if (aipHelper.IsSupportCardHolderVerify())
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测卡片是否支持执行终端风险管理功能
        /// </summary>
        public void GPO_008()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var aip = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(aip);
            if (aipHelper.IsSupportTerminalRiskManagement())
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测卡片是否支持发卡行认证功能
        /// </summary>
        public void GPO_009()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var aip = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(aip);
            if (aipHelper.IsSupportIssuerAuth())
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测卡片是否支持CDA功能
        /// </summary>
        public void GPO_010()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var aip = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(aip);
            if (aipHelper.IsSupportCDA())
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

    }
}

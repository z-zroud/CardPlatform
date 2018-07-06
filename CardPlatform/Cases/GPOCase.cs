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
        private List<TLV> TLVs;

        public GPOCase()
        {
            response = new ApduResponse();
            TLVs = new List<TLV>();
        }

        protected override void Load()
        {
            base.Load();
        }

        public override void ExcuteCase(TransactionStep step, object srcData)
        {
            response = (ApduResponse)srcData;
            TLVs = DataParse.ParseTLV(response.Response);
            CheckTemplateTag(TLVs);
            base.ExcuteCase(step, srcData);
        }

        /// <summary>
        /// 响应数据是否以80开头
        /// </summary>
        public void PBOC_GPO_SJHGX_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!CaseUtil.RespStartWith(response.Response, "80"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }


        /// <summary>
        /// 去除AIP后检查AFL是否为4字节的倍数
        /// </summary>
        public void PBOC_GPO_SJHGX_003()
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
        /// AFL中记录没有重复/AIP重复
        /// </summary>
        public void PBOC_GPO_SJHGX_004()
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
                        tls[i].RecordNo == tls[j].SFI)
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
        /// 按4字节分组后的AFL，第2个字节应该小于第3个字节
        /// </summary>
        public void PBOC_GPO_ZQX_002()
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
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
    }
}

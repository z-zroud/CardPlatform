using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Business;
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

        public override void ExcuteCase(object srcData)
        {
            response = (ApduResponse)srcData;
            TLVs = DataParse.ParseTLV(response.Response);
            CheckTemplateTag(TLVs);
            base.ExcuteCase(srcData);
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

            var tag94 = TransactionTag.GetInstance().GetTag("94");
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
            var tag94 = TransactionTag.GetInstance().GetTag("94");
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
    }
}

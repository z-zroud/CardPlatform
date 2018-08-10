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
                var firstRecord = Convert.ToInt16(tag94.Substring(2 + i * 8, 2), 16);
                var secondRecord = Convert.ToInt16(tag94.Substring(4 + i * 8, 2), 16);

                if (firstRecord > secondRecord)
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
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

        /// <summary>
        /// 检测GPO响应数据是否以77开头,格式是否正确
        /// </summary>
        public void GPO_011()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!CaseUtil.RespStartWith(response.Response, "77"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测GPO响应数据，联机/拒绝交易包含数据(tag82/9F36/57/9F10/9F26/9F63/9F34/9F6C/9F5D/5F20),脱机批准(tag82/94/9F36/9F26/9F10/57/5F34/9F4B/9F6C/9F5D)
        /// </summary>
        public void GPO_012()
        {
            List<Tuple<string, TipLevel>> onlineTags = new List<Tuple<string, TipLevel>>
            {
                new Tuple<string, TipLevel>("82",TipLevel.Failed),
                new Tuple<string, TipLevel>("9F36",TipLevel.Failed),
                new Tuple<string, TipLevel>("57",TipLevel.Failed),
                new Tuple<string, TipLevel>("9F10",TipLevel.Failed),
                new Tuple<string, TipLevel>("9F26",TipLevel.Failed),
                new Tuple<string, TipLevel>("9F63",TipLevel.Warn),
                new Tuple<string, TipLevel>("5F34",TipLevel.Warn),
                new Tuple<string, TipLevel>("9F6C",TipLevel.Warn),
                new Tuple<string, TipLevel>("9F5D",TipLevel.Warn),
                new Tuple<string, TipLevel>("5F20",TipLevel.Tip)
            };
            List<Tuple<string, TipLevel>> accptedTags = new List<Tuple<string, TipLevel>>
            {
                new Tuple<string, TipLevel>("82",TipLevel.Failed),
                new Tuple<string, TipLevel>("94",TipLevel.Failed),
                new Tuple<string, TipLevel>("9F36",TipLevel.Failed),
                new Tuple<string, TipLevel>("57",TipLevel.Warn),
                new Tuple<string, TipLevel>("9F10",TipLevel.Failed),
                new Tuple<string, TipLevel>("9F26",TipLevel.Failed),
                new Tuple<string, TipLevel>("5F34",TipLevel.Warn),
                new Tuple<string, TipLevel>("9F6C",TipLevel.Warn),
                new Tuple<string, TipLevel>("9F5D",TipLevel.Warn),
            };
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F27 = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "9F27");
            bool bSuccess = true;
            if(tag9F27 == "40")
            {
                foreach(var tag in accptedTags)
                {
                    var transTag = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, tag.Item1);
                    if(string.IsNullOrEmpty(transTag))
                    {
                        TraceInfo(tag.Item2, caseNo, "QUICS/QPBOC脱机批准GPO返回缺少tag{0}", tag.Item1);
                        bSuccess = false;
                    }
                }
            }
            else
            {
                foreach (var tag in onlineTags)
                {
                    var transTag = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, tag.Item1);
                    if (string.IsNullOrEmpty(transTag))
                    {
                        TraceInfo(tag.Item2, caseNo, "QUICS/QPBOC脱机批准GPO返回缺少tag{0}", tag.Item1);
                        bSuccess = false;
                    }
                }
            }
            if(bSuccess)
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测tag9F10的规范性(密文版本号01/17)(算法标识01/04)(IDD取值范围01~06)
        /// </summary>
        public void GPO_013()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tag9F10 = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "9F10");
            if(tag9F10.Length < 16)
            {
                TraceInfo(TipLevel.Failed, caseNo, "9F10长度过小");
                return;
            }
            string cvn = tag9F10.Substring(4, 2);
            string algorithmFlag = tag9F10.Substring(14, 2);
            if(cvn != "01" && cvn != "17")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            if(algorithmFlag != "01" && algorithmFlag != "04")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            if(tag9F10.Length > 16)
            {
                string idd = tag9F10.Substring(18, 2);
                int nIDD = Convert.ToInt32(idd, 16);
                if(nIDD < 1 || nIDD > 6)
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F10中MAC的正确性
        /// </summary>
        public void GPO_014()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if(Check9F10Mac())
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F26的值与终端计算的mac一致性
        /// </summary>
        public void GPO_015()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if(TransactionConfig.GetInstance().CurrentApp == TransactionApp.VISA)
            {
                if (CheckEmvAc())
                {
                    TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    return;
                }
            }
            else
            {
                if (CheckPbocAc())
                {
                    TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    return;
                }
            }

            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// Tag57的合规性检查（卡号不以62开头则警告）
        /// </summary>
        public void GPO_016()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            string tag57Value = string.Empty;
            foreach(var tlv in tlvs)
            {
                if(tlv.Tag == "57")
                {
                    tag57Value = tlv.Value;
                }
            }
            if (string.IsNullOrEmpty(tag57Value))
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录缺少tag57");
                return;
            }
            if (!CaseUtil.RespStartWith(tag57Value, "62"))
            {
                TraceInfo(TipLevel.Warn, caseNo, "银联卡号一般以62开头");
            }
            if (!CaseUtil.IsCorrectTag57Format(tag57Value))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5F34的长度是否为1字节
        /// </summary>
        public void GPO_017()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            bool bExisted = false;
            foreach(var tlv in tlvs)
            {
                if(tlv.Tag == "5F34")
                {
                    bExisted = true;
                    if(tlv.Len != 1)
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        return;
                    }
                }
            }
            if(!bExisted)
            {
                TraceInfo(TipLevel.Failed, caseNo, "读记录中缺少tag5F34");
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

    }
}

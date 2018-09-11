using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Business;
using CardPlatform.Common;
using CardPlatform.Config;
using CardPlatform.Models;
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

        public override void Excute(int batchNo, AppType app, TransactionStep step, object srcData)
        {
            response = (ApduResponse)srcData;
            tlvs = DataParse.ParseTLV(response.Response);
            base.Excute(batchNo, app, step, srcData);
            CheckTemplateTag(tlvs);            
        }

        #region 通用检测case
        /// <summary>
        /// 检测GPO响应数据是否以80开头,格式是否正确
        /// </summary>
        public void GPO_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            caseItem.Description += "【GPO响应数据:" + response.Response + "】";
            if (!CaseUtil.RespStartWith(response.Response, "80"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            var tlvs = DataParse.ParseTLV(response.Response);
            if (tlvs.Count != 1 || tlvs[0].Value.Length <= 4)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[GPO数据无法TLV分解]");
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag94是否为4字节的整数倍
        /// </summary>
        public void GPO_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag94Value = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "94");
            caseItem.Description += "【tag94=" + tag94Value + "】";
            if (tag94Value.Length % 4 != 0)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测tag94是否包含重复的AFL记录
        /// </summary>
        public void GPO_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            bool hasDuplex = false;
            var tag94 = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "94");
            var tls = DataParse.ParseAFL(tag94);
            for (int i = 0; i < tls.Count; i++)
            {
                for (int j = i + 1; j < tls.Count; j++)
                {
                    if (tls[i].SFI == tls[j].SFI &&
                        tls[i].RecordNo == tls[j].RecordNo)
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[重复的记录号为:" + tls[i].RecordNo + "]");
                        hasDuplex = true;
                    }
                }
            }
            if (!hasDuplex)
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测tag94的合规性(按4字节分组后的AFL，第2个字节应该小于等于第3个字节)
        /// </summary>
        public void GPO_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag94 = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "94");
            int count = tag94.Length / 8;
            for (int i = 0; i < count; i++)
            {
                var firstRecord = Convert.ToInt16(tag94.Substring(2 + i * 8, 2), 16);
                var secondRecord = Convert.ToInt16(tag94.Substring(4 + i * 8, 2), 16);

                if (firstRecord > secondRecord)
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[请检查tag94是否按序排列]");
                    return;
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测卡片是否支持DDA功能
        /// </summary>
        public void GPO_006()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var aip = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "82");
            caseItem.Description += "【tag82=" + aip + "】";
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
            caseItem.Description += "【tag82=" + aip + "】";
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
            caseItem.Description += "【tag82=" + aip + "】";
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
            caseItem.Description += "【tag82=" + aip + "】";
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
            caseItem.Description += "【tag82=" + aip + "】";
            var aipHelper = new AipHelper(aip);
            if (aipHelper.IsSupportCDA())
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        #endregion
        #region Visa case
        #endregion
        #region qUICS
        #endregion
        #region qVSDC
        /// <summary>
        /// 检测qVSDC AIP的规范性
        /// </summary>
        public void GPO_021()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag82 = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "82");
            var intTag82 = Convert.ToInt32(tag82, 16);
            if ((intTag82 & 0x9F1F) != 0)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[AIP保留位需置零]");
                return;
            }
            if ((intTag82 & 0x0020) != 0x0020)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[非接交易位需置1]");
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测GPO响应数据的规范性(只能出现指定范围内的tag)
        /// </summary>
        public void GPO_022()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tags = from tlv in tlvs where tlv.Tag != "77" select tlv.Tag;
            List<string> tagSpecified = new List<string> { "82", "94", "57", "5F20", "5F34", "9F10", "9F26", "9F27", "9F36", "9F4B", "9F5D", "9F6C", "9F6E", "9F7C" };
            foreach(var item in tagSpecified)
            {
                caseItem.Description += "[" + item + "]";
            }
            foreach (var tag in tags)
            {
                if (!tagSpecified.Contains(tag))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[包含了其他tag{0}]",tag);
                    return;
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测GPO 响应数据的合规性(必须出现的tag,可能出现的tag,一定条件出现的tag)
        /// </summary>
        public void GPO_018()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tagMustIncludeWithODA = new List<string>() { "82", "94", "9F10", "9F26", "9F27", "9F36" };
            var tagMustIncludeWithoutODA = new List<string>() { "82", "9F10", "9F26", "9F27", "9F36" };
            var tagNeedAppearWithODA = new List<string>() { "57", "5F20", "5F34", "9F4B", "9F6C", "9F6E" };
            var tag9F27 = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "9F27");
            var tag94 = TransactionTag.GetInstance().GetTag(TransactionStep.GPO, "94");
            var gpoTags = from tag in tlvs select tag.Tag;

            bool isSucess = true;
            if (string.IsNullOrEmpty(tag94))
            {
                //说明不走ODA流程，GPO Online and Decline without ODA返回数据
                foreach (var tag in tagMustIncludeWithoutODA)
                {
                    if (!gpoTags.Contains(tag))
                    {
                        isSucess = false;
                        TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "[响应数据缺失必要tag{0}]", tag);
                    }
                }
            }
            else
            {
                if (tag9F27 == "80" || tag9F27 == "40")
                {
                    foreach (var tag in tagMustIncludeWithODA)
                    {
                        if (!gpoTags.Contains(tag))
                        {
                            isSucess = false;
                            TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "[响应数据缺失必要tag{0}]", tag);
                        }
                    }
                    foreach (var tag in tagNeedAppearWithODA)
                    {
                        if (!gpoTags.Contains(tag))
                        {
                            TraceInfo(TipLevel.Warn, caseNo, caseItem.Description + "[响应数据缺失必要tag{0}],请在读记录数据中查找该值，该值务必存在", tag);
                        }
                    }
                    return;
                }
                else
                {
                    //数据有问题
                    isSucess = false;
                    TraceInfo(TipLevel.Failed, caseNo, caseItem.Description + "[请检查tag94的值是否符合规范tag9F27={0}]", tag9F27);
                    return;
                }
            }
            if (isSucess)
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
        #endregion

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
            if(TransactionConfig.GetInstance().CurrentApp == AppType.qVSDC_offline ||
                TransactionConfig.GetInstance().CurrentApp == AppType.qVSDC_online)
            {
                if (CheckVisaAc())
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
                    caseItem.Description += "【tag5F35=" + tlv.Value + "】";
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

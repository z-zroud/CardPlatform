using CplusplusDll;
using System;
using CardPlatform.Common;
using System.Collections.Generic;
using System.Reflection;
using CardPlatform.Config;
using CardPlatform.Models;
using UtilLib;

namespace CardPlatform.Cases
{
    public class PPSECases : CaseBase
    {
        private ApduResponse response;
        private List<TLV> tlvs;

        public PPSECases()
        {
            response = new ApduResponse();
            tlvs = new List<TLV>();
        }

        public override void Excute(int batchNo, AppType app, TransactionStep step,Object srcData)
        {
            response = (ApduResponse)srcData;
            tlvs = DataParse.ParseTLV(response.Response);
            base.Excute(batchNo, app, step, srcData);
        }

        /// <summary>
        /// 检测PPSE响应数据是否以6F模板开头
        /// </summary>
        public TipLevel PPSE_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!CaseUtil.RespStartWith(response.Response, "6F"))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测PPSE响应数据6F模板下依次、仅包含Tag84和A5模板
        /// </summary>
        public TipLevel PPSE_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var template6F = CaseUtil.GetSubTags("6F", tlvs);
            if (template6F.Count != 2 ||
                template6F[0].Tag != "84" ||
                template6F[1].Tag != "A5")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测PPSE响应数据tag6F下的tag84是否为银联规范规定的值325041592E5359532E4444463031
        /// </summary>
        public TipLevel PPSE_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in tlvs)
            {
                if (item.Tag == "84")
                {
                    if (item.Value != Constant.PPSE)
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测PPSE响应数据A5模板下仅包含BF0C模板
        /// </summary>
        public TipLevel PPSE_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tags = CaseUtil.GetSubTags("A5", tlvs);
            if(tags.Count != 1 && tags[0].Tag != "BF0C")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测PPSE响应数据61模板中仅包含4F,50和87，其中4F、50必备，多应用下tag87必备
        /// </summary>
        public TipLevel PPSE_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            int template61Count = CaseUtil.GetDuplexTemplate("BF0C", "61", tlvs);
            List<string> subTags = new List<string>() { "4F", "50", "87", "9F0A", "BF63" };
            for (int i = 0; i < template61Count; i++)
            {
                var tags = CaseUtil.GetSubTags("61", i + 1, tlvs);
                foreach (var item in tags)
                {
                    if (!subTags.Contains(item.Tag))
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[包含非法tag{0}]", item.Tag);
                    }
                }
                if (!CaseUtil.HasTag("4F",tags))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[缺少必备的tag4F]");
                }
                if (!CaseUtil.HasTag("50", tags))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[缺少必备的tag50]");
                }
                if (template61Count > 1)
                {
                    if(!CaseUtil.HasTag("87", tags))
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[多应用下，tag87必须存在于每个应用的61模板中]");
                    }
                }
            }

            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }


        /// <summary>
        /// 检测PPSE响应数据61模板中4F的长度是否在(5~16)字节之间
        /// </summary>
        public TipLevel PPSE_006()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in tlvs)
            {
                if (item.Tag == "4F")
                {
                    if (item.Value.Length % 2 != 0 ||
                        item.Value.Length > 32 ||
                        item.Value.Length < 10)
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测PPSE响应数据tag87的长度是否为1字节，正确的值范围为00-0F，80-8F"
        /// </summary>
        public TipLevel PPSE_008()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in tlvs)
            {
                if (item.Tag == "87")
                {
                    var tag87Value = Convert.ToInt16(item.Value, 16);
                    if (item.Len == 1 &&
                        ((tag87Value >= 0 && tag87Value <= 0xF) ||
                        (tag87Value > 0x80 && tag87Value <= 0x8F))
                        )
                    {
                        return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                    else
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                }
            }
            return TipLevel.Unknown;
        }

        /// <summary>
        /// 检测PPSE响应数据tag4F和tag50关联性，例如4F为借记，则50显示Debit
        /// </summary>
        public TipLevel PPSE_009()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            int template61Count = CaseUtil.GetDuplexTemplate("BF0C", "61", tlvs);

            for (int i = 0; i < template61Count; i++)
            {
                var template61 = CaseUtil.GetSubTags("61", i + 1, tlvs);
                var tag4F = CaseUtil.GetTag("4F", template61);
                var tag50 = CaseUtil.GetTag("50", template61);
                if(string.IsNullOrEmpty(tag4F) || string.IsNullOrEmpty(tag50))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[缺少必备tag50或tag4F]");
                }
                var value = Utils.BcdToStr(tag50);
                if (value.Length > 32 || !CaseUtil.IsAlphaAndBlank(value))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag50不符合规范.卡片中值为{0}]",value);
                }
                var lowerValue = value.ToLower();
                if (tag4F == "A000000333010101")
                {
                    if (tag50 != "556E696F6E506179204465626974")
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag50={0}]", value);
                }
                else if (tag4F == "A000000333010102")
                {
                    if (tag50 != "556E696F6E50617920437265646974")
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag50={0}]", value);
                }
                else if (tag4F == "A0000000031010")
                {
                    if (lowerValue != "visa credit" && lowerValue != "visa debit" && lowerValue != "visa business")
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag50={0}]", value);
                    }
                }
                else if (tag4F == "A0000000032010")
                {
                    if (lowerValue != "visa" && lowerValue != "visa electron")
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag50={0}]", value);
                    }
                }
                else if (tag4F == "A0000000033010")
                {
                    if (lowerValue != "interlink" && lowerValue != "visa interlink")
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag50={0}]", value);
                    }
                }
                else if (tag4F == "A0000000038010")
                {
                    if (lowerValue != "plus" && lowerValue != "plus atm")
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag50={0}]", value);
                    }
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测PPSE响应数据是否包含有重复的tag或模板
        /// </summary>
        public TipLevel PPSE_010()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var dstTags = new List<string> { "6F", "A5", "BF0C" };
            var results = CaseUtil.HasDuplexTag(tlvs, dstTags);
            if(results.Count != 0)
            {
                string duplexTag = string.Empty;
                foreach(var result in results)
                {
                    duplexTag += "[" + result + "]";
                }
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[重复的tag列表{0}]",duplexTag);
            }
            int template61Count = CaseUtil.GetDuplexTemplate("BF0C", "61", tlvs);
            for(int i = 0; i < template61Count; i++)
            {
                var tags = CaseUtil.GetSubTags("61", i + 1, tlvs);
                if(CaseUtil.HasDuplexTag(tags))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[61模板包含了重复的tag]");
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
    }
}

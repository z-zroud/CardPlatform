using CplusplusDll;
using System;
using CardPlatform.Common;
using System.Collections.Generic;
using System.Reflection;
using CardPlatform.Config;

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

        public override void Excute(int batchNo, TransactionApp app, TransactionStep step,Object srcData)
        {
            response = (ApduResponse)srcData;
            tlvs = DataParse.ParseTLV(response.Response);
            base.Excute(batchNo, app, step, srcData);
            CheckTemplateTag(tlvs);
        }

        /// <summary>
        /// 检测数据是否以6F模板开头
        /// </summary>
        public void PPSE_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!CaseUtil.RespStartWith(response.Response, "6F"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测6F模板下包含且只包含Tag84和A5模板，顺序不能颠倒
        /// </summary>
        public void PPSE_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var template6F = new List<TLV>();
            foreach (var item in tlvs)
            {
                if (item.Level == 1)
                {
                    template6F.Add(item);
                }
            }
            if (template6F.Count != 2 ||
                template6F[0].Tag != "84" ||
                template6F[1].Tag != "A5")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测tag84是否为银联规范规定的值325041592E5359532E4444463031
        /// </summary>
        public void PPSE_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in tlvs)
            {
                if (item.Tag == "84")
                {
                    if (item.Value != Constant.PPSE)
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                    else
                    {
                        TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                    break;
                }
            }
        }

        /// <summary>
        /// 检测A5模板下只能包含BF0C，其他Tag不能存在
        /// </summary>
        public void PPSE_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var tags = CaseUtil.GetSubTags("A5", tlvs);
            if(tags.Count != 1 && tags[0].Tag != "BF0C")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                return;
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测61模板中只能包含4F,50和87，其中4F为强制要求
        /// </summary>
        public void PPSE_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            int tag61Count = CaseUtil.GetDuplexTemplate("BF0C", "61", tlvs);
            for(int i = 0; i < tag61Count; i++)
            {
                var tags = CaseUtil.GetSubTags("61", i + 1, tlvs);
                List<string> subTags = new List<string>() { "4F","50","87" };
                foreach (var item in tags)
                {
                    if (!subTags.Contains(item.Tag))
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        return;
                    }
                }
                if (!subTags.Contains("4F"))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
            }

            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }


        /// <summary>
        /// 检测4F的长度是否符合规范(5~16)字节之间
        /// </summary>
        public void PPSE_006()
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
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        return;
                    }
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测50的长度是否在(1~16)字节之间
        /// </summary>
        public void PPSE_007()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in tlvs)
            {
                if (item.Tag == "50")
                {
                    if (item.Value.Length % 2 != 0 ||
                        item.Value.Length < 2 ||
                        item.Value.Length > 32)
                    {
                        
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        return;
                    }
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测87的合规性(长度是否为1字节，正确的值范围为00-0F，80-8F)
        /// </summary>
        public void PPSE_008()
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
                        TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                    else
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                }
            }
        }

        /// <summary>
        /// 检测tag4F和tag50关联的一致性，例如4F为借记，则50显示Debit
        /// </summary>
        public void PPSE_009()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            List<Tuple<string, string>> tuples = new List<Tuple<string, string>>()
             {
                 new Tuple<string, string>("A000000333010101","556E696F6E506179204465626974"),
                 new Tuple<string, string>("A000000333010102","556E696F6E50617920437265646974"),
             };

            var tag4F = CaseUtil.GetTag("4F", tlvs);
            var tag50 = CaseUtil.GetTag("50", tlvs);

            foreach (var item in tuples)
            {
                if (item.Item1 == tag4F)
                {
                    if (item.Item2 == tag50)
                    {
                        TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                    else
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                    break;
                }
            }
        }

        /// <summary>
        /// 检测PPSEtag的重复性，6F,A5，BF0C模板只能存在一个，同一个61模板下Tag不能重复
        /// </summary>
        public void PPSE_010()
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
                TraceInfo(caseItem.Level, caseNo, caseItem.Description + "重复的tag列表" + duplexTag);
            }
            int tag61Count = CaseUtil.GetDuplexTemplate("BF0C", "61", tlvs);
            for(int i = 0; i < tag61Count; i++)
            {
                var tags = CaseUtil.GetSubTags("61", i + 1, tlvs);
                if(CaseUtil.HasDuplexTag(tags))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
            }
            TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

    }
}

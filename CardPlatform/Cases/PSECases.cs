using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using CardPlatform.Common;
using CardPlatform.Config;
using CardPlatform.Models;

namespace CardPlatform.Cases
{
    public class PSECases : CaseBase
    {
        private ApduResponse response;
        private List<TLV> tlvs;

        public PSECases()
        {
            response = new ApduResponse();
            tlvs = new List<TLV>();
        }

        public override void Excute(int batchNo, AppType app, TransactionStep step, object srcData)
        {
            response = (ApduResponse)srcData;
            tlvs = DataParse.ParseTLV(response.Response);
            base.Excute(batchNo,app,step, srcData);
            CheckTemplateTag(tlvs);
        }

        /// <summary>
        /// 数据是否以6F开头
        /// </summary>
        public TipLevel PSE_001()
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
        /// 6F模板下只能并且包含Tag84和A5模板，顺序不能颠倒
        /// </summary>
        public TipLevel PSE_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var template6F = new List<TLV>();
            foreach(var item in tlvs)
            {
                if(item.Level == 1)
                {
                    template6F.Add(item);
                }
            }
            if(template6F.Count != 2 ||
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
        /// 84是否为银联规范规定的值
        /// </summary>
        public TipLevel PSE_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach(var item in tlvs)
            {
                if(item.Tag == "84")
                {
                    string value = UtilLib.Utils.BcdToStr(item.Tag);
                    if(item.Value != Constant.PSE)
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "转码为:{0}",value);
                    }
                    else
                    {
                        return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description + "转码为:{0}", value);
                    }
                }
            }
            return TipLevel.Unknown;
        }

        /// <summary>
        /// A5模板下只能包含必选数据tag88和可选数据5F2D、9F11和BF0C，其他Tag不能存在。
        /// </summary>
        public TipLevel PSE_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            List<string> tags = new List<string>() { "88", "5F2D", "9F11", "BF0C" };
            var templateA5 = CaseUtil.GetSubTags("A5", tlvs);
            foreach(var item in templateA5)
            {
                if(!tags.Contains(item.Tag))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            if(!CaseUtil.HasTag("88",templateA5))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[A5模板缺少必须的tag88]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测A5模板下tag88是否符合规范(必须存在，长度01，值1-1F)
        /// </summary>
        public TipLevel PSE_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            var templateA5 = CaseUtil.GetSubTags("A5", tlvs);
            foreach (var item in templateA5)
            {
                if (item.Tag == "88" && item.Len == 1)
                {
                    var value = Convert.ToInt16(item.Value, 16);
                    if (value >= 1 && value <= 0x1F)
                    {
                        return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                }
            }
            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测5F2D是否符合规范(长度必须是2字节的倍数，2～8字节之间,能转可读字符串)
        /// </summary>
        public TipLevel PSE_006()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in tlvs)
            {
                if(item.Tag == "5F2D")
                {
                    string value = UtilLib.Utils.BcdToStr(item.Value);
                    if (item.Len % 2 == 0 &&
                        item.Len >= 2 &&
                        item.Len <= 8 &&
                        CaseUtil.IsAlpha(value))
                    {
                        return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                    }
                }
            }
            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }


        /// <summary>
        /// 9F11的长度必须是1字节，值在01～10之间
        /// </summary>
        public TipLevel PSE_007()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in tlvs)
            {
                if (item.Tag == "9F11")
                {
                    var value = Convert.ToInt16(item.Value);
                    if (item.Len != 1 ||
                        value == 0 ||
                        value > 10)
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 每个Tag只能存在一个，包括6F，A5，BF0C模板
        /// </summary>
        public TipLevel PSE_008()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            Dictionary<string, TLV> tags = new Dictionary<string, TLV>();
            var results = CaseUtil.HasDuplexTag(tlvs);
            if(results)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
    }
}

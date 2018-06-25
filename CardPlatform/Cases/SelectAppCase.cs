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
    public class SelectAppCase : CaseBase
    {
        private ApduResponse response;
        private List<TLV> TLVs;

        public SelectAppCase()
        {
            response = new ApduResponse();
            TLVs = new List<TLV>();
        }

        protected override void Load()
        {
            Step = "SelectAID";
            base.Load();
        }

        public override void ExcuteCase(object srcData)
        {
            response = (ApduResponse)srcData;
            TLVs = DataParse.ParseTLV(response.Response);
            base.ExcuteCase(srcData);
        }

        /// <summary>
        /// 是否以6F开头
        /// </summary>
        public void PBOC_sAID_SJHGX_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (!CaseUtil.RespStartWith(response.Response,"6F"))
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 6F模板下只能包含Tag84和A5模板，顺序不能颠倒,84的长度是否正确且在5～16字节之间
        /// </summary>
        public void PBOC_sAID_SJHGX_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var items = from tlv in TLVs where tlv.Level == 1 select tlv;
            if(items.Count() != 2)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                var itemList = items.ToList();
                if(itemList[0].Tag != "84" ||
                    itemList[0].Len < 5 ||
                    itemList[0].Len > 16 ||
                    itemList[1].Tag != "A5")
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
                else
                {
                    TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
            }
        }

        /// <summary>
        /// 84的长度是否正确且在5～16字节之间
        /// </summary>
        public void PBOC_sAID_SJHGX_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag84 = from tlv in TLVs where tlv.Tag == "84" select tlv;
            if (tag84.First() != null)
            {
                if (CaseUtil.IsExpectedLen(tag84.First().Value, 5, 16))
                {
                    TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
                else
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }

        /// <summary>
        /// A5模板下必须包含tag50
        /// </summary>
        public void PBOC_sAID_SJHGX_006()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var item = from tlv in TLVs where tlv.Tag == "50" select tlv;
            if(item == null || item.Count() != 1)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// A5模板下可包含的tag包括87，9F38,5F2D,9F11，9F12，BF0C
        /// </summary>
        public void PBOC_sAID_SJHGX_007()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var items = new List<string>() { "50", "87", "9F38", "5F2D", "9F11", "9F12", "BF0C" };
            var tlvs = from tlv in TLVs where tlv.Level == 1 select tlv;

            bool hasOtherTag = false;
            foreach(var tlv in tlvs)
            {
                if(!items.Contains(tlv.Tag))
                {
                    hasOtherTag = true;
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            if (!hasOtherTag)
            {
                TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }



        /// <summary>
        /// 87的长度是否为1字节
        /// </summary>
        public void PBOC_sAID_SJHGX_008()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag87 = from tlv in TLVs where tlv.Tag == "87" select tlv;
            if (tag87.First() != null)
            {
                if (tag87.First().Value.Length == 1)
                {
                    TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
                else
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }

        /// <summary>
        /// 5F2D的长度必须是2字节的倍数，2～8字节之间,5F2D转换后的语言是否合规
        /// </summary>
        public void PBOC_sAID_SJHGX_010()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag5F2D = from tlv in TLVs where tlv.Tag == "5F2D" select tlv;
            if (tag5F2D.First() != null)
            {
                string value = UtilLib.Utils.BcdToStr(tag5F2D.First().Value);
                if (tag5F2D.First().Value.Length % 2 != 0 &&
                    CaseUtil.IsExpectedLen(tag5F2D.First().Value,2,8) &&
                    CaseUtil.IsAlpha(value))
                {
                    TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
                else
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }

        /// <summary>
        /// 9F11的长度必须是1字节，值在01～10之间
        /// </summary>
        public void PBOC_sAID_SJHGX_012()
        {

        }

        /// <summary>
        /// 9F12的长度必须在1～16之间,Tag9F12转成BCD显示
        /// </summary>
        public void PBOC_sAID_SJHGX_013()
        {

        }

        /// <summary>
        /// 9F4D和DF4D的长度是否为2字节
        /// </summary>
        public void PBOC_sAID_SJHGX_015()
        {

        }

        /// <summary>
        /// PSE的FCI中有9F11的话在PSE的DIR文件中是否存在9F12
        /// </summary>
        public void PBOC_sPSE_GLX_001()
        {
            var tag9F11 = TagDict.GetInstance().GetTag("9F11");
            if(tag9F11.Length >0)
            {

            }
        }

        /// <summary>
        /// 每个Tag只能存在一个，包括6F，A5，BF0C模板
        /// </summary>
        public void PBOC_sPSE_CFX_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var dupTags = new List<string>();
            if(CaseUtil.IsUniqTag(response.Response, out dupTags))
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

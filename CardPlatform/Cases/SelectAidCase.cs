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
    public class SelectAidCase : CaseBase
    {

        protected override void Load()
        {
            Step = "SelectAID";
            base.Load();
        }

        /// <summary>
        /// 是否以6F开头
        /// </summary>
        public void PBOC_sAID_SJHGX_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (response.Response.Length < 2 || response.Response.Substring(0, 2) != "6F")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(CaseLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 6F模板下只能包含Tag84和A5模板，顺序不能颠倒,84的长度是否正确且在5～16字节之间
        /// </summary>
        public void PBOC_sAID_SJHGX_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var items = from tlv in arrTLV where tlv.Level == 1 select tlv;
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
                    TraceInfo(CaseLevel.Sucess, caseNo, caseItem.Description);
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

            var item = from tlv in arrTLV where tlv.Tag == "50" select tlv;
            if(item == null || item.Count() != 1)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(CaseLevel.Sucess, caseNo, caseItem.Description);
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
            var tlvs = from tlv in arrTLV where tlv.Level == 1 select tlv;

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
                TraceInfo(CaseLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 87的长度是否为1字节
        /// </summary>
        public void PBOC_sAID_SJHGX_008()
        {

        }

        /// <summary>
        /// 5F2D的长度必须是2字节的倍数，2～8字节之间,5F2D转换后的语言是否合规
        /// </summary>
        public void PBOC_sAID_SJHGX_010()
        {

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
    }
}

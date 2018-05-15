using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using CardPlatform.Business;
using CardPlatform.Config;

namespace CardPlatform.Cases
{
    public class PSECases : CaseBase
    {
        public PSECases()
        {
        }

        protected override void Load()
        {
            Step = "SelectPSE";
            base.Load();           
        }

        /// <summary>
        /// 数据是否以6F开头
        /// </summary>
        public void PBOC_sPSE_SJHGX_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (response.Response.Length < 2 || response.Response.Substring(0,2) != "6F")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(CaseLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 6F模板下只能包含Tag84和A5模板，顺序不能颠倒
        /// </summary>
        public void PBOC_sPSE_SJHGX_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var subItemOf61 = new List<TLV>();
            foreach(var item in arrTLV)
            {
                if(item.Level == 1)
                {
                    subItemOf61.Add(item);
                }
            }
            if(subItemOf61.Count != 2 ||
                subItemOf61[0].Tag != "84" ||
                subItemOf61[1].Tag != "A5")
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(CaseLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 84的长度是否正确且在5～16字节之间，其值是否为银联规范规定的值
        /// </summary>
        public void PBOC_sPSE_SJHGX_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach(var item in arrTLV)
            {
                if(item.Tag == "84")
                {
                    if(item.Value != Constant.PSE)
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                    else
                    {
                        TraceInfo(CaseLevel.Sucess, caseNo, caseItem.Description);
                    }
                    break;
                }
            }
        }

        /// <summary>
        /// A5模板下只能包含必选数据tag88和可选数据5F2D、9F11和BF0C，其他Tag不能存在。
        /// </summary>
        public void PBOC_sPSE_SJHGX_006()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            List<string> tags = new List<string>() { "88", "5F2D", "9F11", "BF0C" };
            bool hasTag88 = false;
            foreach(var item in arrTLV)
            {
                if(item.Level == 2) //A5模板的数据
                {
                    if(!tags.Contains(item.Tag))
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        return;
                    }
                    if(item.Tag == "88" && item.Len == 1)
                    {
                        hasTag88 = true;
                    }
                }
            }
            if(!hasTag88)
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(CaseLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 5F2D的长度必须是2字节的倍数，2～8字节之间
        /// </summary>
        public void PBOC_sPSE_SJHGX_009()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            bool hasTag5F2D = false;
            foreach (var item in arrTLV)
            {
                if(item.Tag == "5F2D")
                {
                    hasTag5F2D = true;
                    if(item.Level != 2 || 
                        item.Len % 2 != 0 ||
                        item.Len < 2 ||
                        item.Len > 8)
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        return;
                    }
                }
            }
            if(hasTag5F2D)
            {
                TraceInfo(CaseLevel.Sucess, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 5F2D转换成BCD码后，是否合规
        /// </summary>
        public void PBOC_sPSE_SJHGX_010()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in arrTLV)
            {
                if(item.Tag == "5F2D")
                {
                    string value = UtilLib.Utils.BcdToStr(item.Value);
                    if(CaseUtil.IsAlpha(value))
                    {
                        TraceInfo(CaseLevel.Sucess, caseNo, caseItem.Description);
                    }
                    else
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                }
            }
        }

        /// <summary>
        /// 9F11的长度必须是1字节，值在01～10之间
        /// </summary>
        public void PBOC_sPSE_SJHGX_011()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            bool hasTag9F11 = false;
            foreach (var item in arrTLV)
            {
                if (item.Tag == "9F11")
                {
                    hasTag9F11 = true;
                    if (item.Level != 2 ||
                        item.Len < 1 ||
                        item.Len > 10)
                    {
                        TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        return;
                    }
                }
            }
            if (hasTag9F11)
            {
                TraceInfo(CaseLevel.Sucess, caseNo, caseItem.Description);
            }
            else
            {
                TraceInfo(CaseLevel.Warn, caseNo, "PSE响应数据中缺少tag9F11");
            }
        }

        /// <summary>
        /// Tag长度为00的要提示
        /// </summary>
        public void PBOC_sPSE_SJHGX_012()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach (var item in arrTLV)
            {
                if(item.Len == 0)
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
        }


        public void PBOC_sPSE_GLX_001()
        {

        }

        /// <summary>
        /// 每个Tag只能存在一个，包括6F，A5，BF0C模板
        /// </summary>
        public void PBOC_sPSE_CFX_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            Dictionary<string, TLV> tags = new Dictionary<string, TLV>();
            foreach(var item in arrTLV)
            {
                if(tags.ContainsKey(item.Tag))
                {
                    TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    return;
                }
                else
                {
                    tags.Add(item.Tag, item);
                }
            }
            TraceInfo(CaseLevel.Sucess, caseNo, caseItem.Description);
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using CardPlatform.ViewModel;
using CardPlatform.Cases;
using System.Reflection;
using CplusplusDll;
using CardPlatform.Config;

namespace CardPlatform.Business
{

    /*************************************************************
     * 该类描述了QPBOC交易流程
     *************************************************************/
    public class BusinessQPBOC : BusinessBase
    {
        private TagDict tagDict = TagDict.GetInstance();
        private ViewModelLocator locator = new ViewModelLocator();
        private IExcuteCase baseCase = new CaseBase();
        private bool isQPBOCTranction = false;

        /// <summary>
        /// 开始交易流程
        /// </summary>
        /// <param name="aid"></param>
        /// <param name="doDesTrans"></param>
        /// <param name="doSMTrans"></param>
        public override void DoTrans(string aid, bool doDesTrans, bool doSMTrans)
        {
            base.DoTrans(aid, doDesTrans, doSMTrans);

            locator.Terminal.TermianlSettings.Tag9C = "00";         //交易类型(消费交易)
            locator.Terminal.TermianlSettings.Tag9F66 = "2A000080"; //终端交易属性
            locator.Terminal.TermianlSettings.TagDF60 = "00";   //扩展交易指示位           
            // 基于DES算法的QPBOC流程
            if (doDesTrans)
            {
                locator.Terminal.TermianlSettings.TagDF69 = "00";   //SM算法支持指示位
                curTransAlgorithmCategory = AlgorithmCategory.DES;
                DoTransEx();
            }
            //基于国密算法的交易流程
            if (doSMTrans)   
            {
                locator.Terminal.TermianlSettings.TagDF69 = "01";
                curTransAlgorithmCategory = AlgorithmCategory.SM;
                DoTransEx();
            }
        }

        protected void DoTransEx()
        {
            tagDict.Clear();    //做交易之前，需要将tag清空，避免与上次交易重叠
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (!SelectApp(aid))
            {
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "选择应用失败，交易流程终止");
                return;
            }
            var AFLs = GPOEx();
            if (AFLs.Count == 0)
            {
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "GPO命令发送失败，交易流程终止");
                return;
            }
            if (!ReadAppRecords(AFLs))
            {
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "读取应用记录失败，交易流程终止");
                return;
            }

            //Step 4, 此时卡片可以离开读卡器，终端进行后续的步骤
            if (isQPBOCTranction)
            {
                OfflineAuthcation();
            }
            else
            {
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "进行QPBOC交易失败");
                return;
            }
        }

        /// <summary>
        /// 选择应用
        /// </summary>
        /// <param name="aid"></param>
        /// <returns></returns>
        protected bool SelectApp(string aid)
        {
            bool result = false;
            ApduResponse response = base.SelectAid(aid);
            if(response.SW == 0x9000)
            {
                if(ParseAndSave(response.Response))
                {
                    IExcuteCase excuteCase = new SelectAidCase();
                    excuteCase.ExcuteCase(response);
                    result = true;
                }
            }
            else
            {
                var caseNo = MethodBase.GetCurrentMethod().Name;
                if(response.SW != 0x9000)
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "选择应用{0}失败,SW={1}", aid, response.SW);
                }             
            }
            return result;
        }

        /// <summary>
        /// 应用初始化
        /// </summary>
        /// <param name="pdol"></param>
        /// <returns></returns>
        protected List<AFL> GPOEx()
        {
            var AFLs = new List<AFL>();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var tls = DataParse.ParseTL(tagDict.GetTag("9F38"));

            var tlTags = from tl in tls select tl.Tag;
            if (curTransAlgorithmCategory == AlgorithmCategory.SM)
            {   //国密算法判断PDOL是否包含tagDF69 SM算法指示器

                if(!tlTags.Contains("DF69"))
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "国密算法，PDOL中缺少tagDF69");
                    return AFLs;
                }
            }
            string[] terminalTags = { "9F66", "9F37","9F02","5F2A" }; //QPBOC交易中，PDOL需包含关键tag值
            foreach(var tag in terminalTags)
            {
                if(!tlTags.Contains(tag))
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "PDOL中缺少tag{0}", tag);
                    return AFLs;
                }
            }
            
            string PDOLData = string.Empty;
            foreach (var tl in tls)
            {
                PDOLData += locator.Terminal.TermianlSettings.GetTag(tl.Tag);
            }

            ApduResponse response = base.GPO(PDOLData);           
            if (response.SW != 0x9000)
            {               
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "GPO命令发送失败，SW={0}", response.SW);
                return AFLs;
            }
            if(ParseAndSave(response.Response))
            {               
                string tag9F26 = tagDict.GetTag("9F26");
                string tag9F27 = tagDict.GetTag("9F27");
                if (!string.IsNullOrEmpty(tag9F26))
                {
                    isQPBOCTranction = true;    //表明卡片支持QPBOC交易
                }
                int cardAction = Convert.ToInt32(tag9F27, 16);
                if(cardAction != Constant.TC)
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "卡片拒绝此次交易，卡片代码[{0}]", tag9F27);
                    return AFLs;
                }
                AFLs = DataParse.ParseAFL(tagDict.GetTag("94"));
                IExcuteCase excuteCase = new GPOCase();
                excuteCase.ExcuteCase(response);
            }
            return AFLs;
        }

        /// <summary>
        /// 读记录
        /// </summary>
        /// <param name="afls"></param>
        /// <returns></returns>
        protected bool ReadAppRecords(List<AFL> afls)
        {
            var resps = base.ReadRecords(afls);
            var caseNo = MethodBase.GetCurrentMethod().Name;

            foreach (var resp in resps)
            {
                if(resp.SW != 0x9000)
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "读取应用记录失败,SW={0}", resp.SW);
                    return false;
                }
                if(!ParseAndSave(resp.Response))
                {
                    return false;
                }               
            }

            CheckTag9F10Mac();  //校验MAC值

            //IExcuteCase excuteCase = new ReadRecordCase();
            //excuteCase.ExcuteCase(resps);

            return true;
        }

        /// <summary>
        /// 脱机数据认证
        /// </summary>
        /// <returns></returns>
        protected int OfflineAuthcation()
        {
            IExcuteCase excuteCase = new CaseBase();
            var caseNo = MethodBase.GetCurrentMethod().Name;

            string issuerPublicKey = string.Empty;
            if (!SDA(ref issuerPublicKey))
            {
                excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "SDA脱机数据认证失败");
                return -3;
            }

            string tag9F37 = locator.Terminal.TermianlSettings.GetTag("9F37");
            string tag9F02 = locator.Terminal.TermianlSettings.GetTag("9F02");
            string tag5F2A = locator.Terminal.TermianlSettings.GetTag("5F2A");
            string tag9F69 = tagDict.GetTag("9F69");
            string ddolData = tag9F37 + tag9F02 + tag5F2A + tag9F69;
            string tag9F4B = tagDict.GetTag("9F4B");
            if (string.IsNullOrWhiteSpace(tag9F4B))
            {
                excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "Tag9F4B不存在");
                return -7;
            }

            if (!DDA(issuerPublicKey, tag9F4B, ddolData))
            {
                excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "DDA脱机数据认证失败");
                return -3;
            }
            return 0;
            
        }        
    }
}

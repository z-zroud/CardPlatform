using System;
using System.Collections.Generic;
using System.Linq;
using CardPlatform.ViewModel;
using CardPlatform.Cases;
using System.Reflection;
using CplusplusDll;
using CardPlatform.Config;
using CardPlatform.Models;

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
                TransResultModel TransactionResult = new TransResultModel(TransType.QPBOC_DES, TransResult.Unknown);
                TransactionResult.TransType = TransType.QPBOC_DES;
                locator.Terminal.TermianlSettings.TagDF69 = "00";   //SM算法支持指示位
                curTransAlgorithmCategory = AlgorithmCategory.DES;
                if (DoTransEx())
                {
                    TransactionResult.Result = TransResult.Sucess;
                }
                else
                {
                    TransactionResult.Result = TransResult.Failed;
                }
                locator.Transaction.TransResult.Add(TransactionResult);
            }
            //基于国密算法的交易流程
            if (doSMTrans)   
            {
                TransResultModel TransactionResult = new TransResultModel(TransType.QPBOC_SM, TransResult.Unknown);
                TransactionResult.TransType = TransType.QPBOC_SM;
                locator.Terminal.TermianlSettings.TagDF69 = "01";
                curTransAlgorithmCategory = AlgorithmCategory.SM;
                if (DoTransEx())
                {
                    TransactionResult.Result = TransResult.Sucess;
                }
                else
                {
                    TransactionResult.Result = TransResult.Failed;
                }
                locator.Transaction.TransResult.Add(TransactionResult);
            }
        }

        protected bool DoTransEx()
        {
            tagDict.Clear();    //做交易之前，需要将tag清空，避免与上次交易重叠
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (!SelectApp(aid))
            {
                baseCase.TraceInfo(TipLevel.Failed, caseNo, "选择应用失败，交易流程终止");
                return false;
            }
            var AFLs = GPOEx();
            if (AFLs.Count == 0)
            {
                baseCase.TraceInfo(TipLevel.Failed, caseNo, "GPO命令发送失败，交易流程终止");
                return false;
            }
            if (!ReadAppRecords(AFLs))
            {
                baseCase.TraceInfo(TipLevel.Failed, caseNo, "读取应用记录失败，交易流程终止");
                return false;
            }

            //Step 4, 此时卡片可以离开读卡器，终端进行后续的步骤
            if (isQPBOCTranction)
            {
                OfflineAuthcation();
            }
            else
            {
                baseCase.TraceInfo(TipLevel.Failed, caseNo, "进行QPBOC交易失败");
                return false;
            }
            return true;
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
                if(ParseTLVAndSave(response.Response))
                {
                    IExcuteCase excuteCase = new SelectAppCase();
                    excuteCase.ExcuteCase(response);
                    result = true;
                }
            }
            else
            {
                var caseNo = MethodBase.GetCurrentMethod().Name;
                if(response.SW != 0x9000)
                {
                    baseCase.TraceInfo(TipLevel.Failed, caseNo, "选择应用{0}失败,SW={1}", aid, response.SW);
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
                    baseCase.TraceInfo(TipLevel.Failed, caseNo, "国密算法，PDOL中缺少tagDF69");
                    return AFLs;
                }
            }
            string[] terminalTags = { "9F66", "9F37","9F02","5F2A" }; //QPBOC交易中，PDOL需包含关键tag值
            foreach(var tag in terminalTags)
            {
                if(!tlTags.Contains(tag))
                {
                    baseCase.TraceInfo(TipLevel.Failed, caseNo, "PDOL中缺少tag{0}", tag);
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
                baseCase.TraceInfo(TipLevel.Failed, caseNo, "GPO命令发送失败，SW={0}", response.SW);
                return AFLs;
            }
            if(ParseTLVAndSave(response.Response))
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
                    baseCase.TraceInfo(TipLevel.Failed, caseNo, "卡片拒绝此次交易，卡片代码[{0}]", tag9F27);
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
                    baseCase.TraceInfo(TipLevel.Failed, caseNo, "读取应用记录失败,SW={0}", resp.SW);
                    return false;
                }
                if(!ParseTLVAndSave(resp.Response))
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
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string AIP = tagDict.GetTag("82");
            if (IsSupportSDA(AIP))
            {
                if (!SDA())
                {
                    baseCase.TraceInfo(TipLevel.Failed, caseNo, "SDA脱机数据认证失败");
                    return -3;
                }
            }
            if (IsSupportDDA(AIP))
            {
                string ddol = tagDict.GetTag("9F49");
                string ddolData = "12345678";
                var tag9F4B = APDU.GenDynamicDataCmd(ddolData);
                if (string.IsNullOrWhiteSpace(tag9F4B))
                {
                    baseCase.TraceInfo(TipLevel.Failed, caseNo, "Tag9F4B不存在");
                    return -7;
                }
                string issuerPublicKey = GetIssuerPublicKey();
                if (!DDA(issuerPublicKey, tag9F4B, ddolData))
                {
                    baseCase.TraceInfo(TipLevel.Failed, caseNo, "DDA脱机数据认证失败");
                    return -3;
                }
            }
            return 0;
        }        
    }
}

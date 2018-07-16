using System;
using System.Collections.Generic;
using System.Linq;
using CardPlatform.ViewModel;
using CardPlatform.Cases;
using System.Reflection;
using CplusplusDll;
using CardPlatform.Config;
using CardPlatform.Models;
using CardPlatform.Helper;
using CardPlatform.Common;
using GalaSoft.MvvmLight.Threading;

namespace CardPlatform.Business
{

    /*************************************************************
     * 该类描述了QPBOC交易流程
     *************************************************************/
    public class BusinessQPBOC : BusinessBase
    {
        private TransactionTag transTags = TransactionTag.GetInstance();
        private ViewModelLocator locator = new ViewModelLocator();
        private bool isQPBOCTranction = false;

        /// <summary>
        /// 开始交易流程
        /// </summary>
        /// <param name="aid"></param>
        /// <param name="doDesTrans"></param>
        /// <param name="doSMTrans"></param>
        public override void DoTransaction(string aid, bool doDesTrans, bool doSmTrans)
        {
            transTags.Clear();    //做交易之前，需要将tag清空，避免与上次交易重叠
            base.DoTransaction(aid, doDesTrans, doSmTrans);
            locator.Terminal.TermianlSettings.Tag9C = "00";         //交易类型(消费交易)
            locator.Terminal.TermianlSettings.Tag9F66 = "2A000080"; //终端交易属性
            locator.Terminal.TermianlSettings.TagDF60 = "00";   //扩展交易指示位  

            if (doDesTrans)  // 做国际算法交易
            {
                DoTransaction(TransType.QPBOC_DES, DoTransactionEx);
            }
            if (doSMTrans)  //做国密算法交易
            {
                DoTransaction(TransType.QPBOC_SM, DoTransactionEx);
            }
        }

        protected bool DoTransactionEx()
        {
            transTags.Clear();    //做交易之前，需要将tag清空，避免与上次交易重叠
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (!SelectApp(aid))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "选择应用失败，交易流程终止");
                return false;
            }
            var AFLs = GPOEx();
            if (AFLs.Count == 0)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "GPO命令发送失败，交易流程终止");
                return false;
            }
            if (!ReadAppRecords(AFLs))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "读取应用记录失败，交易流程终止");
                return false;
            }

            GetRequirementData();

            //Step 4, 此时卡片可以离开读卡器，终端进行后续的步骤
            if (isQPBOCTranction)
            {
                OfflineAuthcation();
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "进行QPBOC交易失败");
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
            if (response.SW == 0x9000)
            {
                if (SaveTags(TransactionStep.SelectApp, response.Response))
                {
                    var stepCase = new SelectAppCase() { CurrentApp = Constant.APP_UICS };
                    stepCase.Excute(BatchNo, CurrentApp, TransactionStep.SelectApp, response);
                    result = true;
                }
            }
            else
            {
                var caseNo = MethodBase.GetCurrentMethod().Name;
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "选择应用{0}失败,SW={1}", aid, response.SW);
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
            var afls = new List<AFL>();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var tls = DataParse.ParseTL(transTags.GetTag("9F38"));

            var tlTags = from tl in tls select tl.Tag;
            if (curTransAlgorithmCategory == AlgorithmCategory.SM)
            {   //国密算法判断PDOL是否包含tagDF69 SM算法指示器

                if(!tlTags.Contains("DF69"))
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "国密算法，PDOL中缺少tagDF69");
                    return afls;
                }
            }
            string[] terminalTags = { "9F66", "9F37","9F02","5F2A" }; //QPBOC交易中，PDOL需包含关键tag值
            foreach(var tag in terminalTags)
            {
                if(!tlTags.Contains(tag))
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "PDOL中缺少tag{0}", tag);
                    return afls;
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
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "GPO命令发送失败，SW={0}", response.SW);
                return afls;
            }
            if(SaveTags(TransactionStep.GPO,response.Response))
            {               
                string tag9F26 = transTags.GetTag("9F26");
                string tag9F27 = transTags.GetTag("9F27");
                if (!string.IsNullOrEmpty(tag9F26))
                {
                    isQPBOCTranction = true;    //表明卡片支持QPBOC交易
                }
                int cardAction = Convert.ToInt32(tag9F27, 16);
                if(cardAction != Constant.TC)
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "卡片拒绝此次交易，卡片代码[{0}]", tag9F27);
                    return afls;
                }
                afls = DataParse.ParseAFL(transTags.GetTag("94"));
                IExcuteCase excuteCase = new GPOCase();
                excuteCase.Excute(BatchNo, CurrentApp, TransactionStep.GPO, response);
            }
            return afls;
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
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "读取应用记录失败,SW={0}", resp.SW);
                    return false;
                }
                if(!SaveTags(TransactionStep.ReadRecord,resp.Response))
                {
                    return false;
                }               
            }

            CheckTag9F10Mac();  //校验MAC值

            //IExcuteCase excuteCase = new ReadRecordCase();
            //excuteCaseExcute(resps);

            return true;
        }

        public override void GetRequirementData()
        {
            base.GetRequirementData();
            var caseNo = MethodBase.GetCurrentMethod().Name;

            RequirementData[] tagStandards =
            {
                new RequirementData("9F51",2,TipLevel.Failed,"如果执行频度检查，需要此应用货币代码"),  //如果执行频度检查，需要此应用货币代码
                new RequirementData("9F52",2,TipLevel.Failed,"如果支持发卡行认证，需要ADA应用缺省行为"),  //如果支持发卡行认证，需要ADA应用缺省行为
                new RequirementData("9F53",1,TipLevel.Failed,"如果执行国际货币频度检查，需要此连续脱机交易限制数(国际-货币)"),  //如果执行国际货币频度检查，需要此连续脱机交易限制数(国际-货币)
                new RequirementData("9F54",6,TipLevel.Failed,"如果执行国际国际频度检查，需要此连续脱机交易限制数(国际-国家)"),  //如果执行国际国际频度检查，需要此连续脱机交易限制数(国际-国家)
                new RequirementData("9F55",0,TipLevel.Warn,""),
                new RequirementData("9F56",1,TipLevel.Failed,"如果支持发卡行认证，需要此发卡行认证指示位"),  //如果支持发卡行认证，需要此发卡行认证指示位
                new RequirementData("9F57",2,TipLevel.Warn,"如果支持卡片频度检查，需要此发卡行国家代码"),  //如果支持卡片频度检查，需要此发卡行国家代码
                new RequirementData("9F58",1,TipLevel.Failed,"如果执行卡片频度检查，需要此连续脱机交易下限"),  //如果执行卡片频度检查，需要此连续脱机交易下限
                new RequirementData("9F59",1,TipLevel.Failed,"如果无法联机，卡片风险管理需要此连续脱机交易上限做出拒绝交易"),  //如果无法联机，卡片风险管理需要此连续脱机交易上限做出拒绝交易
                new RequirementData("9F5C",6,TipLevel.Failed,"累计脱机交易金额上限"),  //累计脱机交易金额上限
                new RequirementData("9F5D",0,TipLevel.Warn,""),
                new RequirementData("9F72",1,TipLevel.Warn,"连续脱机交易限制数"),  //连续脱机交易限制数
                new RequirementData("9F75",1,TipLevel.Warn,"累计脱机交易金额(双货币)"),  //累计脱机交易金额(双货币)
                new RequirementData("9F76",0,TipLevel.Failed,"第二应用货币代码"),  //第二应用货币代码
                new RequirementData("9F77",0,TipLevel.Failed,""),
                new RequirementData("9F78",0,TipLevel.Failed,""),
                new RequirementData("9F79",0,TipLevel.Failed,""),
                new RequirementData("9F4F",0,TipLevel.Failed,"交易日志格式"),  //交易日志格式
                new RequirementData("9F68",0,TipLevel.Failed,""),
                new RequirementData("9F6B",0,TipLevel.Warn,""),
                new RequirementData("9F6D",0,TipLevel.Failed,""),
                new RequirementData("9F36",2,TipLevel.Failed,"应用交易计数器"),  //应用交易计数器
                new RequirementData("9F13",0,TipLevel.Failed,"终端风险管理阶段需要此数据用于新卡检查，发卡行认证通过后，需要设置该值"),  //如果卡片或终端执行频度检查，或新卡检查，需要此上次联机应用交易计数器
                new RequirementData("9F17",0,TipLevel.Failed,"如果支持脱机PIN,需要此PIN尝试计数器"),  //如果支持脱机PIN,需要此PIN尝试计数器
                new RequirementData("DF4F",0,TipLevel.Warn,""),
                new RequirementData("DF62",0,TipLevel.Failed,""),
            };
            for (int i = 0; i < tagStandards.Length; i++)
            {
                var resp = APDU.GetDataCmd(tagStandards[i].Tag);
                if (resp.SW != 0x9000)
                {
                    caseObj.TraceInfo(tagStandards[i].Level, caseNo, "无法获取tag[{0}],返回码:[{1:X}]", tagStandards[i].Tag, resp.SW);
                }
                else
                {
                    var tlvs = DataParse.ParseTLV(resp.Response);
                    var tlv = from tmp in tlvs where tmp.Tag == tagStandards[i].Tag select tmp;

                    if (tagStandards[i].Len != 0)
                    {
                        if (tlv.First().Len != tagStandards[i].Len)
                        {
                            caseObj.TraceInfo(tagStandards[i].Level, caseNo, "tag[{0}]长度不匹配，标准规范为[{1}],实际长度为[{2}]", tagStandards[i].Tag, tagStandards[i].Len, tlv.First().Len);
                        }
                    }
                    transTags.SetTag(TransactionStep.GetData, tlv.First().Tag, tlv.First().Value); //保存
                }
            }
        }

        /// <summary>
        /// 脱机数据认证
        /// </summary>
        /// <returns></returns>
        protected int OfflineAuthcation()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string aip = transTags.GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(aip);
            if (!aipHelper.IsSupportSDA())
            {
                return 1;
            }
            if (!SDA())
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "SDA脱机数据认证失败");
                return 1;
            }
            if (!aipHelper.IsSupportDDA())
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "DDA脱机数据认证失败");
                return 1;
            }
            string ddol = transTags.GetTag(TransactionStep.ReadRecord, "9F49");
            string ddolData = "12345678";
            var tag9F4B = APDU.GenDynamicDataCmd(ddolData);
            if (string.IsNullOrWhiteSpace(tag9F4B))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "Tag9F4B不存在");
                return 1;
            }
            string issuerPublicKey = GetIssuerPublicKey();
            if (!DDA(issuerPublicKey, tag9F4B, ddolData))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "DDA脱机数据认证失败");
                return 1;
            }
            return 0;
        }        
    }
}

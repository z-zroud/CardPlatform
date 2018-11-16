using System;
using System.Collections.Generic;
using System.Linq;
using CardPlatform.ViewModel;
using CardPlatform.Cases;
using System.Reflection;
using CplusplusDll;
using CardPlatform.Config;
using CardPlatform.Models;
using CardPlatform.Common;

namespace CardPlatform.Business
{
    public class BusinessMC : BusinessBase
    {

        /// <summary>
        /// 开始UICS交易流程
        /// </summary>
        /// <param name="aid"></param>
        /// <param name="doDesTrans"></param>
        /// <param name="doSMTrans"></param>
        public override void DoTransaction(string aid)
        {
            base.DoTransaction(aid);
            locator.Terminal.SetTag("9C", "00", "交易类型");
            locator.Terminal.SetTag("9F66", "46000000", "终端交易属性");
            locator.Terminal.SetTag("9F08", "0002", "终端应用版本号");
            DoTransaction(DoTransactionEx);

        }

        private bool DoTransactionEx()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (!ApplicationSelection(currentAid))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "选择应用失败，交易流程终止");
                return false;
            }
            var ret = InitiateApplicationProcessing();
            if (!ret)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "GPO命令发送失败，交易流程终止");
                return false;
            }
            string tag94 = transTags.GetTag("94");
            if (!string.IsNullOrEmpty(tag94))
            {
                var afls = DataParse.ParseAFL(tag94);
                if (!ReadApplicationData(afls))
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "读取应用记录失败，交易流程终止");
                    return false;
                }
            }
            GetRequirementData();   //在脱机之前先进行必要数据的获取
            string aip = transTags.GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(aip);
            if (aipHelper.IsSupportSDA() || aipHelper.IsSupportDDA() || aipHelper.IsSupportCDA())
            {
                OfflineAuthcation();    //脱机数据认证
            }
            ProcessingRestrictions();         //处理限制
            if (aipHelper.IsSupportCardHolderVerify())
            {
                CardHolderVerification();     //持卡人验证
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Warn, caseNo, "卡片不支持此卡人认证，请确保tag82的正确性");
            }
            if(aipHelper.IsSupportTerminalRiskManagement())
            {
                TerminalRiskManagement();   //终端风险管理
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Warn, caseNo, "卡片不支持终端风险管理，请确保tag82的正确性");
            }
            if (0 != TerminalActionAnalyze())   //终端行为分析
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "终端行为分析失败，交易终止");
                return false;
            }

            if (0 != TransactionEnd())   //交易结束处理
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "交易结束处理失败，交易终止");
                return false;
            }

            return true;
        }

        /// <summary>
        /// 选择应用,只要发送选择AID命令成功返回0x9000,则表示成功
        /// </summary>
        /// <param name="aid"></param>
        /// <returns></returns>
        public new bool ApplicationSelection(string aid)
        {
            log.TraceLog("执行应用选择流程...");
            bool result = false;
            ApduResponse resp = base.ApplicationSelection(aid);
            if (resp.SW == 0x9000)
            {
                var tlvs = DataParse.ParseTLV(resp.Response);
                businessUtil.SaveTags(TransactionStep.SelectApp, tlvs);
                var selectAppCase = new SelectAppCase();
                selectAppCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.SelectApp, resp);
                result = true;
            }
            else
            {
                var caseNo = MethodBase.GetCurrentMethod().Name;
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "选择应用{0}失败,SW={1}", aid, resp.SW);
            }
            return result;
        }

        /// <summary>
        /// 应用初始化
        /// </summary>
        /// <param name="pdol"></param>
        /// <returns></returns>
        protected bool InitiateApplicationProcessing()
        {
            log.TraceLog("开始执行应用初始化流程检测...");
            var afls = new List<AFL>();
            var caseNo = MethodBase.GetCurrentMethod().Name;

            string tag9F38 = transTags.GetTag(TransactionStep.SelectApp, "9F38");
            var pdolData = businessUtil.GetDolData(tag9F38);
            var resp = base.InitiateApplicationProcessing(pdolData);
            if (resp.SW != 0x9000)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "GPO命令失败，SW={0}", resp.SW);
                return false;
            }
            var tlvs = DataParse.ParseTLV(resp.Response);
            businessUtil.SaveTags(TransactionStep.GPO, tlvs);

            var gpoCase = new GPOCase();
            gpoCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.GPO, resp);
            return true;
        }

        /// <summary>
        /// 读记录
        /// </summary>
        /// <param name="afls"></param>
        /// <returns></returns>
        protected new bool ReadApplicationData(List<AFL> afls)
        {
            log.TraceLog("开始执行读记录数据流程检测...");
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var resps = base.ReadApplicationData(afls);

            foreach (var resp in resps)
            {
                if (resp.SW != 0x9000)
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "读取应用记录失败,SW={0}", resp.SW);
                    return false;
                }
                var tlvs = DataParse.ParseTLV(resp.Response);
                businessUtil.SaveTags(TransactionStep.ReadRecord, tlvs);
            }

            IExcuteCase readRecordCase = new ReadRecordCase();
            readRecordCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.ReadRecord, resps);
            return true;
        }


        public void GetRequirementData()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;

            RequirementData[] tagStandards =
            {
                new RequirementData("C4",3,TipLevel.Failed,""),  //如果执行频度检查，需要此应用货币代码
                new RequirementData("C5",3,TipLevel.Failed,""),  //如果支持发卡行认证，需要ADA应用缺省行为
                new RequirementData("C3",3,TipLevel.Failed,""),  //如果执行国际货币频度检查，需要此连续脱机交易限制数(国际-货币)
                new RequirementData("D1",25,TipLevel.Failed,""),  //如果执行国际国际频度检查，需要此连续脱机交易限制数(国际-国家)
                new RequirementData("D3",18,TipLevel.Failed,""),  //如果支持发卡行认证，需要此发卡行认证指示位
                new RequirementData("D5",6,TipLevel.Failed,""),  //如果支持卡片频度检查，需要此发卡行国家代码
                new RequirementData("D6",2,TipLevel.Failed,""),  //如果执行卡片频度检查，需要此连续脱机交易下限
                new RequirementData("9F17",1,TipLevel.Failed,""),  //如果无法联机，卡片风险管理需要此连续脱机交易上限做出拒绝交易
                new RequirementData("9F14",1,TipLevel.Failed,""),  //累计脱机交易金额上限
                new RequirementData("9F23",1,TipLevel.Failed,""),  //连续脱机交易限制数
                new RequirementData("CA",6,TipLevel.Failed,""),  //累计脱机交易金额(双货币)
                new RequirementData("CB",6,TipLevel.Failed,""),  //第二应用货币代码
                new RequirementData("C7",1,TipLevel.Failed,""),  //交易日志格式
                new RequirementData("C8",2,TipLevel.Failed,""),
                new RequirementData("C9",2,TipLevel.Failed,""),  //应用交易计数器
                new RequirementData("9F4F",26,TipLevel.Failed,""),  //如果卡片或终端执行频度检查，或新卡检查，需要此上次联机应用交易计数器
                new RequirementData("9F7E",48,TipLevel.Failed,""),  //如果支持脱机PIN,需要此PIN尝试计数器
                new RequirementData("C6",1,TipLevel.Failed,""),  //如果支持脱机PIN,需要此PIN尝试计数器
               
            };
            for (int i = 0; i < tagStandards.Length; i++)
            {
                var resp = APDU.GetDataCmd(tagStandards[i].Tag);
                if (resp.SW != 0x9000)
                {
                    caseObj.TraceInfo(tagStandards[i].Level, caseNo, "{0},缺少Tag{1},", tagStandards[i].Desc, tagStandards[i].Tag);
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
            if (aipHelper.IsSupportSDA())   //如果支持SDA
            {
                if (!SDA())
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "SDA脱机数据认证失败");
                    return 1;
                }
            }

            if (!aipHelper.IsSupportDDA())  //必须支持DDA
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


        /// <summary>
        /// 终端行为分析，1，终端检查脱机处理结果，决定交易是拒绝/接受/联机(默认联机)
        /// 2. 终端请求卡片生成密文
        /// </summary>
        /// <returns></returns>
        protected int TerminalActionAnalyze()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string CDOL1 = transTags.GetTag(TransactionStep.ReadRecord, "8C");
            ApduResponse resp = GAC(Constant.ARQC, CDOL1);
            if (resp.SW != 0x9000)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "第一次发送GAC命令失败,返回{0:4X}", resp.SW);
                return -1;
            }
            var tlvs = DataParse.ParseTLV(resp.Response);
            transTags.SetTags(TransactionStep.TerminalActionAnalyze, tlvs);

            var terminalActionAnalyzeCase = new TerminalActionAnalyzeCase();
            terminalActionAnalyzeCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.TerminalActionAnalyze, resp);
            return 0;
        }

        protected int TransactionEnd()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;

            if (string.IsNullOrEmpty(transCfg.TransDesAcKey))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "国际算法UDK/MDK不存在");
                return 1;
            }
            string tag9F36 = transTags.GetTag("9F36");
            string acSessionKey = Authencation.GenMcSessionKeyCsk(transCfg.TransDesAcKey, tag9F36);

            string applicationCryptogram = transTags.GetTag(TransactionStep.TerminalActionAnalyze, "9F26");
            string arc = "0012";
            string arpc = Authencation.GenArpc(acSessionKey, applicationCryptogram, arc);

            locator.Terminal.SetTag("91", arpc + arc, "Issuer Authentication Data");
            locator.Terminal.SetTag("8A", "0012");

            string CDOL2 = transTags.GetTag(TransactionStep.ReadRecord, "8D");
            ApduResponse resp = GAC(Constant.TC, CDOL2);
            var tlvs = DataParse.ParseTLV(resp.Response);
            transTags.SetTags(TransactionStep.TerminalActionAnalyze, tlvs);

            var transactionEndCase = new TransactionEndCase();
            transactionEndCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.TransactionEnd, resp);

            return 0;
        }
    }
}

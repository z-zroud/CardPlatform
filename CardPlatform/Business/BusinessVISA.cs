﻿using System;
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
    public class BusinessVISA : BusinessBase
    {
        /// <summary>
        /// 开始UICS交易流程
        /// </summary>
        /// <param name="aid"></param>
        /// <param name="doDesTrans"></param>
        /// <param name="doSMTrans"></param>
        public override void DoTransaction(string aid)
        {
            transTags.Clear();    //做交易之前，需要将tag清空，避免与上次交易重叠
            base.DoTransaction(aid);
            locator.Terminal.SetTag("9F7A", "00", "电子现金支持指示器(这里走借贷记交易流程)");
            locator.Terminal.SetTag("9C", "00", "交易类型");
            locator.Terminal.SetTag("9F66", "46000000", "终端交易属性");
            DoTransaction(DoTransactionEx);
        }

        private bool DoTransactionEx()
        {
            log.TraceLog(LogLevel.Info, "========================= Begin Visa transaction  =========================\n");
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (!ApplicationSelection(currentAid))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "选择应用失败，交易流程终止");
                return false;
            }
            var afls = InitiateApplicationProcessing();
            if (afls.Count == 0)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "GPO命令发送失败，交易流程终止");
                return false;
            }
            if (!ReadApplicationData(afls))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "读取应用记录失败，交易流程终止");
                return false;
            }
            GetRequirementData();   //在脱机之前先进行必要数据的获取
            string aip = transTags.GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(aip);
            if (aipHelper.IsSupportSDA() || aipHelper.IsSupportDDA() || aipHelper.IsSupportCDA())
            {
                OfflineDataAuthentication();    //脱机数据认证
            }
            ProcessingRestrictions();         //处理限制
            if (aipHelper.IsSupportCardHolderVerify())
            {
                CardHolderVerification();     //持卡人验证
            }
            TerminalRiskManagement();   //终端风险管理
            if (0 != TerminalActionAnalysis())   //终端行为分析
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "终端行为分析失败，交易终止");
                return false;
            }
            if (0 != IssuerAuthencation())   //发卡行认证
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "联机处理失败，交易终止");
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
        protected bool ApplicationSelection(string aid)
        {
            log.TraceLog(LogLevel.Info, "========================= Application Selection  =========================");
            bool result = false;
            ApduResponse resp = base.SelectAid(aid);
            if (resp.SW == 0x9000)
            {
                if (SaveTags(TransactionStep.SelectApp, resp.Response))
                {
                    var stepCase = new SelectAppCase() { CurrentApp = Constant.APP_VISA };
                    stepCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.SelectApp, resp);
                    result = true;
                }
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
        protected List<AFL> InitiateApplicationProcessing()
        {
            log.TraceLog(LogLevel.Info, "========================= Initiate Application Processing  =========================");
            var afls = new List<AFL>();
            var caseNo = MethodBase.GetCurrentMethod().Name;

            string tag9F38 = transTags.GetTag(TransactionStep.SelectApp, "9F38");
            var pdolData = GetDolData(tag9F38);
            var response = base.GPO(pdolData);
            if (response.SW != 0x9000)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "GPO命令失败，SW={0}", response.SW);
                return afls;
            }
            var tlvs = DataParse.ParseTLV(response.Response);
            if (tlvs.Count != 1 || tlvs[0].Value.Length <= 4)
            {
                return afls;
            }
            transTags.SetTag(TransactionStep.GPO, "82", tlvs[0].Value.Substring(0, 4));
            transTags.SetTag(TransactionStep.GPO, "94", tlvs[0].Value.Substring(4));
            afls = DataParse.ParseAFL(transTags.GetTag(TransactionStep.GPO, "94"));
            var gpoCase = new GPOCase() { CurrentApp = Constant.APP_VISA };
            gpoCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.GPO, response);
            return afls;
        }

        /// <summary>
        /// 读记录
        /// </summary>
        /// <param name="afls"></param>
        /// <returns></returns>
        protected bool ReadApplicationData(List<AFL> afls)
        {
            log.TraceLog(LogLevel.Info, "========================= Read Application Data  =========================");
            var resps = base.ReadRecords(afls);
            var caseNo = MethodBase.GetCurrentMethod().Name;

            foreach (var resp in resps)
            {
                if (resp.SW != 0x9000)
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "读取应用记录失败,SW={0}", resp.SW);
                    return false;
                }
                if (!SaveTags(TransactionStep.ReadRecord, resp.Response))
                {
                    return false;
                }
            }
            IExcuteCase readRecordCase = new ReadRecordCase() { CurrentApp = Constant.APP_VISA };
            readRecordCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.ReadRecord, resps);
            return true;
        }


        public override void GetRequirementData()
        {
            base.GetRequirementData();
            var caseNo = MethodBase.GetCurrentMethod().Name;

            RequirementData[] tagStandards =
            {
                //new RequirementData("9F51",2,TipLevel.Warn,"如果卡片执行连续脱机交易-国际货币频度检查或累计金额(包括双币)频度检测，需要此应用货币代码"),  //如果执行频度检查，需要此应用货币代码
                //new RequirementData("9F52",2,TipLevel.Failed,"如果支持发卡行认证，需要ADA应用缺省行为"),  //如果支持发卡行认证，需要ADA应用缺省行为
                //new RequirementData("9F53",1,TipLevel.Failed,"如果执行国际货币频度检查，需要此连续脱机交易限制数(国际-货币)"),  //如果执行国际货币频度检查，需要此连续脱机交易限制数(国际-货币)
                //new RequirementData("9F54",6,TipLevel.Failed,"如果执行国际国际频度检查，需要此连续脱机交易限制数(国际-国家)"),  //如果执行国际国际频度检查，需要此连续脱机交易限制数(国际-国家)
                //new RequirementData("9F56",1,TipLevel.Failed,"如果支持发卡行认证，需要此发卡行认证指示位"),  //如果支持发卡行认证，需要此发卡行认证指示位
                //new RequirementData("9F57",2,TipLevel.Warn,"如果支持卡片频度检查，需要此发卡行国家代码"),  //如果支持卡片频度检查，需要此发卡行国家代码
                //new RequirementData("9F58",1,TipLevel.Failed,"如果执行卡片频度检查，需要此连续脱机交易下限"),  //如果执行卡片频度检查，需要此连续脱机交易下限
                //new RequirementData("9F59",1,TipLevel.Failed,"如果无法联机，卡片风险管理需要此连续脱机交易上限做出拒绝交易"),  //如果无法联机，卡片风险管理需要此连续脱机交易上限做出拒绝交易
                //new RequirementData("9F5C",6,TipLevel.Failed,"累计脱机交易金额上限"),  //累计脱机交易金额上限
                //new RequirementData("9F72",1,TipLevel.Warn,"连续脱机交易限制数,如果执行国际-国家频度检查,该数据必须存在"),  //连续脱机交易限制数
                //new RequirementData("9F75",1,TipLevel.Warn,"累计脱机交易金额(双货币)"),  //累计脱机交易金额(双货币)
                //new RequirementData("9F76",0,TipLevel.Warn,"第二应用货币代码"),  //第二应用货币代码
                //new RequirementData("9F4F",0,TipLevel.Failed,"交易日志格式"),  //交易日志格式
                //new RequirementData("9F68",0,TipLevel.Warn,"缺少卡片附加处理选项"),
                //new RequirementData("9F36",2,TipLevel.Failed,"应用交易计数器"),  //应用交易计数器
                //new RequirementData("9F13",0,TipLevel.Failed,"终端风险管理阶段需要此数据用于新卡检查，发卡行认证通过后，需要设置该值"),  //如果卡片或终端执行频度检查，或新卡检查，需要此上次联机应用交易计数器
                //new RequirementData("9F17",0,TipLevel.Warn,"如果支持脱机PIN,需要此PIN尝试计数器"),  //如果支持脱机PIN,需要此PIN尝试计数器
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
        protected int OfflineDataAuthentication()
        {
            log.TraceLog(LogLevel.Info, "========================= Offline Data Authentication  =========================");
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string aip = transTags.GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(aip);
            if (aipHelper.IsSupportSDA())
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "VISA不应支持SDA脱机数据认证");
                if (!SDA())
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "SDA脱机数据认证失败");
                    return 1;
                }
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

        /// <summary>
        /// 处理限制,
        /// </summary>
        /// <returns></returns>
        protected int ProcessingRestrictions()
        {
            log.TraceLog(LogLevel.Info, "========================= Processing Restrictions  =========================");
            var handleLimitationCase = new ProcessRestrictionCase() { CurrentApp = Constant.APP_VISA };
            handleLimitationCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.ProcessRestriction, null);
            return 0;
        }

        /// <summary>
        /// 持卡人认证
        /// </summary>
        /// <returns></returns>
        protected int CardHolderVerification()
        {
            log.TraceLog(LogLevel.Info, "========================= Cardholder Verification  =========================");
            var cardHolderVerifyCase = new CardHolderVerifyCase() { CurrentApp = Constant.APP_VISA };
            cardHolderVerifyCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.CardHolderVerify, null);
            return 0;
        }

        /// <summary>
        /// 终端风险管理为大额交易提供了发卡行授权，确保卡片交易可以周期性的
        /// 进行联机处理。
        /// </summary>
        /// <returns></returns>
        protected int TerminalRiskManagement()
        {
            log.TraceLog(LogLevel.Info, "========================= Terminal Risk Management  =========================");
            var terminalRishManagementCase = new TerminalRiskManagementCase() { CurrentApp = Constant.APP_VISA };
            terminalRishManagementCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.TerminalRiskManagement, null);
            return 0;
        }

        /// <summary>
        /// 终端行为分析:终端根据发卡行设置在卡片里的规则，基于脱机数据认证处理结果
        /// 决定交易是拒绝/接受/联机(默认联机)
        /// 终端行为分析包括以下两步骤：
        /// 1. 查看脱机数据处理结果
        /// 2. 请求密文
        /// 终端完成请求密文命令后，卡片执行卡片风险管理，然后返回应用密文给终端
        /// </summary>
        /// <returns></returns>
        protected int TerminalActionAnalysis()
        {
            log.TraceLog(LogLevel.Info, "========================= Terminal Action Analysis  =========================");
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string CDOL1 = transTags.GetTag(TransactionStep.ReadRecord, "8C");
            ApduResponse resp = FirstGAC(Constant.ARQC, CDOL1);
            if (resp.SW != 0x9000)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "第一次发送GAC命令失败,返回{0:4X}", resp.SW);
                return -1;
            }
            var tlvs = DataParse.ParseTLV(resp.Response);
            if (tlvs.Count > 0 && tlvs[0].Tag == "80")
            {
                string result = tlvs[0].Value;  //第一次GAC返回的数据

                string tag9F27 = result.Substring(0, 2);
                string tag9F36 = result.Substring(2, 4);
                string tag9F26 = result.Substring(6, 16);
                string tag9F10 = result.Substring(22);

                transTags.SetTag(TransactionStep.TerminalActionAnalyze, "9F27", tag9F27);
                transTags.SetTag(TransactionStep.TerminalActionAnalyze, "9F36", tag9F36);
                transTags.SetTag(TransactionStep.TerminalActionAnalyze, "9F26", tag9F26);
                transTags.SetTag(TransactionStep.TerminalActionAnalyze, "9F10", tag9F10);
            }
            var terminalActionAnalyzeCase = new TerminalActionAnalyzeCase() { CurrentApp = Constant.APP_VISA };
            terminalActionAnalyzeCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.TerminalActionAnalyze, resp);
            return 0;
        }

        protected int IssuerAuthencation()
        {
            log.TraceLog(LogLevel.Info, "========================= Online Processing  =========================");
            var caseNo = MethodBase.GetCurrentMethod().Name;

            if (string.IsNullOrEmpty(transCfg.TransDesAcKey))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "国际算法UDK/MDK不存在");
                return 1;
            }

            string ac = transTags.GetTag(TransactionStep.TerminalActionAnalyze, "9F26");
            string arpc = Authencation.GenArpc(transCfg.TransDesAcKey, ac, "3030", (int)AlgorithmType.DES);
            var resp = APDU.ExtAuthCmd(arpc, "3030");
            if (resp.SW != 0x9000)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "ARPC校验不正确");
                return 1;
            }
            caseObj.TraceInfo(TipLevel.Sucess, caseNo, "发卡行认证成功");
            locator.Terminal.SetTag("8A", "3030");
            return 0;
        }

        protected int TransactionEnd()
        {
            log.TraceLog(LogLevel.Info, "========================= Completion Processing  =========================");
            string CDOL2 = transTags.GetTag(TransactionStep.ReadRecord, "8D");
            ApduResponse resp = SecondGAC(Constant.TC, CDOL2);
            var tlvs = DataParse.ParseTLV(resp.Response);
            if (tlvs.Count > 0 && tlvs[0].Tag == "80")
            {
                string result = tlvs[0].Value;  //第一次GAC返回的数据

                string tag9F27 = result.Substring(0, 2);
                string tag9F36 = result.Substring(2, 4);
                string tag9F26 = result.Substring(6, 16);
                string tag9F10 = result.Substring(22);

                transTags.SetTag(TransactionStep.TerminalActionAnalyze, "9F27", tag9F27);
                transTags.SetTag(TransactionStep.TerminalActionAnalyze, "9F36", tag9F36);
                transTags.SetTag(TransactionStep.TerminalActionAnalyze, "9F26", tag9F26);
                transTags.SetTag(TransactionStep.TerminalActionAnalyze, "9F10", tag9F10);
            }
            var transactionEndCase = new TransactionEndCase() { CurrentApp = Constant.APP_VISA };
            transactionEndCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.TransactionEnd, resp);

            return 0;
        }
    }
}

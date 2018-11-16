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
            base.DoTransaction(aid);
            locator.Terminal.SetTag("9F7A", "00", "电子现金支持指示器(这里走借贷记交易流程)");
            locator.Terminal.SetTag("9C", "00", "交易类型");
            locator.Terminal.SetTag("9F09", "00A0", "终端应用版本号");
            locator.Terminal.SetTag("9F66", "46000000", "终端交易属性");
            DoTransaction(DoTransactionEx);
        }

        private bool DoTransactionEx()
        {
            log.TraceLog("开始执行VISA借贷记交易流程...\n");
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
            var response = base.InitiateApplicationProcessing(pdolData);
            if (response.SW != 0x9000)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "GPO命令失败，SW={0}", response.SW);
                return false;
            }
            var tlvs = DataParse.ParseTLV(response.Response);
            if (tlvs.Count != 1 || tlvs[0].Value.Length <= 4)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "请检查GPO响应数据格式是否正确");
                return false;
            }
            transTags.SetTag(TransactionStep.GPO, "82", tlvs[0].Value.Substring(0, 4));
            transTags.SetTag(TransactionStep.GPO, "94", tlvs[0].Value.Substring(4));
            var gpoCase = new GPOCase();
            gpoCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.GPO, response);
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
            IExcuteCase getDataCase = new GetDataCase();
            getDataCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.GetData, null);
        }

        /// <summary>
        /// 脱机数据认证
        /// </summary>
        /// <returns></returns>
        protected bool OfflineDataAuthentication()
        {
            log.TraceLog(LogLevel.Info, "开始脱机数据认证流程检测...");
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string aip = transTags.GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(aip);

            if (!aipHelper.IsSupportDDA())
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "该卡片不支持DDA脱机数据认证，请检查tag82是否符合要求");
                return false;
            }
            string ddol = transTags.GetTag(TransactionStep.ReadRecord, "9F49");
            string ddolData = "12345678";
            var tag9F4B = APDU.GenDynamicDataCmd(ddolData);
            if (string.IsNullOrWhiteSpace(tag9F4B))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "无法获取tag9F4B,请检查内部认证命令是否成功");
                return false;
            }
            string issuerPublicKey = GetIssuerPublicKey();
            if (!DDA(issuerPublicKey, tag9F4B, ddolData))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "DDA脱机数据认证失败");
                return false;
            }
            return true;
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
            ApduResponse resp = GAC(Constant.ARQC, CDOL1);
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
            var terminalActionAnalyzeCase = new TerminalActionAnalyzeCase();
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
            ApduResponse resp = GAC(Constant.TC, CDOL2);
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
            var transactionEndCase = new TransactionEndCase();
            transactionEndCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.TransactionEnd, resp);

            return 0;
        }
    }
}

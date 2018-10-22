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
    public enum CardAction
    {
        TC,
        ARQC,
        AAC
    }

    public class BusinessqVSDC : BusinessBase
    {
        /// <summary>
        /// 开始交易流程
        /// </summary>
        /// <param name="aid"></param>
        /// <param name="doDesTrans"></param>
        /// <param name="doSMTrans"></param>
        public override void DoTransaction(string aid)
        {
            base.DoTransaction(aid);
            locator.Terminal.SetTag("9C", "00", "交易类型(消费交易)");
            if(transCfg.CurrentApp == AppType.qVSDC_offline)
            {
                locator.Terminal.SetTag("9F66", "26000000", "终端交易属性");
            }
            else
            {
                locator.Terminal.SetTag("9F66", "27800000", "终端交易属性");
            }
            
            DoTransaction(DoTransactionEx);
        }

        protected bool DoTransactionEx()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (!ApplicationSelection(currentAid))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "选择应用失败，交易流程终止");
                return false;
            }
            if (!InitiateApplication())
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "应用初始化失败，交易流程终止");
                return false;
            }
            string tag94 = transTags.GetTag("94");
            if(!string.IsNullOrEmpty(tag94))
            {
                var afls = DataParse.ParseAFL(tag94);
                if (!ReadApplicationData(afls))
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "读取应用记录失败，交易流程终止");
                    return false;
                }

                GetRequirementData();

                //Step 4, 此时卡片可以离开读卡器，终端进行后续的步骤

                OfflineAuthcation();
            }


            return true;
        }

        /// <summary>
        /// 选择应用
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
        protected bool InitiateApplication()
        {
            var afls = new List<AFL>();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string tag9F38 = transTags.GetTag("9F38");
            if(string.IsNullOrEmpty(tag9F38))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "选择应用响应没返回PDOL数据");
                return false;
            }
            var tls = DataParse.ParseTL(tag9F38);
            string pdolData = string.Empty;
            bool hasTag9F66 = false;
            foreach (var tl in tls)
            {
                if (tl.Tag == "9F66")
                    hasTag9F66 = true;
                pdolData += locator.Terminal.GetTag(tl.Tag);
            }
            if(!hasTag9F66)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "PDOL中不包含tag9F66终端交易属性");
                return false;
            }

            ApduResponse response = base.InitiateApplicationProcessing(pdolData);
            if (response.SW != 0x9000)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "GPO命令发送失败，SW={0}", response.SW);
                return false;
            }
            var tlvs = DataParse.ParseTLV(response.Response);
            businessUtil.SaveTags(TransactionStep.GPO, tlvs);
            string tag9F27 = transTags.GetTag("9F27");
            string tag94 = transTags.GetTag("94");
            if(transCfg.CurrentApp == AppType.qVSDC_offline)
            {
                if(tag9F27 != "40")
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "期望执行qVSDC脱机交易，卡片行为不一致，请检查卡片是否支持脱机交易");
                    return false;
                }
            }else if(transCfg.CurrentApp == AppType.qVSDC_online)
            {
                if(tag9F27 != "80")
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "期望执行qVSDC联机交易，卡片行为不一致，请检查卡片是否支持联机ODA");
                    return false;
                }
            }
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
            var caseNo = MethodBase.GetCurrentMethod().Name;

            RequirementData[] tagStandards =
            {
                //new RequirementData("9F51",2,TipLevel.Failed,"如果执行频度检查，需要此应用货币代码"),  //如果执行频度检查，需要此应用货币代码
                //new RequirementData("9F52",2,TipLevel.Failed,"如果支持发卡行认证，需要ADA应用缺省行为"),  //如果支持发卡行认证，需要ADA应用缺省行为
                //new RequirementData("9F53",1,TipLevel.Failed,"如果执行国际货币频度检查，需要此连续脱机交易限制数(国际-货币)"),  //如果执行国际货币频度检查，需要此连续脱机交易限制数(国际-货币)
                //new RequirementData("9F54",6,TipLevel.Failed,"如果执行国际国际频度检查，需要此连续脱机交易限制数(国际-国家)"),  //如果执行国际国际频度检查，需要此连续脱机交易限制数(国际-国家)
                //new RequirementData("9F55",0,TipLevel.Warn,""),
                //new RequirementData("9F56",1,TipLevel.Failed,"如果支持发卡行认证，需要此发卡行认证指示位"),  //如果支持发卡行认证，需要此发卡行认证指示位
                //new RequirementData("9F57",2,TipLevel.Warn,"如果支持卡片频度检查，需要此发卡行国家代码"),  //如果支持卡片频度检查，需要此发卡行国家代码
                //new RequirementData("9F58",1,TipLevel.Failed,"如果执行卡片频度检查，需要此连续脱机交易下限"),  //如果执行卡片频度检查，需要此连续脱机交易下限
                //new RequirementData("9F59",1,TipLevel.Failed,"如果无法联机，卡片风险管理需要此连续脱机交易上限做出拒绝交易"),  //如果无法联机，卡片风险管理需要此连续脱机交易上限做出拒绝交易
                //new RequirementData("9F5C",6,TipLevel.Failed,"累计脱机交易金额上限"),  //累计脱机交易金额上限
                //new RequirementData("9F5D",0,TipLevel.Warn,""),
                //new RequirementData("9F72",1,TipLevel.Warn,"连续脱机交易限制数"),  //连续脱机交易限制数
                //new RequirementData("9F75",1,TipLevel.Warn,"累计脱机交易金额(双货币)"),  //累计脱机交易金额(双货币)
                //new RequirementData("9F76",0,TipLevel.Failed,"第二应用货币代码"),  //第二应用货币代码
                //new RequirementData("9F77",0,TipLevel.Failed,""),
                //new RequirementData("9F78",0,TipLevel.Failed,""),
                //new RequirementData("9F79",0,TipLevel.Failed,""),
                //new RequirementData("9F4F",0,TipLevel.Failed,"交易日志格式"),  //交易日志格式
                //new RequirementData("9F68",0,TipLevel.Failed,""),
                //new RequirementData("9F6B",0,TipLevel.Warn,""),
                //new RequirementData("9F6D",0,TipLevel.Failed,""),
                //new RequirementData("9F36",2,TipLevel.Failed,"应用交易计数器"),  //应用交易计数器
                //new RequirementData("9F13",0,TipLevel.Failed,"终端风险管理阶段需要此数据用于新卡检查，发卡行认证通过后，需要设置该值"),  //如果卡片或终端执行频度检查，或新卡检查，需要此上次联机应用交易计数器
                //new RequirementData("9F17",0,TipLevel.Failed,"如果支持脱机PIN,需要此PIN尝试计数器"),  //如果支持脱机PIN,需要此PIN尝试计数器
                //new RequirementData("DF4F",0,TipLevel.Warn,""),
                //new RequirementData("DF62",0,TipLevel.Failed,""),
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

        public void ProcessRestriction()
        {
            var processRestrictionCase = new ProcessRestrictionCase() ;
            processRestrictionCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.ProcessRestriction, null);
        }

        /// <summary>
        /// 脱机数据认证
        /// </summary>
        /// <returns></returns>
        protected int OfflineAuthcation()
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

            var tag9F4B = transTags.GetTag("9F4B");
            if (string.IsNullOrWhiteSpace(tag9F4B))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "Tag9F4B不存在");
                return 1;
            }
            string issuerPublicKey = GetIssuerPublicKey();
            string tag9F37 = locator.Terminal.GetTag("9F37");
            string tag9F02 = locator.Terminal.GetTag("9F02");
            string tag5F2A = locator.Terminal.GetTag("5F2A");
            string tag9F69 = transTags.GetTag("9F69");
            if (string.IsNullOrEmpty(tag9F69))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "Tag9F69不存在");
                return 1;
            }
            if (string.IsNullOrEmpty(issuerPublicKey))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "无法获取发卡行公钥");
                return 1;
            }
            string dynamicData = tag9F37 + tag9F02 + tag5F2A + tag9F69;
            if (!DDA(issuerPublicKey, tag9F4B, dynamicData))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "fDDA脱机数据认证失败");
                return 1;
            }
            return 0;
        }
    }
}

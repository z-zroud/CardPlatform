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
    public class BusinessECC : BusinessBase
    {
        private bool isEccTranction         = false;
        private bool isSupportCDA           = false;
        private string pdolData             = string.Empty;
        
        /// <summary>
        /// 开始交易流程，该交易流程仅包含国际/国密电子现金的消费交易，暂不包含圈存
        /// </summary>
        /// <param name="aid"></param>
        /// <param name="doDesTrans"></param>
        /// <param name="doSMTrans"></param>
        public override void DoTransaction(string aid)
        {
            base.DoTransaction(aid);     
            locator.Terminal.SetTag("9F7A", "01", "电子现金交易指示器");
            locator.Terminal.SetTag("9C", "00", "交易类型(消费)");
            DoTransaction(DoTransactionEx);
        }

        protected bool DoTransactionEx()
        {
            pdolData = string.Empty;    //PDOL Data也需要清空，防止做CDA时，出现重叠
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
            GetRequirementData();
           
            var tag9F74 = transTags.GetTag("9F74");
            if (tag9F74.Length == 12)
            {
                var resp = APDU.GetDataCmd("9F79");
                //SaveTags(TransactionStep.GetData, resp.Response);
                resp = APDU.GetDataCmd("9F6D");
                //SaveTags(TransactionStep.GetData, resp.Response);
                var tag9F79 = transTags.GetTag("9F79");
                if (int.Parse(tag9F79) > 0)
                {
                    isEccTranction = true;  //说明是电子现金交易
                }
            }
            if (isEccTranction)
            {
                string aip = transTags.GetTag(TransactionStep.GPO, "82");
                var aipHelper = new AipHelper(aip);
                if (aipHelper.IsSupportSDA() || aipHelper.IsSupportDDA() || aipHelper.IsSupportCDA())
                {
                    OfflineAuthcation();    //脱机数据认证
                }

                ProcessingRestrictions();
                if (aipHelper.IsSupportCardHolderVerify())
                {
                    CardHolderVerification();     //持卡人验证
                }

                TerminalRiskManagement();
                if (0 != TerminalActionAnalyze())
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "终端行为分析失败，交易终止");
                    return false;
                }
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "此交易不是脱机电子现金交易");
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
                    transTags.SetTag(TransactionStep.GetData,tlv.First().Tag, tlv.First().Value); //保存
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

        protected int TerminalActionAnalyze()
        {
            //如果卡片支持CDA，在执行终端行为分析之前获取发卡行公钥、IC卡公钥
            string iccPulicKey = string.Empty;
            if(isSupportCDA)
            {
                string issuerPublicKey = GetIssuerPublicKey();
                iccPulicKey = GetIccPublicKey(issuerPublicKey);
            }
            string CDOL1 = transTags.GetTag("8C");
            var response = new ApduResponse();
            if(isSupportCDA)
            {
                string CDOL1Data = string.Empty;
                var tls = DataParse.ParseTL(CDOL1);
                foreach (var tl in tls)
                {
                    CDOL1Data += locator.Terminal.GetTag(tl.Tag);
                }
                response = APDU.GACCmd(Constant.TC_CDA, CDOL1Data);
                var tlvs = DataParse.ParseTLV(response.Response);
                businessUtil.SaveTags(TransactionStep.TerminalActionAnalyze, tlvs);
                if(transCfg.Algorithm == AlgorithmType.DES)
                {
                    string tag9F4B = transTags.GetTag("9F4B");
                    string exp = transTags.GetTag("9F47");
                    string recoveryData = Authencation.GenRecoveryData(iccPulicKey, exp, tag9F4B);
                    if (recoveryData.Length == 0 || !recoveryData.StartsWith("6A05"))
                    {

                    }
                    string recoveryHash = recoveryData.Substring(recoveryData.Length - 42, 40);
                    string hashInput = recoveryData.Substring(2, recoveryData.Length - 44);
                    hashInput += locator.Terminal.GetTag("9F37");
                    string hash = Authencation.GetHash(hashInput);
                    if (hash != recoveryHash)
                    {

                    }
                    string recoveryTag9F36 = recoveryData.Substring(10, 4);
                    string recoveryTag9F27 = recoveryData.Substring(14, 2);
                    string recoveryTag9F26 = recoveryData.Substring(16, 16);
                    string recoveryHash2 = recoveryData.Substring(32, 40);
                    string hashInput2 = pdolData + CDOL1Data;
                    string[] tags = { "9F27", "9F36", "9F10" };
                    foreach (var tag in tags)
                    {
                        string tagValue = transTags.GetTag(tag);
                        string len = UtilLib.Utils.IntToHexStr(tagValue.Length / 2, 2);
                        hashInput2 += tag + len + tagValue;
                    }
                    string hash2 = Authencation.GetHash(hashInput2);
                    if (hash2 != recoveryHash2)
                    {

                    }
                }
                else
                {
                    var tag9F4B = transTags.GetTag("9F4B");
                    if(string.IsNullOrEmpty(tag9F4B))
                    {
                        return -1;
                    }
                    if(!tag9F4B.StartsWith("15"))
                    {
                        return -2;
                    }
                    int icDynamicLen = Convert.ToInt32(tag9F4B.Substring(2, 2), 16);
                    if(icDynamicLen < 44)
                    {
                        return -2;
                    }
                    int iccDynamicDigitLen = Convert.ToInt32(tag9F4B.Substring(4, 2));
                    string dynamicDigit = tag9F4B.Substring(6, iccDynamicDigitLen * 2);
                    string cryptoInfoData = tag9F4B.Substring(6 + iccDynamicDigitLen * 2, 2);
                    string ac = tag9F4B.Substring(8 + iccDynamicDigitLen * 2, 16);
                    string hash = tag9F4B.Substring(24 + iccDynamicDigitLen * 2, 64);
                    string hashInput = pdolData + CDOL1Data;
                    string[] tags = { "9F27", "9F36", "9F10" };
                    foreach (var tag in tags)
                    {
                        string tagValue = transTags.GetTag(tag);
                        string len = UtilLib.Utils.IntToHexStr(tagValue.Length / 2, 2);
                        hashInput += tag + len + tagValue;
                    }
                    string verfiyHash = Authencation.GetSMHash(hashInput);
                    if(hash != verfiyHash)
                    {
                        return -4;
                    }
                    string verifyData = tag9F4B.Substring(0, 4 + icDynamicLen * 2);
                    verifyData += locator.Terminal.GetTag("9F37");
                    string signedData = tag9F4B.Substring(4 + icDynamicLen * 2);
                    if(Authencation.SM2Verify(iccPulicKey, verifyData, signedData) != 0)
                    {
                        return -5;
                    }
                }
            }
            else
            {
                response = GAC(Constant.TC, CDOL1);
            }
            
            if (response.SW == 0x9000)
            {
                if (!isSupportCDA)
                {
                    var tlvs = DataParse.ParseTLV(response.Response);
                    if (tlvs.Count > 0 && tlvs[0].Tag == "80")
                    {
                        string result = tlvs[0].Value;  //第一次GAC返回的数据
                        string tag9F27 = result.Substring(0, 2);
                        string tag9F36 = result.Substring(2, 4);
                        string tag9F26 = result.Substring(6, 16);
                        string tag9F10 = result.Substring(22);

                        transTags.SetTag(TransactionStep.TerminalActionAnalyze,"9F27", tag9F27);
                        transTags.SetTag(TransactionStep.TerminalActionAnalyze, "9F36", tag9F36);
                        transTags.SetTag(TransactionStep.TerminalActionAnalyze, "9F26", tag9F26);
                        transTags.SetTag(TransactionStep.TerminalActionAnalyze, "9F10", tag9F10);    //更新后的电子余额在此处返回
                    }
                }
                CheckTag9F10Mac();
                //TransType type = TransCfg.AlgorithmFlag == AlgorithmCategory.DES ? TransType.ECC_DES : TransType.ECC_SM;
                int cardAction = Convert.ToInt32(transTags.GetTag("9F27"), 16);
                if(cardAction != Constant.TC)
                {
                    var caseNo = MethodBase.GetCurrentMethod().Name;
                    caseObj.TraceInfo(TipLevel.Failed,caseNo, "脱机电子现金交易失败");
                        
                }                                       
            }

            return 0;
        }
    }
}

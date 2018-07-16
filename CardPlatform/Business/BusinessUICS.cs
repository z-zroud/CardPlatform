using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CplusplusDll;
using CardPlatform.Cases;
using CardPlatform.ViewModel;
using System.Reflection;
using CardPlatform.Config;
using CardPlatform.Models;
using CardPlatform.Helper;
using CardPlatform.Common;
using GalaSoft.MvvmLight.Threading;

namespace CardPlatform.Business
{
    public class BusinessUICS : BusinessBase
    {
        private TransactionTag transTag = TransactionTag.GetInstance();
        private ViewModelLocator locator = new ViewModelLocator();

        /// <summary>
        /// 开始UICS交易流程
        /// </summary>
        /// <param name="aid"></param>
        /// <param name="doDesTrans"></param>
        /// <param name="doSMTrans"></param>
        public override void DoTransaction(string aid, bool doDesTrans, bool doSmTrans)
        {
            transTag.Clear();    //做交易之前，需要将tag清空，避免与上次交易重叠
            base.DoTransaction(aid, doDesTrans, doSmTrans);
            locator.Terminal.TermianlSettings.Tag9F7A   = "00";         //电子现金支持指示器(这里走借贷记交易流程)
            locator.Terminal.TermianlSettings.Tag9C     = "00";         //交易类型
            locator.Terminal.TermianlSettings.Tag9F66   = "46000000";   //终端交易属性
            
            if (doDesTrans)  // 做国际算法交易
            {
                DoTransaction(TransType.UICS_DES, DoTransactionEx);
            }
            
            if (doSMTrans)  //做国密算法交易
            {
                DoTransaction(TransType.UICS_SM, DoTransactionEx);
            }
        }

        private bool DoTransactionEx()
        {
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
            GetRequirementData();   //在脱机之前先进行必要数据的获取
            string aip      = transTag.GetTag(TransactionStep.GPO, "82");
            var aipHelper   = new AipHelper(aip);
            if(aipHelper.IsSupportSDA() || aipHelper.IsSupportDDA() || aipHelper.IsSupportCDA())
            {
                OfflineAuthcation();    //脱机数据认证
            }           
            HandleLimitation();         //处理限制
            if(aipHelper.IsSupportCardHolderVerify())
            {
                CardHolderVerify();     //持卡人验证
            }            
            TerminalRiskManagement();   //终端风险管理
            if( 0 == TerminalActionAnalyze())   //终端行为分析
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "终端行为分析失败，交易终止");
                return false;
            }
            if(0 == IssuerAuthencation())   //发卡行认证
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "联机处理失败，交易终止");
                return false;
            }            
            if(0 == TransactionEnd())   //交易结束处理
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "交易结束处理失败，交易终止");
                return false;
            }
            //LoadBalance("9F79", "000000010000");
            return true;
        }

        /// <summary>
        /// 选择应用,只要发送选择AID命令成功返回0x9000,则表示成功
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
                    var stepCase = new SelectAppCase() { CurrentApp = Constant.APP_UICS};
                    stepCase.Excute(BatchNo, CurrentApp,TransactionStep.SelectApp, response);
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

            string tag9F38 = transTag.GetTag(TransactionStep.SelectApp, "9F38");
            if(string.IsNullOrEmpty(tag9F38))
            {
                return afls;
            }
            var tls = DataParse.ParseTL(tag9F38);
            string pdolData = string.Empty;
            foreach (var tl in tls)
            {
                var terminalData = locator.Terminal.TermianlSettings.GetTag(tl.Tag);
                if (terminalData.Length != tl.Len * 2)      return afls;
                if (string.IsNullOrEmpty(terminalData))     return afls;
                pdolData += terminalData;
            }
            var response = base.GPO(pdolData);
            if(response.SW != 0x9000)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "GPO命令失败，SW={0}", response.SW);
                return afls;
            }
            var gpoCase = new GPOCase() { CurrentApp = Constant.APP_UICS };
            gpoCase.Excute(BatchNo, CurrentApp, TransactionStep.GPO, response);

            var tlvs = DataParse.ParseTLV(response.Response);
            if(tlvs.Count != 1 || tlvs[0].Value.Length <= 4)
            {
                return afls;
            }
            transTag.SetTag(TransactionStep.GPO, "82", tlvs[0].Value.Substring(0, 4));
            transTag.SetTag(TransactionStep.GPO, "94", tlvs[0].Value.Substring(4));
            afls = DataParse.ParseAFL(transTag.GetTag(TransactionStep.GPO, "94"));
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
                if (resp.SW != 0x9000)
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "读取应用记录失败,SW={0}", resp.SW);
                    return false;
                }
                if (!SaveTags(TransactionStep.ReadRecord,resp.Response))
                {
                    return false;
                }
            }
            IExcuteCase readRecordCase = new ReadRecordCase() { CurrentApp = Constant.APP_UICS };
            readRecordCase.Excute(BatchNo, CurrentApp, TransactionStep.ReadRecord, resps);
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
            for(int i = 0; i < tagStandards.Length; i++)
            {
                var resp = APDU.GetDataCmd(tagStandards[i].Tag);
                if(resp.SW != 0x9000)
                {
                    caseObj.TraceInfo(tagStandards[i].Level, caseNo, tagStandards[i].Desc, tagStandards[i].Tag, resp.SW);
                }
                else
                {
                    var tlvs = DataParse.ParseTLV(resp.Response);
                    var tlv = from tmp in tlvs where tmp.Tag == tagStandards[i].Tag select tmp;

                    if(tagStandards[i].Len != 0)
                    {
                        if(tlv.First().Len != tagStandards[i].Len)
                        {
                            caseObj.TraceInfo(tagStandards[i].Level, caseNo, "tag[{0}]长度不匹配，标准规范为[{1}],实际长度为[{2}]", tagStandards[i].Tag, tagStandards[i].Len, tlv.First().Len);
                        }
                    }
                    transTag.SetTag(TransactionStep.GetData,tlv.First().Tag, tlv.First().Value); //保存
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
            string aip = transTag.GetTag(TransactionStep.GPO,"82");
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
            string ddol = transTag.GetTag(TransactionStep.ReadRecord, "9F49");
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
        protected int HandleLimitation()
        {
            var handleLimitationCase = new HandleLimitationCase() { CurrentApp = Constant.APP_UICS };
            handleLimitationCase.Excute(BatchNo, CurrentApp, TransactionStep.HandleLimitation, null);
            return 0;
        }

        /// <summary>
        /// 持卡人认证
        /// </summary>
        /// <returns></returns>
        protected int CardHolderVerify()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string CVM = transTag.GetTag(TransactionStep.ReadRecord, "8E");
            if(CVM.Length < 10)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "CVM格式不正确，长度过小");
                return -1;
            }
            string moneyX = CVM.Substring(0, 8);
            string moneyY = CVM.Substring(8, 8);
            bool checkMoney = true;
            bool checkStandardCVM = true;
            if (moneyX != "00000000" || moneyY != "00000000")
            {
                checkMoney = false;
            }           
            if(CVM.Substring(16) != "42031E031F00")
            {
                checkStandardCVM = false;
            }
            caseObj.TraceInfo(GetTipLevel(checkMoney, TipLevel.Warn), caseNo, "检测CVM 金额X或金额Y是否为零");
            caseObj.TraceInfo(GetTipLevel(checkStandardCVM), caseNo, "检测CVM是否为银联要求的CVM(联机PIN+签名+NoCVM)");

            bool checkOfflinePIN = true;
            for (int i = 16; i < CVM.Length - 2; i += 4)
            {
                int method = Convert.ToInt32(CVM.Substring(i, 2), 16);
                int condition = Convert.ToInt32(CVM.Substring(i + 2, 2), 16);
                int methodSixBit = method & 0x3F;
                if(methodSixBit == 1)   //存在脱机PIN入口，需要判断是否有脱机PIN相关tag
                {
                    checkOfflinePIN = false;
                }
            }
            caseObj.TraceInfo(GetTipLevel(checkOfflinePIN, TipLevel.Warn), caseNo, "检测CVM是否存在脱机PIN入口，请注意是否存在脱机PIN相关信息");

            return 0;
        }

        /// <summary>
        /// 终端风险管理为大额交易提供了发卡行授权，确保卡片交易可以周期性的
        /// 进行联机处理。
        /// </summary>
        /// <returns></returns>
        protected int TerminalRiskManagement()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string aip = transTag.GetTag(TransactionStep.GPO, "82");
            var aipHelper = new AipHelper(aip);

            if(aipHelper.IsSupportTerminalRiskManagement())
            {
                caseObj.TraceInfo(TipLevel.Sucess, caseNo, "检测卡片是否支持终端风险管理");
                var tag9F14 = transTag.GetTag(TransactionStep.ReadRecord, "9F14");
                var tag9F23 = transTag.GetTag(TransactionStep.ReadRecord, "9F23");
                if (string.IsNullOrEmpty(tag9F14) || string.IsNullOrEmpty(tag9F23))
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "发卡行若支持终端频度检查，则需要此数据");
                }
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
            bool checkStartWith80 = false;
            bool checkTag9F27 = false;
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string CDOL1 = transTag.GetTag(TransactionStep.ReadRecord, "8C");
            ApduResponse resp = FirstGAC(Constant.ARQC, CDOL1);
            if(resp.SW != 0x9000)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "第一次发送GAC命令失败,返回{0:4X}",resp.SW);
                return - 1;
            }
            //需要对IAC tag进行分析

            var tlvs = DataParse.ParseTLV(resp.Response);
            if(tlvs.Count > 0 && tlvs[0].Tag == "80")
            {
                string result = tlvs[0].Value;  //第一次GAC返回的数据

                string tag9F27 = result.Substring(0, 2);
                string tag9F36 = result.Substring(2, 4);
                string tag9F26 = result.Substring(6, 16);
                string tag9F10 = result.Substring(22);

                checkTag9F27 = tag9F27 != "00" ? true : false;
                checkStartWith80 = true;

                transTag.SetTag(TransactionStep.TerminalActionAnalyze,"9F27", tag9F27);
                transTag.SetTag(TransactionStep.TerminalActionAnalyze, "9F36", tag9F36);
                transTag.SetTag(TransactionStep.TerminalActionAnalyze, "9F26", tag9F26);
                transTag.SetTag(TransactionStep.TerminalActionAnalyze, "9F10", tag9F10);
            }

            caseObj.TraceInfo(GetTipLevel(checkStartWith80), caseNo, "第一次GAC返回数据有误，请检查是否以80开头");
            caseObj.TraceInfo(GetTipLevel(checkTag9F27), caseNo, "终端请求80时，卡片tag9F27应返回80");
            return 0;
        }

        protected int IssuerAuthencation()
        {
            bool checkMac = false;
            bool checkTag9F26 = false;

            string acSessionKey;
            string ATC = transTag.GetTag(TransactionStep.TerminalActionAnalyze, "9F36");
            var caseNo = MethodBase.GetCurrentMethod().Name;
            
            if (curTransAlgorithmCategory == AlgorithmCategory.DES)
            {
                if(string.IsNullOrEmpty(TransDesACKey))
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "国际算法UDK/MDK不存在");
                    return -1;
                }
                acSessionKey = GenSessionKey(TransDesACKey, KeyType, curTransAlgorithmCategory);
            }
            else
            {
                if (string.IsNullOrEmpty(TransSMACKey))
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "国密算法UDK/MDK不存在");
                    return -1;
                }
                acSessionKey = GenSessionKey(TransSMACKey, KeyType, curTransAlgorithmCategory);
            }

            checkMac = CheckTag9F10Mac() ? true : false;
            caseObj.TraceInfo(GetTipLevel(checkMac), caseNo, "9F10后面的MAC必须与工具计算不一致");

            string AC = transTag.GetTag(TransactionStep.TerminalActionAnalyze, "9F26");
            string ARPC;
            if(doDesTrans)
                ARPC = Authencation.GenArpc(acSessionKey, AC, "3030", (int)AlgorithmCategory.DES);
            else
                ARPC = Authencation.GenArpc(acSessionKey, AC, "3030", (int)AlgorithmCategory.SM);

            var resp = APDU.ExtAuthCmd(ARPC, "3030");
            if (resp.SW == 0x9000)
            {
                locator.Terminal.TermianlSettings.Tag8A = "3030";
                checkTag9F26 = true;
            }
            caseObj.TraceInfo(GetTipLevel(checkTag9F26), caseNo, "卡片返回密文（9F26）必须与终端计算一致");

            return 0;
        }

        protected int TransactionEnd()
        {
            string CDOL2 = transTag.GetTag(TransactionStep.ReadRecord, "8D");
            ApduResponse resp = SecondGAC(Constant.TC, CDOL2);

            return 0;
        }

        protected int LoadBalance(string tag, string value)
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string macSessionKey = string.Empty;
            string ATC = transTag.GetTag(TransactionStep.TerminalActionAnalyze, "9F36");
            if (KeyType == TransKeyType.MDK)
            {
                string cardAcct = transTag.GetTag(TransactionStep.ReadRecord, "5A");
                string cardSeq = transTag.GetTag(TransactionStep.ReadRecord, "5F34");
                if(TransDesMACKey != null)
                {
                    string UDKMACKey = Authencation.GenUdk(TransDesMACKey, cardAcct, cardSeq);
                    macSessionKey = Authencation.GenUdkSessionKey(UDKMACKey, ATC);
                }
            }
            else
            {
                if(TransDesACKey != null)
                    macSessionKey = Authencation.GenUdkSessionKey(TransDesACKey, ATC);
            }
            if(macSessionKey.Length != 32)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "无法获取mac session密钥");
                return -1;
            }
            if(tag.Length == 2)
            {
                tag = "00" + tag;
            }
            var macData = "04DA" + tag + "0A" + transTag.GetTag(TransactionStep.TerminalActionAnalyze,"9F36") + transTag.GetTag(TransactionStep.TerminalActionAnalyze,"9F26") + value;
            string mac = Authencation.GenIssuerScriptMac(macSessionKey, macData);
            var resp = APDU.PutDataCmd(tag, value, mac);
            if(resp.SW == 0x9000)
            {
                return 0;
            }
            return -1;
        }

    }
}

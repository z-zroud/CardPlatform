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
        public override void DoTrans(string aid, bool doDesTrans, bool doSMTrans)
        {
            transTag.Clear();    //做交易之前，需要将tag清空，避免与上次交易重叠
            base.DoTrans(aid, doDesTrans, doSMTrans);
            locator.Terminal.TermianlSettings.Tag9F7A = "00";   //电子现金支持指示器(这里走借贷记交易流程)
            locator.Terminal.TermianlSettings.Tag9C = "00";     //交易类型
            locator.Terminal.TermianlSettings.Tag9F66 = "46000000"; //终端交易属性

            var songJianHelper = new SongJianHelper(PersoFile);

            // 做国际算法交易
            if (doDesTrans)  
            {
                locator.Terminal.TermianlSettings.TagDF69 = "00";
                bool isSucess = DoTransEx();
                if (!string.IsNullOrEmpty(PersoFile))
                {
                    if (IsContactTrans)
                    {
                        songJianHelper.WriteToFile(TagType.ContactDC_DES);
                    }
                    else
                    {
                        songJianHelper.WriteToFile(TagType.ContactlessDC_DES);
                    }
                }
                DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    TransResultModel TransactionResult = new TransResultModel(TransType.UICS_DES, TransResult.Unknown);
                    TransactionResult.TransType = TransType.UICS_DES;
                    if (isSucess)
                    {
                        TransactionResult.Result = TransResult.Sucess;
                    }
                    else
                    {
                        TransactionResult.Result = TransResult.Failed;
                    }

                    locator.Transaction.TransResult.Add(TransactionResult);
                });
            }
            //做国密算法交易
            if (doSMTrans)
            {
                curTransAlgorithmCategory = AlgorithmCategory.SM;
                locator.Terminal.TermianlSettings.TagDF69 = "01";
                bool isSuccess = DoTransEx();
                if (IsContactTrans && !string.IsNullOrEmpty(PersoFile))
                {
                    songJianHelper.WriteToFile(TagType.ContactDC_SM);
                }
                DispatcherHelper.CheckBeginInvokeOnUI(() => 
                {
                    TransResultModel TransactionResult = new TransResultModel(TransType.UICS_SM, TransResult.Unknown);
                    TransactionResult.TransType = TransType.UICS_SM;
                    if (isSuccess)
                    {
                        TransactionResult.Result = TransResult.Sucess;
                    }
                    else
                    {
                        TransactionResult.Result = TransResult.Failed;
                    }

                    locator.Transaction.TransResult.Add(TransactionResult);
                });
            }
        }

        protected bool DoTransEx()
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

            string AIP = transTag.GetTag(TransactionStep.GPO, "82");
            if(IsSupportSDA(AIP) || IsSupportDDA(AIP) || IsSupportCDA(AIP))
            {
                OfflineAuthcation();
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "脱机数据认证不支持");
            }
            
            HandleLimitation();
            if(IsSupportCardHolderVerify(AIP))
            {
                CardHolderVerify();
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "持卡人验证不支持");
            }
            
            TerminalRiskManagement();
            TerminalActionAnalyze();

            IssuerAuthencation();

            
            TransactionEnd();
            LoadBalance("9F79", "000000010000");
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
                    IExcuteCase stepCase = new SelectAppCase() { CurrentApp = Constant.APP_UICS};
                    stepCase.ExcuteCase(TransactionStep.SelectApp, response);
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
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "无法获取tag9F38");
                return afls;
            }
            var tls = DataParse.ParseTL(tag9F38);
            string pdolData = string.Empty;
            foreach (var tl in tls)
            {
                var terminalData = locator.Terminal.TermianlSettings.GetTag(tl.Tag);
                if(string.IsNullOrEmpty(terminalData))
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "终端数据tag{0}不存在", tl.Tag);
                    return afls;
                }
                pdolData += terminalData;
            }

            ApduResponse response = base.GPO(pdolData);
            if(response.SW != 0x9000)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "GPO命令失败，SW={0}", response.SW);
                return afls;
            }
            var tlvs = DataParse.ParseTLV(response.Response);
            if (tlvs.Count == 1 && tlvs[0].Value.Length > 4)
            {
                transTag.SetTag(TransactionStep.GPO, "82", tlvs[0].Value.Substring(0, 4));
                transTag.SetTag(TransactionStep.GPO, "94", tlvs[0].Value.Substring(4));
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "GPO响应数据格式解析不正确");
                return afls;
            }

            afls = DataParse.ParseAFL(transTag.GetTag(TransactionStep.GPO, "94"));

            IExcuteCase stepCase = new GPOCase() { CurrentApp = Constant.APP_UICS };
            stepCase.ExcuteCase(TransactionStep.GPO, response);

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
            IExcuteCase stepCase = new ReadRecordCase() { CurrentApp = Constant.APP_UICS };
            stepCase.ExcuteCase(TransactionStep.ReadRecord, resps);
            return true;
        }


        public override void GetRequirementData()
        {
            base.GetRequirementData();
            var caseNo = MethodBase.GetCurrentMethod().Name;

            RequirementData[] tagStandards =
            {
                new RequirementData("9F51",2,TipLevel.Failed,""),  //如果执行频度检查，需要此应用货币代码
                new RequirementData("9F52",2,TipLevel.Failed,""),  //如果支持发卡行认证，需要ADA应用缺省行为
                new RequirementData("9F53",1,TipLevel.Failed,""),  //如果执行国际货币频度检查，需要此连续脱机交易限制数(国际-货币)
                new RequirementData("9F54",6,TipLevel.Failed,""),  //如果执行国际国际频度检查，需要此连续脱机交易限制数(国际-国家)
                new RequirementData("9F55",0,TipLevel.Warn,""),
                new RequirementData("9F56",1,TipLevel.Failed,""),  //如果支持发卡行认证，需要此发卡行认证指示位
                new RequirementData("9F57",2,TipLevel.Warn,""),  //如果支持卡片频度检查，需要此发卡行国家代码
                new RequirementData("9F58",1,TipLevel.Failed,""),  //如果执行卡片频度检查，需要此连续脱机交易下限
                new RequirementData("9F59",1,TipLevel.Failed,""),  //如果无法联机，卡片风险管理需要此连续脱机交易上限做出拒绝交易
                new RequirementData("9F5C",6,TipLevel.Failed,""),  //累计脱机交易金额上限
                new RequirementData("9F5D",0,TipLevel.Warn,""),
                new RequirementData("9F72",1,TipLevel.Warn,""),  //连续脱机交易限制数
                new RequirementData("9F75",1,TipLevel.Warn,""),  //累计脱机交易金额(双货币)
                new RequirementData("9F76",0,TipLevel.Failed,""),  //第二应用货币代码
                new RequirementData("9F77",0,TipLevel.Failed,""),
                new RequirementData("9F78",0,TipLevel.Failed,""),
                new RequirementData("9F79",0,TipLevel.Failed,""),
                new RequirementData("9F4F",0,TipLevel.Failed,""),  //交易日志格式
                new RequirementData("9F68",0,TipLevel.Failed,""),
                new RequirementData("9F6B",0,TipLevel.Warn,""),
                new RequirementData("9F6D",0,TipLevel.Failed,""),
                new RequirementData("9F36",2,TipLevel.Failed,""),  //应用交易计数器
                new RequirementData("9F13",0,TipLevel.Failed,"终端风险管理阶段需要此数据用于新卡检查，发卡行认证通过后，需要设置该值"),  //如果卡片或终端执行频度检查，或新卡检查，需要此上次联机应用交易计数器
                new RequirementData("9F17",0,TipLevel.Failed,""),  //如果支持脱机PIN,需要此PIN尝试计数器
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
            string AIP = transTag.GetTag(TransactionStep.GPO,"82");
            if(AIP != "7C00")   //标准默认值
            {
                caseObj.TraceInfo(TipLevel.Warn, caseNo, "借贷记AIP不为7C00");
            }
            if (IsSupportSDA(AIP))
            {
                if (!SDA())
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "SDA脱机数据认证失败");
                    return -3;
                }
            }
            if (IsSupportDDA(AIP))
            {
                string ddol = transTag.GetTag(TransactionStep.ReadRecord,"9F49");
                string ddolData = "12345678";
                var tag9F4B = APDU.GenDynamicDataCmd(ddolData);
                if (string.IsNullOrWhiteSpace(tag9F4B))
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "Tag9F4B不存在");
                    return -7;
                }
                string issuerPublicKey = GetIssuerPublicKey();
                if (!DDA(issuerPublicKey, tag9F4B, ddolData))
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "DDA脱机数据认证失败");
                    return -3;
                }
            }
            return 0;
        }

        /// <summary>
        /// 处理限制
        /// </summary>
        /// <returns></returns>
        protected int HandleLimitation()
        {
            int expiryDate;
            int effectiveDate;
            int currentDate;
            int.TryParse(transTag.GetTag(TransactionStep.ReadRecord,"5F24"), out expiryDate);
            int.TryParse(transTag.GetTag(TransactionStep.ReadRecord, "5F25"), out effectiveDate);
            int.TryParse(DateTime.Now.ToString("yyMMdd"), out currentDate);

            var caseBase = new CaseBase();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (expiryDate < currentDate)    //应用已失效
            {
                caseBase.TraceInfo(TipLevel.Warn, caseNo, "应用失效日期大于当前日期，应用已失效");
            }

            if (effectiveDate < currentDate) // 应用未生效
            {
                caseBase.TraceInfo(TipLevel.Warn, caseNo, "应用生效日期大于当前日期，应用未生效");
            }

            if (expiryDate <= effectiveDate) //应用失效日期 大于生效日期
            {
                caseBase.TraceInfo(TipLevel.Failed, caseNo, "应用失效日期大于生效日期，应用不合法");
            }
            return 0;
        }

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
            if(moneyX != "00000000" || moneyY != "00000000")
            {
                caseObj.TraceInfo(TipLevel.Warn, caseNo, "金额X或金额Y不为零");
                return -2;
            }
            if(CVM.Substring(16) != "42031E031F00")
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "银联要求CVM为联机PIN+签名+NoCVM");
            }
            for(int i = CVM.Length - 16; i < CVM.Length - 2; i += 4)
            {

                int method = Convert.ToInt32(CVM.Substring(i, 2), 16);
                int condition = Convert.ToInt32(CVM.Substring(i + 2, 2), 16);
                if((method & 0x80) == 0)
                {
                    caseObj.TraceInfo(TipLevel.Warn, caseNo, "如果CVM{}失败，则持卡人验证失败。推荐如果此CVM失败，应用后续的", CVM.Substring(i, 4));
                }
                int methodSixBit = method & 0xC0;
                if(methodSixBit == 1)   //存在脱机PIN入口，需要判断是否有脱机PIN相关tag
                {
                    caseObj.TraceInfo(TipLevel.Warn, caseNo, "存在脱机PIN入口，请注意是否存在脱机PIN相关信息");
                }
            }
            //分析8E结构，并枚举CVM方法
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
            string AIP = transTag.GetTag(TransactionStep.GPO, "82");

            if(!IsSupportTerminalRiskManagement(AIP))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "卡片不支持终端风险管理");
                return -1;
            }
            var tag9F14 = transTag.GetTag(TransactionStep.ReadRecord, "9F14");
            var tag9F23 = transTag.GetTag(TransactionStep.ReadRecord, "9F23");
            if(string.IsNullOrEmpty(tag9F14) || string.IsNullOrEmpty(tag9F23))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "发卡行若支持终端频度检查，则需要此数据");
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
            string CDOL1 = transTag.GetTag(TransactionStep.ReadRecord, "8C");
            ApduResponse resp = FirstGAC(Constant.ARQC, CDOL1);
            if(resp.SW != 0x9000)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "第一次发送GAC命令失败,返回{04X}",resp.SW);
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

                transTag.SetTag(TransactionStep.TerminalActionAnalyze,"9F27", tag9F27);
                transTag.SetTag(TransactionStep.TerminalActionAnalyze, "9F36", tag9F36);
                transTag.SetTag(TransactionStep.TerminalActionAnalyze, "9F26", tag9F26);
                transTag.SetTag(TransactionStep.TerminalActionAnalyze, "9F10", tag9F10);
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo,"第一次GAC返回数据有误，请检查是否以80开头");
            }


            return 0;
        }

        protected int IssuerAuthencation()
        {
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
            if(!CheckTag9F10Mac())
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "9F10后面的MAC必须与工具计算不一致");
            }
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
                return 0;
            }
            else
            {

            }
            return -1;
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

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

namespace CardPlatform.Business
{
    public class BusinessUICS : BusinessBase
    {
        private TagDict tagDict = TagDict.GetInstance();
        private ViewModelLocator locator = new ViewModelLocator();

        /// <summary>
        /// 开始UICS交易流程
        /// </summary>
        /// <param name="aid"></param>
        /// <param name="doDesTrans"></param>
        /// <param name="doSMTrans"></param>
        public override void DoTrans(string aid, bool doDesTrans, bool doSMTrans)
        {
            tagDict.Clear();    //做交易之前，需要将tag清空，避免与上次交易重叠
            base.DoTrans(aid, doDesTrans, doSMTrans);
            locator.Terminal.TermianlSettings.Tag9F7A = "00";   //电子现金支持指示器
            locator.Terminal.TermianlSettings.Tag9C = "00";     //交易类型
            locator.Terminal.TermianlSettings.Tag9F66 = "46000000"; //终端交易属性
            var tagFileHelper = new TagFileHelper(PersoFile);
            // 做国际算法交易
            if (doDesTrans)  
            {
                TransResultModel TransactionResult = new TransResultModel(TransType.UICS_DES, TransResult.Unknown);
                TransactionResult.TransType = TransType.UICS_DES;
                locator.Terminal.TermianlSettings.TagDF69 = "00";               
                if (DoTransEx())
                {
                    TransactionResult.Result = TransResult.Sucess;
                }
                else
                {
                    TransactionResult.Result = TransResult.Failed;
                }
                locator.Transaction.TransResult.Add(TransactionResult);
                if(!string.IsNullOrEmpty(PersoFile))
                {
                    if (IsContactTrans)
                    {
                        tagFileHelper.WriteToFile(TagType.ContactDC_DES);
                    }
                    else
                    {
                        tagFileHelper.WriteToFile(TagType.ContactlessDC_DES);
                    }
                }

            }
            //做国密算法交易
            if (doSMTrans)
            {
                TransResultModel TransactionResult = new TransResultModel(TransType.UICS_SM, TransResult.Unknown);
                TransactionResult.TransType = TransType.UICS_SM;
                curTransAlgorithmCategory = AlgorithmCategory.SM;
                locator.Terminal.TermianlSettings.TagDF69 = "01";
                if (DoTransEx())
                {
                    TransactionResult.Result = TransResult.Sucess;
                }
                else
                {
                    TransactionResult.Result = TransResult.Failed;
                }
                locator.Transaction.TransResult.Add(TransactionResult);
                if (IsContactTrans && !string.IsNullOrEmpty(PersoFile))
                {
                    tagFileHelper.WriteToFile(TagType.ContactDC_SM);
                }
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
            OfflineAuthcation();
            HandleLimitation();
            CardHolderVerify();
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
                if (ParseTLVAndSave(response.Response))
                {
                    IExcuteCase stepCase = new SelectAppCase() { CurrentApp = Constant.APP_UICS, Step = Constant.STEP_SELECT_APP };
                    stepCase.ExcuteCase(response);
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
            var AFLs = new List<AFL>();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string tag9F38 = tagDict.GetTag("9F38");
            if(string.IsNullOrEmpty(tag9F38))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "无法获取tag9F38");
                return AFLs;
            }
            var tls = DataParse.ParseTL(tag9F38);
            string pdolData = string.Empty;
            foreach (var tl in tls)
            {
                pdolData += locator.Terminal.TermianlSettings.GetTag(tl.Tag);
            }

            ApduResponse response = base.GPO(pdolData);
            if(response.SW != 0x9000)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "GPO命令失败，SW={0}", response.SW);
                return AFLs;
            }
            var tlvs = DataParse.ParseTLV(response.Response);
            if (tlvs.Count == 1 && tlvs[0].Value.Length > 4)
            {
                tagDict.SetTag("82", tlvs[0].Value.Substring(0, 4));
                tagDict.SetTag("94", tlvs[0].Value.Substring(4));
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "GPO响应数据格式解析不正确");
                return AFLs;
            }

            AFLs = DataParse.ParseAFL(tagDict.GetTag("94"));

            IExcuteCase stepCase = new GPOCase() { CurrentApp = Constant.APP_UICS, Step = Constant.STEP_GPO };
            stepCase.ExcuteCase(response);

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
                if (resp.SW != 0x9000)
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "读取应用记录失败,SW={0}", resp.SW);
                    return false;
                }
                if (!ParseTLVAndSave(resp.Response))
                {
                    return false;
                }
            }
            IExcuteCase stepCase = new ReadRecordCase() { CurrentApp = Constant.APP_UICS, Step = Constant.STEP_READ_RECORD };
            stepCase.ExcuteCase(resps);
            return true;
        }

        public override void GetRequirementData()
        {
            base.GetRequirementData();
            var caseNo = MethodBase.GetCurrentMethod().Name;

            TagStandard[] tagStandards =
            {
                new TagStandard("9F51",2,TipLevel.Failed),
                new TagStandard("9F52",2,TipLevel.Failed),
                new TagStandard("9F53",1,TipLevel.Failed),
                new TagStandard("9F54",6,TipLevel.Failed),
                new TagStandard("9F55",0,TipLevel.Failed),
                new TagStandard("9F56",1,TipLevel.Failed),
                new TagStandard("9F57",2,TipLevel.Failed),
                new TagStandard("9F58",1,TipLevel.Failed),
                new TagStandard("9F59",1,TipLevel.Failed),
                new TagStandard("9F5C",6,TipLevel.Failed),
                new TagStandard("9F5D",0,TipLevel.Failed),
                new TagStandard("9F72",1,TipLevel.Failed),
                new TagStandard("9F75",1,TipLevel.Failed),
                new TagStandard("9F76",0,TipLevel.Failed),
                new TagStandard("9F77",0,TipLevel.Failed),
                new TagStandard("9F78",0,TipLevel.Failed),
                new TagStandard("9F79",0,TipLevel.Failed),
                new TagStandard("9F4F",0,TipLevel.Failed),
                new TagStandard("9F68",0,TipLevel.Failed),
                new TagStandard("9F6B",0,TipLevel.Failed),
                new TagStandard("9F6D",0,TipLevel.Failed),
                new TagStandard("9F36",2,TipLevel.Failed),
                new TagStandard("9F13",0,TipLevel.Failed),
                new TagStandard("9F17",0,TipLevel.Failed),
                new TagStandard("DF4F",0,TipLevel.Failed),
                new TagStandard("DF62",0,TipLevel.Failed),
            };
            for(int i = 0; i < tagStandards.Length; i++)
            {
                var resp = APDU.GetDataCmd(tagStandards[i].Tag);
                if(resp.SW != 0x9000)
                {
                    caseObj.TraceInfo(tagStandards[i].Level, caseNo, "无法获取tag[{0}],返回码:[{1:X}]", tagStandards[i].Tag, resp.SW);
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
                    tagDict.SetTag(tlv.First().Tag, tlv.First().Value); //保存
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
            string AIP = tagDict.GetTag("82");
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
                string ddol = tagDict.GetTag("9F49");
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
            int.TryParse(tagDict.GetTag("5F24"), out expiryDate);
            int.TryParse(tagDict.GetTag("5F25"), out effectiveDate);
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
            string CVM = tagDict.GetTag("8E");
            return 0;
        }

        protected int TerminalRiskManagement()
        {
            return 0;
        }

        protected int TerminalActionAnalyze()
        {
            string CDOL1 = tagDict.GetTag("8C");
            ApduResponse resp = FirstGAC(Constant.ARQC, CDOL1);
            if(resp.SW == 0x9000)
            {
                var tlvs = DataParse.ParseTLV(resp.Response);
                if(tlvs.Count > 0 && tlvs[0].Tag == "80")
                {
                    string result = tlvs[0].Value;  //第一次GAC返回的数据
                    string tag9F27 = result.Substring(0, 2);
                    string tag9F36 = result.Substring(2, 4);
                    string tag9F26 = result.Substring(6, 16);
                    string tag9F10 = result.Substring(22);

                    tagDict.SetTag("9F27", tag9F27);
                    tagDict.SetTag("9F36", tag9F36);
                    tagDict.SetTag("9F26", tag9F26);
                    tagDict.SetTag("9F10", tag9F10);
                }
            }

            return 0;
        }

        protected int IssuerAuthencation()
        {
            string acSessionKey;
            string ATC = tagDict.GetTag("9F36");
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
            string AC = tagDict.GetTag("9F26");
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
            return -1;
        }

        protected int TransactionEnd()
        {
            string CDOL2 = tagDict.GetTag("8D");
            ApduResponse resp = SecondGAC(Constant.TC, CDOL2);

            return 0;
        }

        protected int LoadBalance(string tag, string value)
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string macSessionKey = string.Empty;
            string ATC = tagDict.GetTag("9F36");
            if (KeyType == TransKeyType.MDK)
            {
                string cardAcct = tagDict.GetTag("5A");
                string cardSeq = tagDict.GetTag("5F34");
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
            var macData = "04DA" + tag + "0A" + tagDict.GetTag("9F36") + tagDict.GetTag("9F26") + value;
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

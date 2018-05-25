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

namespace CardPlatform.Business
{
    public class BusinessECC : BusinessBase
    {
        private TagDict tagDict = TagDict.GetInstance();
        private ViewModelLocator locator = new ViewModelLocator();
        private IExcuteCase baseCase = new CaseBase();
        private bool isEccTranction = false;
        private bool isSupportCDA = false;
        private string pdolData = string.Empty;
        
        /// <summary>
        /// 开始交易流程，该交易流程仅包含国际/国密电子现金的消费交易，暂不包含圈存
        /// </summary>
        /// <param name="aid"></param>
        /// <param name="doDesTrans"></param>
        /// <param name="doSMTrans"></param>
        public override void DoTrans(string aid, bool doDesTrans, bool doSMTrans)
        {
            base.DoTrans(aid, doDesTrans, doSMTrans);

            locator.Terminal.TermianlSettings.Tag9F7A = "01";   //电子现金交易指示器
            locator.Terminal.TermianlSettings.Tag9C = "00";     //交易类型(消费)
            var tagFileHelper = new TagFileHelper(PersoFile);
            // 做国际交易
            if (doDesTrans)
            {
                TransResultModel TransactionResult = new TransResultModel(TransType.ECC_DES, TransResult.Unknown);
                TransactionResult.TransType = TransType.ECC_DES;
                curTransAlgorithmCategory = AlgorithmCategory.DES;
                locator.Terminal.TermianlSettings.TagDF69 = "00";
               
                if(DoTransEx())
                {
                    TransactionResult.Result = TransResult.Sucess;
                }
                else
                {
                    TransactionResult.Result = TransResult.Failed;
                }
                locator.Transaction.TransResult.Add(TransactionResult);
                if (!string.IsNullOrEmpty(PersoFile))
                {
                    tagFileHelper.WriteToFile(TagType.ECC_DES);
                }
            }
            // 做国密交易
            if (doSMTrans)
            {
                TransResultModel TransactionResult = new TransResultModel(TransType.ECC_SM, TransResult.Unknown);
                TransactionResult.TransType = TransType.ECC_SM;
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
                if (!string.IsNullOrEmpty(PersoFile))
                {
                    tagFileHelper.WriteToFile(TagType.ECC_SM);
                }
            }
        }

        protected bool DoTransEx()
        {
            tagDict.Clear();    //做交易之前，需要将tag清空，避免与上次交易重叠
            pdolData = string.Empty;    //PDOL Data也需要清空，防止做CDA时，出现重叠
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
            GetRequirementData();
            if (isEccTranction)
            {
                OfflineAuthcation();
                HandleLimitation();
                CardHolderVerify();
                TerminalRiskManagement();
                TerminalActionAnalyze();
            }
            else
            {
                baseCase.TraceInfo(TipLevel.Failed, caseNo, "此交易不是脱机电子现金交易");
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
                if (ParseTLVAndSave(response.Response))
                {
                    IExcuteCase excuteCase = new SelectAppCase();
                    excuteCase.ExcuteCase(response);
                    result = true;
                }
            }
            else
            {
                var caseNo = MethodBase.GetCurrentMethod().Name;
                baseCase.TraceInfo(TipLevel.Failed, caseNo, "选择应用{0}失败,SW={1}", aid, response.SW);
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
            string tag9F38 = tagDict.GetTag("9F38");
            var tls = DataParse.ParseTL(tag9F38);
            foreach (var tl in tls)
            {
                pdolData += locator.Terminal.TermianlSettings.GetTag(tl.Tag);
            }

            ApduResponse response = base.GPO(pdolData);
            var tlvs = DataParse.ParseTLV(response.Response);
            if (tlvs.Count == 1 && tlvs[0].Value.Length > 4)
            {

                tagDict.SetTag("82", tlvs[0].Value.Substring(0, 4));
                tagDict.SetTag("94", tlvs[0].Value.Substring(4));
            }

            var AFLs = DataParse.ParseAFL(tagDict.GetTag("94"));

            IExcuteCase excuteCase = new GPOCase();
            excuteCase.ExcuteCase(response);

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
                    baseCase.TraceInfo(TipLevel.Failed, caseNo, "读取应用记录失败,SW={0}", resp.SW);
                    return false;
                }
                if (!ParseTLVAndSave(resp.Response))
                {
                    return false;
                }
            }

            var tag9F74 = tagDict.GetTag("9F74");
            if(tag9F74.Length == 12)
            {
                var resp = APDU.GetDataCmd("9F79");
                ParseTLVAndSave(resp.Response);
                resp = APDU.GetDataCmd("9F6D");
                ParseTLVAndSave(resp.Response);
                var tag9F79 = tagDict.GetTag("9F79");
                if(int.Parse(tag9F79) > 0)
                {
                    isEccTranction = true;  //说明是电子现金交易
                }
            }

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
            if(IsSupportSDA(AIP))
            {
                if (!SDA())
                {
                    baseCase.TraceInfo(TipLevel.Failed, caseNo, "SDA脱机数据认证失败");
                    return -3;
                }
            }
            if(IsSupportDDA(AIP))
            {
                string ddolData = string.Empty;
                string ddol = tagDict.GetTag("9F49");
                var tls = DataParse.ParseTL(ddol);
                foreach(var tl in tls)
                {
                    ddolData += locator.Terminal.TermianlSettings.GetTag(tl.Tag);
                }
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
            isSupportCDA = IsSupportCDA(AIP);
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
            //电子现金不包括持卡人认证，这里仅判断数据是否为1E031F00
            //联机PIN不能设为首选CVM
            string CVM = tagDict.GetTag("8E");
            return 0;
        }

        protected int TerminalRiskManagement()
        {
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
            string CDOL1 = tagDict.GetTag("8C");
            var response = new ApduResponse();
            if(isSupportCDA)
            {
                string CDOL1Data = string.Empty;
                var tls = DataParse.ParseTL(CDOL1);
                foreach (var tl in tls)
                {
                    CDOL1Data += locator.Terminal.TermianlSettings.GetTag(tl.Tag);
                }
                response = APDU.GACCmd(Constant.TC_CDA, CDOL1Data);

                if (ParseTLVAndSave(response.Response))
                {
                    if(curTransAlgorithmCategory == AlgorithmCategory.DES)
                    {
                        string tag9F4B = tagDict.GetTag("9F4B");
                        string exp = tagDict.GetTag("9F47");
                        string recoveryData = Authencation.GenRecoveryData(iccPulicKey, exp, tag9F4B);
                        if (recoveryData.Length == 0 || !recoveryData.StartsWith("6A05"))
                        {

                        }
                        string recoveryHash = recoveryData.Substring(recoveryData.Length - 42, 40);
                        string hashInput = recoveryData.Substring(2, recoveryData.Length - 44);
                        hashInput += locator.Terminal.TermianlSettings.GetTag("9F37");
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
                            string tagValue = tagDict.GetTag(tag);
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
                        var tag9F4B = tagDict.GetTag("9F4B");
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
                            string tagValue = tagDict.GetTag(tag);
                            string len = UtilLib.Utils.IntToHexStr(tagValue.Length / 2, 2);
                            hashInput += tag + len + tagValue;
                        }
                        string verfiyHash = Authencation.GetSMHash(hashInput);
                        if(hash != verfiyHash)
                        {
                            return -4;
                        }
                        string verifyData = tag9F4B.Substring(0, 4 + icDynamicLen * 2);
                        verifyData += locator.Terminal.TermianlSettings.GetTag("9F37");
                        string signedData = tag9F4B.Substring(4 + icDynamicLen * 2);
                        if(Authencation.SM2Verify(iccPulicKey, verifyData, signedData) != 0)
                        {
                            return -5;
                        }
                    }
                    
                }
            }
            else
            {
                response = FirstGAC(Constant.TC, CDOL1);
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

                        tagDict.SetTag("9F27", tag9F27);
                        tagDict.SetTag("9F36", tag9F36);
                        tagDict.SetTag("9F26", tag9F26);
                        tagDict.SetTag("9F10", tag9F10);    //更新后的电子余额在此处返回
                    }
                }
                CheckTag9F10Mac();
                TransType type = curTransAlgorithmCategory == AlgorithmCategory.DES ? TransType.ECC_DES : TransType.ECC_SM;
                int cardAction = Convert.ToInt32(tagDict.GetTag("9F27"), 16);
                if(cardAction != Constant.TC)
                {
                    var caseNo = MethodBase.GetCurrentMethod().Name;
                    baseCase.TraceInfo(TipLevel.Failed,caseNo, "脱机电子现金交易失败");
                        
                }                                       
            }

            return 0;
        }
    }
}

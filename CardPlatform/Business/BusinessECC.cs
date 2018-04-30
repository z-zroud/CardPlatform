using System;
using System.Collections.Generic;
using System.Linq;
using CardPlatform.ViewModel;
using CardPlatform.Cases;
using System.Reflection;
using CplusplusDll;
using CardPlatform.Config;

namespace CardPlatform.Business
{
    public class BusinessECC : BusinessBase
    {
        private TagDict tagDict = TagDict.GetInstance();
        private ViewModelLocator locator = new ViewModelLocator();
        private IExcuteCase baseCase = new CaseBase();
        private bool isEccTranction = false;
        private AlgorithmCategory curTransAlgorithmCategory = AlgorithmCategory.DES;    //default

        /// <summary>
        /// 开始交易流程，该交易流程仅包含国际/国密电子现金的消费交易，暂不包含圈存
        /// </summary>
        /// <param name="aid"></param>
        /// <param name="doDesTrans"></param>
        /// <param name="doSMTrans"></param>
        public override void DoTrans(string aid, bool doDesTrans, bool doSMTrans)
        {
            base.DoTrans(aid, doDesTrans, doSMTrans);

            locator.Terminal.TermianlSettings.Tag9F7A = "01";
            locator.Terminal.TermianlSettings.Tag9C = "00";

            // 做国际交易
            if (doDesTrans)
            {
                locator.Terminal.TermianlSettings.TagDF69 = "00";
                var caseNo = MethodBase.GetCurrentMethod().Name;

                var resp = SelectAid(aid);
                if (resp.SW != 0x9000)
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "选择应用失败，交易流程终止");
                    return;
                }

                var AFLs = GPOEx();
                ReadRecords(AFLs);
                if(isEccTranction)
                {
                    OfflineAuthcation();
                    HandleLimitation();
                    CardHolderVerify();
                    TerminalRiskManagement();
                    TerminalActionAnalyze();
                }

            }
            if (doSMTrans)   // do sm uics transaction
            {
                locator.Terminal.TermianlSettings.TagDF69 = "01";
                SelectAid(aid);
                string tag9F38 = tagDict.GetTag("9F38");
                var tls = DataParse.ParseTL(tag9F38);
                string pdol = string.Empty;
                foreach (var tl in tls)
                {
                    pdol += locator.Terminal.TermianlSettings.GetTag(tl.Tag);
                }
                var AFLs = GPOEx();
                ReadRecords(AFLs);
                if (isEccTranction)
                {
                    OfflineAuthcation();
                    HandleLimitation();
                    CardHolderVerify();
                    TerminalRiskManagement();
                    TerminalActionAnalyze();
                }
            }
        }

        /// <summary>
        /// 选择应用
        /// </summary>
        /// <param name="aid"></param>
        /// <returns></returns>
        protected override ApduResponse SelectAid(string aid)
        {
            ApduResponse response = base.SelectAid(aid);
            ParseAndSave(response.Response);
            IExcuteCase excuteCase = new SelectAidCase();
            excuteCase.ExcuteCase(response);

            return response;
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
            string pdolData = string.Empty;
            foreach (var tl in tls)
            {
                pdolData += locator.Terminal.TermianlSettings.GetTag(tl.Tag);
            }

            ApduResponse response = base.GPO(pdolData);
            var tlvs = DataParse.ParseTLV(response.Response);
            if (tlvs.Count == 1 &&
                tlvs[0].Value.Length > 4)
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
        protected override List<ApduResponse> ReadRecords(List<AFL> afls)
        {
            var resps = base.ReadRecords(afls);

            foreach (var resp in resps)
            {
                ParseAndSave(resp.Response);
            }

            //IExcuteCase excuteCase = new ReadRecordCase();
            //excuteCase.ExcuteCase(resps);

            var tag9F74 = tagDict.GetTag("9F74");
            if(tag9F74.Length == 12)
            {
                var resp = APDU.GetDataCmd("9F79");
                ParseAndSave(resp.Response);
                resp = APDU.GetDataCmd("9F6D");
                ParseAndSave(resp.Response);
                var tag9F79 = tagDict.GetTag("9F79");
                if(int.Parse(tag9F79) > 0)
                {
                    isEccTranction = true;  //说明是电子现金交易
                }
            }

            return resps;
        }

        /// <summary>
        /// 脱机数据认证
        /// </summary>
        /// <returns></returns>
        protected int OfflineAuthcation()
        {
            int result;
            IExcuteCase excuteCase = new CaseBase();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (aid.Length < 10)
            {
                excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "应用AID长度太短");
                return -1;
            }
            string rid = aid.Substring(0, 10);
            string CAIndex = tagDict.GetTag("8F");
            if (CAIndex.Length != 2)
            {
                excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取CA 索引,请检查8F是否存在");
                return -2;
            }
            string CAPublicKey = Authencation.GenCAPublicKey(CAIndex, rid);
            if (string.IsNullOrWhiteSpace(CAPublicKey))
            {
                excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取CA公钥，请检查RID及索引是否正确");
                return -3;
            }

            string issuerPublicKey;
            string issuerPublicCert = tagDict.GetTag("90");
            string PAN = tagDict.GetTag("5A");
            string signedStaticAppData = tagDict.GetTag("93");
            string iccPublicCert = tagDict.GetTag("9F46");
            string AIP = tagDict.GetTag("82");

            string ddol = tagDict.GetTag("9F49");
            string terminalRandom = "12345678";

            var tag9F4B = APDU.GenDynamicDataCmd(terminalRandom);
            if (string.IsNullOrWhiteSpace(tag9F4B))
            {
                excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "Tag9F4B不存在");
                return -7;
            }

            if (doDesTrans) //DES算法
            {
                string issuerPublicKeyRemainder = tagDict.GetTag("92");
                string issuerExp = tagDict.GetTag("9F32");
                issuerPublicKey = Authencation.GenDesIssuerPublicKey(CAPublicKey, issuerPublicCert, issuerPublicKeyRemainder, issuerExp);
                if (string.IsNullOrWhiteSpace(issuerPublicKey))
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取发卡行公钥，请检查tag90,92,9F32是否存在");
                    return -4;
                }

                result = Authencation.DES_SDA(issuerPublicKey, issuerExp, signedStaticAppData, toBeSignAppData, AIP);
                if (result != 0)
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "静态数据认证失败!");
                    return -5;
                }

                string iccPublicKeyRemainder = tagDict.GetTag("9F48");
                string iccPublicKeyExp = tagDict.GetTag("9F47");
                string iccPublicKey = Authencation.GenDesICCPublicKey(issuerPublicKey, iccPublicCert, iccPublicKeyRemainder, toBeSignAppData, iccPublicKeyExp, AIP);
                if (string.IsNullOrWhiteSpace(iccPublicKey))
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取IC卡公钥，请确保tag9F46,9F48,9F47是否存在");
                    return -6;
                }

                result = Authencation.DES_DDA(iccPublicKey, iccPublicKeyExp, tag9F4B, terminalRandom);
                if (result != 0)
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "动态数据认证失败!");
                    return -8;
                }
            }
            else //SM算法
            {
                issuerPublicKey = Authencation.GenSMIssuerPublicKey(CAPublicKey, issuerPublicCert, PAN);
                if (string.IsNullOrWhiteSpace(issuerPublicKey))
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取发卡行公钥，请检查tag90,5A是否存在");
                    return -4;
                }

                result = Authencation.SM_SDA(issuerPublicKey, toBeSignAppData, signedStaticAppData, AIP);
                if (result != 0)
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "SM算法 静态数据认证失败!");
                    return -9;
                }

                string iccPublicKey = Authencation.GenSMICCPublicKey(issuerPublicKey, iccPublicCert, toBeSignAppData, AIP, PAN);
                if (string.IsNullOrWhiteSpace(iccPublicKey))
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取IC卡公钥");
                    return -4;
                }
                result = Authencation.SM_DDA(iccPublicKey, tag9F4B, terminalRandom);
                if (result != 0)
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "SM算法 动态数据认证失败!");
                    return -10;
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
                caseBase.TraceInfo(CaseLevel.Warn, caseNo, "应用失效日期大于当前日期，应用已失效");
            }

            if (effectiveDate < currentDate) // 应用未生效
            {
                caseBase.TraceInfo(CaseLevel.Warn, caseNo, "应用生效日期大于当前日期，应用未生效");
            }

            if (expiryDate <= effectiveDate) //应用失效日期 大于生效日期
            {
                caseBase.TraceInfo(CaseLevel.Failed, caseNo, "应用失效日期大于生效日期，应用不合法");
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
            string CDOL1 = tagDict.GetTag("8C");
            ApduResponse resp = GAC1(Constant.TC, CDOL1);
            if (resp.SW == 0x9000)
            {
                var tlvs = DataParse.ParseTLV(resp.Response);
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
                    tagDict.SetTag("9F10", tag9F10);
                }
            }

            return 0;
        }
    }
}

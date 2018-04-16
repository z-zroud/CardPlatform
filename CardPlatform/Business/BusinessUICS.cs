﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CplusplusDll;
using CardPlatform.Cases;
using CardPlatform.ViewModel;
using System.Reflection;
using CardPlatform.Config;

namespace CardPlatform.Business
{
    public class BusinessUICS : BusinessBase
    {
        private TagDict tagDict = TagDict.GetInstance();
        private ViewModelLocator locator = new ViewModelLocator();
        private string aid;

        public override void DoTrans(string aid, bool doDesTrans, bool doSMTrans)
        {
            this.aid = aid;
            
            locator.Terminal.TermianlSettings.Tag9F7A = "00";
            locator.Terminal.TermianlSettings.Tag9C = "00";

            if (doDesTrans)  // do des uics transaction
            {
                locator.Terminal.TermianlSettings.TagDF69 = "00";
                SelectAid(aid);
                string tag9F38 = tagDict.GetTag("9F38");
                var tls = DataParse.ParseTL(tag9F38);
                string pdol = string.Empty;
                foreach (var tl in tls)
                {
                    pdol += locator.Terminal.TermianlSettings.GetTag(tl.Tag);
                }
                var AFLs = GPOEx(pdol);
                ReadRecords(AFLs);
                OfflineAuthcation();
                HandleLimitation();
                CardHolderVerify();
                TerminalRiskManagement();
                TerminalActionAnalyze();
                IssuerAuthencation();
                TransactionEnd();
            }
            if (doSMTrans)   // do sm uics transaction
            {
                locator.Terminal.TermianlSettings.TagDF69 = "01";
                SelectAid(aid);
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
            excuteCase.ExcuteCase(response, CardRespDataType.SelectAid);

            return response;
        }

        /// <summary>
        /// 应用初始化
        /// </summary>
        /// <param name="pdol"></param>
        /// <returns></returns>
        protected List<AFL> GPOEx(string pdol)
        {
            ApduResponse response = base.GPO(pdol);
            var tlvs = DataParse.ParseTLV(response.Response);
            if (tlvs.Count == 1 &&
                tlvs[0].Value.Length > 4)
            {

                tagDict.SetTag("82", tlvs[0].Value.Substring(0, 4));
                tagDict.SetTag("94", tlvs[0].Value.Substring(4));
            }

            var AFLs = DataParse.ParseAFL(tagDict.GetTag("94"));

            IExcuteCase excuteCase = new GPOCase();
            excuteCase.ExcuteCase(response, CardRespDataType.GPO);

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

            IExcuteCase excuteCase = new ReadRecordCase();
            excuteCase.ExcuteCase(resps, CardRespDataType.ReadPSERecord);

            return resps;
        }

        /// <summary>
        /// 脱机数据认证
        /// </summary>
        /// <returns></returns>
        protected int OfflineAuthcation()
        {
            int result;
            if (aid.Length < 10)
                return -1;
            string rid = aid.Substring(0, 10);
            string CAIndex = tagDict.GetTag("8F");
            if (CAIndex.Length != 2)
                return -2;
            string CAPublicKey = Authencation.GenCAPublicKey(CAIndex, rid);
            string issuerPublicCert = tagDict.GetTag("90");
            string issuerPublicKeyRemainder = tagDict.GetTag("92");
            string issuerExp = tagDict.GetTag("9F32");
            string issuerPublicKey = Authencation.GenDesIssuerPublicKey(CAPublicKey, issuerPublicCert, issuerPublicKeyRemainder, issuerExp);
            string signedStaticAppData = tagDict.GetTag("93");
            string AIP = tagDict.GetTag("82");
            result = Authencation.DES_SDA(issuerPublicKey, issuerExp, signedStaticAppData, joinSignedData, AIP);

            string iccPublicCert = tagDict.GetTag("9F46");
            string iccPublicKeyRemainder = tagDict.GetTag("9F48");
            string iccPublicKeyExp = tagDict.GetTag("9F47");
            string iccPublicKey = Authencation.GenDesICCPublicKey(issuerPublicKey, iccPublicCert, iccPublicKeyRemainder, joinSignedData, iccPublicKeyExp, AIP);

            string ddol = tagDict.GetTag("9F49");
            string terminalRandom = "12345678";

            var Tag9F4B = APDU.GenDynamicDataCmd(terminalRandom);


            result = Authencation.DES_DDA(iccPublicKey, iccPublicKeyExp, Tag9F4B, terminalRandom);

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
                caseBase.ShowInfo(caseNo, "应用失效日期大于当前日期，应用已失效", CaseLevel.CaseWarn);
            }

            if (effectiveDate < currentDate) // 应用未生效
            {
                caseBase.ShowInfo(caseNo, "应用生效日期大于当前日期，应用未生效", CaseLevel.CaseWarn);
            }

            if (expiryDate <= effectiveDate) //应用失效日期 大于生效日期
            {
                caseBase.ShowInfo(caseNo, "应用失效日期大于生效日期，应用不合法", CaseLevel.CaseFailed);
            }

            string appVersion = tagDict.GetTag("9F08"); //需要判断版本号
            DataCompareConfig dataCompareConfig = DataCompareConfig.GetInstance();
            if (!dataCompareConfig.HasLoaded)
                dataCompareConfig.Load(Constant.DataComparedConfigFile);
            var templateAppVersion = dataCompareConfig.GetComparedTag("9F08").Value;
            if(appVersion != templateAppVersion)
            {
                caseBase.ShowInfo(caseNo, "应用版本不一致", CaseLevel.CaseFailed);
            }
            string AUC = tagDict.GetTag("9F07");

            string cardIssuerCountryCode = tagDict.GetTag("5F28");

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
            ApduResponse resp = GAC1(Constant.ARQC, CDOL1);
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
            if (KeyType == TransKeyType.MDK)
            {
                string cardAcct = tagDict.GetTag("5A");
                string cardSeq = tagDict.GetTag("5F34");
                string UDKACKey = Authencation.GenUdk(TransDesACKey, cardAcct, cardSeq);
                acSessionKey = Authencation.GenUdkSessionKey(UDKACKey, ATC);
            }
            else
            {
                acSessionKey = Authencation.GenUdkSessionKey(TransDesACKey, ATC);
            }
            string AC = tagDict.GetTag("9F26");
            string ARPC = Authencation.GenArpc(acSessionKey, AC, "3030");
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
            ApduResponse resp = GAC2(Constant.TC, CDOL2);

            return 0;
        }

        protected int HandleIssuerScript(string tag, string value)
        {
            string macSessionKey;
            string ATC = tagDict.GetTag("9F36");
            if (KeyType == TransKeyType.MDK)
            {
                string cardAcct = tagDict.GetTag("5A");
                string cardSeq = tagDict.GetTag("5F34");
                string UDKMACKey = Authencation.GenUdk(TransDesMACKey, cardAcct, cardSeq);
                macSessionKey = Authencation.GenUdkSessionKey(UDKMACKey, ATC);
            }
            else
            {
                macSessionKey = Authencation.GenUdkSessionKey(TransDesACKey, ATC);
            }
            if(tag.Length == 2)
            {
                tag = "00" + tag;
            }
            var DataLen = UtilLib.Utils.GetBcdLen(value);
            var macData = "04DA" + tag + DataLen + value;
            string mac = Authencation.GenIssuerScriptMac(macSessionKey, value);
            var resp = APDU.PutDataCmd(tag, value, mac);
            if(resp.SW == 0x9000)
            {
                return 0;
            }
            return -1;
        }

    }
}

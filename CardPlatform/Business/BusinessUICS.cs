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

namespace CardPlatform.Business
{
    public class BusinessUICS : BusinessBase
    {
        private TagDict tagDict = TagDict.GetInstance();
        private ViewModelLocator locator = new ViewModelLocator();
        private IExcuteCase baseCase = new CaseBase();

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

            // 做国际算法交易
            if (doDesTrans)  
            {
                locator.Terminal.TermianlSettings.TagDF69 = "00";
                DoTransEx();
            }
            //做国密算法交易
            if (doSMTrans)
            {
                curTransAlgorithmCategory = AlgorithmCategory.SM;
                locator.Terminal.TermianlSettings.TagDF69 = "01";
                DoTransEx();
            }
        }

        protected void DoTransEx()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (!SelectApp(aid))
            {
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "选择应用失败，交易流程终止");
                return;
            }
            var AFLs = GPOEx();
            if (AFLs.Count == 0)
            {
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "GPO命令发送失败，交易流程终止");
                return;
            }
            if (!ReadAppRecords(AFLs))
            {
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "读取应用记录失败，交易流程终止");
                return;
            }
            OfflineAuthcation();
            HandleLimitation();
            CardHolderVerify();
            TerminalRiskManagement();
            TerminalActionAnalyze();
            IssuerAuthencation();
            TransactionEnd();
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
                if (ParseAndSave(response.Response))
                {
                    IExcuteCase excuteCase = new SelectAidCase();
                    excuteCase.ExcuteCase(response);
                    result = true;
                }
            }
            else
            {
                var caseNo = MethodBase.GetCurrentMethod().Name;
                if (response.SW != 0x9000)
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "选择应用{0}失败,SW={1}", aid, response.SW);
                }
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
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取tag9F38");
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
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "GPO命令失败，SW={0}", response.SW);
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
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "GPO响应数据格式解析不正确");
                return AFLs;
            }

            AFLs = DataParse.ParseAFL(tagDict.GetTag("94"));

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
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "读取应用记录失败,SW={0}", resp.SW);
                    return false;
                }
                if (!ParseAndSave(resp.Response))
                {
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// 脱机数据认证
        /// </summary>
        /// <returns></returns>
        protected int OfflineAuthcation()
        {
            IExcuteCase excuteCase = new CaseBase();
            var caseNo = MethodBase.GetCurrentMethod().Name;
     
            string ddol = tagDict.GetTag("9F49");
            string ddolData = "12345678";

            var tag9F4B = APDU.GenDynamicDataCmd(ddolData);
            if (string.IsNullOrWhiteSpace(tag9F4B))
            {
                excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "Tag9F4B不存在");
                return -7;
            }

            string issuerPublicKey = string.Empty;
            if(!SDA(ref issuerPublicKey))
            {
                excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "SDA脱机数据认证失败");
                return -3;
            }
            if(!DDA(issuerPublicKey,tag9F4B,ddolData))
            {
                excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "DDA脱机数据认证失败");
                return -3;
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

            //模板值对比判断放到程序交易结束之后
            //string appVersion = tagDict.GetTag("9F08"); //需要判断版本号
            ////DataCompareConfig dataCompareConfig = DataCompareConfig.GetInstance();
            ////if (!dataCompareConfig.HasLoaded)
            ////    dataCompareConfig.Load(Constant.DataComparedConfigFile);
            ////var templateAppVersion = dataCompareConfig.GetComparedTag("9F08").Value;
            ////if(appVersion != templateAppVersion)
            ////{
            ////    caseBase.ShowInfo(caseNo, "应用版本不一致", CaseLevel.CaseFailed);
            ////}
            //string AUC = tagDict.GetTag("9F07");

            //string cardIssuerCountryCode = tagDict.GetTag("5F28");

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
            var caseBase = new CaseBase();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (curTransAlgorithmCategory == AlgorithmCategory.DES)
            {
                acSessionKey = GenSessionKey(TransDesACKey, KeyType, curTransAlgorithmCategory);
            }
            else
            {
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

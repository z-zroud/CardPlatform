using System;
using System.Collections.Generic;
using System.Linq;
using CardPlatform.ViewModel;
using CardPlatform.Cases;
using System.Reflection;
using CplusplusDll;
using CardPlatform.Config;
using CardPlatform.Models;

namespace CardPlatform.Business
{
    public class BusinessVISA : BusinessBase
    {
        private TagDict tagDict = TagDict.GetInstance();
        private ViewModelLocator locator = new ViewModelLocator();
        private IExcuteCase baseCase = new CaseBase();
        private bool isEccTranction = false;

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

            // 做国际交易
            if (doDesTrans)
            {
                TransResultModel TransactionResult = new TransResultModel(TransType.ECC_DES, TransResult.Unknown);
                TransactionResult.TransType = TransType.ECC_DES;
                curTransAlgorithmCategory = AlgorithmCategory.DES;
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
            }
        }

        protected bool DoTransEx()
        {
            tagDict.Clear();    //做交易之前，需要将tag清空，避免与上次交易重叠
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (!SelectApp(aid))
            {
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "选择应用失败，交易流程终止");
                return false;
            }
            var AFLs = GPOEx();
            if (AFLs.Count == 0)
            {
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "GPO命令发送失败，交易流程终止");
                return false;
            }
            if (!ReadAppRecords(AFLs))
            {
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "读取应用记录失败，交易流程终止");
                return false;
            }

            OfflineAuthcation();
            HandleLimitation();
            CardHolderVerify();
            TerminalRiskManagement();
            TerminalActionAnalyze();

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
            string tag9F38 = tagDict.GetTag("9F38");
            var tls = DataParse.ParseTL(tag9F38);
            string pdolData = string.Empty;
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
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string AIP = tagDict.GetTag("82");
            if (IsSupportSDA(AIP))
            {
                if (!SDA())
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "SDA脱机数据认证失败");
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
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "Tag9F4B不存在");
                    return -7;
                }
                string issuerPublicKey = GetIssuerPublicKey();
                if (!DDA(issuerPublicKey, tag9F4B, ddolData))
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "DDA脱机数据认证失败");
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
            ApduResponse resp = GAC1(Constant.ARQC, CDOL1);
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
                    tagDict.SetTag("9F10", tag9F10);    //更新后的电子余额在此处返回
                    CheckAC(tag9F26);
                }
            }

            return 0;
        }
    }
}

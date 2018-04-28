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
    /*************************************************************
     * 该类描述了QPBOC交易流程
     *************************************************************/
    public class BusinessQPBOC : BusinessBase
    {
        private TagDict tagDict = TagDict.GetInstance();
        private ViewModelLocator locator = new ViewModelLocator();
        private IExcuteCase baseCase = new CaseBase();
        private bool isQPBOCTranction = false;

        /// <summary>
        /// 开始交易流程
        /// </summary>
        /// <param name="aid"></param>
        /// <param name="doDesTrans"></param>
        /// <param name="doSMTrans"></param>
        public override void DoTrans(string aid, bool doDesTrans, bool doSMTrans)
        {
            base.DoTrans(aid, doDesTrans, doSMTrans);

            locator.Terminal.TermianlSettings.Tag9C = "00";         //交易类型(消费交易)
            locator.Terminal.TermianlSettings.Tag9F66 = "2A000080"; //终端交易属性
            locator.Terminal.TermianlSettings.TagDF60 = "00";   //扩展交易指示位

            var caseNo = MethodBase.GetCurrentMethod().Name;
            // 基于DES算法的QPBOC流程
            if (doDesTrans)
            {
                locator.Terminal.TermianlSettings.TagDF69 = "00";   //SM算法支持指示位
                
                if(!SelectApp(aid))
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "选择应用失败，交易流程终止");
                    return;
                }
                var AFLs = GPOEx();
                if(AFLs.Count == 0)
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "GPO命令发送失败，交易流程终止");
                    return;
                }
                if(ReadAppRecords(AFLs))
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "读取应用记录失败，交易流程终止");
                    return;
                }

                //Step 4, 此时卡片可以离开读卡器，终端进行后续的步骤
                if (isQPBOCTranction)
                {
                    OfflineAuthcation();
                }
                else
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "进行QPBOC交易失败");
                    return;
                }

            }

            //基于国密算法的交易流程
            if (doSMTrans)   
            {
                locator.Terminal.TermianlSettings.TagDF69 = "01";
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
                if (ReadAppRecords(AFLs))
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "读取应用记录失败，交易流程终止");
                    return;
                }

                //Step 4, 此时卡片可以离开读卡器，终端进行后续的步骤
                if (isQPBOCTranction)
                {
                    OfflineAuthcation();
                }
                else
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "进行QPBOC交易失败");
                    return;
                }
            }
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
            if(response.SW == 0x9000)
            {
                if(ParseAndSave(response.Response))
                {
                    IExcuteCase excuteCase = new SelectAidCase();
                    excuteCase.ExcuteCase(response);
                    result = true;
                }
            }
            else
            {
                var caseNo = MethodBase.GetCurrentMethod().Name;
                if(response.SW != 0x9000)
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

            var tls = DataParse.ParseTL(tagDict.GetTag("9F38"));
            string PDOLData = string.Empty;
            foreach (var tl in tls)
            {
                PDOLData += locator.Terminal.TermianlSettings.GetTag(tl.Tag);
            }

            ApduResponse response = base.GPO(PDOLData);
            
            if (response.SW != 0x9000)
            {
                var caseNo = MethodBase.GetCurrentMethod().Name;
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "GPO命令发送失败，SW={0}", response.SW);
                return AFLs;
            }
            if(ParseAndSave(response.Response))
            {
                AFLs = DataParse.ParseAFL(tagDict.GetTag("94"));
                string tag9F26 = tagDict.GetTag("9F26");
                if (!string.IsNullOrEmpty(tag9F26))
                {
                    isQPBOCTranction = true;    //表明卡片支持QPBOC交易
                }

                IExcuteCase excuteCase = new GPOCase();
                excuteCase.ExcuteCase(response);
            }
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
                if(resp.SW != 0x9000)
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "读取应用记录失败,SW={0}", resp.SW);
                    return false;
                }
                if(!ParseAndSave(resp.Response))
                {
                    return false;
                }               
            }

            IExcuteCase excuteCase = new ReadRecordCase();
            excuteCase.ExcuteCase(resps);

            return true;
        }

        /// <summary>
        /// 脱机数据认证
        /// </summary>
        /// <returns></returns>
        protected int OfflineAuthcation()
        {
            int result;
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (aid.Length < 10)
            {
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "应用AID长度太短");
                return -1;
            }
            string rid = aid.Substring(0, 10);
            string CAIndex = tagDict.GetTag("8F");
            if (CAIndex.Length != 2)
            {
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取CA 索引,请检查8F是否存在");
                return -2;
            }
            string CAPublicKey = Authencation.GenCAPublicKey(CAIndex, rid);
            if (string.IsNullOrWhiteSpace(CAPublicKey))
            {
                baseCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取CA公钥，请检查RID及索引是否正确");
                return -3;
            }

            string issuerPublicKey;
            string issuerPublicCert = tagDict.GetTag("90");
            string PAN = tagDict.GetTag("5A");
            string signedStaticAppData = tagDict.GetTag("93");
            string iccPublicCert = tagDict.GetTag("9F46");
            string AIP = tagDict.GetTag("82");

            string tag9F37 = locator.Terminal.TermianlSettings.GetTag("9F37");
            string tag9F02 = locator.Terminal.TermianlSettings.GetTag("9F02");
            string tag5F2A = locator.Terminal.TermianlSettings.GetTag("5F2A");
            string tag9F69 = tagDict.GetTag("9F69");
            string hashInput = tag9F37 + tag9F02 + tag5F2A + tag9F69;
            string tag9F4B = tagDict.GetTag("9F4B");

            if (doDesTrans) //DES算法
            {
                string issuerPublicKeyRemainder = tagDict.GetTag("92");
                string issuerExp = tagDict.GetTag("9F32");
                issuerPublicKey = Authencation.GenDesIssuerPublicKey(CAPublicKey, issuerPublicCert, issuerPublicKeyRemainder, issuerExp);
                if (string.IsNullOrWhiteSpace(issuerPublicKey))
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取发卡行公钥，请检查tag90,92,9F32是否存在");
                    return -4;
                }

                result = Authencation.DES_SDA(issuerPublicKey, issuerExp, signedStaticAppData, toBeSignAppData, AIP);
                if (result != 0)
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "静态数据认证失败!");
                    return -5;
                }

                string iccPublicKeyRemainder = tagDict.GetTag("9F48");
                string iccPublicKeyExp = tagDict.GetTag("9F47");
                string iccPublicKey = Authencation.GenDesICCPublicKey(issuerPublicKey, iccPublicCert, iccPublicKeyRemainder, toBeSignAppData, iccPublicKeyExp, AIP);
                if (string.IsNullOrWhiteSpace(iccPublicKey))
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取IC卡公钥，请确保tag9F46,9F48,9F47是否存在");
                    return -6;
                }

                result = Authencation.DES_DDA(iccPublicKey, iccPublicKeyExp, tag9F4B, hashInput);
                if (result != 0)
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "动态数据认证失败!");
                    return -8;
                }
            }
            else //SM算法
            {
                issuerPublicKey = Authencation.GenSMIssuerPublicKey(CAPublicKey, issuerPublicCert, PAN);
                if (string.IsNullOrWhiteSpace(issuerPublicKey))
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取发卡行公钥，请检查tag90,5A是否存在");
                    return -4;
                }

                result = Authencation.SM_SDA(issuerPublicKey, toBeSignAppData, signedStaticAppData, AIP);
                if (result != 0)
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "SM算法 静态数据认证失败!");
                    return -9;
                }

                string iccPublicKey = Authencation.GenSMICCPublicKey(issuerPublicKey, iccPublicCert, toBeSignAppData, AIP, PAN);
                if (string.IsNullOrWhiteSpace(iccPublicKey))
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取IC卡公钥");
                    return -4;
                }
                result = Authencation.SM_DDA(iccPublicKey, tag9F4B, hashInput);
                if (result != 0)
                {
                    baseCase.TraceInfo(CaseLevel.Failed, caseNo, "SM算法 动态数据认证失败!");
                    return -10;
                }
            }
            return 0;
        }
    }
}

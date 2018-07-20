using System;
using System.Collections.Generic;
using CardPlatform.ViewModel;
using CardPlatform.Models;
using UtilLib;
using System.Reflection;
using CardPlatform.Config;
using CplusplusDll;
using System.Linq;
using CardPlatform.Common;
using GalaSoft.MvvmLight.Threading;

namespace CardPlatform.Cases
{
    /// <summary>
    /// 所有Case基类，描述了如何在界面中显示test case信息
    /// 交易流程中的每个阶段的case由配置文件配置
    /// </summary>
    public class CaseBase : IExcuteCase
    {
        private static Dictionary<string, List<TransStepCase>> caseDict = new Dictionary<string, List<TransStepCase>>();
        private static List<CaseInfo> caseInfos = new List<CaseInfo>();
        public string CurrentApp { get; set; }
        protected TransactionStep Step = TransactionStep.Base;
        private ViewModelLocator locator = new ViewModelLocator();
        private TransactionConfig TransConfig = TransactionConfig.GetInstance();

        public CaseBase()
        {
            Load();           
        }

        /// <summary>
        /// 基类中不执行任何case,由子类执行
        /// </summary>
        /// <param name="srcData"></param>
        public virtual void Excute(int batchNo, TransactionApp app, TransactionStep step,object srcData)
        {
            Step = step;
            Load();
            if(caseInfos.Count > 0)
            {
                foreach (var item in caseInfos)
                {
                    var methods = GetType().GetMethods(BindingFlags.Public | BindingFlags.Instance);
                    var methodsName = from method in methods select method.Name;
                    if (methodsName.Contains(item.CaseNo))
                        GetType().GetMethod(item.CaseNo).Invoke(this, null);
                }
            }
               
        }

        /// <summary>
        /// 检测模板数据是否合规
        /// </summary>
        public virtual void CheckTemplateTag(List<TLV> TLVs)
        {
            ViewModelLocator locator = new ViewModelLocator();
            var compareObj = DataTemplateConfig.GetInstance();
            foreach (var tlv in TLVs)
            {                               
                foreach (var item in locator.TemplateCompare.TemplateComparedInfos)
                {
                    if(CurrentApp == item.CurrentApp && Step == item.Step)
                    {
                        if (tlv.Tag == item.Tag)
                        {
                            item.CardValue = tlv.Value;
                            item.HasCheck = true;
                            if (tlv.Value == item.TemplateValue)
                            {
                                item.ActualLevel = TipLevel.Sucess;
                            }
                            else
                            {
                                item.ActualLevel = item.ConfigLevel;
                            }
                            break;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// 将test case输出到界面中
        /// </summary>
        /// <param name="level"></param>
        /// <param name="caseNo"></param>
        /// <param name="format"></param>
        /// <param name="args"></param>
        public virtual void TraceInfo(TipLevel level, string caseNo, string format, params object[] args)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
            {
                string description = string.Format(format, args);
                string caseLevel = string.Empty;
                TransInfoModel caseInfo = new TransInfoModel();

                caseInfo.CaseNo = caseNo;
                caseInfo.CaseInfo = description;
                caseInfo.Level = level;
                ViewModelLocator locator = new ViewModelLocator();
                locator.Transaction.CaseInfos.Add(caseInfo);
            });

        }


        /// <summary>
        /// 根据case编号获取描述信息
        /// </summary>
        /// <param name="caseNo"></param>
        /// <returns></returns>
        public CaseInfo GetCaseItem(string caseNo)
        {
            if(caseInfos != null)
            {
                var item = from info in caseInfos where caseNo == info.CaseNo select info;
                if (item != null)
                    return item.First();
            }

            return null;
        }

        /// <summary>
        /// 加载配置文件
        /// </summary>
        protected virtual void Load()
        {
            var caseConfig = CaseConfig.GetInstance();
            if(!caseConfig.HasLoaded)
            {
                caseDict = caseConfig.Load(".\\Configuration\\AppConfig\\CaseConfiguration.xml");
            }
            var allStepCases = new List<TransStepCase>();
            caseInfos.Clear();
            if (CurrentApp != null)
            {
                caseDict.TryGetValue(CurrentApp, out allStepCases);
                if (allStepCases != null)
                {
                    foreach (var stepCases in allStepCases)
                    {
                        if (stepCases.Step == Step)
                        {
                            caseInfos = new List<CaseInfo>(stepCases.Cases.ToArray());
                            break;
                        }
                    }
                }
            }
        }

        #region 一些复杂通用的case在基类中实现
        /// <summary>
        /// 根据MDK/UDK生成过程密钥
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        protected string GenSessionKey(string key, TransKeyType transKeyType, AlgorithmCategory algorithmFlag)
        {
            string sessionKey = string.Empty;
            string atc = TransactionTag.GetInstance().GetTag("9F36");
            if (transKeyType == TransKeyType.MDK)
            {
                string cardAcct = TransactionTag.GetInstance().GetTag("5A");
                string cardSeq = TransactionTag.GetInstance().GetTag("5F34");
                string udkKey = Authencation.GenUdk(key, cardAcct, cardSeq, (int)algorithmFlag);
                sessionKey = Authencation.GenUdkSessionKey(udkKey, atc, (int)algorithmFlag);
            }
            else
            {
                sessionKey = Authencation.GenUdkSessionKey(key, atc, (int)algorithmFlag);
            }
            return sessionKey;
        }

        public bool Check9F10Mac()
        {
            string tag9F10 = TransactionTag.GetInstance().GetTag("9F10");
            string tag9F10Mac = tag9F10.Substring(tag9F10.Length - 8);
            string atc = TransactionTag.GetInstance().GetTag("9F36");
            string data = string.Empty;

            if (tag9F10.Substring(18, 2) == "01" ||
                tag9F10.Substring(18, 2) == "06") //暂时只支持IDD为01,06类型
            {
                data = atc + tag9F10.Substring(20, 10) + "00";
            }           
            string cardAcct = TransactionTag.GetInstance().GetTag("5A");
            string cardSeq = TransactionTag.GetInstance().GetTag("5F34");
            string mac = string.Empty;
            string macSessionKey = string.Empty;
            if (TransConfig.AlgorithmFlag == AlgorithmCategory.DES)
            {
                if (string.IsNullOrEmpty(TransConfig.TransDesMacKey) || TransConfig.TransDesMacKey.Length != 32)
                    return false;
                macSessionKey = GenSessionKey(TransConfig.TransDesMacKey, TransConfig.KeyType, TransConfig.AlgorithmFlag);
            }
            else
            {
                if (string.IsNullOrEmpty(TransConfig.TransSmMacKey) || TransConfig.TransSmMacKey.Length != 32)
                    return false;
                macSessionKey = GenSessionKey(TransConfig.TransSmMacKey, TransConfig.KeyType, TransConfig.AlgorithmFlag);
            }
            mac = Authencation.GenTag9F10Mac(macSessionKey, data, (int)TransConfig.AlgorithmFlag);
            if (mac == tag9F10Mac)
            {
                return true;
            }
            return false;
        }

        public bool CheckAc()
        {      
            string tag9F02  = locator.Terminal.TermianlSettings.GetTag("9F02");  //授权金额
            string tag9F03  = locator.Terminal.TermianlSettings.GetTag("9F03");  //其他金额
            string tag9F1A  = locator.Terminal.TermianlSettings.GetTag("9F1A");  //终端国家代码
            string tag95    = locator.Terminal.TermianlSettings.GetTag("95");      //终端验证结果           
            string tag5A    = locator.Terminal.TermianlSettings.GetTag("5F2A");  //交易货币代码
            string tag9A    = locator.Terminal.TermianlSettings.GetTag("9A");      //交易日期
            string tag9C    = locator.Terminal.TermianlSettings.GetTag("9C");      //交易类型
            string tag9F37  = locator.Terminal.TermianlSettings.GetTag("9F37");  //不可预知数
            string tag82    = TransactionTag.GetInstance().GetTag("82");
            string tag9F36  = TransactionTag.GetInstance().GetTag("9F36");
            string tag9F10  = TransactionTag.GetInstance().GetTag("9F10");
            var cvr = tag9F10.Substring(6, 8);   //卡片验证结果

            string input = tag9F02 + tag9F03 + tag9F1A + tag95 + tag5A + tag9A + tag9C + tag9F37 + tag82 + tag9F36 + cvr;
            int zeroCount = input.Length % 16;
            if (zeroCount != 0)
            {
                input.PadRight(zeroCount, '0');
            }
            string acSessionKey = string.Empty;
            if (TransConfig.AlgorithmFlag == AlgorithmCategory.DES)
            {
                if (string.IsNullOrEmpty(TransConfig.TransDesAcKey) || TransConfig.TransDesAcKey.Length != 32)
                    return false;
                acSessionKey = GenSessionKey(TransConfig.TransDesAcKey, TransConfig.KeyType, TransConfig.AlgorithmFlag);
            }
            else
            {
                if (string.IsNullOrEmpty(TransConfig.TransSmAcKey) || TransConfig.TransSmAcKey.Length != 32)
                    return false;
                acSessionKey = GenSessionKey(TransConfig.TransSmAcKey, TransConfig.KeyType, TransConfig.AlgorithmFlag);
            }
            var mac = Authencation.GenAc(acSessionKey, input, (int)TransConfig.AlgorithmFlag);
            string tag9F26 = TransactionTag.GetInstance().GetTag("9F26");
            if(mac == tag9F26)
            {
                return true;
            }
            return false;
        }
        #endregion
    }
}

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
        private static Dictionary<string, List<TransStepCase>>  caseDict    = new Dictionary<string, List<TransStepCase>>();
        private static List<CaseInfo>                           caseInfos   = new List<CaseInfo>();        
        protected TransactionStep                               step        = TransactionStep.Base;
        protected static ViewModelLocator                       locator     = new ViewModelLocator();
        protected static TransactionConfig                      transConfig = TransactionConfig.GetInstance();
        protected static Log                                    log         = Log.CreateLog(Constant.LogPath);

        public CaseBase()
        {
            //Load();           
        }

        /// <summary>
        /// 基类中不执行任何case,由子类执行
        /// </summary>
        /// <param name="srcData"></param>
        public virtual void Excute(int batchNo, AppType app, TransactionStep step,object srcData)
        {
            this.step = step;
            Load();
            if(caseInfos.Count > 0)
            {
                foreach (var item in caseInfos)
                {
                    var methods = GetType().GetMethods(BindingFlags.Public | BindingFlags.Instance);
                    var methodsName = from method in methods select method.Name;
                    if (methodsName.Contains(item.CaseNo))
                    {
                        log.TraceLog("执行case [{0}]", item.CaseNo);
                        log.TraceLog("描述:{0}", item.Description);
                        var result = (TipLevel)GetType().GetMethod(item.CaseNo).Invoke(this, null);
                        string caseReuslt = "";
                        if (result == TipLevel.Failed) caseReuslt = "失败";
                        if (result == TipLevel.Sucess) caseReuslt = "成功";
                        if (result == TipLevel.Warn) caseReuslt = "警告";
                        if (result == TipLevel.Tip) caseReuslt = "提示";
                        if (result == TipLevel.Unknown) caseReuslt = "未知";
                        log.TraceLog("结果: {0}\n", caseReuslt);
                    }
                }
            }
            //case执行完之后，检测所有tag长度的规范性
            var tags = TransactionTag.GetInstance().GetTags(step);
            foreach (var item in tags)
            {
                CheckTagLen(item.Tag, item.Value);
            }
        }

        /// <summary>
        /// 检测模板数据是否合规
        /// </summary>
        public static void CheckTemplateTag()
        {
            ViewModelLocator locator = new ViewModelLocator();
            var transTags = TransactionTag.GetInstance();
                   
            foreach (var item in locator.TemplateCompare.TemplateComparedInfos)
            {
                if(transConfig.CurrentApp.ToString() == item.CurrentApp)
                {
                    item.CardValue = transTags.GetTag(item.Step, item.Tag);
                    item.HasCheck = true;
                    if (item.CardValue == item.TemplateValue)
                    {
                        item.ActualLevel = TipLevel.Sucess;
                    }
                    else
                    {
                        item.ActualLevel = item.ConfigLevel;
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
        public virtual TipLevel TraceInfo(TipLevel level, string caseNo, string format, params object[] args)
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

                if (level == TipLevel.Failed || level == TipLevel.Warn || level == TipLevel.Tip)
                {
                    locator.Transaction.ErrorCaseInfos.Add(caseInfo);
                }
            });
            return level;
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

        public static void ResetCase()
        {
            var caseConfig = CaseConfig.GetInstance();
            caseConfig.HasLoaded = false;
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
            string appName = transConfig.CurrentApp.ToString();
            if (appName.Contains("_"))
            {
                appName = appName.Substring(0, appName.IndexOf('_'));
            }
            caseDict.TryGetValue(appName, out allStepCases);
            if (allStepCases != null)
            {
                foreach (var stepCases in allStepCases)
                {
                    if (stepCases.Step == step)
                    {
                        caseInfos = new List<CaseInfo>(stepCases.Cases.ToArray());
                        break;
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
        protected string GenSessionKey(string key, AppKeyType transKeyType, AlgorithmType algorithmFlag)
        {
            string sessionKey = string.Empty;
            string atc = TransactionTag.GetInstance().GetTag("9F36");
            if (transKeyType == AppKeyType.MDK)
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
            if (transConfig.Algorithm == AlgorithmType.DES)
            {
                if (string.IsNullOrEmpty(transConfig.TransDesMacKey) || transConfig.TransDesMacKey.Length != 32)
                    return false;
                macSessionKey = GenSessionKey(transConfig.TransDesMacKey, transConfig.KeyType, transConfig.Algorithm);
            }
            else
            {
                if (string.IsNullOrEmpty(transConfig.TransSmMacKey) || transConfig.TransSmMacKey.Length != 32)
                    return false;
                macSessionKey = GenSessionKey(transConfig.TransSmMacKey, transConfig.KeyType, transConfig.Algorithm);
            }
            mac = Authencation.GenTag9F10Mac(macSessionKey, data, (int)transConfig.Algorithm);
            if (mac == tag9F10Mac)
            {
                return true;
            }
            return false;
        }

        public bool CheckVisaAc()
        {
            string tag9F02 = locator.Terminal.GetTag("9F02");  //授权金额
            string tag9F03 = locator.Terminal.GetTag("9F03");  //其他金额
            string tag9F1A = locator.Terminal.GetTag("9F1A");  //终端国家代码
            string tag95 = locator.Terminal.GetTag("95");      //终端验证结果           
            string tag5F2A = locator.Terminal.GetTag("5F2A");  //交易货币代码
            string tag9A = locator.Terminal.GetTag("9A");      //交易日期
            string tag9C = locator.Terminal.GetTag("9C");      //交易类型
            string tag9F37 = locator.Terminal.GetTag("9F37");  //不可预知数
            string tag82 = TransactionTag.GetInstance().GetTag("82");
            string tag9F36 = TransactionTag.GetInstance().GetTag("9F36");
            string tag9F10 = TransactionTag.GetInstance().GetTag("9F10");
            var cvr = tag9F10.Substring(6, 8);   //卡片验证结果

            string inputCVN10 = tag9F02 + tag9F03 + tag9F1A + tag95 + tag5F2A + tag9A + tag9C + tag9F37 + tag82 + tag9F36 + cvr;
            int zeroCount = inputCVN10.Length % 16;
            if (zeroCount != 0)
            {
                inputCVN10.PadRight(zeroCount, '0');
            }
            if (string.IsNullOrEmpty(transConfig.TransDesAcKey))
                return false;
            var mac = Authencation.GenEMVAC(transConfig.TransDesAcKey, inputCVN10);
            string tag9F26 = TransactionTag.GetInstance().GetTag("9F26");
            if (mac == tag9F26)
            {
                return true;
            }
            return false;
        }

        public bool CheckMcAc()
        {
            string tag9F02 = locator.Terminal.GetTag("9F02");  //授权金额
            string tag9F03 = locator.Terminal.GetTag("9F03");  //其他金额
            string tag9F1A = locator.Terminal.GetTag("9F1A");  //终端国家代码
            string tag95 = locator.Terminal.GetTag("95");      //终端验证结果           
            string tag5A = locator.Terminal.GetTag("5F2A");  //交易货币代码
            string tag9A = locator.Terminal.GetTag("9A");      //交易日期
            string tag9C = locator.Terminal.GetTag("9C");      //交易类型
            string tag9F37 = locator.Terminal.GetTag("9F37");  //不可预知数
            string tag82 = TransactionTag.GetInstance().GetTag("82");
            string tag9F36 = TransactionTag.GetInstance().GetTag("9F36");
            string tag9F10 = TransactionTag.GetInstance().GetTag("9F10");
            var cvr = tag9F10.Substring(4, 12);   //卡片验证结果
            var plaintextCounters = tag9F10.Substring(20, 16);

            string input = tag9F02 + tag9F03 + tag9F1A + tag95 + tag5A + tag9A + tag9C + tag9F37 + tag82 + tag9F36 + cvr;
            if (string.IsNullOrEmpty(transConfig.TransDesAcKey) || transConfig.TransDesAcKey.Length != 32)
                return false;
            //input += plaintextCounters;
            string acSessionKey = Authencation.GenMcSessionKeyCsk(transConfig.TransDesAcKey, tag9F36);
            //string acSessionKey = Authencation.GenMcSessionKeySkd(transConfig.TransDesAcKey, tag9F36,tag9F37);
            var mac = Authencation.GenAc(acSessionKey, input, (int)transConfig.Algorithm);
            string tag9F26 = TransactionTag.GetInstance().GetTag("9F26");
            if (mac == tag9F26)
            {
                return true;
            }
            return false;
        }

        public bool CheckPbocAc()
        {      
            string tag9F02  = locator.Terminal.GetTag("9F02");  //授权金额
            string tag9F03  = locator.Terminal.GetTag("9F03");  //其他金额
            string tag9F1A  = locator.Terminal.GetTag("9F1A");  //终端国家代码
            string tag95    = locator.Terminal.GetTag("95");      //终端验证结果           
            string tag5A    = locator.Terminal.GetTag("5F2A");  //交易货币代码
            string tag9A    = locator.Terminal.GetTag("9A");      //交易日期
            string tag9C    = locator.Terminal.GetTag("9C");      //交易类型
            string tag9F37  = locator.Terminal.GetTag("9F37");  //不可预知数
            string tag82    = TransactionTag.GetInstance().GetTag("82");
            string tag9F36  = TransactionTag.GetInstance().GetTag("9F36");
            string tag9F10  = TransactionTag.GetInstance().GetTag("9F10");
            var cvr = tag9F10.Substring(6, 8);   //卡片验证结果
            var plaintextCounters = tag9F10.Substring(20, 16);

            string input = tag9F02 + tag9F03 + tag9F1A + tag95 + tag5A + tag9A + tag9C + tag9F37 + tag82 + tag9F36 + cvr;
            int zeroCount = input.Length % 16;
            if (zeroCount != 0)
            {
                input.PadRight(zeroCount, '0');
            }
            string acSessionKey = string.Empty;
            if (transConfig.Algorithm == AlgorithmType.DES)
            {
                if (string.IsNullOrEmpty(transConfig.TransDesAcKey) || transConfig.TransDesAcKey.Length != 32)
                    return false;
                acSessionKey = GenSessionKey(transConfig.TransDesAcKey, transConfig.KeyType, transConfig.Algorithm);
            }
            else
            {
                if (string.IsNullOrEmpty(transConfig.TransSmAcKey) || transConfig.TransSmAcKey.Length != 32)
                    return false;
                acSessionKey = GenSessionKey(transConfig.TransSmAcKey, transConfig.KeyType, transConfig.Algorithm);
            }
            var mac = Authencation.GenAc(acSessionKey, input, (int)transConfig.Algorithm);
            string tag9F26 = TransactionTag.GetInstance().GetTag("9F26");
            if(mac == tag9F26)
            {
                return true;
            }
            return false;
        }

        public TipLevel CheckTagLen(string tag, string value)
        {
            var ignoreTags = new List<string> { "5A" };
            var caseNo = MethodBase.GetCurrentMethod().Name;

            if(!ignoreTags.Contains(tag))
            {
                if (!TagLenInfo.CheckTagLen(tag, value))
                {
                    var lenInfo = TagLenInfo.GetLenInfo(tag);
                    if (lenInfo == null)
                    {
                        return TraceInfo(TipLevel.Failed, caseNo, "未知的tag{0}，请在字典中添加该tag的引用", tag);
                    }
                    if (lenInfo.type == LenType.Range)
                        return TraceInfo(TipLevel.Failed, caseNo, "检测tag{0}长度是否符合规范.[tag{1}规范长度值在{2}~{3}之间，但实际长度为{4}]", tag, tag, lenInfo.min, lenInfo.max, value.Length / 2);
                    else
                        return TraceInfo(TipLevel.Failed, caseNo, "检测tag{0}长度是否符合规范.[tag{1}规范长度为{2}，但实际长度为{3}]", tag, tag, lenInfo.fixedLen, value.Length / 2);
                }
                return TraceInfo(TipLevel.Sucess, caseNo, "检测tag{0}长度是否符合规范", tag);
            }
            return TipLevel.Sucess;
        }
        #endregion
    }
}

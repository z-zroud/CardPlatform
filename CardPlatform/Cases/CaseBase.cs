﻿using System;
using System.Collections.Generic;
using CardPlatform.ViewModel;
using CardPlatform.Models;
using UtilLib;
using System.Reflection;
using System.Windows.Media;
using CardPlatform.Config;
using CplusplusDll;
using System.Linq;
using CardPlatform.Business;
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
        public string Step { get; set; }


        public CaseBase()
        {
            Step = "BaseStep";
            Load();           
        }

        /// <summary>
        /// 基类中不执行任何case,由子类执行
        /// </summary>
        /// <param name="srcData"></param>
        public virtual void ExcuteCase(object srcData)
        {
            //if(Step != Constant.STEP_READ_RECORD)
            //{
            //    var response = (ApduResponse)srcData;
            //    var TLVs = DataParse.ParseTLV(response.Response);
            //    CheckTemplateTag(TLVs);
            //}
            //else
            //{
            //    var resps = (List<ApduResponse>)srcData;
            //    foreach(var resp in resps)
            //    {
            //        var TLVs = DataParse.ParseTLV(resp.Response);
            //        CheckTemplateTag(TLVs);
            //    }
            //}

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
                                item.ColorMark = new SolidColorBrush(Colors.Black);
                                item.CaseLevel = "成功";
                            }
                            else
                            {
                                if (item.Level == TipLevel.Sucess) { item.ColorMark = new SolidColorBrush(Colors.Black); item.CaseLevel = "成功"; }
                                else if (item.Level == TipLevel.Warn) { item.ColorMark = new SolidColorBrush(Colors.Yellow); item.CaseLevel = "警告"; }
                                else if (item.Level == TipLevel.Failed) { item.ColorMark = new SolidColorBrush(Colors.Red); item.CaseLevel = "失败"; }
                            }
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
    }
}

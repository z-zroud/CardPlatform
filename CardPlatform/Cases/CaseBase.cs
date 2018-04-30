using System;
using System.Collections.Generic;
using CardPlatform.ViewModel;
using CardPlatform.Models;
using UtilLib;
using System.Windows.Media;
using CardPlatform.Config;
using CplusplusDll;

namespace CardPlatform.Cases
{
    /// <summary>
    /// 所有Case基类，描述了如何在界面中显示test case信息
    /// 交易流程中的每个阶段的case由配置文件配置
    /// </summary>
    public class CaseBase : IExcuteCase
    {
        private static Dictionary<string, Dictionary<string,CaseInfo>> CaseDict = new Dictionary<string, Dictionary<string, CaseInfo>>();
        private static Dictionary<string, CaseInfo> CaseInfos = new Dictionary<string, CaseInfo>();
        public string Step { get; set; }

        protected List<TLV> arrTLV;
        protected ApduResponse response;

        public CaseBase()
        {
            response = new ApduResponse();
            arrTLV = new List<TLV>();
            Step = "CaseBase";
            Load();           
        }

        /// <summary>
        /// 基类中不执行任何case,由子类执行
        /// </summary>
        /// <param name="srcData"></param>
        public virtual void ExcuteCase(object srcData)
        {
            response = (ApduResponse)srcData;
            arrTLV = DataParse.ParseTLV(response.Response);

            if(CaseInfos != null)
            {
                foreach (var item in CaseInfos.Keys)
                {
                    //var methods = GetType().GetMethods();
                    if (!string.IsNullOrEmpty(item))
                        GetType().GetMethod(item).Invoke(this, null);
                }
            }
            var compObj = DataTemplateConfig.GetInstance();
            if (!compObj.HasLoaded)
                compObj.Load("ProjectTemplate");
        }

        /// <summary>
        /// 将test case输出到界面中
        /// </summary>
        /// <param name="level"></param>
        /// <param name="caseNo"></param>
        /// <param name="format"></param>
        /// <param name="args"></param>
        public virtual void TraceInfo(CaseLevel level, string caseNo, string format, params object[] args)
        {
            string description = string.Format(format, args);
            string caseLevel = string.Empty;
            TransInfoModel caseInfo = new TransInfoModel();

            if (level == CaseLevel.Sucess) { caseInfo.ColorMark = new SolidColorBrush(Colors.Black); caseLevel = "成功"; }
            else if (level == CaseLevel.Warn) { caseInfo.ColorMark = new SolidColorBrush(Colors.Yellow); caseLevel = "警告"; }
            else if (level == CaseLevel.Failed) { caseInfo.ColorMark = new SolidColorBrush(Colors.Red); caseLevel = "失败"; }

            caseInfo.CaseNo = caseNo;
            caseInfo.CaseInfo = description;
            caseInfo.CaseLevel = caseLevel;

            ViewModelLocator locator = new ViewModelLocator();
            locator.Transaction.CaseInfos.Add(caseInfo);
        }


        /// <summary>
        /// 根据case编号获取描述信息
        /// </summary>
        /// <param name="caseNo"></param>
        /// <returns></returns>
        public CaseInfo GetCaseItem(string caseNo)
        {
            if(CaseInfos != null)
            {
                CaseInfo item;
                if (CaseInfos.TryGetValue(caseNo, out item))
                {
                    return item;
                }
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
                CaseDict = caseConfig.Load("CaseConfiguration.xml");
            }
            CaseDict.TryGetValue(Step, out CaseInfos);
        }
    }
}

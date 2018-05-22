using CardPlatform.Business;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Linq;

namespace CardPlatform.Config
{
    /// <summary>
    /// 定义了当Case执行不成功时，该如何显示Case结果
    /// </summary>
    public enum TipLevel
    {
        Sucess = 0,
        Warn = 1,
        Failed = 2
    }


    /// <summary>
    /// 定义了在UI界面中执行Case显示的内容
    /// </summary>
    public class CaseInfo
    {
        public string CaseApp { get; set; }
        public string CaseStep { get; set; }
        public string CaseNo { get; set; }
        public string Description { get; set; }
        public string Result { get; set; }
        public TipLevel Level { get; set; }
    }

    /// <summary>
    /// 定义每个交易流程中需要执行的Case
    /// </summary>
    public class TransStepCase
    {
        public TransStepCase()
        {
            Cases = new List<CaseInfo>();
        }

        public string Step { get; set; }
        public List<CaseInfo> Cases { get; set; }
    }

    public class CaseConfig
    {
        private static CaseConfig config;
        private Dictionary<string,List<TransStepCase>> allAppCases;

        public bool HasLoaded { get; private set; }

        private CaseConfig()
        {
            HasLoaded = false;
            allAppCases = new Dictionary<string, List<TransStepCase>>();
        }

        /// <summary>
        /// 配置类均为单例类
        /// </summary>
        /// <returns></returns>
        public static CaseConfig GetInstance()
        {
            if (config == null)
            {
                config = new CaseConfig();
            }
            return config;
        }

        /// <summary>
        /// 加载配置文件
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        public Dictionary<string, List<TransStepCase>> Load(string path)
        {
            XDocument doc = XDocument.Load(path);
            if (doc != null)
            {
                var root = doc.Root;
                var apps = new List<string>
                {
                    Constant.APP_PSE,
                    Constant.APP_PPSE,
                    Constant.APP_UICS,
                    Constant.APP_ECC,
                    Constant.APP_QUICS
                };
                foreach (var app in apps)
                {
                    var appNode = root.Element(app);
                    if (appNode == null)
                        continue;   //表明没有此应用的模板
                    string appName = appNode.Name.LocalName;    //应用类型
                    var stepCaseNodes = appNode.Elements("StepCase");   //交易步骤节点
                    var appCase = new List<TransStepCase>();
                    foreach (var stepCase in stepCaseNodes) //遍历每个交易步骤节点下的Case
                    {
                        var caseItems = stepCase.Elements("CaseItem");
                        var transStepCase = new TransStepCase();
                        transStepCase.Step = stepCase.Attribute("name").Value;
                        foreach (var item in caseItems)
                        {
                            CaseInfo caseInfo = new CaseInfo();
                            caseInfo.CaseApp = appName;
                            caseInfo.CaseStep = transStepCase.Step;
                            caseInfo.CaseNo = item.Attribute("num").Value;
                            caseInfo.Description = item.Attribute("desc").Value;
                            caseInfo.Level = (TipLevel)Enum.Parse(typeof(TipLevel), item.Attribute("level").Value, true);

                            transStepCase.Cases.Add(caseInfo);
                        }
                        appCase.Add(transStepCase); //保存了应用下的case案例
                    }
                    allAppCases.Add(appName, appCase); //保存所有应用下的case案例
                }
            }
            HasLoaded = true;   //只load一次
            return allAppCases;
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Linq;
using CardPlatform.Business;

namespace CardPlatform.Config
{
    /// <summary>
    /// 定义值检测模式，固定值比较/正则表达式模式
    /// </summary>
    public enum CheckMode
    {
        Fixed = 0,
        Regex = 1
    }

    /// <summary>
    /// 定义了TemplateTag数据结构
    /// </summary>
    public class TemplateTag
    {
        public string Name { get; set; }
        public string Value { get; set; }
        public TipLevel Level { get; set; }
        public CheckMode Mode { get; set; }
    }

    /// <summary>
    /// 定义了在每个交易流程步骤中，需要校验tag值的集合
    /// </summary>
    public class ProcssStep
    {
        public ProcssStep()
        {
            Tags = new List<TemplateTag>();
        }
        public string Step { get; set; }
        public List<TemplateTag> Tags;
    }

    public class DataTemplateConfig
    {
        private static DataTemplateConfig config;
        public Dictionary<string, List<ProcssStep>> TemplateTags { get; private set; }

        private DataTemplateConfig()
        {
            TemplateTags = new Dictionary<string, List<ProcssStep>>();
        }

        public static DataTemplateConfig GetInstance()
        {
            if(config == null)
            {
                config = new DataTemplateConfig();
            }
            return config;
        }

        /// <summary>
        /// 加载配置文件
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        public void Load(string path)
        {
            XDocument doc = XDocument.Load(path);
            if (doc != null)
            {
                var root = doc.Root;
                var apps = new List<string> { Constant.APP_UICS, Constant.APP_ECC, Constant.APP_QUICS };
                foreach(var app in apps)
                {
                    var appNode = root.Element(app);
                    if (appNode == null)
                        continue;   //表明没有此应用的模板
                    string appName = appNode.Name.LocalName;
                    var steps = appNode.Element("TagTemplate").Elements("Step");
                    var appTags = new List<ProcssStep>();
                    foreach(var step in steps)
                    {
                        var templateTags = step.Elements("Tag");
                        var stepName = step.Attribute("name");                
                        var processStep = new ProcssStep();
                        processStep.Step = stepName.Value;
                        foreach (var item in templateTags)
                        {
                            var templateTag = new TemplateTag();
                            templateTag.Name = item.Attribute("name").Value;
                            templateTag.Mode = (CheckMode)Enum.Parse(typeof(CheckMode), item.Attribute("checkMode").Value, true);
                            templateTag.Value = item.Attribute("value").Value;
                            templateTag.Level = (TipLevel)Enum.Parse(typeof(TipLevel), item.Attribute("level").Value, true);
                            processStep.Tags.Add(templateTag);
                        }
                        appTags.Add(processStep);
                    }
                    TemplateTags.Add(appName, appTags);
                }

            }
        }

        /// <summary>
        /// 获取指定的tag
        /// </summary>
        /// <param name="tag"></param>
        /// <returns></returns>
        public TemplateTag GetTemplateTag(string app, string step, string tag)
        {
            if (TemplateTags.ContainsKey(app))
            {
                var stepDict = TemplateTags[app];
                foreach(var processStep in stepDict)
                {
                    if(processStep.Step == step)
                    {
                        var templateTag = from item in processStep.Tags where tag == item.Name select item;
                        if (templateTag != null)
                            return templateTag.First();
                        break;
                    }
                }
            }
                
            return null;
        }
    }
}

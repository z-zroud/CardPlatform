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
    /// 定义值检测模式，固定值比较/正则表达式模式
    /// </summary>
    public enum CheckMode
    {
        FixedMode = 0,
        RegMode = 1
    }

    /// <summary>
    /// 定义了TemplateTag数据结构
    /// </summary>
    public class TemplateTag
    {
        public string Name { get; set; }
        public string Value { get; set; }
        public string TipLevel { get; set; }
        public CheckMode Mode { get; set; }
        public bool HasCheckted { get; set; }
    }

    public class DataTemplateConfig
    {
        private static DataTemplateConfig config;
        private Dictionary<string, TemplateTag> TemplateTags;

        public bool HasLoaded { get; private set; }

        private DataTemplateConfig()
        {
            HasLoaded = false;
            TemplateTags = new Dictionary<string, TemplateTag>();
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
            if(!HasLoaded)
            {
                XDocument doc = XDocument.Load(path);
                if (doc != null)
                {
                    HasLoaded = true;
                    var root = doc.Root;
                    var templateTags = root.Element("TagTemplate").Elements("Tag");
                    foreach (var item in templateTags)
                    {
                        var templateTag = new TemplateTag();
                        templateTag.Name = item.Attribute("name").Value;
                        templateTag.Mode = (CheckMode)Enum.Parse(typeof(CheckMode), item.Attribute("checkMode").Value);
                        templateTag.Value = item.Attribute("value").Value;
                        templateTag.TipLevel = item.Attribute("level").Value;
                        templateTag.HasCheckted = false;
                        TemplateTags.Add(templateTag.Name, templateTag);
                    }
                }
            }
        }

        /// <summary>
        /// 获取指定的tag
        /// </summary>
        /// <param name="tag"></param>
        /// <returns></returns>
        public TemplateTag GetTemplateTag(string tag)
        {
            if (TemplateTags.ContainsKey(tag))
            {
                TemplateTags[tag].HasCheckted = true;
                return TemplateTags[tag];
            }
                
            return null;
        }
    }
}

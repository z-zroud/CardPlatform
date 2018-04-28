﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace CardPlatform.Config
{
    /// <summary>
    /// 定义了模板数据结构
    /// </summary>
    public class TemplateTag
    {
        public string Name { get; set; }
        public string Value { get; set; }
        public string TipLevel { get; set; }
    }

    public class DataTemplateConfig
    {
        private static DataTemplateConfig config;
        private Dictionary<string, TemplateTag> ComparedTags;

        public bool HasLoaded { get; private set; }

        private DataTemplateConfig()
        {
            HasLoaded = false;
            ComparedTags = new Dictionary<string, TemplateTag>();
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
        public bool Load(string path)
        {
            var reader = new XmlTextReader(path);
            if (reader == null)
            {
                return false;
            }
            HasLoaded = true;
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    if (reader.Name == "Tag")
                    {
                        TemplateTag comparedTag = new TemplateTag();
                        bool hasAttr = reader.MoveToFirstAttribute();
                        while (hasAttr)
                        {
                            if (reader.Name == "name") { comparedTag.Name = reader.Value; }
                            else if (reader.Name == "TipLevel") { comparedTag.TipLevel = reader.Value; }
                            else if (reader.Name == "value") { comparedTag.Value = reader.Value; }

                            hasAttr = reader.MoveToNextAttribute();
                        }
                        if(!string.IsNullOrWhiteSpace(comparedTag.Name))
                            ComparedTags.Add(comparedTag.Name, comparedTag);
                    }
                }
            }

            return true;
        }

        /// <summary>
        /// 获取指定的tag
        /// </summary>
        /// <param name="tag"></param>
        /// <returns></returns>
        public TemplateTag GetTemplateTag(string tag)
        {
            if (ComparedTags.ContainsKey(tag))
                return ComparedTags[tag];
            return null;
        }

    }
}

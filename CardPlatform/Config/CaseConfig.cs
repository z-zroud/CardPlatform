using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

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

    public class CaseConfig
    {
        private static CaseConfig config;
        private Dictionary<string,Dictionary<string, CaseInfo>> CaseDict;

        public bool HasLoaded { get; private set; }

        private CaseConfig()
        {
            HasLoaded = false;
            CaseDict = new Dictionary<string, Dictionary<string, CaseInfo>>();
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
        public Dictionary<string, Dictionary<string, CaseInfo>> Load(string path)
        {
            var reader = new XmlTextReader(path);
            if (reader == null)
            {
                return null;
            }
            HasLoaded = true;
            string casesName = string.Empty;
            Dictionary<string, CaseInfo> items = new Dictionary<string, CaseInfo>();
            while (reader.Read())
            {               
                if (reader.NodeType == XmlNodeType.Element)
                {
                    if (reader.Name == "Cases")
                    {
                        items = new Dictionary<string, CaseInfo>();
                        casesName = reader.GetAttribute("name");
                    }else if(reader.Name == "CaseItem")
                    {
                        CaseInfo caseInfo = new CaseInfo();
                        caseInfo.CaseNo = reader.GetAttribute("num");
                        caseInfo.Description = reader.GetAttribute("desc");
                        caseInfo.Level = (TipLevel)Enum.Parse(typeof(TipLevel), reader.GetAttribute("level"), true);
                        
                        items.Add(caseInfo.CaseNo, caseInfo);
                    }
                }else if(reader.NodeType == XmlNodeType.EndElement)
                {
                    if(reader.Name == "Cases")
                    {
                        CaseDict.Add(casesName, items);
                    }
                }
            }

            return CaseDict;
        }
    }
}

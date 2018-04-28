using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace CardPlatform.Models
{


    public class DpDll
    {
        [XmlAttribute]
        public string Name { get; set; }

        [XmlAttribute]
        public string DllName { get; set; }

        [XmlAttribute]
        public string FuncName { get; set; }
    }

    
    public class DpParseResultModel : ObservableObject
    {
        /// <summary>
        /// 返回是否成功字符串信息
        /// </summary>
        private string _info;
        public string Info
        {
            get { return _info; }
            set
            {
                Set(ref _info, value);
            }
        }

        /// <summary>
        /// 状态颜色
        /// </summary>
        public string StatusColor { get; set; }
    }

    public class DpGenCpsModel : ObservableObject
    {
        /// <summary>
        /// 解析之后的CPS文件
        /// </summary>
        private string _cpsFilePath;
        public string CpsFilePath
        {
            get { return _cpsFilePath; }
            set
            {
                Set(ref _cpsFilePath, value);
            }
        }

        /// <summary>
        /// 文件名
        /// </summary>
        private string _cpsFileName;
        public string CpsFileName
        {
            get { return _cpsFileName; }
            set
            {
                Set(ref _cpsFileName, value);
            }
        }

        /// <summary>
        /// 是否需要进行个人化
        /// </summary>
        private bool _isSelected;
        public bool IsSelected
        {
            get { return _isSelected; }
            set
            {
                Set(ref _isSelected, value);
            }
        }
    }

    public class DpParseModel : ObservableObject
    {
        public DpParseModel()
        {
            DpTypeCollection = new List<DpDll>();
        }

        /// <summary>
        /// DP文件路径
        /// </summary>
        private string _dpFilePath;
        public string DpFilePath
        {
            get { return _dpFilePath; }
            set
            {
                Set(ref _dpFilePath, value);
            }
        }

        /// <summary>
        /// DP数据类型
        /// </summary>
        private List<DpDll> _dpTypeCollection;
        public List<DpDll> DpTypeCollection
        {
            get { return _dpTypeCollection; }
            set
            {
                Set(ref _dpTypeCollection, value);
            }
        }

        /// <summary>
        /// 规则配置路径
        /// </summary>
        private string _ruleFilePath;
        public string RuleFilePath
        {
            get { return _ruleFilePath; }
            set
            {
                Set(ref _ruleFilePath, value);
            }
        }
    }

    public class PersonlizeConfiguartion
    {
        public PersonlizeConfiguartion()
        {
            DpType = new List<DpDll>();
            ISDs = new List<string>();
            KMCs = new List<string>();
            Secure = new List<string>();
            DivType = new List<string>();
            DelInst = new List<string>();
        }
        public List<DpDll> DpType { get; set; }

        [XmlArray(ElementName ="AIDs")]
        [XmlArrayItem(ElementName ="AID")]
        public List<string> ISDs { get; set; }

        [XmlArray(ElementName = "KMCs")]
        [XmlArrayItem(ElementName = "KMC")]
        public List<string> KMCs { get; set; }

        [XmlArray(ElementName = "DivType")]
        [XmlArrayItem(ElementName = "Type")]
        public List<string> DivType { get; set; }

        [XmlArray(ElementName = "Secure")]
        [XmlArrayItem(ElementName = "SecureLevel")]
        public List<string> Secure { get; set; }

        [XmlArray(ElementName = "DelInst")]
        [XmlArrayItem(ElementName = "Type")]
        public List<string> DelInst { get; set; }
    }
}

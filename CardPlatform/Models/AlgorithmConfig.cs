using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace CardPlatform.Models
{
    public class AlgorithmConfig
    {
        public AlgorithmConfig()
        {
            AlgorithmType = new List<string>();
            FileDecryptType = new List<string>();
            KMCs = new List<string>();
            DivType = new List<string>();
            Secure = new List<string>();
        }

        [XmlArray(ElementName = "Algorithm")]
        [XmlArrayItem(ElementName = "Type")]
        public List<string> AlgorithmType { get; set; }

        [XmlArray(ElementName = "FileDecryptAlgorithm")]
        [XmlArrayItem(ElementName = "Type")]
        public List<string> FileDecryptType { get; set; }

        [XmlArray(ElementName = "KMCs")]
        [XmlArrayItem(ElementName = "KMC")]
        public List<string> KMCs{ get; set; }

        [XmlArray(ElementName = "DivType")]
        [XmlArrayItem(ElementName = "Type")]
        public List<string> DivType { get; set; }

        [XmlArray(ElementName = "Secure")]
        [XmlArrayItem(ElementName = "SecureLevel")]
        public List<string> Secure { get; set; }
    }
}

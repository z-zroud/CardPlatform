using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace CardPlatform.Models
{
    public class TransConfig
    {
        [XmlArray(ElementName = "TransCategory")]
        [XmlArrayItem(ElementName = "Category")]
        public List<string> TransCates { get; set; }

        [XmlArray(ElementName = "KeyTypeList")]
        [XmlArrayItem(ElementName = "KeyType")]
        public List<string> KeyTypeList { get; set; }
    }
}

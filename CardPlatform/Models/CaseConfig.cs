using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace CardPlatform.Models
{
    public class CaseItem
    {
        public string CaseNo { get; set; }

        public string CaseDescription { get; set; }
    }
    public class CaseConfig
    {
        public CaseConfig()
        {
            CaseList = new List<CaseItem>();
        }

        [XmlArray(ElementName = "Cases")]
        public List<CaseItem> CaseList { get; set; }
    }
}

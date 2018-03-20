using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace CardPlatform.Models
{
    public class DpParsedProgram
    {
        [XmlAttribute]
        public string Name { get; set; }

        [XmlAttribute]
        public string ProgramName { get; set; }

        [XmlAttribute]
        public string FuncName { get; set; }
    }

    public class DpParseStatus : ViewModelBase
    {
        private string _info;
        public string Info
        {
            get { return _info; }
            set
            {
                Set(ref _info, value);
            }
        }
        public string StatusColor { get; set; }
    }

    public class DpFileStatus : ViewModelBase
    {
        private string _dpFileName;
        public string DpFileName
        {
            get { return _dpFileName; }
            set
            {
                Set(ref _dpFileName, value);
            }
        }

        private bool _isSelected;
        public bool IsSelected
        {
            get { return _isSelected; }
            set
            {
                Set(ref _isSelected, value);
            }
        }

        private string _dpFilePath;
        public string DpFilePath
        {
            get { return _dpFilePath; }
            set
            {
                Set(ref _dpFilePath, value);
            }
        }
    }

    public class PersonlizeConfiguartion
    {
        public PersonlizeConfiguartion()
        {
            DpType = new List<DpParsedProgram>();
            ISDs = new List<string>();
            KMCs = new List<string>();
        }
        public List<DpParsedProgram> DpType { get; set; }

        [XmlArray(ElementName ="AIDs")]
        [XmlArrayItem(ElementName ="AID")]
        public List<string> ISDs { get; set; }

        [XmlArray(ElementName = "KMCs")]
        [XmlArrayItem(ElementName = "KMC")]
        public List<string> KMCs { get; set; }
    }
}

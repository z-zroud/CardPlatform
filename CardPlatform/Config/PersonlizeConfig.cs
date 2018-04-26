using CardPlatform.Models;
using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Config
{
    public class PersonlizeConfig : ObservableObject
    {
        public PersonlizeConfig()
        {
            DpType = new DpDll();
        }

        public string ConfigName { get; set; }

        private string _dpFilePath;
        public string DpFilePath
        {
            get { return _dpFilePath; }
            set
            {
                Set(ref _dpFilePath, value);
            }
        }

        private DpDll _dpType;
        public DpDll DpType
        {
            get { return _dpType; }
            set
            {
                Set(ref _dpType, value);
            }
        }

        private string _dpRulePath;
        public string DpRulePath
        {
            get { return _dpRulePath; }
            set
            {
                Set(ref _dpRulePath, value);
            }
        }

        private string _isd;
        public string ISD
        {
            get { return _isd; }
            set
            {
                Set(ref _isd, value);
            }
        }

        private string _KMC;
        public string KMC
        {
            get { return _KMC; }
            set
            {
                Set(ref _KMC, value);
            }
        }

        private int _divMethod;
        public int DivMethod
        {
            get { return _divMethod; }
            set
            {
                Set(ref _divMethod, value);
            }
        }

        private int _secureLevel;
        public int SecureLevel
        {
            get { return _secureLevel; }
            set
            {
                Set(ref _secureLevel, value);
            }
        }

        private string _installParamsFilePath;
        public string InstallParamsFilePath
        {
            get { return _installParamsFilePath; }
            set
            {
                Set(ref _installParamsFilePath, value);
            }
        }

        private int _delInst;
        public int DelInst
        {
            get { return _delInst; }
            set
            {
                Set(ref _delInst, value);
            }
        }
    }
}

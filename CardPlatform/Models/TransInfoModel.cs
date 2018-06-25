using CardPlatform.Config;
using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;

namespace CardPlatform.Models
{
    public class TransInfoModel : ObservableObject
    {
        public TransInfoModel()
        {
        }

        private string _caseNo;
        public string CaseNo
        {
            get { return _caseNo; }
            set
            {
                Set(ref _caseNo, value);
            }
        }

        private string _caseInfo;
        public string CaseInfo
        {
            get { return _caseInfo; }
            set
            {
                Set(ref _caseInfo, value);
            }
        }

        private TipLevel _tipLevel;
        public TipLevel Level
        {
            get { return _tipLevel; }
            set
            {
                Set(ref _tipLevel, value);
            }
        }

    }
}

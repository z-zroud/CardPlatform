using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Models
{
    public class TransResultModel : ObservableObject
    {
        private string _PBOCResult;
        public string PBOCResult
        {
            get { return _PBOCResult; }
            set
            {
                Set(ref _PBOCResult, value);
            }
        }

        private string _UICSResult;
        public string UICSResult
        {
            get { return _UICSResult; }
            set
            {
                Set(ref _UICSResult, value);
            }
        }

        private string _ECCResult;
        public string ECCResult
        {
            get { return _ECCResult; }
            set
            {
                Set(ref _ECCResult, value);
            }
        }

        private string _QPBOCResult;
        public string QPBOCResult
        {
            get { return _QPBOCResult; }
            set
            {
                Set(ref _QPBOCResult, value);
            }
        }
    }
}

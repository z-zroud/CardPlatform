using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Models
{
    public class AlgorithmModel : ObservableObject
    {
        private bool _isCheckDES;
        public bool IsCheckDES
        {
            get { return _isCheckDES; }
            set
            {
                Set(ref _isCheckDES, value);
            }
        }

        private bool _isCheckSM;
        public bool IsCheckSM
        {
            get { return _isCheckDES; }
            set
            {
                Set(ref _isCheckSM, value);
            }
        }
    }
}

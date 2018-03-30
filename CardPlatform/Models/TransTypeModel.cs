using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Models
{
    public class TransTypeModel : ObservableObject
    {
        private bool _isCheckPBOC;
        public bool IsCheckPBOC
        {
            get { return _isCheckPBOC; }
            set
            {
                Set(ref _isCheckPBOC, value);
            }
        }

        private bool _isCheckUICS;
        public bool IsCheckUICS
        {
            get { return _isCheckUICS; }
            set
            {
                Set(ref _isCheckUICS, value);
            }
        }

        private bool _isCheckECC;
        public bool IsCheckECC
        {
            get { return _isCheckECC; }
            set
            {
                Set(ref _isCheckECC, value);
            }
        }

        private bool _isCheckQPBOC;
        public bool IsCheckQPBOC
        {
            get { return _isCheckQPBOC; }
            set
            {
                Set(ref _isCheckQPBOC, value);
            }
        }
    }

}

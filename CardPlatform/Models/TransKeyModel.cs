using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Models
{
    public class TransKeyModel : ObservableObject
    {
        private string _desAC;
        public string DES_AC
        {
            get { return _desAC; }
            set
            {
                Set(ref _desAC, value);
            }
        }

        private string _desMAC;
        public string DES_MAC
        {
            get { return _desMAC; }
            set
            {
                Set(ref _desMAC, value);
            }
        }

        private string _desENC;
        public string DES_ENC
        {
            get { return _desENC; }
            set
            {
                Set(ref _desENC, value);
            }
        }

        private string _smAC;
        public string SM_AC
        {
            get { return _smAC; }
            set
            {
                Set(ref _smAC, value);
            }
        }

        private string _smMAC;
        public string SM_MAC
        {
            get { return _smMAC; }
            set
            {
                Set(ref _smMAC, value);
            }
        }

        private string _smENC;
        public string SM_ENC
        {
            get { return _smENC; }
            set
            {
                Set(ref _smENC, value);
            }
        }
    }
}

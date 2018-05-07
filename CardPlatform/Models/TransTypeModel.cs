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
        public TransTypeModel()
        {
            IsCheckUICS = true;
        }

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

        private bool _isCheckVISA;
        public bool IsCheckVISA
        {
            get { return _isCheckVISA; }
            set
            {
                Set(ref _isCheckVISA, value);
            }
        }

        private bool _isCheckMC;
        public bool IsCheckMC
        {
            get { return _isCheckMC; }
            set
            {
                Set(ref _isCheckMC, value);
            }
        }

        private bool _isCheckJETCO;
        public bool IsCheckJETCO
        {
            get { return _isCheckJETCO; }
            set
            {
                Set(ref _isCheckJETCO, value);
            }
        }

        private bool _isCheckAMEX;
        public bool IsCheckAMEX
        {
            get { return _isCheckAMEX; }
            set
            {
                Set(ref _isCheckAMEX, value);
            }
        }
    }

}

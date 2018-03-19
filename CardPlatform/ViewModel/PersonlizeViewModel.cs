using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using GalaSoft.MvvmLight;

namespace CardPlatform.ViewModel
{
    public class PersonlizeViewModel : ViewModelBase
    {
        private string _dpPath;
        public string DpPath
        {
            get { return _dpPath; }
            set
            {
                Set(ref _dpPath, value);
            }
        }

        private List<string> _dpType;
        public List<string> DpType
        {
            get { return _dpType; }
            set
            {
                Set(ref _dpType, value);
            }
        }

        private string _SelectedDpType;
        public string SelectedDpType
        {
            get { return _SelectedDpType; }
            set
            {
                Set(ref _SelectedDpType, value);
            }
        }

        private string _paramPath;
        public string ParamPath
        {
            get { return _paramPath; }
            set
            {
                Set(ref _paramPath, value);
            }
        }

        private ICommand _parseDpCmd;
        public ICommand ParseDpCmd
        {
            get { return _parseDpCmd; }
            set { _parseDpCmd = value; }
        }


        private void DoParseDp()
        {

        }
    }
}

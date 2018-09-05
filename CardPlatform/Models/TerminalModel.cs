using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Models
{
    public class TerminalModel : ObservableObject
    {
        private string _tag;
        public string Tag
        {
            get { return _tag; }
            set
            {
                Set(ref _tag, value);
            }
        }

        private string _tagValue;
        public string TagValue
        {
            get { return _tagValue; }
            set
            {
                Set(ref _tagValue, value);
            }
        }

        private string _mark;
        public string Mark
        {
            get { return _mark; }
            set
            {
                Set(ref _mark, value);
            }
        }
    }
}

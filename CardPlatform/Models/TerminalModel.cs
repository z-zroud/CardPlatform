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
        public TerminalModel()
        {
            Tag9F02 = "000000000100";
            Tag9F03 = "000000000000";
            Tag9F04 = "000000000000";
            Tag9F1A = "0156";
            Tag5F2A = "0156";
            Tag95 = "0000000000";
            Tag9F66 = "26000080";
            Tag9F4E = "476F6C647061635F54435F4465706172746D656E74";
        }
        private string _tag9F02;
        public string Tag9F02
        {
            get { return _tag9F02; }
            set
            {
                Set(ref _tag9F02, value);
            }
        }

        private string _tag9F03;
        public string Tag9F03
        {
            get { return _tag9F03; }
            set
            {
                Set(ref _tag9F03, value);
            }
        }

        private string _tag9F04;
        public string Tag9F04
        {
            get { return _tag9F04; }
            set
            {
                Set(ref _tag9F04, value);
            }
        }

        private string _tag9F1A;
        public string Tag9F1A
        {
            get { return _tag9F1A; }
            set
            {
                Set(ref _tag9F1A, value);
            }
        }

        private string _tag5F2A;
        public string Tag5F2A
        {
            get { return _tag5F2A; }
            set
            {
                Set(ref _tag5F2A, value);
            }
        }

        private string _tag95;
        public string Tag95
        {
            get { return _tag95; }
            set
            {
                Set(ref _tag95, value);
            }
        }

        private string _tag9F66;
        public string Tag9F66
        {
            get { return _tag9F66; }
            set
            {
                Set(ref _tag9F66, value);
            }
        }

        private string _tag9F4E;
        public string Tag9F4E
        {
            get { return _tag9F4E; }
            set
            {
                Set(ref _tag9F4E, value);
            }
        }

        public string Tag9F7A { get; set; }
        public string TagDF69 { get; set; }
        public string Tag8A { get; set; }
        public string TagDF60 { get; set; }
        public string Tag9C { get; set; }
    }
}

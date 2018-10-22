using CardPlatform.Common;
using GalaSoft.MvvmLight;
using System.Windows.Media;

namespace CardPlatform.Models
{
    public class TransResultModel : ObservableObject
    {
        private string _tag5A;
        public string Tag5A
        {
            get { return _tag5A; }
            set
            {
                Set(ref _tag5A, value);
            }
        }

        private string _tag5F34;
        public string Tag5F34
        {
            get { return _tag5F34; }
            set
            {
                Set(ref _tag5F34, value);
            }
        }

        private string _tag5F20;
        public string Tag5F20
        {
            get { return _tag5F20; }
            set
            {
                Set(ref _tag5F20, value);
            }
        }


        private string _tag50;
        public string Tag50
        {
            get { return _tag50; }
            set
            {
                Set(ref _tag50, value);
            }
        }

        private string _tag9F08;
        public string Tag9F08
        {
            get { return _tag9F08; }
            set
            {
                Set(ref _tag9F08, value);
            }
        }

        private string _tag5F24;
        public string Tag5F24
        {
            get { return _tag5F24; }
            set
            {
                Set(ref _tag5F24, value);
            }
        }

        private string _tag5F25;
        public string Tag5F25
        {
            get { return _tag5F25; }
            set
            {
                Set(ref _tag5F25, value);
            }
        }

        public void Clear()
        {

        }

    }
}

using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;

namespace CardPlatform.Models
{
    public class TemplateComparedInfo : ObservableObject
    {
        public TemplateComparedInfo()
        {
            ColorMark = new SolidColorBrush(Colors.Black);
        }

        private string _tag;
        public string Tag
        {
            get { return _tag; }
            set
            {
                Set(ref _tag, value);
            }
        }

        private string _cardValue;
        public string CardValue
        {
            get { return _cardValue; }
            set
            {
                Set(ref _cardValue, value);
            }
        }

        private string _templateValue;
        public string TemplateValue
        {
            get { return _templateValue; }
            set
            {
                Set(ref _templateValue, value);
            }
        }

        private SolidColorBrush _colorMark;
        public SolidColorBrush ColorMark
        {
            get { return _colorMark; }
            set
            {
                Set(ref _colorMark, value);
            }
        }
    }
}

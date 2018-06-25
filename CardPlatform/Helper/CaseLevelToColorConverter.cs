using CardPlatform.Config;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Windows.Media;

namespace CardPlatform.Helper
{
    public class CaseLevelToColorConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is TipLevel)
            {
                var level = (TipLevel)value;
                switch (level)
                {
                    case TipLevel.Failed: return new SolidColorBrush(Colors.Red);
                    case TipLevel.Warn: return new SolidColorBrush(Colors.Yellow);
                    case TipLevel.Sucess: return new SolidColorBrush(Colors.Black);
                    case TipLevel.Unknown:  return new SolidColorBrush(Colors.Blue);
                }
            }
            return new SolidColorBrush(Colors.Red);

        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException("ConvertBack not supported");
        }
    }
}

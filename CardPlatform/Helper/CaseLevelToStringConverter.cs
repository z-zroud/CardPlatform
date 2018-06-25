using CardPlatform.Config;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace CardPlatform.Helper
{
    public class CaseLevelToStringConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if(value is TipLevel)
            {
                var level = (TipLevel)value;
                switch(level)
                {
                    case TipLevel.Failed:   return "失败";
                    case TipLevel.Warn:     return "警告";
                    case TipLevel.Sucess:   return "成功";
                    case TipLevel.Unknown:  return "未校验";
                }
            }
            return string.Empty;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException("ConvertBack not supported");
        }
    }
}

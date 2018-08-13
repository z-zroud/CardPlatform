using CardPlatform.Models;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;

namespace CardPlatform.Helper
{
    public class AppKeyTypeToVisibility : IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
        {
            if(values.Length == 2 && values[1] is AlgorithmType)
            {
                if((string)values[0] == "DES")  //DES密钥控件
                {
                    switch ((AlgorithmType)values[1])
                    {
                        case AlgorithmType.DES: return Visibility.Visible;  //DES算法 DES控件 可见
                        case AlgorithmType.SM: return Visibility.Hidden;    //SM算法  DES控件 隐藏
                    }
                }
                else
                {//SM密钥控件
                    switch ((AlgorithmType)values[1])
                    {
                        case AlgorithmType.DES: return Visibility.Hidden;   //DES算法 SM控件 隐藏
                        case AlgorithmType.SM: return Visibility.Visible;   //SM算法  SM控件 可见
                    }
                }

            }
            return Visibility.Visible;
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException("ConvertBack not supported");
        }
    }
}

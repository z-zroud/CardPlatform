using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace CardPlatform.Helper
{
    public static class DialogCloseHelper
    {
        public static readonly DependencyProperty DialogResultProperty = 
            DependencyProperty.RegisterAttached(
            "DialogResult",
            typeof(bool?),
            typeof(DialogCloseHelper),
            new PropertyMetadata(DialogResultChanged));

        private static void DialogResultChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var window = d as Window;
            if (window != null)
            {
                window.DialogResult = e.NewValue as bool?;
            }
        }

        public static void SetDialogResult(Window target, bool? value)
        {
            target.SetValue(DialogResultProperty, value);
        }
    }
}

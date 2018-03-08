using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using CplusplusDll;
using ExcelLib;

namespace CheckPicture
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            DataContext = this;
        }
        private ICDll _cdll;
        public List<string> Readers { get; set; }

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                if(comboReader.Items.Count == 0)    //如果读卡器没获取，则获取读卡器
                {
                    _cdll = new SCReader();
                    Readers = _cdll.GetReaders();
                }
                else if(string.IsNullOrWhiteSpace(tbSeq.Text))   //获取了读卡器，没有输入序号，则表示显示卡号
                {
                    if(_cdll.OpenReader(comboReader.SelectedValue.ToString()))
                    {
                        if(0x9000 == _cdll.SendApdu("00A40400 08 A000000333010101"))
                        {
                            string result = string.Empty;
                            if(0x9000 == _cdll.SendApdu("00B20114", ref result))
                            {
                                tbCard.Text = result.Substring(22, 19);
                            }
                        }
                    }
                }
                else if(!string.IsNullOrWhiteSpace(tbCard.Text))    //表示读取到了卡号，序号也存在，则读取照片信息
                {

                }
            }
        }
    }
}

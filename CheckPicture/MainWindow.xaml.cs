using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Data;
using System.IO;
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
        private IExcelOp _excelWrite;
        private DataTable _picInfos;
        public List<string> saveInfo = new List<string>();
        public List<string> Readers { get; set; }
        public string SelectedReader { get; set; }

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
           
            if (e.Key == Key.Enter)
            {
                if(comboReader.Items.Count == 0 && string.IsNullOrWhiteSpace(SelectedReader))    //如果读卡器没获取，则获取读卡器
                {
                    _cdll = new SCReader();
                    Readers = _cdll.GetReaders();
                    SelectedReader = Readers[0];
                }
                else if(string.IsNullOrWhiteSpace(tbCard.Text))   //获取了读卡器，没有输入序号，则表示显示卡号
                {
                    if(_cdll.OpenReader(SelectedReader))
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
                else if(!string.IsNullOrWhiteSpace(tbSeq.Text) && !string.IsNullOrWhiteSpace(tbCard.Text) && familyPic.Tag == null)    //表示读取到了卡号，序号也存在，则读取照片信息
                {
                    string uriString = string.Empty;
                    foreach (DataRow row in _picInfos.Rows)
                    {
                        if(row[0].ToString() == tbSeq.Text)
                        {
                            saveInfo.Add(row[0].ToString());
                            saveInfo.Add(tbCard.Text);
                            saveInfo.Add(row[1].ToString());
                            saveInfo.Add(row[2].ToString());
                            saveInfo.Add(row[3].ToString());
                            uriString = Directory.GetCurrentDirectory() + "\\" + row[1].ToString() + "+" + row[2].ToString() + "+" + row[3].ToString() + ".jpg";
                            break;
                        }
                    }
                    
                    Uri uri = new Uri(uriString, UriKind.RelativeOrAbsolute);
                    familyPic.Source = new BitmapImage(uri);
                    familyPic.Tag = "Image";
                }
                else if(familyPic.Tag != null && familyPic.Tag.ToString() == "Image")
                {
                    var result = MessageBox.Show("核对是否成功?","保存", MessageBoxButton.OKCancel, MessageBoxImage.Information);
                    if(result == MessageBoxResult.OK)
                    {
                        _excelWrite.AppendRow("Sheet1", saveInfo);                       
                    }
                    tbCard.Text = string.Empty;
                    saveInfo.Clear();
                    familyPic.Tag = null;
                    familyPic.Source = null;
                }
            }
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            IExcelOp excelOp = new ExcelOp();
            string appDir = Directory.GetCurrentDirectory() + "\\";
            if (excelOp.OpenExcel(appDir + "照片信息清单.xlsx", OpExcelType.Modify))
            {
                //string sheetName = "Sheet1";
                var sheetName = excelOp.GetSheetNames().First();
                _picInfos = excelOp.GetSheetData(sheetName);
            }
            _excelWrite = new ExcelOp();
            _excelWrite.OpenExcel(appDir + "全家福卡片信息清单.xlsx", OpExcelType.Modify);
        }
    }
}

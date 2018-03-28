using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Data;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using CplusplusDll;
using UtilLib;

namespace CheckPicture
{
    

    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window ,INotifyPropertyChanged
    {
        [DllImport("user32.dll")]
        public static extern UInt32 SendInput(UInt32 nInputs, ref INPUT pInputs, int cbSize);



        [StructLayout(LayoutKind.Explicit)]
        public struct INPUT
        {
            [FieldOffset(0)]
            public Int32 type;
            [FieldOffset(4)]
            public KEYBDINPUT ki;
            [FieldOffset(4)]
            public MOUSEINPUT mi;
            [FieldOffset(4)]
            public HARDWAREINPUT hi;

        }



        [StructLayout(LayoutKind.Sequential)]
        public struct MOUSEINPUT
        {
            public Int32 dx;
            public Int32 dy;
            public Int32 mouseData;
            public Int32 dwFlags;
            public Int32 time;
            public IntPtr dwExtraInfo;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct KEYBDINPUT
        {
            public Int16 wVk;
            public Int16 wScan;
            public Int32 dwFlags;
            public Int32 time;
            public IntPtr dwExtraInfo;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct HARDWAREINPUT
        {
            public Int32 uMsg;
            public Int16 wParamL;
            public Int16 wParamH;
        }
        public const int INPUT_KEYBOARD = 1;
        public const int KEYEVENTF_KEYUP = 0x0002;


        public MainWindow()
        {
            
            InitializeComponent();
            DataContext = this;
            SeqNo = 1;
        }
        private ISCReader _cdll;
        private IExcelOp _excelWrite;
        private DataTable _picInfos;
        public List<string> saveInfo = new List<string>();

        public event PropertyChangedEventHandler PropertyChanged;

        public List<string> Readers { get; set; }
        private string _selectedReader;
        public string SelectedReader
        {
            get { return _selectedReader; }
            set
            {
                _selectedReader = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("SelectedReader"));
            }
        }
        private int _seqNo;
        public int SeqNo
        {
            get { return _seqNo; }
            set
            {
                _seqNo = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("SeqNo"));
            }
        }

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
           
            if (e.Key == Key.Enter)
            {
                if(comboReader.Items.Count == 0 && string.IsNullOrWhiteSpace(SelectedReader))    //如果读卡器没获取，则获取读卡器
                {
                    _cdll = new SCReader();
                    try
                    {
                        Readers = _cdll.GetReaders();
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(ex.Message);
                    }
                    if(Readers.Count > 0)
                    {
                        SelectedReader = Readers[0];
                        comboReader.ItemsSource = Readers;
                    }

                    //comboReader.SelectedValue = SelectedReader;
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
                else if(!string.IsNullOrWhiteSpace(tbSeq.Text) && !string.IsNullOrWhiteSpace(tbCard.Text))    //表示读取到了卡号，序号也存在，则读取照片信息
                {
                    string uriString = string.Empty;
                    string text = tbSeq.Text.ToString().PadLeft(8, '0');
                    foreach (DataRow row in _picInfos.Rows)
                    {
                        if(row[0].ToString() == text)
                        {
                            saveInfo.Clear();
                            saveInfo.Add(row[0].ToString());
                            saveInfo.Add(tbCard.Text);
                            saveInfo.Add(row[1].ToString());
                            saveInfo.Add(row[2].ToString());
                            saveInfo.Add(row[3].ToString());
                            //saveInfo.Add("打印成功");

                            string tmp = row[2].ToString();
                            string fuck = tmp.Replace("行", "行+");
                            uriString = Directory.GetCurrentDirectory() + "\\Picture\\"
                                + row[0].ToString() + "+"
                                + row[1].ToString() + "+" 
                                + fuck + "+" 
                                + row[3].ToString() + ".jpg";
                            break;
                        }
                    }
                    
                    Uri uri = new Uri(uriString, UriKind.RelativeOrAbsolute);
                    try
                    {
                        familyPic.Source = new BitmapImage(uri);
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(ex.Message);
                    }
                    familyPic.Tag = "Image";
                }
            }
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            string appDir = Directory.GetCurrentDirectory() + "\\";
            _excelWrite = new ExcelOp();
            _excelWrite.OpenExcel(appDir + "全家福卡片信息清单.xlsx", OpExcelType.Modify);
        }

        private void btReader_Click(object sender, RoutedEventArgs e)
        {
            _cdll = new SCReader();
            try
            {
                Readers = _cdll.GetReaders();
            }catch(Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            
            if(Readers.Count > 0)
            {
                SelectedReader = Readers[0];
                comboReader.ItemsSource = Readers;
                comboReader.SelectedValue = SelectedReader;
            }

        }

        private void btReadCardNo_Click(object sender, RoutedEventArgs e)
        {
            if (_cdll.OpenReader(SelectedReader))
            {
                if (0x9000 == _cdll.SendApdu("00A40400 08 A000000333010101"))
                {
                    string result = string.Empty;
                    if (0x9000 == _cdll.SendApdu("00B20114", ref result))
                    {
                        tbCard.Text = result.Substring(34, 16);
                    }
                }
            }
        }

        private void btNext_Click(object sender, RoutedEventArgs e)
        {
            SeqNo++;
            string uriString = string.Empty;
            string text = SeqNo.ToString().PadLeft(8, '0');
            foreach (DataRow row in _picInfos.Rows)
            {
                if (row[0].ToString() == text)
                {
                    saveInfo.Clear();
                    saveInfo.Add(row[0].ToString());
                    saveInfo.Add(tbCard.Text);
                    saveInfo.Add(row[1].ToString());
                    saveInfo.Add(row[2].ToString());
                    saveInfo.Add(row[3].ToString());
                    //saveInfo.Add("打印失败");
                    string tmp = row[2].ToString();
                    string fuck = tmp.Replace("行", "行+");
                    uriString = Directory.GetCurrentDirectory() + "\\Picture\\" +
                        row[0].ToString() + "+" +
                        row[1].ToString() + "+" +
                        fuck + "+" +
                        row[3].ToString() + ".jpg";
                    break;
                }
            }

            Uri uri = new Uri(uriString, UriKind.RelativeOrAbsolute);
            try
            {
                familyPic.Source = new BitmapImage(uri);
            }catch(Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            
            familyPic.Tag = "Image";
        }

        private void btOK_Click(object sender, RoutedEventArgs e)
        {
            var result = MessageBox.Show("核对是否成功?", "保存", MessageBoxButton.YesNoCancel, MessageBoxImage.Information);
            if (result == MessageBoxResult.Yes)
            {
                saveInfo.Add("打印成功");
                _excelWrite.AppendRow("Sheet1", saveInfo);
            }else if(result == MessageBoxResult.No)
            {
                saveInfo.Add("打印作废");
                _excelWrite.AppendRow("Sheet1", saveInfo);
            }
            tbCard.Text = string.Empty;
            saveInfo.Clear();
            familyPic.Tag = null;
            familyPic.Source = null;
        }

        private void btScanFile_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog dialog = new Microsoft.Win32.OpenFileDialog();
            dialog.Filter = "*|*.xlsx";
            if (dialog.ShowDialog() == true)
            {
                tbFilePath.Text = dialog.FileName;
            }

            IExcelOp excelOp = new ExcelOp();
            if (excelOp.OpenExcel(tbFilePath.Text))
            {
                //string sheetName = "Sheet1";
                var sheetName = excelOp.GetSheetNames().First();
                _picInfos = excelOp.GetSheetData(sheetName);
            }
        }
    }
}

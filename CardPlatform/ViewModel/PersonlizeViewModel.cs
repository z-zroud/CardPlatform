using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Windows.Input;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using UtilLib;
using CardPlatform.Models;
using CplusplusDll;
using System.Collections.ObjectModel;
using MahApps.Metro.Controls;
using System.Windows;
using MahApps.Metro.Controls.Dialogs;
using System.Xml;
using System.Xml.Linq;
using CardPlatform.Config;
using CardPlatform.View;
using GalaSoft.MvvmLight.Threading;
using System.Threading;

namespace CardPlatform.ViewModel
{
    public class PersonlizeViewModel : ViewModelBase
    {
        private MetroWindow _dialog;
        public PersonlizeViewModel()
        {
            DpParseParams = new DpParseModel();
            DpParseResult = new DpParseResultModel();
            DpGenCpsResults = new ObservableCollection<DpGenCpsModel>();
            SecureChannelParams = new SecureChannelModel();
            DivMethodCollection = new List<string>();
            DelInstCollection = new List<string>();
            SecureLevelCollection = new List<string>();
            Config = new PersonlizeConfig();
            ConfigCollection = new List<PersonlizeConfig>();
            _dialog = (MetroWindow)Application.Current.MainWindow;
            Load();            
        }

        #region data binding

        /// <summary>
        /// DP解析参数
        /// </summary>
        public DpParseModel DpParseParams { get; set; }

        /// <summary>
        /// DP解析结果
        /// </summary>
        public DpParseResultModel DpParseResult { get; set; }

        /// <summary>
        /// DP生成CPS文件结果列表
        /// </summary>
        public ObservableCollection<DpGenCpsModel> DpGenCpsResults { get; set; }

        /// <summary>
        /// 打开安全通道参数
        /// </summary>
        public SecureChannelModel SecureChannelParams { get; set; }

        /// <summary>
        /// 界面配置参数由该属性绑定
        /// </summary>
        private PersonlizeConfig _config;
        public PersonlizeConfig Config
        {
            get { return _config; }
            set
            {
                Set(ref _config, value);
            }
        }

        /// <summary>
        /// 加载配置信息
        /// </summary>
        public List<PersonlizeConfig> ConfigCollection { get; set; }

        /// <summary>
        /// 分散方式
        /// </summary>
        private List<string> _divMethodCollection;
        public List<string> DivMethodCollection
        {
            get { return _divMethodCollection; }
            set
            {
                Set(ref _divMethodCollection, value);
            }
        }

        /// <summary>
        /// 安全等级
        /// </summary>
        private List<string> _secureLevelCollection;
        public List<string> SecureLevelCollection
        {
            get { return _secureLevelCollection; }
            set
            {
                Set(ref _secureLevelCollection, value);
            }
        }

        /// <summary>
        /// 删除实例方式
        /// </summary>
        private List<string> _delInstCollection;
        public List<string> DelInstCollection
        {
            get { return _delInstCollection; }
            set
            {
                Set(ref _delInstCollection, value);
            }
        }

        #endregion

        #region command binding
        /// <summary>
        /// 解析DP文件
        /// </summary>
        private ICommand _parseDpCmd;
        public ICommand ParseDpCmd
        {
            get
            {
                if (_parseDpCmd == null)
                    _parseDpCmd = new RelayCommand(DoParseDp);
                return _parseDpCmd;
            }
        }

        /// <summary>
        /// 打开选择DP文件对话框
        /// </summary>
        private ICommand _openDpFileCmd;
        public ICommand OpenDpFileCmd
        {
            get
            {
                if (_openDpFileCmd == null)
                    _openDpFileCmd = new RelayCommand(()=> { Config.DpFilePath = OpenFileDialog(); });
                return _openDpFileCmd;
            }
        }

        /// <summary>
        /// 打开选择DP解析规则文件对话框
        /// </summary>
        private ICommand _openDpRuleFileCmd;
        public ICommand OpenDpRuleFileCmd
        {
            get
            {
                if (_openDpRuleFileCmd == null)
                    _openDpRuleFileCmd = new RelayCommand(()=> { Config.DpRulePath = OpenFileDialog(); });
                return _openDpRuleFileCmd;
            }
        }

        /// <summary>
        /// 打开安装参数文件对话框
        /// </summary>
        private ICommand _openInstallParamFileCmd;
        public ICommand OpenInstallParamFileCmd
        {
            get
            {
                if (_openInstallParamFileCmd == null)
                    _openInstallParamFileCmd = new RelayCommand(()=> { Config.InstallParamsFilePath = OpenFileDialog(); });
                return _openInstallParamFileCmd;
            }
        }

        /// <summary>
        /// 个人化CPS
        /// </summary>
        private ICommand _personlizeCmd;
        public ICommand PersonlizeCmd
        {
            get
            {
                if (_personlizeCmd == null)
                    _personlizeCmd = new RelayCommand(DoPersonlize);
                return _personlizeCmd;
            }
        }

        /// <summary>
        /// 加载配置
        /// </summary>
        private ICommand _loadConfigCmd;
        public ICommand LoadConfigCmd
        {
            get
            {
                if (_loadConfigCmd == null)
                    _loadConfigCmd = new RelayCommand(()=> 
                    {
                        SaveSettingDialog dialog = new SaveSettingDialog();
                        dialog.ShowDialog();
                    });
                return _loadConfigCmd;
            }
        }

        private ICommand _saveConfigCmd;
        public ICommand SaveConfigCmd
        {
            get
            {
                if (_saveConfigCmd == null)
                    _saveConfigCmd = new RelayCommand(()=>
                    {
                        SettingDialog dialog = new SettingDialog();
                        dialog.ShowDialog();
                    });
                return _saveConfigCmd;
            }
        }
        #endregion

        /// <summary>
        /// 打开系统默认的文件对话框，返回选中文件的路径
        /// </summary>
        /// <returns></returns>
        private string OpenFileDialog()
        {
            Microsoft.Win32.OpenFileDialog dialog = new Microsoft.Win32.OpenFileDialog();
            dialog.Filter = "*|*";
            if (dialog.ShowDialog() == true)
            {
                return dialog.FileName;
            }
            return string.Empty;
        }

        private void DoCps()
        {
            ICPS cps = new CPS();
            cps.SetPersonlizationConfig(Config.ISD, Config.KMC, Config.DivMethod, Config.SecureLevel);
            foreach (var cpsFile in DpGenCpsResults)
            {
                if (cpsFile.IsSelected)
                {
                    string fileName = Path.GetFileNameWithoutExtension(cpsFile.CpsFilePath);
                    bool sucess = cps.DoPersonlization(cpsFile.CpsFilePath, Config.InstallParamsFilePath);
                }
            }

        }

        private async void DoPersonlize()
        {
            MetroDialogSettings settings = new MetroDialogSettings();

            ViewModelLocator locator = new ViewModelLocator();
            if (string.IsNullOrWhiteSpace(locator.Main.SelectedReader))
            {
                await _dialog.ShowMessageAsync("Warning", "You need select a smart card reader!");
                return;
            }
            if (SCReader.OpenReader(locator.Main.SelectedReader))
            {
                Thread thread = new Thread(DoCps);
                thread.Start();
            }
            else
            {
                await _dialog.ShowMessageAsync("Error", "Can not open the smart card reader!");
            }
        }


        private void DoParseDp()
        {
            DpGenCpsResults.Clear();
            ICPS cps = new CPS();
            var cpsFiles = cps.GenCpsFile(Config.DpType.DllName, Config.DpType.FuncName, Config.DpFilePath, Config.DpRulePath);
            //var task = new Task<List<string>>(() => cps.GenCpsFile(Config.DpType.DllName, Config.DpType.FuncName, Config.DpFilePath, Config.DpRulePath));
            //ProgressDialogController controller;
            //DispatcherHelper.CheckBeginInvokeOnUI(() => 
            //{
            //    controller = await _dialog.ShowProgressAsync("Progress Dp File", "Progressing all the things, wait for seconds");
            //});

            //task.Start();

            //controller.SetIndeterminate();
            //task.Wait();

            //await controller.CloseAsync();
            //var cpsFiles = task.Result;

            foreach (var cpsFile in cpsFiles)
            {
                DpGenCpsModel cpsInfo = new DpGenCpsModel();
                cpsInfo.CpsFilePath = cpsFile;
                cpsInfo.CpsFileName = Path.GetFileName(cpsFile);
                cpsInfo.IsSelected = true;
                DpGenCpsResults.Add(cpsInfo);
            }
            if (DpGenCpsResults.Count > 0)
            {
                DpParseResult.Info = "成功";
                DpParseResult.StatusColor = "blue";
            }
            else
            {
                DpParseResult.Info = "失败";
                DpParseResult.StatusColor = "red";
            }
        }



        /// <summary>
        /// 加载初始化数据
        /// </summary>
        private void Load()
        {
            XDocument doc = XDocument.Load(".\\Configuration\\AppConfig\\PersonlizeConfiguration.xml");
            if(doc != null)
            {
                var root = doc.Root;
                var dpTypeElements = root.Element("DpType").Elements("Type");
                foreach(var item in dpTypeElements)
                {
                    var dpDll = new DpDll();
                    dpDll.Name = item.Attribute("name").Value;
                    dpDll.DllName = item.Attribute("dllName").Value;
                    dpDll.FuncName = item.Attribute("funcName").Value;
                    DpParseParams.DpTypeCollection.Add(dpDll);
                }
                
                var KMCElements = root.Element("KMCs").Elements("KMC");
                foreach(var item in KMCElements)
                {
                    SecureChannelParams.KMCCollection.Add(item.Value);
                }
                var aidElements = root.Element("AIDs").Elements("AID");
                foreach(var item in aidElements)
                {
                    SecureChannelParams.ISDCollection.Add(item.Value);
                }
                var divMethodElements = root.Element("DivType").Elements("Type");
                foreach(var item in divMethodElements)
                {
                    DivMethodCollection.Add(item.Value);
                }
                var secureLevelElements = root.Element("Secure").Elements("SecureLevel");
                foreach(var item in secureLevelElements)
                {
                    SecureLevelCollection.Add(item.Value);
                }
                var delInstElements = root.Element("DelInst").Elements("Type");
                foreach(var item in delInstElements)
                {
                    DelInstCollection.Add(item.Value);
                }

                Config.DpType = DpParseParams.DpTypeCollection[0];
                Config.KMC = SecureChannelParams.KMCCollection[0];
                Config.ISD = SecureChannelParams.ISDCollection[0];
                Config.DivMethod = 0;
                Config.DelInst = 0;
                Config.SecureLevel = 0;
            }

            ISerialize serialize = new XmlSerialize();
            ConfigCollection = (List< PersonlizeConfig>)serialize.DeserizlizeFromFile(".\\Configuration\\AppConfig\\PersonlizeSettings.xml", typeof(List<PersonlizeConfig>));
        }
    }
}



  

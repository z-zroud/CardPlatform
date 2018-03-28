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

namespace CardPlatform.ViewModel
{
    public class PersonlizeViewModel : ViewModelBase
    {
        private MetroWindow _dialog;
        public PersonlizeViewModel()
        {
            //_dialogCoordinator = dialogCoordinator;
            DpType = new List<DpParsedProgram>();
            ISDs = new List<string>();
            KMCs = new List<string>();
            Status = new DpParseStatus();
            FileStatus = new ObservableCollection<DpFileStatus>();
            DivType = new List<string>();
            Secure = new List<string>();
            DelInst = new List<string>();
            _dialog = (MetroWindow)Application.Current.MainWindow;
            Load();
            
        }

        #region data binding
        /// <summary>
        /// DP文件路径属性
        /// </summary>
        private string _dpPath;
        public string DpPath
        {
            get { return _dpPath; }
            set
            {
                Set(ref _dpPath, value);
            }
        }

        /// <summary>
        /// DP文件 数据类型属性
        /// </summary>
        private List<DpParsedProgram> _dpType;
        public List<DpParsedProgram> DpType
        {
            get { return _dpType; }
            set
            {
                Set(ref _dpType, value);
            }
        }

        /// <summary>
        /// 被选中的DP数据类型
        /// </summary>
        private DpParsedProgram _SelectedDpType;
        public DpParsedProgram SelectedDpType
        {
            get { return _SelectedDpType; }
            set
            {
                Set(ref _SelectedDpType, value);
            }
        }

        /// <summary>
        /// DP解析规则 配置文件路径
        /// </summary>
        private string _rulePath;
        public string RulePath
        {
            get { return _rulePath; }
            set
            {
                Set(ref _rulePath, value);
            }
        }

        /// <summary>
        /// DP解析是否成功，状态属性
        /// </summary>
        private DpParseStatus _status;
        public DpParseStatus Status
        {
            get { return _status; }
            set
            {
                Set(ref _status, value);
            }
        }

        /// <summary>
        /// DP解析之后的文件属性
        /// </summary>
        private ObservableCollection<DpFileStatus> _fileStatus;
        public ObservableCollection<DpFileStatus> FileStatus
        {
            get { return _fileStatus; }
            set
            {
                Set(ref _fileStatus, value);
            }
        }

        /// <summary>
        /// ISD 主安全域ID
        /// </summary>
        private List<string> _isds;
        public List<string> ISDs
        {
            get { return _isds; }
            set
            {
                Set(ref _isds, value);
            }
        }

        /// <summary>
        /// 选中的主安全域
        /// </summary>
        private string _selectedISD;
        public string SelectedISD
        {
            get { return _selectedISD; }
            set
            {
                Set(ref _selectedISD, value);
            }
        }

        /// <summary>
        /// KMC列表
        /// </summary>
        private List<string> _kmcs;
        public List<string> KMCs
        {
            get { return _kmcs; }
            set
            {
                Set(ref _kmcs, value);
            }
        }

        /// <summary>
        /// 选中的KMC
        /// </summary>
        private string _selectedKMC;
        public string SelectedKMC
        {
            get { return _selectedKMC; }
            set
            {
                Set(ref _selectedKMC, value);
            }
        }

        /// <summary>
        /// 个人化安装参数路径
        /// </summary>
        private string _installParamPath;
        public string InstallParamPath
        {
            get { return _installParamPath; }
            set
            {
                Set(ref _installParamPath, value);
            }
        }

        /// <summary>
        /// KMC分散方式列表
        /// </summary>
        private List<string> _divType;
        public List<string> DivType
        {
            get { return _divType; }
            set
            {
                Set(ref _divType, value);
            }
        }

        /// <summary>
        /// 被选中的分散方式
        /// </summary>
        private int _divTypeIndex;
        public int DivTypeIndex
        {
            get { return _divTypeIndex; }
            set
            {
                Set(ref _divTypeIndex, value);
            }
        }

        /// <summary>
        /// 打开安全通道的安全等级
        /// </summary>
        private List<string> _secure;
        public List<string> Secure
        {
            get { return _secure; }
            set
            {
                Set(ref _secure, value);
            }
        }

        /// <summary>
        /// 被选中的安全等级
        /// </summary>
        private int _secureIndex;
        public int SecureIndex
        {
            get { return _secureIndex; }
            set
            {
                Set(ref _secureIndex, value);
            }
        }

        /// <summary>
        /// 删除实例方式
        /// </summary>
        private List<string> _delInst;
        public List<string> DelInst
        {
            get { return _delInst; }
            set
            {
                Set(ref _delInst, value);
            }
        }
        /// <summary>
        /// 选中的删除实例的方式
        /// </summary>
        private int _delInstIndex;
        public int DelInstIndex
        {
            get { return _delInstIndex; }
            set
            {
                Set(ref _delInstIndex, value);
            }
        }

        #endregion

        #region command binding
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

        private ICommand _openDpFileCmd;
        public ICommand OpenDpFileCmd
        {
            get
            {
                if (_openDpFileCmd == null)
                    _openDpFileCmd = new RelayCommand(OpenDpFile);
                return _openDpFileCmd;
            }
        }

        private ICommand _openDpRuleFileCmd;
        public ICommand OpenDpRuleFileCmd
        {
            get
            {
                if (_openDpRuleFileCmd == null)
                    _openDpRuleFileCmd = new RelayCommand(OpenDpRuleFile);
                return _openDpRuleFileCmd;
            }
        }

        private ICommand _openInstallParamFileCmd;
        public ICommand OpenInstallParamFileCmd
        {
            get
            {
                if (_openInstallParamFileCmd == null)
                    _openInstallParamFileCmd = new RelayCommand(OpenInstallParamFile);
                return _openInstallParamFileCmd;
            }
        }

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

        private async void DoPersonlize()
        {
            MetroDialogSettings settings = new MetroDialogSettings();

            ViewModelLocator locator = new ViewModelLocator();
            if (string.IsNullOrWhiteSpace(locator.Main.SelectedReader))
            {
                await _dialog.ShowMessageAsync("Warning", "You need select a smart card reader!");
                return;
            }

            if(SCReader.OpenReader(locator.Main.SelectedReader))
            {
                ICPS cps = new CPS();
                cps.SetPersonlizationConfig(SelectedISD, SelectedKMC, DivTypeIndex, SecureIndex);
                foreach(var cpsFile in FileStatus)
                {
                    if(cpsFile.IsSelected)
                    {

                        MessageDialogResult result = await _dialog.ShowMessageAsync("Message", string.Format("Please Input card : {0}", cpsFile.DpFileName.Substring(0, cpsFile.DpFileName.Length - 4)),MessageDialogStyle.AffirmativeAndNegative);
                        if(result == MessageDialogResult.Affirmative)
                        {
                            bool sucess = cps.DoPersonlization(cpsFile.DpFilePath, InstallParamPath);
                            if (!sucess)
                            {
                                await _dialog.ShowMessageAsync("Failed", string.Format("Personlize card: {0} failed.", cpsFile.DpFileName.Substring(0, cpsFile.DpFileName.Length - 4)));
                                return;
                            }
                        }
                        else
                        {
                            return;
                        }

                    }
                }
            }
            else
            {
                await _dialog.ShowMessageAsync("Error", "Can not open the smart card reader!");
            }
        }

        private void GenCpsFile()
        {
        }

        private async void DoParseDp()
        {
            ICPS cps = new CPS();
            var task = new Task<List<string>>(() => cps.GenCpsFile(SelectedDpType.ProgramName, SelectedDpType.FuncName, DpPath, RulePath));
            //var cpsFiles = cps.GenCpsFile(SelectedDpType.ProgramName, SelectedDpType.FuncName, DpPath, RulePath);
            var controller = await _dialog.ShowProgressAsync("Progress Dp File", "Progressing all the things, wait for seconds");
            task.Start();
            
            controller.SetIndeterminate();
            task.Wait();
   
            await controller.CloseAsync();
            var cpsFiles = task.Result;
            
            foreach(var cpsFile in cpsFiles)
            {
                DpFileStatus fileStatus = new DpFileStatus();
                fileStatus.DpFilePath = cpsFile;
                fileStatus.DpFileName = Path.GetFileName(cpsFile);
                fileStatus.IsSelected = true;
                FileStatus.Add(fileStatus);
            }
            if(FileStatus.Count > 0)
            {
                Status.Info = "成功";
            }
            else
            {
                Status.Info = "失败";
            }            
        }

        private void OpenDpFile()
        {
            DpPath = OpenFileDialog();
        }

        private void OpenDpRuleFile()
        {
            RulePath = OpenFileDialog();
        }

        private void OpenInstallParamFile()
        {
            InstallParamPath = OpenFileDialog();
        }

        /// <summary>
        /// 加载初始化数据
        /// </summary>
        private void Load()
        {
            ISerialize serialize = new XmlSerialize();
            var personlizeCfg = (PersonlizeConfiguartion)serialize.DeserizlizeFromFile("PersonlizeConfiguration.xml", typeof(PersonlizeConfiguartion));
            DpType = personlizeCfg.DpType;
            if (DpType.Count > 0)
                SelectedDpType = DpType.First();
            ISDs = personlizeCfg.ISDs;
            if (ISDs.Count > 0)
                SelectedISD = ISDs.First();
            KMCs = personlizeCfg.KMCs;
            if (KMCs.Count > 0)
                SelectedKMC = KMCs.First();
            DivType = personlizeCfg.DivType;
            if (DivType.Count > 0)
                DivTypeIndex = 0;
            Secure = personlizeCfg.Secure;
            if (Secure.Count > 0)
                SecureIndex = 0;
            DelInst = personlizeCfg.DelInst;
            if (DelInst.Count > 0)
                DelInstIndex = 0;
        }
    }
}

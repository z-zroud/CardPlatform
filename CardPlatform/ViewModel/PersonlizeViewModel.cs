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

namespace CardPlatform.ViewModel
{
    public class PersonlizeViewModel : ViewModelBase
    {
        public PersonlizeViewModel()
        {
            DpType = new List<DpParsedProgram>();
            ISDs = new List<string>();
            KMCs = new List<string>();
            Status = new DpParseStatus();
            FileStatus = new ObservableCollection<DpFileStatus>();

            Load();
        }

        #region data binding
        private string _dpPath;
        public string DpPath
        {
            get { return _dpPath; }
            set
            {
                Set(ref _dpPath, value);
            }
        }

        private List<DpParsedProgram> _dpType;
        public List<DpParsedProgram> DpType
        {
            get { return _dpType; }
            set
            {
                Set(ref _dpType, value);
            }
        }

        private DpParsedProgram _SelectedDpType;
        public DpParsedProgram SelectedDpType
        {
            get { return _SelectedDpType; }
            set
            {
                Set(ref _SelectedDpType, value);
            }
        }

        private string _rulePath;
        public string RulePath
        {
            get { return _rulePath; }
            set
            {
                Set(ref _rulePath, value);
            }
        }

        private DpParseStatus _status;
        public DpParseStatus Status
        {
            get { return _status; }
            set
            {
                Set(ref _status, value);
            }
        }

        private ObservableCollection<DpFileStatus> _fileStatus;
        public ObservableCollection<DpFileStatus> FileStatus
        {
            get { return _fileStatus; }
            set
            {
                Set(ref _fileStatus, value);
            }
        }

        private List<string> _isds;
        public List<string> ISDs
        {
            get { return _isds; }
            set
            {
                Set(ref _isds, value);
            }
        }

        private string _selectedISD;
        public string SelectedISD
        {
            get { return _selectedISD; }
            set
            {
                Set(ref _selectedISD, value);
            }
        }

        private List<string> _kmcs;
        public List<string> KMCs
        {
            get { return _kmcs; }
            set
            {
                Set(ref _kmcs, value);
            }
        }

        private string _selectedKMC;
        public string SelectedKMC
        {
            get { return _selectedKMC; }
            set
            {
                Set(ref _selectedKMC, value);
            }
        }

        private string _installParamPath;
        public string InstallParamPath
        {
            get { return _installParamPath; }
            set
            {
                Set(ref _installParamPath, value);
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

        private void DoPersonlize()
        {
            ViewModelLocator locator = new ViewModelLocator();
            ISCReader reader = new SCReader();
            if(reader.OpenReader(locator.Main.SelectedReader))
            {
                ICPS cps = new CPS();
                int divType = 0;
                int secureLevel = 0;
                cps.SetPersonlizationConfig(SelectedISD, SelectedKMC, divType, secureLevel);
                foreach(var cpsFile in FileStatus)
                {
                    if(cpsFile.IsSelected)
                    {
                        cps.DoPersonlization(cpsFile.DpFilePath, InstallParamPath);
                    }
                }
            }
        }

        private void DoParseDp()
        {
            ICPS cps = new CPS();
            var cpsFiles = cps.GenCpsFile(SelectedDpType.ProgramName, SelectedDpType.FuncName, DpPath, RulePath);
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
        }
    }
}

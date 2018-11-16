using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using CardPlatform.Common;
using System.Windows.Input;
using CplusplusDll;
using UtilLib;
using CardPlatform.Models;
using CardPlatform.Business;
using System.Threading;
using GalaSoft.MvvmLight.Threading;
using CardPlatform.Helper.EnumToListHelper;

namespace CardPlatform.ViewModel
{
    public class TransactionViewModel : ViewModelBase
    {
        
        public TransactionViewModel()
        {
            Aids                        = new ObservableCollection<string>();
            CaseInfos                   = new ObservableCollection<TransInfoModel>();
            ErrorCaseInfos              = new ObservableCollection<TransInfoModel>();
            TransResult                 = new TransResultModel();
        }

        #region data binding

        /// <summary>
        /// 交易方式(接触/非接)
        /// </summary>
        public EnumListItemCollection<TransType> TransTypes { get; } = new EnumListItemCollection<TransType>();
        public TransType _currentTransType;
        public TransType CurrentTransType
        {
            get
            {
                return _currentTransType;
            }
            set
            {
                Set(ref _currentTransType, value);
            }
        }

        private ObservableCollection<string> _aids;
        public ObservableCollection<string> Aids
        {
            get { return _aids; }
            set
            {
                Set(ref _aids, value);
            }
        }

        private string _currentdAid;
        public string CurrentAid
        {
            get { return _currentdAid; }
            set
            {
                Set(ref _currentdAid, value);
            }
        }

        /// <summary>
        /// 应用类型 PBOC/UICS/ECC/QPBOC
        /// </summary>
        public EnumListItemCollection<AppType> Apps { get; } = new EnumListItemCollection<AppType>();
        public AppType _currentApp;
        public AppType CurrentApp
        {
            get
            {
                return _currentApp;
            }
            set
            {
                Set(ref _currentApp, value);
            }
        }

        /// <summary>
        /// MDK or UDK
        /// </summary>
        public EnumListItemCollection<AppKeyType> AppKeyTypes { get; } = new EnumListItemCollection<AppKeyType>();
        public AppKeyType _currentAppKeyType;
        public AppKeyType CurrentAppKeyType
        {
            get
            {
                return _currentAppKeyType;
            }
            set
            {
                Set(ref _currentAppKeyType, value);
            }
        }


        /// <summary>
        /// 交易使用的算法
        /// </summary>
        public EnumListItemCollection<AlgorithmType> AlgorithmTypes { get; } = new EnumListItemCollection<AlgorithmType>();
        public AlgorithmType _currentAlgorithm;
        public AlgorithmType CurrentAlgorithm
        {
            get
            {
                return _currentAlgorithm;
            }
            set
            {
                Set(ref _currentAlgorithm, value);
            }
        }

        /// <summary>
        /// 交易结果显示
        /// </summary>
        private TransResultModel _transResult;
        public TransResultModel TransResult
        {
            get { return _transResult; }
            set
            {
                Set(ref _transResult, value);
            }
        }

        private string _idnKey;
        public string IdnKey
        {
            get { return _idnKey; }
            set
            {
                Set(ref _idnKey, value);
            }
        }

        private string _cvcKey;
        public string CvcKey
        {
            get { return _cvcKey; }
            set
            {
                Set(ref _cvcKey, value);
            }
        }

        private string _applicationDesKey;
        public string ApplicationDesKey
        {
            get {return _applicationDesKey;}
            set
            {
                Set(ref _applicationDesKey, value);
            }
        }

        private string _applicationSmKey;
        public string ApplicationSmKey
        {
            get { return _applicationSmKey; }
            set
            {
                Set(ref _applicationSmKey, value);
            }
        }

        private ObservableCollection<TransInfoModel> _caseInfos;
        public ObservableCollection<TransInfoModel> CaseInfos
        {
            get { return _caseInfos; }
            set
            {
                Set(ref _caseInfos, value);
            }
        }

        private ObservableCollection<TransInfoModel> _errCaseInfos;
        public ObservableCollection<TransInfoModel> ErrorCaseInfos
        {
            get { return _errCaseInfos; }
            set
            {
                Set(ref _errCaseInfos, value);
            }
        }

        /// <summary>
        /// 个人化信息表路径
        /// </summary>
        private string _tagInfoFile;
        public string TagInfoFile
        {
            get { return _tagInfoFile; }
            set
            {
                Set(ref _tagInfoFile, value);
            }
        }

        #endregion

        #region command binding
        private ICommand _loadApp;
        public ICommand LoadAppCmd
        {
            get
            {
                if (_loadApp == null)
                    _loadApp = new RelayCommand(LoadApp);
                return _loadApp;
            }
        }


        private ICommand _doTransCmd;
        public ICommand DoTransCmd
        {
            get
            {
                if (_doTransCmd == null)
                    _doTransCmd = new RelayCommand(()=>
                    {
                        Thread thread = new Thread(() =>
                        {
                            DoTrans();
                        });
                        thread.Start();
                    });
                return _doTransCmd;
            }
        }

        /// <summary>
        /// 打开选择DP文件对话框
        /// </summary>
        private ICommand _openTagInfoFileCmd;
        public ICommand OpenTagInfoFileCmd
        {
            get
            {
                if (_openTagInfoFileCmd == null)
                    _openTagInfoFileCmd = new RelayCommand(() => { TagInfoFile = OpenFileDialog(); });
                return _openTagInfoFileCmd;
            }
        }
        #endregion

        private void LoadApp()
        {
            //在加载之前，清理上次加载信息
            
            Aids.Clear();
            CaseInfos.Clear();
            
            ApduResponse response = new ApduResponse();
            List<string> aids = new List<string>();
            ViewModelLocator locator = new ViewModelLocator();
            if (!APDU.RegisterReader(locator.Main.SelectedReader))
            {
                return;
            }
            Thread thread = new Thread(() =>
            {
                switch (CurrentTransType)
                {
                    case TransType.Contact:
                        TransactionConfig.GetInstance().CurrentApp = AppType.PSE;
                        BusinessPSE businessPSE = new BusinessPSE();
                        aids = businessPSE.SelectPSE();
                        break;
                    case TransType.Contactless:
                        TransactionConfig.GetInstance().CurrentApp = AppType.PPSE;
                        BusinessPPSE businessPPSE = new BusinessPPSE();
                        aids = businessPPSE.SelectPPSE();
                        break;
                }
                DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    if(aids != null)
                    {
                        foreach (var aid in aids)
                        {
                            Aids.Add(aid);
                        }
                        if (Aids.Count > 0)
                            CurrentAid = Aids.First();
                    }
                });
            });
            thread.Start();           
        }

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

        //private void Load()
        //{
        //    ISerialize serialize = new XmlSerialize();
        //    var transactionCfg = (TransConfig)serialize.DeserizlizeFromFile(".\\Configuration\\AppConfig\\TransConfiguration.xml", typeof(TransConfig));
        //}

        private void DoTrans()
        {
            BusinessBase trans = new BusinessBase();
            trans.PersoInfoFile = TagInfoFile;

            TransactionConfig transConfig = TransactionConfig.GetInstance();
            transConfig.CurrentApp      = CurrentApp;
            transConfig.Algorithm       = CurrentAlgorithm;
            transConfig.KeyType         = CurrentAppKeyType;
            transConfig.TransType       = CurrentTransType;
            if(!string.IsNullOrEmpty(ApplicationDesKey))
            {
                transConfig.TransDesAcKey   = ApplicationDesKey.Length >= 32 ? ApplicationDesKey.Substring(0, 32) : string.Empty;
                transConfig.TransDesMacKey  = ApplicationDesKey.Length >= 64 ? ApplicationDesKey.Substring(32, 32) : string.Empty;
                transConfig.TransDesEncKey  = ApplicationDesKey.Length >= 96 ? ApplicationDesKey.Substring(64, 32) : string.Empty;
            }
            if(!string.IsNullOrEmpty(ApplicationSmKey))
            {
                transConfig.TransSmAcKey    = ApplicationSmKey.Length >= 32 ? ApplicationSmKey.Substring(0, 32) : string.Empty;
                transConfig.TransSmMacKey   = ApplicationSmKey.Length >= 32 ? ApplicationSmKey.Substring(32, 32) : string.Empty;
                transConfig.TransSmEncKey   = ApplicationSmKey.Length >= 32 ? ApplicationSmKey.Substring(64, 32) : string.Empty;
            }
            transConfig.TransCvcKey     = string.IsNullOrEmpty(CvcKey) ? string.Empty : CvcKey;
            transConfig.TransIdnKey     = string.IsNullOrEmpty(IdnKey) ? string.Empty : IdnKey;
            
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
            {
                TransResult.Clear();
            });

            switch(CurrentApp)
            {
                case AppType.qVSDC_offline:
                case AppType.qVSDC_online:
                    trans = new BusinessqVSDC();                   
                    break;
                case AppType.UICS:
                    trans = new BusinessUICS();
                    break;
                case AppType.qPBOC:
                case AppType.qUICS:
                    trans = new BusinessQPBOC();
                    break;
                case AppType.ECC:
                    trans = new BusinessECC();
                    break;
                case AppType.VISA:
                    trans = new BusinessVISA();
                    break;
                case AppType.MC:
                    trans = new BusinessMC();
                    break;
                case AppType.Paypass:
                    trans = new BusinessPaypass();
                    break;
                case AppType.MSD:
                    trans = new BusinessMSD();
                    break;
            }
            trans.DoTransaction(CurrentAid);
        }

    }
}

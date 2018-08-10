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
            AlgorithmType               = new AlgorithmModel();
            AlgorithmType.IsCheckDES    = true;
            AlgorithmType.IsCheckSM     = true;
            HasSelectedSMKey            = false;
            HasSelectedDESKey           = true;
            Aids                        = new ObservableCollection<string>();
            CaseInfos                   = new ObservableCollection<TransInfoModel>();
            ErrorCaseInfos              = new ObservableCollection<TransInfoModel>();
            TransCategorys              = new List<string>();
            KeyTypeList                 = new List<string>();
            TransKeys                   = new TransKeyModel();
            TransResult                 = new ObservableCollection<TransResultModel>();
            Load();
        }

        #region data binding
        /// <summary>
        /// Aid
        /// </summary>
        private ObservableCollection<string> _aids;
        public ObservableCollection<string> Aids
        {
            get { return _aids; }
            set
            {
                Set(ref _aids, value);
            }
        }

        private string _selectedAid;
        public string SelectedAid
        {
            get { return _selectedAid; }
            set
            {
                Set(ref _selectedAid, value);
            }
        }

        /// <summary>
        /// 交易方式
        /// </summary>
        private List<string> _transCategorys;
        public List<string> TransCategorys
        {
            get { return _transCategorys; }
            set
            {
                Set(ref _transCategorys, value);
            }
        }

        private int _transCategory;
        public int SelectedCategory
        {
            get { return _transCategory; }
            set
            {
                Set(ref _transCategory, value);
                if(_transCategory == 0)
                {
                    ContactEnable = true;
                    ContactlessEnable = false;
                }
                else
                {
                    ContactEnable = false;
                    ContactlessEnable = true;
                }
            }
        }

        /// <summary>
        /// MDK or UDK
        /// </summary>
        private List<string> _keyTypeList;
        public List<string> KeyTypeList
        {
            get { return _keyTypeList; }
            set
            {
                Set(ref _keyTypeList, value);
            }
        }

        private int _selectedKeyType;
        public int SelectedKeyType
        {
            get { return _selectedKeyType; }
            set
            {
                Set(ref _selectedKeyType, value);
            }
        }

        private bool _contactEnable;
        public bool ContactEnable
        {
            get { return _contactEnable; }
            set
            {
                Set(ref _contactEnable, value);
            }
        }

        private bool _contactlessEnable;
        public bool ContactlessEnable
        {
            get { return _contactlessEnable; }
            set
            {
                Set(ref _contactlessEnable, value);
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
        /// 交易使用的算法
        /// </summary>
        private AlgorithmModel _algorithmType;
        public AlgorithmModel AlgorithmType
        {
            get
            {
                if(_algorithmType.IsCheckSM == false)
                {
                    HasSelectedSMKey = false;
                }
                return _algorithmType;
            }
            set
            {
                Set(ref _algorithmType, value);
            }
        }

        /// <summary>
        /// 交易结果显示
        /// </summary>
        public ObservableCollection<TransResultModel> TransResult { get; set; }

        /// <summary>
        /// 选中国密
        /// </summary>
        private bool _hasSelectedSMKey;
        public bool HasSelectedSMKey
        {
            get { return _hasSelectedSMKey; }
            set
            {
                Set(ref _hasSelectedSMKey, value);
            }
        }

        private bool _hasSelectedDESKey;
        public bool HasSelectedDESKey
        {
            get { return _hasSelectedDESKey; }
            set
            {
                Set(ref _hasSelectedDESKey, value);
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

        /// <summary>
        /// 应用密钥信息
        /// </summary>
        public TransKeyModel TransKeys { get; set; }

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

        private ICommand _selectSMCmd;
        public ICommand SelectSMCmd
        {
            get
            {
                if (_selectSMCmd == null)
                    _selectSMCmd = new RelayCommand(SelectSMKey);
                return _selectSMCmd;
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
                switch ((TransCategory)SelectedCategory)
                {
                    case TransCategory.Contact:
                        BusinessPSE businessPSE = new BusinessPSE();
                        aids = businessPSE.SelectPSE();
                        break;
                    case TransCategory.Contactless:
                        BusinessPPSE businessPPSE = new BusinessPPSE();
                        aids = businessPPSE.SelectPPSE();
                        break;
                }
                DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    foreach (var aid in aids)
                    {
                        Aids.Add(aid);
                    }
                    if (Aids.Count > 0)
                        SelectedAid = Aids.First();
                });
            });
            thread.Start();           
        }

        private void SelectSMKey()
        {
            HasSelectedDESKey = !HasSelectedSMKey;
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

        private void Load()
        {
            ISerialize serialize = new XmlSerialize();
            var transactionCfg = (TransConfig)serialize.DeserizlizeFromFile(".\\Configuration\\AppConfig\\TransConfiguration.xml", typeof(TransConfig));
            TransCategorys = transactionCfg.TransCates;
            if (TransCategorys.Count > 0)
                SelectedCategory = 0;
            KeyTypeList = transactionCfg.KeyTypeList;
            if (KeyTypeList.Count > 0)
                SelectedKeyType = 0;
        }

        private void DoTrans()
        {
            BusinessBase trans;
            BusinessBase.PersoFile = TagInfoFile;
            
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
            {
                TransResult.Clear();
            });
            
            //if (TransType.IsCheckPBOC || TransType.IsCheckUICS)
            //{
            //    trans = new BusinessUICS();
            //    trans.TransCfg.SaveConfig();
            //    trans.IsContactTrans = ((TransCategory)SelectedCategory == TransCategory.Contact) ? true : false;
            //    trans.DoTransaction(SelectedAid, AlgorithmType.IsCheckDES, AlgorithmType.IsCheckSM);

            //}
            //if (TransType.IsCheckECC)
            //{
            //    trans = new BusinessECC();
            //    trans.TransCfg.SaveConfig();
            //    trans.IsContactTrans = ((TransCategory)SelectedCategory == TransCategory.Contact) ? true : false;
            //    trans.DoTransaction(SelectedAid, AlgorithmType.IsCheckDES, AlgorithmType.IsCheckSM);
            //}
            //if (TransType.IsCheckQPBOC)
            //{
            //    trans = new BusinessQPBOC();
            //    trans.TransCfg.SaveConfig();
            //    trans.IsContactTrans = ((TransCategory)SelectedCategory == TransCategory.Contact) ? true : false;
            //    trans.DoTransaction(SelectedAid, AlgorithmType.IsCheckDES, AlgorithmType.IsCheckSM);
            //}
            //if (TransType.IsCheckVISA)
            //{
            //    trans = new BusinessVISA();
            //    trans.TransCfg.SaveConfig();
            //    trans.IsContactTrans = ((TransCategory)SelectedCategory == TransCategory.Contact) ? true : false;
            //    trans.DoTransaction(SelectedAid, true, false);
            //}
            //if (TransType.IsCheckqVSDC)
            //{
            //    trans = new BusinessqVSDC();
            //    trans.TransCfg.SaveConfig();
            //    trans.IsContactTrans = ((TransCategory)SelectedCategory == TransCategory.Contact) ? true : false;
            //    trans.DoTransaction(SelectedAid, true, false);
            //}
            //if (TransType.IsCheckAMEX)
            //{
            //    trans = new BusinessAMEX();
            //    trans.TransCfg.SaveConfig();
            //    trans.IsContactTrans = ((TransCategory)SelectedCategory == TransCategory.Contact) ? true : false;
            //    trans.DoTransaction(SelectedAid, true, false);
            //}
            //if (TransType.IsCheckMC)
            //{
            //    trans = new BusinessMC();
            //    trans.TransCfg.SaveConfig();
            //    trans.IsContactTrans = ((TransCategory)SelectedCategory == TransCategory.Contact) ? true : false;
            //    trans.DoTransaction(SelectedAid, true, false);
            //}
            //if (TransType.IsCheckJETCO)
            //{
            //    trans = new BusinessJETCO();
            //    trans.TransCfg.SaveConfig();
            //    trans.IsContactTrans = ((TransCategory)SelectedCategory == TransCategory.Contact) ? true : false;
            //    trans.DoTransaction(SelectedAid, true, false);
            //}
        }

    }
}

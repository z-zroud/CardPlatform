using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using CplusplusDll;
using UtilLib;
using CardPlatform.Models;
using CardPlatform.Business;

namespace CardPlatform.ViewModel
{
    public class TransactionViewModel : ViewModelBase
    {
        public TransactionViewModel()
        {
            TransType = new TransTypeModel();

            TransType.IsCheckPBOC = true;
            TransType.IsCheckUICS = true;
            TransType.IsCheckECC = true;
            TransType.IsCheckQPBOC = true;


            AlgorithmType = new AlgorithmModel();
            AlgorithmType.IsCheckDES = true;
            AlgorithmType.IsCheckSM = true;

            HasSelectedSMKey = false;
            HasSelectedDESKey = true;

            Aids = new ObservableCollection<string>();
            CaseInfos = new ObservableCollection<TransInfoModel>();
            TransCategorys = new List<string>();
            KeyTypeList = new List<string>();
            TransKeyList = new TransKeyModel();
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

        /// <summary>
        /// 交易类型 PBOC/UICS/ECC/QPBOC
        /// </summary>
        public TransTypeModel TransType { get; set; }

        /// <summary>
        /// 交易使用的算法
        /// </summary>
        public AlgorithmModel AlgorithmType { get; set; }

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

        /// <summary>
        /// 应用密钥信息
        /// </summary>
        public TransKeyModel TransKeyList { get; set; }

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
                    _doTransCmd = new RelayCommand(DoTrans);
                return _doTransCmd;
            }
        }
        #endregion

        private void LoadApp()
        {
            ApduResponse response = new ApduResponse();
            List<string> aids = new List<string>();
            ViewModelLocator locator = new ViewModelLocator();
            if(!APDU.RegisterReader(locator.Main.SelectedReader))
            {
                return;
            }
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
            foreach(var aid in aids)
            {               
                Aids.Add(aid);
            }
            if(Aids.Count > 0)
                SelectedAid = Aids.First();
        }

        private void SelectSMKey()
        {
            HasSelectedDESKey = !HasSelectedSMKey;
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

            //if (TransType.IsCheckPBOC || TransType.IsCheckUICS)
            //{
            //    trans = new BusinessUICS();
            //    trans.KeyType = SelectedKeyType == 0 ? TransKeyType.MDK : TransKeyType.UDK;
            //    trans.SetTransDESKeys(TransKeyList.DES_AC, TransKeyList.DES_MAC, TransKeyList.DES_ENC);
            //    trans.SetTransSMKeys(TransKeyList.SM_AC, TransKeyList.SM_MAC, TransKeyList.SM_ENC);
            //    trans.DoTrans(SelectedAid, AlgorithmType.IsCheckDES, AlgorithmType.IsCheckSM);
            //}
            if (TransType.IsCheckECC)
            {
                trans = new BusinessECC();
                trans.KeyType = SelectedKeyType == 0 ? TransKeyType.MDK : TransKeyType.UDK;
                trans.SetTransDESKeys(TransKeyList.DES_AC, TransKeyList.DES_MAC, TransKeyList.DES_ENC);
                trans.SetTransSMKeys(TransKeyList.SM_AC, TransKeyList.SM_MAC, TransKeyList.SM_ENC);
                trans.DoTrans(SelectedAid, AlgorithmType.IsCheckDES, AlgorithmType.IsCheckSM);
            }
            //if (TransType.IsCheckQPBOC)
            //{
            //    trans = new BusinessQPBOC();
            //    trans.KeyType = SelectedKeyType == 0 ? TransKeyType.MDK : TransKeyType.UDK;
            //    trans.SetTransDESKeys(TransKeyList.DES_AC, TransKeyList.DES_MAC, TransKeyList.DES_ENC);
            //    trans.SetTransSMKeys(TransKeyList.SM_AC, TransKeyList.SM_MAC, TransKeyList.SM_ENC);
            //    trans.DoTrans(SelectedAid, AlgorithmType.IsCheckDES, AlgorithmType.IsCheckSM);
            //}
        }
    }
}

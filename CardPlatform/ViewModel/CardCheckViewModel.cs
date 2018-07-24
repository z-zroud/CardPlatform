using CardPlatform.Config;
using CardPlatform.View;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Messaging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using UtilLib;

namespace CardPlatform.ViewModel
{
    public class CardCheckViewModel : ViewModelBase
    {
        private ViewModelLocator locator = new ViewModelLocator();
        public CardCheckViewModel()
        {
            
            ConfigCollection    = new List<CardCheckConfig>();
            Messenger.Default.Register<string>(this, nameof(CardCheckViewModel), SaveConfiguration);
            Messenger.Default.Register<string>(this, nameof(CardCheckViewModel) + "_Load", LoadConfiguration);
            Messenger.Default.Register<string>(this, nameof(CardCheckViewModel) + "_Delete", DeleteConfiguration);
            Load();
        }

        /// <summary>
        /// 界面配置参数由该属性绑定
        /// </summary>
        private CardCheckConfig _config;
        public CardCheckConfig Config
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
        public List<CardCheckConfig> ConfigCollection { get; set; }

        public void LoadConfiguration(string msg)
        {
            foreach(var config in ConfigCollection)
            {
                if(config.ConfigName == msg)
                {
                    locator.Transaction.SelectedKeyType = config.KeyType;
                    locator.Transaction.TransKeys.DES_AC = config.TransDesAcKey;
                    locator.Transaction.TransKeys.DES_MAC = config.TransDesMacKey;
                    locator.Transaction.TransKeys.DES_ENC = config.TransDesEncKey;
                    locator.Transaction.TransKeys.SM_AC = config.TransSmAcKey;
                    locator.Transaction.TransKeys.SM_MAC = config.TransSmMacKey;
                    locator.Transaction.TransKeys.SM_ENC = config.TransSmEncKey;
                    break;
                }
            }
        }

        public void DeleteConfiguration(string msg)
        {
            for (int i = 0; i < ConfigCollection.Count; i++)
            {
                if (ConfigCollection[i].ConfigName == msg)
                {
                    ConfigCollection.RemoveAt(i);
                    ISerialize serialize = new XmlSerialize();
                    serialize.Serialize(ConfigCollection, ".\\Configuration\\AppConfig\\CardCheckSettings.xml");
                    break;
                }
            }
        }

        public void SaveConfiguration(string msg)
        {
            Config                  = new CardCheckConfig();
            Config.ConfigName       = msg;
            Config.KeyType          = locator.Transaction.SelectedKeyType;
            Config.TransDesAcKey    = locator.Transaction.TransKeys.DES_AC;
            Config.TransDesMacKey   = locator.Transaction.TransKeys.DES_MAC;
            Config.TransDesEncKey   = locator.Transaction.TransKeys.DES_ENC;
            Config.TransSmAcKey     = locator.Transaction.TransKeys.SM_AC;
            Config.TransSmMacKey    = locator.Transaction.TransKeys.SM_MAC;
            Config.TransSmEncKey    = locator.Transaction.TransKeys.SM_ENC;
            ConfigCollection.Add(Config);
            ISerialize serialize = new XmlSerialize();
            serialize.Serialize(ConfigCollection, ".\\Configuration\\AppConfig\\CardCheckSettings.xml");
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
                    _loadConfigCmd = new RelayCommand(() =>
                    {
                        ConfigHelper.CurrentPage = nameof(CardCheckViewModel);
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
                    _saveConfigCmd = new RelayCommand(() =>
                    {
                        ConfigHelper.CurrentPage = nameof(CardCheckViewModel);
                        SettingDialog dialog = new SettingDialog();
                        dialog.ShowDialog();
                    });
                return _saveConfigCmd;
            }
        }

        /// <summary>
        /// 加载初始化数据
        /// </summary>
        private void Load()
        {
            ISerialize serialize = new XmlSerialize();
            ConfigCollection = (List<CardCheckConfig>)serialize.DeserizlizeFromFile(".\\Configuration\\AppConfig\\CardCheckSettings.xml", typeof(List<CardCheckConfig>));
        }
    }
}

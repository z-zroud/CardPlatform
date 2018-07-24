using CardPlatform.Config;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Messaging;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using UtilLib;

namespace CardPlatform.ViewModel
{
    public class SettingVeiwModel : ViewModelBase
    {

        public SettingVeiwModel()
        {
            Load();
        }

        public void Load()
        {
            ConfigCollection = new ObservableCollection<string>();
            ViewModelLocator locator = new ViewModelLocator();
            if (ConfigHelper.CurrentPage == nameof(PersonlizeViewModel))
            {
                foreach (var item in locator.Personlize.ConfigCollection)
                {
                    ConfigCollection.Add(item.ConfigName);
                }
            }
            else if (ConfigHelper.CurrentPage == nameof(CardCheckViewModel))
            {
                foreach (var item in locator.CardCheck.ConfigCollection)
                {
                    ConfigCollection.Add(item.ConfigName);
                }
            }
        }

        private ObservableCollection<string> _configCollection;
        public ObservableCollection<string> ConfigCollection
        {
            get { return _configCollection; }
            set
            {
                Set(ref _configCollection, value);
            }
        }

        private string _selectedConfig;
        public string SelectedConfig
        {
            get { return _selectedConfig; }
            set
            {
                Set(ref _selectedConfig, value);
            }
        }

        private bool? _dialogResult;
        public bool? DialogResult
        {
            get { return this._dialogResult; }
            set
            {
                Set(ref _dialogResult, value);
            }
        }

        //用来接收关闭按钮的Command
        private ICommand _loadCmd;
        public ICommand LoadCmd
        {
            get
            {
                if (_loadCmd == null)
                    _loadCmd = new RelayCommand(()=> 
                    {
                        Messenger.Default.Send(SelectedConfig, ConfigHelper.CurrentPage + "_Load");
                        Close();
                    });
                return _loadCmd;
            }
        }

        private ICommand _renameCmd;
        public ICommand RenameCmd
        {
            get
            {
                if (_renameCmd == null)
                    _renameCmd = new RelayCommand(Close);
                return _renameCmd;
            }
        }

        private ICommand _deleteCmd;
        public ICommand DeleteCmd
        {
            get
            {
                if (_deleteCmd == null)
                    _deleteCmd = new RelayCommand(()=> 
                    {
                        Messenger.Default.Send(SelectedConfig, ConfigHelper.CurrentPage + "_Delete");
                        Load();
                    });
                return _deleteCmd;
            }
        }

        private void Close()
        {
            DialogResult = true;
        }

    }
}

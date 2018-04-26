using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
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
            ConfigCollection = new ObservableCollection<string>();
            ViewModelLocator locator = new ViewModelLocator();
            foreach(var item in locator.Personlize.ConfigCollection)
            {
                ConfigCollection.Add(item.ConfigName);
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
        private ICommand _closeCmd;
        public ICommand CloseCmd
        {
            get
            {
                if (_closeCmd == null)
                    _closeCmd = new RelayCommand(Close);
                return _closeCmd;
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
                        ViewModelLocator locator = new ViewModelLocator();
                        for(int i = 0; i < locator.Personlize.ConfigCollection.Count; i++)
                        {
                            if(locator.Personlize.ConfigCollection[i].ConfigName == SelectedConfig)
                            {
                                locator.Personlize.ConfigCollection.RemoveAt(i);
                                ConfigCollection.RemoveAt(i);
                                ISerialize serialize = new XmlSerialize();
                                serialize.Serialize(locator.Personlize.ConfigCollection, "PersonlizeSettings.xml");
                                break;
                            }
                        }
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

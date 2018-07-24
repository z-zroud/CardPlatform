using CardPlatform.Config;
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
    public class SaveSettingViewModel : ViewModelBase
    {

        private string _configName;
        public string ConfigName
        {
            get { return _configName; }
            set
            {
                Set(ref _configName, value);
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

        private ICommand _saveConfigCmd;
        public ICommand SaveConfigCmd
        {
            get
            {
                if (_saveConfigCmd == null)
                    _saveConfigCmd = new RelayCommand(Save);
                return _saveConfigCmd;
            }
        }

        private void Close()
        {
            DialogResult = true;
        }

        private void Save()
        {
            if(ConfigHelper.CurrentPage == nameof(PersonlizeViewModel))
                Messenger.Default.Send<string>(ConfigName, nameof(PersonlizeViewModel));
            else if(ConfigHelper.CurrentPage == nameof(CardCheckViewModel))
                Messenger.Default.Send<string>(ConfigName, nameof(CardCheckViewModel));
            Close();
        }
    }
}

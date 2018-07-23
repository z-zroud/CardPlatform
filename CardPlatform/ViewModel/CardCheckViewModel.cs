using CardPlatform.View;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace CardPlatform.ViewModel
{
    public class CardCheckViewModel : ViewModelBase
    {
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
                        SettingDialog dialog = new SettingDialog();
                        dialog.ShowDialog();
                    });
                return _saveConfigCmd;
            }
        }
    }
}

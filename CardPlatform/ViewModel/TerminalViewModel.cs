using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Models;

namespace CardPlatform.ViewModel
{
    public class TerminalViewModel : ViewModelBase
    {
        public TerminalViewModel()
        {
            TermianlSettings = new TerminalModel();
        }

        private TerminalModel _terminalSettings;
        public TerminalModel TermianlSettings
        {
            get { return _terminalSettings; }
            set
            {
                Set(ref _terminalSettings, value);
            }
        }
    }
}

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using System.Collections.Generic;
using System.Windows.Input;
using CplusplusDll;
using GalaSoft.MvvmLight.Threading;
using UtilLib;

namespace CardPlatform.ViewModel
{
    /// <summary>
    /// This class contains properties that the main View can data bind to.
    /// <para>
    /// Use the <strong>mvvminpc</strong> snippet to add bindable properties to this ViewModel.
    /// </para>
    /// <para>
    /// You can also use Blend to data bind with the tool's support.
    /// </para>
    /// <para>
    /// See http://www.galasoft.ch/mvvm
    /// </para>
    /// </summary>
    public class MainViewModel : ViewModelBase
    {
        /// <summary>
        /// Initializes a new instance of the MainViewModel class.
        /// </summary>
        public MainViewModel()
        {
            ////if (IsInDesignMode)
            ////{
            ////    // Code runs in Blend --> create design time data.
            ////}
            ////else
            ////{
            ////    // Code runs "for real"
            ////}
            DispatcherHelper.Initialize();
            var logServer = new NamedPipeServer(@"\\.\pipe\LogNamedPipe", 0);
            logServer.Start(OutputLog);
        }

        private void OutputLog(string message)
        {
            Output = message;
        }

        private string _output;
        public string Output
        {
            get { return _output; }
            set
            {
                Set(ref _output, value);
            }
        }

        private List<string> _readers;
        public List<string> Readers
        {
            get { return _readers; }
            set
            {
                Set(ref _readers, value);
            }
        }

        private string _selectedReader;
        public string SelectedReader
        {
            get { return _selectedReader; }
            set
            {
                Set(ref _selectedReader, value);
            }
        }

        private ICommand _refreshCmd;
        public ICommand RefreshCmd
        {
            get
            {
                if (_refreshCmd == null)
                    _refreshCmd = new RelayCommand(DoRefresh);
                return _refreshCmd;
            }
        }

        private void DoRefresh()
        {
            Readers = SCReader.GetReaders();
            if (Readers.Count > 0)
                SelectedReader = Readers[0];
        }
    }
}
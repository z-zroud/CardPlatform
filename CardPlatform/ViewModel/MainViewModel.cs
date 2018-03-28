using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using System.Collections.Generic;
using System.Windows.Input;
using CplusplusDll;

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
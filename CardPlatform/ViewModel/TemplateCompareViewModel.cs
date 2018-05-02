using CardPlatform.Models;
using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.ViewModel
{

    public class TemplateCompareViewModel : ViewModelBase
    {
        private List<string> _templatePath;
        public List<string> TemplatePath
        {
            get { return _templatePath; }
            set
            {
                Set(ref _templatePath, value);
            }
        }

        private string _selectedTemplatePath;
        public string SelectedTemplatePath
        {
            get { return _selectedTemplatePath; }
            set
            {
                Set(ref _selectedTemplatePath, value);
            }
        }

        private ObservableCollection<TemplateComparedInfo> _templateComparedInfos;
        public ObservableCollection<TemplateComparedInfo> TemplateComparedInfos
        {
            get { return _templateComparedInfos; }
            set
            {
                Set(ref _templateComparedInfos, value);
            }
        }
    }
}

using CardPlatform.Config;
using CardPlatform.Models;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using System.Windows.Media;

namespace CardPlatform.ViewModel
{

    public class TemplateCompareViewModel : ViewModelBase
    {
        private string subAppTemplateDir = "\\Configuration\\AppConfig\\TemplateConfig";
        public TemplateCompareViewModel()
        {
            TemplatePath = new ObservableCollection<string>();
            TemplateComparedInfos = new ObservableCollection<TemplateComparedInfo>();
            var path = Directory.GetCurrentDirectory() + subAppTemplateDir;
            var files = Directory.GetFiles(path);
            foreach(var file in files)
            {
                var fileName = Path.GetFileName(file);
                TemplatePath.Add(fileName);
                if(fileName == "AppTemplate.xml")
                {
                    SelectedTemplatePath = fileName;
                }
            }
            if(TemplatePath.Count > 0 && string.IsNullOrEmpty(SelectedTemplatePath))
            {
                SelectedTemplatePath = TemplatePath.First();
            }
            if(!string.IsNullOrEmpty(SelectedTemplatePath))
            {
                LoaddTemplateConfig();
            }
        }

        private ObservableCollection<string> _templatePath;
        public ObservableCollection<string> TemplatePath
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

        private ICommand _loadTemplateConfigCmd;
        public ICommand LoadTemplateConfigCmd
        {
            get
            {
                if (_loadTemplateConfigCmd == null)
                    _loadTemplateConfigCmd = new RelayCommand(LoaddTemplateConfig);
                return _loadTemplateConfigCmd;
            }
        }

        private void LoaddTemplateConfig()
        {
            var compObj = DataTemplateConfig.GetInstance();

            var path = Directory.GetCurrentDirectory() + subAppTemplateDir + "\\" + SelectedTemplatePath;
            compObj.Load(path);
            TemplateComparedInfos.Clear();
            foreach (var item in compObj.TemplateTags)
            {
                TemplateComparedInfo comparedItem = new TemplateComparedInfo();
                comparedItem.TemplateValue = item.Value.Value;
                comparedItem.Tag = item.Key;
                comparedItem.HasCheck = false;
                comparedItem.ColorMark = new SolidColorBrush(Colors.Blue);
                comparedItem.CaseLevel = "未校验";
                TemplateComparedInfos.Add(comparedItem);
                
            }
        }
    }
}

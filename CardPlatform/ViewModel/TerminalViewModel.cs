using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Models;
using System.Collections.ObjectModel;

namespace CardPlatform.ViewModel
{
    public class TerminalViewModel : ViewModelBase
    {
        public TerminalViewModel()
        {
            TerminalTags = new ObservableCollection<TerminalModel>();
            TerminalTags.Add(new TerminalModel() {Tag="9F02",TagValue= "000000000100",Mark="" });
            TerminalTags.Add(new TerminalModel() { Tag = "9F03", TagValue = "000000000000", Mark = "" });
            TerminalTags.Add(new TerminalModel() { Tag = "9F04", TagValue = "000000000000", Mark = "" });
            TerminalTags.Add(new TerminalModel() { Tag = "9F1A", TagValue = "0156", Mark = "" });
            TerminalTags.Add(new TerminalModel() { Tag = "5F2A", TagValue = "0156", Mark = "" });
            TerminalTags.Add(new TerminalModel() { Tag = "95", TagValue = "0000000000", Mark = "" });
            TerminalTags.Add(new TerminalModel() { Tag = "9F4E", TagValue = "476F6C647061635F54435F4465706172746D656E", Mark = "" });
            TerminalTags.Add(new TerminalModel() { Tag = "9F37", TagValue = "11223344", Mark = "" });
            TerminalTags.Add(new TerminalModel() { Tag = "9F35", TagValue = "A3", Mark = "" });
            TerminalTags.Add(new TerminalModel() { Tag = "DF60", TagValue = "00", Mark = "" });
            TerminalTags.Add(new TerminalModel() { Tag = "9A", TagValue = DateTime.Now.ToString("yyMMdd"), Mark = "交易日期" });
            TerminalTags.Add(new TerminalModel() { Tag = "9F21", TagValue = DateTime.Now.ToString("hhmmss"), Mark = "交易时间" });
        }


        private ObservableCollection<TerminalModel> _terminalTags;
        public ObservableCollection<TerminalModel> TerminalTags
        {
            get { return _terminalTags; }
            set
            {
                Set(ref _terminalTags, value);
            }
        }

        public string GetTag(string tag)
        {
            var ret = TerminalTags.FirstOrDefault(item => item.Tag == tag);
            if (ret != null)
                return ret.TagValue;
            return string.Empty;
        }

        public void SetTag(string tag,string tagValue,string mark="")
        {
            TerminalTags.Add(new TerminalModel() { Tag = tag, TagValue = tagValue, Mark = mark });
        }
    }
}

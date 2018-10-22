using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Models;
using System.Collections.ObjectModel;
using GalaSoft.MvvmLight.Threading;

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
            TerminalTags.Add(new TerminalModel() { Tag = "9F4E", TagValue = "476F6C647061635F54435F4465706172746D656E", Mark = "商户名称" });
            TerminalTags.Add(new TerminalModel() { Tag = "9F7C", TagValue = "476F6C647061635F54435F4465706172746D656E", Mark = "商户名称" });
            TerminalTags.Add(new TerminalModel() { Tag = "9F37", TagValue = "11223344", Mark = "" });
            TerminalTags.Add(new TerminalModel() { Tag = "9F35", TagValue = "A3", Mark = "终端类型" });
            TerminalTags.Add(new TerminalModel() { Tag = "9F45", TagValue = "0000", Mark = "从SDA恢复数据中获取的数据验证代码" });
            TerminalTags.Add(new TerminalModel() { Tag = "9F4C", TagValue = "0000000000000000", Mark = "IC卡动态数据" });
            TerminalTags.Add(new TerminalModel() { Tag = "DF60", TagValue = "00", Mark = "" });
            TerminalTags.Add(new TerminalModel() { Tag = "8A", TagValue = "3030", Mark = "" });
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
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
            {
                bool hasExisted = false;
                foreach(var item in TerminalTags)
                {
                    if(item.Tag == tag)
                    {
                        item.TagValue = tagValue;
                        item.Mark = mark;
                        hasExisted = true;
                    }
                }
                if(!hasExisted)
                    TerminalTags.Add(new TerminalModel() { Tag = tag, TagValue = tagValue, Mark = mark });
            });
            
        }
    }
}

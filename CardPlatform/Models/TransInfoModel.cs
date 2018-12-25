using CardPlatform.Config;
using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;

namespace CardPlatform.Models
{
    public class TransInfoModel : ObservableObject
    {
        public TransInfoModel()
        {
        }

        /// <summary>
        /// 当前消息在日志中显示的行号
        /// </summary>
        public int CurLine { get; set; }

        /// <summary>
        /// case的编号
        /// </summary>
        private string _caseNo;
        public string CaseNo
        {
            get { return _caseNo; }
            set
            {
                Set(ref _caseNo, value);
            }
        }

        /// <summary>
        /// case内容
        /// </summary>
        private string _caseInfo;
        public string CaseInfo
        {
            get { return _caseInfo; }
            set
            {
                Set(ref _caseInfo, value);
            }
        }

        /// <summary>
        /// 日志等级
        /// </summary>
        private TipLevel _tipLevel;
        public TipLevel Level
        {
            get { return _tipLevel; }
            set
            {
                Set(ref _tipLevel, value);
            }
        }

    }
}

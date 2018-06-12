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
    public class TemplateComparedInfo : ObservableObject
    {
        public TemplateComparedInfo()
        {
           
        }

        /// <summary>
        /// 当前模板值所在的应用类型
        /// </summary>
        private string _app;
        public string CurrentApp
        {
            get { return _app; }
            set
            {
                Set(ref _app, value);
            }
        }

        /// <summary>
        /// 当前模板值所在的交易步骤
        /// </summary>
        private string _step;
        public string Step
        {
            get { return _step; }
            set
            {
                Set(ref _step, value);
            }
        }

        /// <summary>
        /// 需对比的模板值tag
        /// </summary>
        private string _tag;
        public string Tag
        {
            get { return _tag; }
            set
            {
                Set(ref _tag, value);
            }
        }

        /// <summary>
        /// 指定tag中的卡片内部值
        /// </summary>
        private string _cardValue;
        public string CardValue
        {
            get { return _cardValue; }
            set
            {
                Set(ref _cardValue, value);
            }
        }

        /// <summary>
        /// 指定tag中所期望的模板值
        /// </summary>
        private string _templateValue;
        public string TemplateValue
        {
            get { return _templateValue; }
            set
            {
                Set(ref _templateValue, value);
            }
        }

        /// <summary>
        /// 配置信息中所期望的等级
        /// </summary>
        private TipLevel _configLevel;
        public TipLevel ConfigLevel
        {
            get { return _configLevel; }
            set
            {
                Set(ref _configLevel, value);
            }
        }

        /// <summary>
        /// 实际结果中显示的等级
        /// </summary>
        private TipLevel _actualLevel;
        public TipLevel ActualLevel
        {
            get { return _actualLevel; }
            set
            {
                Set(ref _actualLevel, value);
            }
        }

        public bool HasCheck { get; set; }
    }
}

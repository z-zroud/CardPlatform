using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Models
{
    public enum DivMethod
    {
        NoDiv = 0,
        CPG202 = 1,
        CPG212 = 2
    }

    public enum SecureLevel
    {
        NoSecure = 0,
        MacSecure = 1
    }

    public enum DelInstance
    {
        DelToBeInstall = 0,
        DelAll = 1
    }

    public class SecureChannelModel : ObservableObject
    {
        public SecureChannelModel()
        {
            ISDCollection = new List<string>();
            KMCCollection = new List<string>();
        }

        /// <summary>
        /// 主安全域AID列表
        /// </summary>
        private List<string> _isds;
        public List<string> ISDCollection
        {
            get { return _isds; }
            set
            {
                Set(ref _isds, value);
            }
        }

        /// <summary>
        /// KMC列表
        /// </summary>
        private List<string> _KMCs;
        public List<string> KMCCollection
        {
            get { return _KMCs; }
            set
            {
                Set(ref _KMCs, value);
            }
        }

        /// <summary>
        /// 安全等级
        /// </summary>
        private SecureLevel _level;
        public SecureLevel Level
        {
            get { return _level; }
            set
            {
                Set(ref _level, value);
            }
        }

        /// <summary>
        /// 分散方式
        /// </summary>
        private DivMethod _divMethod;
        public DivMethod Div
        {
            get { return _divMethod; }
            set
            {
                Set(ref _divMethod, value);
            }
        }
    }
}

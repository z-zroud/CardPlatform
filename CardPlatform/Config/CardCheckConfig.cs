using CardPlatform.Common;
using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Config
{
    public class CardCheckConfig
    {
        public string ConfigName { get; set; }

        public int AlgorithmFlag { get; set; }

        public int KeyType { get; set; }

        public string TransDesAcKey { get; set; }

        public string TransDesMacKey { get; set; }

        public string TransDesEncKey { get; set; }

        public string TransSmAcKey { get; set; }

        public string TransSmMacKey { get; set; }

        public string TransSmEncKey { get; set; }
    }
}

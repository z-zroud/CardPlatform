using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Helper
{
    /// <summary>
    /// 这个类主要用于解析tag57各个元素的分解
    /// </summary>
    public class Tag57Helper
    {
        private string tag57;

        public Tag57Helper(string tag57)
        {
            this.tag57 = tag57;
        }

        public string GetAccount()
        {
            int indexD = tag57.IndexOf('D');
            string account = tag57.Substring(0, indexD);
            return account;
        }

        public string GetServiceCode()
        {
            int indexD = tag57.IndexOf('D');
            string sc = "0" + tag57.Substring(indexD + 4, 3);
            return sc;
        }
    }
}

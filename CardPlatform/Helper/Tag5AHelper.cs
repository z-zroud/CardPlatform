using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Helper
{
    public class Tag5AHelper
    {
        private string tag5A;
        public Tag5AHelper(string tag5A)
        {
            this.tag5A = tag5A;
        }

        public bool IsCorrectFormat()
        {
            int len = tag5A.Length;
            if (len % 2 != 0)
            {
                return false;   //奇数需补F
            }
            string account = string.Empty;
            if(tag5A.Contains("F"))
            {
                account = tag5A.Substring(0, tag5A.Length - 1);
                len = account.Length;
            }
            if (len < 16 || len > 19 || len == 17)
            {
                return false;   //长度有误
            }
            string format = "0123456789";
            foreach(var c in account)
            {
                if(!format.Contains(c))
                {
                    return false;   //账号需为数字
                }
            }

            return true;
        }

        public string GetAccount()
        {
            if (tag5A.Contains("F"))
            {
                tag5A = tag5A.Substring(0, tag5A.Length - 1);
            }
            return tag5A;
        }
    }
}

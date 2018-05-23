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
            if(len < 16 || len > 19 || len == 17)
            {
                return false;   //长度有误
            }
            string format = "0123456789";
            if(len % 2 != 0)
            {
                if(tag5A[len - 1] != 'F')
                {
                    return false;   //奇数需补F
                }
            }
            var acct = tag5A.Substring(0, len - 1);
            foreach(var c in acct)
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
            if(tag5A.Length % 2 != 0)
            {
                return tag5A.Substring(0, tag5A.Length - 1);
            }
            return tag5A;
        }
    }
}

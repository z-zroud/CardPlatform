using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace UtilLib
{
    public static class Utils
    {
        /// <summary>
        /// 获取BCD码的长度
        /// </summary>
        /// <param name="bcdData"></param>
        /// <returns></returns>
        public static string GetBcdLen(string bcdData)
        {
            string bcdLen = Convert.ToString(bcdData.Length / 2, 16).ToUpper();
            if(bcdLen.Length % 2 != 0)
            {
                bcdLen = "0" + bcdLen;
            }

            return bcdLen;
        }

        public static string IntToString(int intValue, int strLen)
        {
            string ret = Convert.ToString(intValue, 16).ToUpper();
            if (ret.Length < strLen)
            {
                int paddingZero = strLen - ret.Length;
                string zero = new string('0', paddingZero);
                ret = zero + ret;
            }
            return ret;
        }
    }
}

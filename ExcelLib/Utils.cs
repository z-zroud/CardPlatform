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


        /// <summary>
        /// 将整形数值转换为指定长度的字符串
        /// </summary>
        /// <param name="intValue"></param>
        /// <param name="strLen">指定字符串长度</param>
        /// <returns></returns>
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

        /// <summary>
        /// 将字符串转换为BCD码
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        public static string StrToBcd(string str)
        {
            byte[] array = System.Text.Encoding.ASCII.GetBytes(str);  //数组array为对应的ASCII数组    
            string bcd = string.Empty;
            for (int i = 0; i < array.Length; i++)
            {
                bcd += Convert.ToString(array[i], 16).ToUpper();//字符串ASCIIstr2 为对应的ASCII字符串
            }

            return bcd;
        }

        /// <summary>
        /// BCD码转换为字符串
        /// </summary>
        /// <param name="bcd"></param>
        /// <returns></returns>
        public static string BcdToStr(string bcd)
        {
            string result = string.Empty;
            
            if (bcd.Length % 2 != 0)
            {
                return result;
            }
            for(int i = 0; i < bcd.Length; i += 2)
            {
                int asc = Convert.ToByte(bcd.Substring(i, 2), 16);
                result += ((char)asc).ToString();
            }
            
            return result;
        }

        /// <summary>
        /// 判断字符串是否为数字字母组合
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        public static bool IsAlphaNum(string str)
        {
            string alphaNum = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
            foreach(var c in str)
            {
                if (!alphaNum.Contains(c))
                    return false;
            }

            return true;
        }
    }
}

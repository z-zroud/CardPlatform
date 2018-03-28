using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CplusplusDll
{
    public static class Algorithm
    {
        /// <summary>
        /// 3DES加密
        /// </summary>
        /// <param name="key"></param>
        /// <param name="data"></param>
        /// <returns></returns>
        public static string Des3Encrypt(string key, string data)
        {
            string result = string.Empty;
            if(data.Length < 16)
            {
                return result;
            }

            StringBuilder output = new StringBuilder(16);

            CDll.Des3Encrypt(output, key, data.Substring(0, 16));
            result = (output.ToString()).Substring(0,16);

            return result;
        }

        /// <summary>
        /// 3DES解密
        /// </summary>
        /// <param name="key"></param>
        /// <param name="encryptedData"></param>
        /// <returns></returns>
        public static string Des3Decrypt(string key, string encryptedData)
        {
            string result = string.Empty;
            if (encryptedData.Length < 16)
            {
                return result;
            }

            StringBuilder output = new StringBuilder(16);

            CDll.Des3Decrypt(output, key, encryptedData.Substring(0, 16));
            result = (output.ToString()).Substring(0, 16);

            return result;
        }

        public static string Des3CBCEncrypt(string key, string data)
        {
            string result = string.Empty;


            return result;
        }

        public static string Des3ECBEncrypt(string key, string data)
        {
            if(data.Length % 16 != 0)
            {
                data += "80";
                int fillZeroCount = 16 - data.Length % 16;
                string zeroString = new string('0', fillZeroCount);
                data += zeroString;
            }
            string result = string.Empty;
            StringBuilder output = new StringBuilder(16);
            for (int i = 0; i < data.Length; i += 16)
            {
                CDll.Des3Encrypt(output, key, data.Substring(i, 16));
                result += (output.ToString()).Substring(0, 16);
            }

            return result;
        }

        public static string Des3ECBDecrypt(string key, string encryptedData)
        {
            string result = string.Empty;
            if (encryptedData.Length % 16 != 0)
            {
                return result;
            }
            
            StringBuilder output = new StringBuilder(16);
            for (int i = 0; i < encryptedData.Length; i += 16)
            {
                CDll.Des3Decrypt(output, key, encryptedData.Substring(i, 16));
                result += (output.ToString()).Substring(0, 16);
            }

            return result;
        }

        public static string Des3Mac(string key, string data, string initVector = "00000000000000000000000000000000")
        {
            StringBuilder output = new StringBuilder(16);
            CDll.Des3Mac(data, key, initVector, output);

            string result = string.Empty;
            if (output.Length >= 16)
            {
                result = output.ToString().Substring(0, 16);
            }
            return result;
        }

        public static string Des3FullMac(string key, string data, string initVector = "00000000000000000000000000000000")
        {
            StringBuilder output = new StringBuilder(16);
            CDll.Des3FullMac(data, key, initVector, output);

            string result = string.Empty;
            if (output.Length >= 16)
            {
                result = output.ToString().Substring(0, 16);
            }
            return result;
        }

        public static string AscToBcd(string asc)
        {
            string result = string.Empty;
            int len = 120;
            for (int i = 0; i < asc.Length; i += 120)
            {
                if (asc.Length - i < len)
                    len = asc.Length - i;
                StringBuilder bcd = new StringBuilder(len * 2);
                if (CDll.AscToBcd(asc.Substring(i, len), bcd, len * 2))
                {
                    result += bcd.ToString();
                }
            }
            return result;
        }

        public static string BcdToAsc(string bcd)
        {
            if(bcd.Length % 2 != 0)
            {
                return string.Empty;
            }
            var ascEncoding = new ASCIIEncoding();
            byte[] asc = new byte[bcd.Length / 2];
            for(int i = 0; i < bcd.Length; i += 2)
            {
                asc[i / 2] = (byte)Convert.ToUInt16(bcd.Substring(i, 2), 16);
                
            }
            string str = ascEncoding.GetString(asc);

            return str;
        }
    }
}

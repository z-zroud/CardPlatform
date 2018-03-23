using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CplusplusDll
{
    public static class Algorithm
    {
        static string Des3Encrypt(string key, string data)
        {
            int len = 128;
            StringBuilder output = new StringBuilder(len);
            if(CDll.Des3Encrypt(output, key, data))
            {
                return output.ToString();
            }
            return string.Empty;
        }

        static string Des3Decrypt(string key, string encryptedData)
        {
            return string.Empty;
        }

        static string Des3CBCEncrypt(string key, string data)
        {
            return string.Empty;
        }

        static string Des3ECBEncrypt(string key, string data)
        {
            return string.Empty;
        }

        static string Des3Mac(string key, string data, string initVector = "00000000000000000000000000000000")
        {
            return string.Empty;
        }

        static string Des3FullMac(string key, string data, string initVector = "00000000000000000000000000000000")
        {
            return string.Empty;
        }

        static string AscToBcd(string asc)
        {
            return string.Empty;
        }

        static string BcdToAsc(string bcd)
        {
            return string.Empty;
        }
    }
}

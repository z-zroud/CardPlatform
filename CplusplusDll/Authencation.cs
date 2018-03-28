using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CplusplusDll
{
    public static class Authencation
    {
        public static string GenDesKcv(string key)
        {
            return string.Empty;
        }

        public static string GenSmKcv(string key)
        {
            return string.Empty;
        }

        public static string GenCAPublicKey(string index, string rid)
        {
            return string.Empty;
        }

        public static string GenDesIssuerPublicKey(string caPublicKey, string issuerPublicCert, string ipkRemainder, string issuerExponent)
        {
            return string.Empty;
        }

        public static string GenDesICCPublicKey(string issuerPublicKey, string iccPublicCert, string iccRemainder, string sigStaticData, string iccExponent, string tag82)
        {
            return string.Empty;
        }

        public static string GenSMIssuerPublicKey(string caPublicKey, string issuerPublicCert)
        {
            return string.Empty;
        }

        public static string GenSMICCPublicKey(string issuerPublicKey, string iccPublicCert, string needAuthStaticData)
        {
            return string.Empty;
        }

        public static int DES_SDA(string issuerPublicKey, string ipkExponent, string tag93, string sigStaticData, string tag82)
        {
            return 0;
        }

        public static int SM_SDA(string issuerPublicKey, string ipkExponent, string sigStaticData, string tag93, string tag82)
        {
            return 0;
        }

        public static int DES_DDA(string iccPublicKey, string iccExponent, string tag9F4B, string dynamicData)
        {
            return 0;
        }

        public static int SM_DDA(string iccPublicKey, string dynamicData)
        {
            return 0;
        }

        public static string GenUdkSessionKey(string udkSubKey, string atc)
        {
            return string.Empty;
        }

        public static string GenUdk(string mdk, string cardNo, string cardSequence)
        {
            return string.Empty;
        }

        public static string GenArpc(string udkAuthSessionKey, string ac, string authCode)
        {
            return string.Empty;
        }

        public static string GenIssuerScriptMac(string udkMacSessionKey, string data)
        {
            return string.Empty;
        }
    }
}

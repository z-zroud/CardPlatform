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
            StringBuilder kcv = new StringBuilder(7);
            CDll.GenDesKcv(key, kcv, 7);

            return kcv.ToString();
        }

        public static string GenSmKcv(string key)
        {
            StringBuilder kcv = new StringBuilder(7);
            CDll.GenSmKcv(key, kcv, 7);

            return kcv.ToString();
        }

        public static string GenCAPublicKey(string index, string rid)
        {
            StringBuilder CAPublicKey = new StringBuilder(2048 * 2);
            CDll.GenCAPublicKey(index, rid, CAPublicKey);

            return CAPublicKey.ToString();
        }

        public static string GenDesIssuerPublicKey(string caPublicKey, string issuerPublicCert, string ipkRemainder, string issuerExponent)
        {
            StringBuilder issuerPublicKey = new StringBuilder(1024 * 4);
            CDll.GenDesIssuerPublicKey(caPublicKey, issuerPublicCert, ipkRemainder, issuerExponent, issuerPublicKey);

            return issuerPublicKey.ToString();
        }

        public static string GenDesICCPublicKey(string issuerPublicKey, string iccPublicCert, string iccRemainder, string sigStaticData, string iccExponent, string tag82)
        {
            StringBuilder iccPublicKey = new StringBuilder(1024 * 4);
            CDll.GenDesICCPublicKey(issuerPublicKey, iccPublicCert, iccRemainder, sigStaticData, iccExponent, tag82, iccPublicKey);

            return iccPublicKey.ToString();
        }

        public static string GenSMIssuerPublicKey(string caPublicKey, string issuerPublicCert)
        {
            StringBuilder issuerPublicKey = new StringBuilder(1024 * 4);
            CDll.GenSMIssuerPublicKey(caPublicKey, issuerPublicCert, issuerPublicKey);

            return issuerPublicKey.ToString();
        }

        public static string GenSMICCPublicKey(string issuerPublicKey, string iccPublicCert, string needAuthStaticData)
        {
            StringBuilder iccPublicKey = new StringBuilder(1024 * 4);
            CDll.GenSMICCPublicKey(issuerPublicKey, iccPublicCert, needAuthStaticData, iccPublicKey);

            return iccPublicKey.ToString();
        }

        public static int DES_SDA(string issuerPublicKey, string ipkExponent, string tag93, string sigStaticData, string tag82)
        {
            return CDll.DES_SDA(issuerPublicKey, ipkExponent, tag93, sigStaticData, tag82);
        }

        public static int SM_SDA(string issuerPublicKey, string ipkExponent, string sigStaticData, string tag93, string tag82)
        {
            return CDll.SM_SDA(issuerPublicKey, ipkExponent, sigStaticData, tag93, tag82);
        }

        public static int DES_DDA(string iccPublicKey, string iccExponent, string tag9F4B, string dynamicData)
        {
            return CDll.DES_DDA(iccPublicKey, iccExponent, tag9F4B, dynamicData);
        }

        public static int SM_DDA(string iccPublicKey, string dynamicData)
        {
            return CDll.SM_DDA(iccPublicKey, dynamicData);
        }

        public static string GenUdkSessionKey(string udkSubKey, string atc)
        {
            StringBuilder udkSessionKey = new StringBuilder(33);
            CDll.GenUdkSessionKey(udkSubKey, atc, udkSessionKey);

            return udkSessionKey.ToString();
        }

        public static string GenUdk(string mdk, string cardNo, string cardSequence)
        {
            StringBuilder udk = new StringBuilder(33);
            CDll.GenUdk(mdk, cardNo, cardSequence, udk);

            return udk.ToString();
        }

        public static string GenArpc(string udkAuthSessionKey, string ac, string authCode)
        {
            StringBuilder arpc = new StringBuilder(17);
            CDll.GenArpc(udkAuthSessionKey, ac, authCode, arpc);

            return arpc.ToString().Substring(0, 16);
        }

        public static string GenIssuerScriptMac(string udkMacSessionKey, string data)
        {
            StringBuilder mac = new StringBuilder(17);
            CDll.GenIssuerScriptMac(udkMacSessionKey, data, mac);

            return mac.ToString().Substring(0, 16);
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CplusplusDll
{
    public static class Authencation
    {
        public static string GetHash(string input)
        {
            StringBuilder hash = new StringBuilder(64);
            CDll.GenHash(input, hash, 64);

            return hash.ToString();
        }

        public static string GenSMHash(string input, string publicKey)
        {
            StringBuilder hash = new StringBuilder(128);
            CDll.GenSMHash(input, publicKey, hash, 128);
            return hash.ToString();
        }

        public static string GenRecoveryData(string publicKey, string publicKeyExp, string encryptionData)
        {
            StringBuilder recoveryData = new StringBuilder(2048);
            CDll.DES_GenRecovery(publicKey, publicKeyExp, encryptionData, recoveryData, 2048);

            return recoveryData.ToString();
        }

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

        public static string GenSMIssuerPublicKey(string caPublicKey, string issuerPublicCert, string PAN)
        {
            StringBuilder issuerPublicKey = new StringBuilder(1024 * 4);
            CDll.GenSMIssuerPublicKey(caPublicKey, issuerPublicCert,  PAN, issuerPublicKey);

            return issuerPublicKey.ToString();
        }

        public static string GenSMICCPublicKey(string issuerPublicKey, string iccPublicCert, string needAuthStaticData, string tag82, string PAN)
        {
            StringBuilder iccPublicKey = new StringBuilder(1024 * 2);
            var result = CDll.GenSMICCPublicKey(issuerPublicKey, iccPublicCert, needAuthStaticData, tag82, PAN, iccPublicKey);

            return iccPublicKey.ToString();
        }

        public static int DES_SDA(string issuerPublicKey, string ipkExponent, string tag93, string sigStaticData, string tag82)
        {
            return CDll.DES_SDA(issuerPublicKey, ipkExponent, tag93, sigStaticData, tag82);
        }

        public static int SM_SDA(string issuerPublicKey, string sigStaticData, string tag93, string tag82)
        {
            return CDll.SM_SDA(issuerPublicKey, sigStaticData, tag93, tag82);
        }

        public static int DES_DDA(string iccPublicKey, string iccExponent, string tag9F4B, string dynamicData)
        {
            return CDll.DES_DDA(iccPublicKey, iccExponent, tag9F4B, dynamicData);
        }

        public static int SM_DDA(string iccPublicKey, string tag9F4B, string dynamicData)
        {
            return CDll.SM_DDA(iccPublicKey, tag9F4B, dynamicData);
        }

        public static string GenUdkSessionKey(string udkSubKey, string atc, int keyType = 0)
        {
            StringBuilder udkSessionKey = new StringBuilder(33);
            CDll.GenUdkSessionKey(udkSubKey, atc, udkSessionKey, keyType);

            return udkSessionKey.ToString();
        }

        public static string GenUdk(string mdk, string cardNo, string cardSequence, int keyType = 0)
        {
            if(mdk.Length != 32 ||
                string.IsNullOrEmpty(cardNo) ||
                string.IsNullOrEmpty(cardSequence))
            {
                return string.Empty;
            }
            StringBuilder udk = new StringBuilder(33);
            CDll.GenUdk(mdk, cardNo, cardSequence, udk, keyType);

            return udk.ToString();
        }

        public static string GenArpc(string udkAuthSessionKey, string ac, string authCode, int keyType = 0)
        {
            if(udkAuthSessionKey.Length != 32 ||
                ac.Length != 16 ||
                authCode.Length != 4)
            {
                return string.Empty;
            }
            StringBuilder arpc = new StringBuilder(17);
            CDll.GenArpc(udkAuthSessionKey, ac, authCode, arpc, keyType);

            return arpc.ToString().Substring(0, 16);
        }

        public static string GenIssuerScriptMac(string udkMacSessionKey, string data, int keyType = 0)
        {
            StringBuilder mac = new StringBuilder(17);
            CDll.GenPBOCMac(udkMacSessionKey, data, mac,keyType);

            return mac.ToString().Substring(0, 8);
        }

        public static string GenEMVAC(string udkAC,string data)
        {
            StringBuilder mac = new StringBuilder(17);
            CDll.GenEMVAC(udkAC, data, mac);

            return mac.ToString().Substring(0, 16);
        }

        public static string GenTag9F10Mac(string udkMacSessionKey, string data, int keyType = 0)
        {
            if(udkMacSessionKey.Length != 32 ||
                string.IsNullOrEmpty(data))
            {
                return string.Empty;
            }
            StringBuilder mac = new StringBuilder(9);
            CDll.GenPBOCMac(udkMacSessionKey, data, mac, keyType);

            return mac.ToString().Substring(0, 8);
        }
    }
}

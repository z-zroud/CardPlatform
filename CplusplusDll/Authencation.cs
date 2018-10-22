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

        public static string GetSMHash(string input)
        {
            StringBuilder hash = new StringBuilder(128);
            CDll.GenSMHash(input, hash);
            return hash.ToString();
        }

        public static int SM2Verify(string publicKey,  string verifyData,string signedMsg)
        {
            return CDll.SM2Verify(publicKey, verifyData, signedMsg);
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

        public static string GenEMVAC(string udkAc,string data)
        {
            StringBuilder mac = new StringBuilder(17);
            CDll.GenEMVAC(udkAc, data, mac);

            return mac.ToString().Substring(0, 16);
        }

        public static string GenAc(string udkMacSessionKey, string data, int keyType = 0)
        {
            if (udkMacSessionKey.Length != 32 ||
                string.IsNullOrEmpty(data))
            {
                return string.Empty;
            }
            StringBuilder mac = new StringBuilder(17);
            CDll.GenPBOCMac(udkMacSessionKey, data, mac, keyType);

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

        public static string GenMcSessionKeySkd(string mkAc,string atc, string un)
        {
            string leftInput = atc + "F000" + un;
            string rightInput = atc + "0F00" + un;

            string leftKey = Algorithm.Des3Encrypt(mkAc, leftInput);
            string rightKey = Algorithm.Des3Encrypt(mkAc, rightInput);

            return leftKey + rightKey;
        }

        public static string GenMcSessionKeyCsk(string mkAc,string atc)
        {
            string leftInput = atc + "F00000000000";
            string rightInput = atc + "0F0000000000";

            string leftKey = Algorithm.Des3Encrypt(mkAc, leftInput);
            string rightKey = Algorithm.Des3Encrypt(mkAc, rightInput);

            return leftKey + rightKey;
        }
    }
}

using CardPlatform.ViewModel;
using CardPlatform.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Common
{
    /// <summary>
    /// 存储交易时的界面配置信息
    /// </summary>
    public class TransactionConfig
    {
        private static TransactionConfig TransObj;
        public static TransactionConfig GetInstance()
        {
            if (TransObj == null)
            {
                TransObj = new TransactionConfig();
            }
            return TransObj;
        }

        private TransactionConfig()
        {
            Aids = new List<string>();
        }

        public List<string> Aids { get; set; }
        public AppType CurrentApp { get; set; }
        public AlgorithmType Algorithm { get; set; }
        public TransType TransType { get; set; }
        public AppKeyType KeyType { get; set; }       //定义界面中输入的是MDK/UDK类型
        public string TransDesAcKey { get; set; }       //DES_AC (MDK/UDK) 由KeyType决定
        public string TransDesMacKey { get; set; }      //DES_MAC
        public string TransDesEncKey { get; set; }      //DES_ENC
        public string TransSmAcKey { get; set; }       //SM_AC (MDK/UDK) 由KeyType决定
        public string TransSmMacKey { get; set; }      //SM_MAC
        public string TransSmEncKey { get; set; }      //SM_ENC
        public string TransIdnKey { get; set; } //IDN key for Mastercard
        public string TransCvcKey { get; set; } //DCVC3 key for Mastercard

        public void Clear()
        {
            Aids.Clear();
            TransDesAcKey = string.Empty;
            TransDesMacKey = string.Empty;
            TransDesEncKey = string.Empty;
            TransSmAcKey = string.Empty;
            TransSmMacKey = string.Empty;
            TransSmEncKey = string.Empty;
            TransIdnKey = string.Empty;
            TransCvcKey = string.Empty;
        }
        
        public void SaveConfig()
        {
            //KeyType         = ui.Transaction.SelectedKeyType == 0 ? TransKeyType.MDK : TransKeyType.UDK;
            //TransDesAcKey   = ui.Transaction.TransKeys.DES_AC;
            //TransDesMacKey  = ui.Transaction.TransKeys.DES_MAC;
            //TransDesEncKey  = ui.Transaction.TransKeys.DES_ENC;
            //TransSmAcKey    = ui.Transaction.TransKeys.SM_AC;
            //TransSmMacKey   = ui.Transaction.TransKeys.SM_MAC;
            //TransSmEncKey   = ui.Transaction.TransKeys.SM_ENC;
        }

        /// <summary>
        /// 判断是否为visa非接交易
        /// </summary>
        /// <returns></returns>
        public bool IsqVSDC()
        {
            if (CurrentApp == AppType.qVSDC_online ||
                CurrentApp == AppType.qVSDC_offline ||
                CurrentApp == AppType.qVSDC_online_without_ODA)
                return true;
            return false;
        }
        /// <summary>
        /// 判断是否为visa接触交易
        /// </summary>
        /// <returns></returns>
        public bool IsVSDC()
        {
            if (CurrentApp == AppType.VISA)
                return true;
            return false;
        }
        /// <summary>
        /// 判断是否为visa交易
        /// </summary>
        /// <returns></returns>
        public bool IsVisa()
        {
            return IsqVSDC() || IsVSDC();
        }

        /// <summary>
        /// 判断是否为接触UICS交易
        /// </summary>
        /// <returns></returns>
        public bool IsUICSContact()
        {
            if (CurrentApp == AppType.UICS &&
                TransType == TransType.Contact)
                return true;
            return false;
        }
        /// <summary>
        /// 判断是否为非接UICS交易
        /// </summary>
        /// <returns></returns>
        public bool IsUICSContactless()
        {
            if (CurrentApp == AppType.UICS &&
                TransType == TransType.Contactless)
                return true;
            return false;
        }
        /// <summary>
        /// 判断是否为qUICS交易
        /// </summary>
        /// <returns></returns>
        public bool IsqUICS()
        {
            if (CurrentApp == AppType.qUICS_offline ||
                CurrentApp == AppType.qUICS_online ||
                CurrentApp == AppType.qUICS_online_without_ODA)
                return true;
            return false;
        }
        /// <summary>
        /// 判断是否为UICS交易
        /// </summary>
        /// <returns></returns>
        public bool IsUICS()
        {
            return IsqUICS() || IsUICSContact() || IsUICSContactless();
        }
    }
}

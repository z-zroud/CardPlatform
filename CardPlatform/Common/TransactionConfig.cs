using CardPlatform.ViewModel;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Common
{
    /// <summary>
    /// 存储交易时的终端配置信息
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
        private ViewModelLocator ui = new ViewModelLocator();
        public TransactionApp CurrentApp { get; set; }
        public AlgorithmCategory AlgorithmFlag { get; set; }
        public TransKeyType KeyType { get; private set; }       //定义界面中输入的是MDK/UDK类型
        public string TransDesAcKey { get; private set; }       //DES_AC (MDK/UDK) 由KeyType决定
        public string TransDesMacKey { get; private set; }      //DES_MAC
        public string TransDesEncKey { get; private set; }      //DES_ENC
        public string TransSmAcKey { get; private set; }       //SM_AC (MDK/UDK) 由KeyType决定
        public string TransSmMacKey { get; private set; }      //SM_MAC
        public string TransSmEncKey { get; private set; }      //SM_ENC
        
        public void SaveConfig()
        {
            KeyType         = ui.Transaction.SelectedKeyType == 0 ? TransKeyType.MDK : TransKeyType.UDK;
            TransDesAcKey   = ui.Transaction.TransKeys.DES_AC;
            TransDesMacKey  = ui.Transaction.TransKeys.DES_MAC;
            TransDesEncKey  = ui.Transaction.TransKeys.DES_ENC;
            TransSmAcKey    = ui.Transaction.TransKeys.SM_AC;
            TransSmMacKey   = ui.Transaction.TransKeys.SM_MAC;
            TransSmEncKey   = ui.Transaction.TransKeys.SM_ENC;
        }
    }
}

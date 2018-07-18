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

        public AlgorithmCategory AlgorithmFlag { get; set; }
        public TransKeyType KeyType { get; set; }       //定义界面中输入的是MDK/UDK类型
        public string TransDesAcKey { get; set; }       //DES_AC (MDK/UDK) 由KeyType决定
        public string TransDesMacKey { get; set; }      //DES_MAC
        public string TransDesEncKey { get; set; }      //DES_ENC
        public string TransSmAcKey { get; set; }       //SM_AC (MDK/UDK) 由KeyType决定
        public string TransSmMacKey { get; set; }      //SM_MAC
        public string TransSmEncKey { get; set; }      //SM_ENC
    }
}

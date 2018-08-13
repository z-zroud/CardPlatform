using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Common
{
    public enum TransactionStep
    {
        Base,
        SelectPSE,      //选择PSE
        SelectPPSE,     //选择PPSE
        ReadPSEDir,     //读取PSE DIR
        SelectApp,      //选择应用
        GPO,            //应用初始化
        ReadRecord,     //读应用记录
        GetData,        //取必要数据(不是标准交易步骤，但需要检查这些数据是否存在于卡片)
        HandleLimitation,   //处理限制
        CardHolderVerify,   //持卡人验证
        TerminalRiskManagement, //终端风险管理
        TerminalActionAnalyze,   //终端行为分析
        TransactionEnd
    }

    //public enum TransactionApp
    //{
    //    PSE,
    //    PPSE,
    //    PBOC_DES,
    //    PBOC_SM,
    //    UICS_DES,
    //    UICS_SM,
    //    ECC_DES,
    //    ECC_SM,
    //    QUICS_DES,
    //    QUICS_SM,
    //    QPBOC_DES,
    //    QPBOC_SM,
    //    VISA,
    //    MC,
    //    JETCO,
    //    qVSDC
    //}

    public class Constant
    {
        public static readonly string PSE = "315041592E5359532E4444463031";
        public static readonly string PPSE = "325041592E5359532E4444463031";
        public static readonly string DebitAid = "A000000333010101";
        public static readonly string CreditAid = "A000000333010102";

        public static readonly string APP_UICS = "UICS";
        public static readonly string APP_ECC = "ECC";
        public static readonly string APP_QUICS = "QUICS";
        public static readonly string APP_PSE = "PSE";
        public static readonly string APP_PPSE = "PPSE";
        public static readonly string APP_VISA = "VISA";
        public static readonly string APP_MC = "MC";
        public static readonly string APP_JETCO = "JETCO";
        public static readonly string APP_PAYWAVE = "PAYWAVE";
        public static readonly string APP_AMEX = "AMEX";

        public static readonly int ARQC = 0x80;
        public static readonly int TC = 0x40;
        public static readonly int AAC = 0x00;
        public static readonly int CDA = 0x10;
        public static readonly int TC_CDA = 0x50;
        public static readonly int ARQC_CDA = 0x90;

        public static readonly string DataComparedConfigFile = "ProjectTemplate.xml";

        public static readonly string TransactionCase = "Transcation case";



    }

    public enum TransResult
    {
        Sucess = 0,
        Failed = 1,
        Unknown = 2
    }

    public enum TransCategory
    {
        Contact = 0,
        Contactless = 1
    }

    //public enum TransType
    //{
    //    PBOC_DES,
    //    UICS_DES,
    //    ECC_DES,
    //    QPBOC_DES,
    //    PBOC_SM,
    //    UICS_SM,
    //    ECC_SM,
    //    QPBOC_SM
    //}

    //public enum AlgorithmCategory
    //{
    //    DES = 0,
    //    SM = 1
    //}

    //public enum TransKeyType
    //{
    //    MDK,
    //    UDK
    //}

    public enum CardRespDataType
    {
        SelectPSE = 0,
        SelectPPSE = 1,
        ReadPSERecord = 2,
        SelectAid = 4,
        GPO = 5,
        ReadRecord = 6
    }
}

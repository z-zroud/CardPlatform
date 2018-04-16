using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Business
{
    public class Constant
    {
        public static readonly string PSE = "315041592E5359532E4444463031";
        public static readonly string PPSE = "325041592E5359532E4444463031";
        public static readonly string DebitAid = "A000000333010101";
        public static readonly string CreditAid = "A000000333010102";

        public static readonly int ARQC = 0x80;
        public static readonly int TC = 0x40;
        public static readonly int AAC = 0x00;
        public static readonly int CDA = 0x10;

        public static readonly string DataComparedConfigFile = "ProjectTemplate.xml";

        public static readonly string TransactionCase = "Transcation case";

    }

    public enum TransCategory
    {
        Contact = 0,
        Contactless = 1
    }

    public enum TransType
    {
        PBOC,
        UICS,
        ECC,
        QPBOC
    }

    public enum AlgorithmCategory
    {
        DES,
        SM
    }

    public enum TransKeyType
    {
        MDK,
        UDK
    }

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

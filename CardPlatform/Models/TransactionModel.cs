using CardPlatform.Helper;
using System.ComponentModel;

namespace CardPlatform.Models
{
    public enum AppType
    {

        [DescriptionEx("UICS")]
        UICS,
        [DescriptionEx("qUICS")]
        qUICS,
        [DescriptionEx("PBOC")]
        PBOC,
        [DescriptionEx("qPBOC")]
        qPBOC,
        [DescriptionEx("电子现金")]
        ECC,
        [DescriptionEx("VISA")]
        VISA,
        [DescriptionEx("qVSDC online ODA")]
        qVSDC_online,
        [DescriptionEx("qVSDC offline ODA")]
        qVSDC_offline,
        [DescriptionEx("MChip Advance")]
        MC,
        [DescriptionEx("Jetco")]
        JETCO,
        [DescriptionEx("Paypass")]
        Paypass,
        [DescriptionEx("MSD")]
        MSD,
        [DescriptionEx("AMEX")]
        AMEX,
        [DescriptionEx("PSE", Hide = true)]
        PSE,
        [DescriptionEx("PPSE", Hide = true)]
        PPSE
    }

    public enum TransType
    {
        [DescriptionEx("接触式交易")]
        Contact,
        [DescriptionEx("非接触式交易")]
        Contactless
    }

    public enum AppKeyType
    {
        [DescriptionEx("MDK发卡行主控密钥")]
        MDK,
        [DescriptionEx("UDK卡片应用密钥")]
        UDK
    }

    public enum AlgorithmType
    {
        [DescriptionEx("DES国际算法")]
        DES,
        [DescriptionEx("SM国密算法")]
        SM
    }

}

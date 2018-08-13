using System.ComponentModel;

namespace CardPlatform.Models
{
    public enum AppType
    {
        [Description("UICS")]
        UICS,
        [Description("qUICS")]
        qUICS,
        [Description("PBOC")]
        PBOC,
        [Description("qPBOC")]
        qPBOC,
        [Description("电子现金")]
        ECC,
        [Description("VISA")]
        VISA,
        [Description("qVSDC online ODA")]
        qVSDC_online,
        [Description("qVSDC offline ODA")]
        qVSDC_offline,
        [Description("MChip Advance")]
        MC,
        [Description("Jetco")]
        JETCO,
        [Description("AMEX")]
        AMEX
    }

    public enum TransType
    {
        [Description("接触式交易")]
        Contact,
        [Description("非接触式交易")]
        Contactless
    }

    public enum AppKeyType
    {
        [Description("MDK发卡行主控密钥")]
        MDK,
        [Description("UDK卡片应用密钥")]
        UDK
    }

    public enum AlgorithmType
    {
        [Description("DES国际算法")]
        DES,
        [Description("SM国密算法")]
        SM
    }

}

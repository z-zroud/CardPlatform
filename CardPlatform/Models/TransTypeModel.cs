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
    }

}

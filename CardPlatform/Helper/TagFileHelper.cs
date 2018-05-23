using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Business;
using UtilLib;

namespace CardPlatform.Helper
{
    public class TagLocaton
    {
        public TagLocaton(int row, int col=4)
        {
            Row = row;
            Column = col;
        }
        public int Row { get; set; }
        public int Column { get; set; }
    }

    /// <summary>
    /// 定义写入的tag交易应用类型
    /// </summary>
    public enum TagType
    {
        ContactDC_DES = 1,
        ContactDC_SM = 2,
        ContactlessDC_DES = 3,
        //ContactlessDC_SM = 4, //非接国际、国密填写的tag一致。
        ECC_DES = 5,
        ECC_SM = 6,
        QPBOC_DES = 7,
        QPBOC_SM = 8
    }

    /// <summary>
    /// 该类用于填写个人化信息表
    /// </summary>
    public class TagFileHelper
    {
        private string shettNmae = "PBOC&UICS";

        private static Dictionary<string, TagLocaton> contactTable;
        private static Dictionary<string, TagLocaton> contactlessTable;
        private static Dictionary<string, TagLocaton> eccTable;
        private static Dictionary<string, TagLocaton> qpbocTable;
        private static Dictionary<string, TagLocaton> keyTable;

        private static Dictionary<string, TagLocaton> sm_contactTable;
        private static Dictionary<string, TagLocaton> sm_eccTable;
        private static Dictionary<string, TagLocaton> sm_qpbocTable;

        private string tagFile;
        public TagFileHelper(string tagFile)
        {
            this.tagFile = tagFile;
        }

        static TagFileHelper()
        {
            //密钥相关信息
            keyTable = new Dictionary<string, TagLocaton>();
            keyTable.Add("des_ac", new TagLocaton(61, 5));
            keyTable.Add("des_mac", new TagLocaton(63, 5));
            keyTable.Add("des_enc", new TagLocaton(65, 5));
            keyTable.Add("sm_ac", new TagLocaton(62, 5));
            keyTable.Add("sm_mac", new TagLocaton(64, 5));
            keyTable.Add("sm_enc", new TagLocaton(66, 5));

            //接触时选择应用
            contactTable = new Dictionary<string, TagLocaton>();
            contactTable.Add("87", new TagLocaton(41));
            contactTable.Add("9F4D", new TagLocaton(42));
            contactTable.Add("9F4F", new TagLocaton(43));
            contactTable.Add("DF4D", new TagLocaton(44));
            contactTable.Add("DF4F", new TagLocaton(45));
            contactTable.Add("50", new TagLocaton(46));
            contactTable.Add("9F12", new TagLocaton(47));
            contactTable.Add("9F11", new TagLocaton(48));
            contactTable.Add("5F2D", new TagLocaton(49));
            contactTable.Add("9F38", new TagLocaton(50));

            //借记/贷记模板缺省数据
            contactTable.Add("9F0E", new TagLocaton(72));
            contactTable.Add("9F0F", new TagLocaton(73));
            contactTable.Add("9F0D", new TagLocaton(74));
            contactTable.Add("82", new TagLocaton(75));
            contactTable.Add("8E", new TagLocaton(76));
            contactTable.Add("9F07", new TagLocaton(79));
            contactTable.Add("9F08", new TagLocaton(80));
            contactTable.Add("9F52", new TagLocaton(81));
            contactTable.Add("8C", new TagLocaton(82));
            contactTable.Add("8D", new TagLocaton(83));

            //借记/贷记 发卡行通用数据
            contactTable.Add("9F63", new TagLocaton(85));
            contactTable.Add("9F56", new TagLocaton(86));
            contactTable.Add("9F53", new TagLocaton(87));
            contactTable.Add("9F54", new TagLocaton(88));
            contactTable.Add("9F5C", new TagLocaton(89));
            contactTable.Add("9F58", new TagLocaton(90));
            contactTable.Add("9F59", new TagLocaton(91));
            contactTable.Add("9F14", new TagLocaton(92));
            contactTable.Add("9F23", new TagLocaton(93));
            contactTable.Add("9F72", new TagLocaton(94));
            contactTable.Add("9F75", new TagLocaton(95));
            contactTable.Add("9F10", new TagLocaton(96,5));
            contactTable.Add("5F28", new TagLocaton(98));
            contactTable.Add("9F57", new TagLocaton(99));
            contactTable.Add("9F51", new TagLocaton(100));
            contactTable.Add("9F42", new TagLocaton(101));
            contactTable.Add("5F25", new TagLocaton(102));
            contactTable.Add("8F", new TagLocaton(103, 5));
            contactTable.Add("90", new TagLocaton(105, 5));
            contactTable.Add("92", new TagLocaton(107));
            contactTable.Add("9F32", new TagLocaton(108));
            contactTable.Add("9F49", new TagLocaton(109));

            //借记/贷记 卡或持卡人特殊数据
            contactTable.Add("5A", new TagLocaton(111));
            contactTable.Add("5F34", new TagLocaton(112));
            contactTable.Add("5F20", new TagLocaton(113));
            contactTable.Add("9F0B", new TagLocaton(114));
            contactTable.Add("9F61", new TagLocaton(115));
            contactTable.Add("9F62", new TagLocaton(116));
            contactTable.Add("5F24", new TagLocaton(117));
            contactTable.Add("5F30", new TagLocaton(118));
            contactTable.Add("9F1F", new TagLocaton(119));
            contactTable.Add("57", new TagLocaton(120));
            contactTable.Add("93", new TagLocaton(121, 5));           
            contactTable.Add("9F4A", new TagLocaton(123));
            contactTable.Add("9F45", new TagLocaton(124));
            contactTable.Add("9F46", new TagLocaton(125, 5));
            contactTable.Add("9F47", new TagLocaton(127));
            contactTable.Add("9F48", new TagLocaton(128));

            sm_contactTable = new Dictionary<string, TagLocaton>();
            sm_contactTable.Add("93_", new TagLocaton(122, 5));
            sm_contactTable.Add("9F10", new TagLocaton(97, 5));
            sm_contactTable.Add("8F", new TagLocaton(104, 5));
            sm_contactTable.Add("9F46", new TagLocaton(126, 5));
            sm_contactTable.Add("90", new TagLocaton(106, 5));

            //非接触式选择应用
            contactlessTable = new Dictionary<string, TagLocaton>();
            contactlessTable.Add("87", new TagLocaton(53));
            contactlessTable.Add("9F4D", new TagLocaton(54));
            contactlessTable.Add("9F4F", new TagLocaton(55));
            contactlessTable.Add("DF4D", new TagLocaton(56));
            contactlessTable.Add("DF4F", new TagLocaton(57));
            contactlessTable.Add("9F38", new TagLocaton(58));
            contactlessTable.Add("DF61", new TagLocaton(59));

            //基于借记/贷记应用的小额支付 模板缺省数据
            eccTable = new Dictionary<string, TagLocaton>();
            eccTable.Add("9F0E", new TagLocaton(131));
            eccTable.Add("9F0F", new TagLocaton(132));
            eccTable.Add("9F0D", new TagLocaton(133));
            eccTable.Add("82", new TagLocaton(134));
            eccTable.Add("8E", new TagLocaton(135));
            eccTable.Add("9F07", new TagLocaton(136));
            eccTable.Add("9F08", new TagLocaton(137));
            eccTable.Add("9F52", new TagLocaton(138));
            eccTable.Add("8C", new TagLocaton(139));
            eccTable.Add("8D", new TagLocaton(140));

            //基于借记/贷记应用的小额支付 发卡行通用数据
            eccTable.Add("9F63", new TagLocaton(143));
            eccTable.Add("9F56", new TagLocaton(144));
            eccTable.Add("9F53", new TagLocaton(145));
            eccTable.Add("9F54", new TagLocaton(146));
            eccTable.Add("9F5C", new TagLocaton(147));
            eccTable.Add("9F58", new TagLocaton(148));
            eccTable.Add("9F59", new TagLocaton(149));
            eccTable.Add("9F14", new TagLocaton(150));
            eccTable.Add("9F23", new TagLocaton(151));
            eccTable.Add("9F72", new TagLocaton(152));
            eccTable.Add("9F75", new TagLocaton(153));
            eccTable.Add("9F10", new TagLocaton(154, 5));           
            eccTable.Add("5F28", new TagLocaton(156));
            eccTable.Add("9F57", new TagLocaton(157));
            eccTable.Add("9F51", new TagLocaton(158));
            eccTable.Add("9F42", new TagLocaton(159));
            eccTable.Add("5F25", new TagLocaton(160));
            eccTable.Add("8F", new TagLocaton(161, 5));
            
            eccTable.Add("90", new TagLocaton(163, 5));
            
            eccTable.Add("92", new TagLocaton(165));
            eccTable.Add("9F32", new TagLocaton(166));
            eccTable.Add("9F49", new TagLocaton(167));

            //基于借记/贷记应用的小额支付 卡或持卡人特殊数据
            eccTable.Add("5A", new TagLocaton(169));
            eccTable.Add("5F34", new TagLocaton(170));
            eccTable.Add("5F20", new TagLocaton(171));
            eccTable.Add("9F0B", new TagLocaton(172));
            eccTable.Add("9F61", new TagLocaton(173));
            eccTable.Add("9F62", new TagLocaton(174));
            eccTable.Add("5F24", new TagLocaton(175));
            eccTable.Add("5F30", new TagLocaton(176));
            eccTable.Add("9F1F", new TagLocaton(177));
            eccTable.Add("57", new TagLocaton(178));
            eccTable.Add("93", new TagLocaton(179, 5));           
            eccTable.Add("9F4A", new TagLocaton(181));
            eccTable.Add("9F45", new TagLocaton(182));
            eccTable.Add("9F46", new TagLocaton(183, 5));          
            eccTable.Add("9F47", new TagLocaton(185));
            eccTable.Add("9F48", new TagLocaton(186));
            eccTable.Add("9F77", new TagLocaton(187));
            eccTable.Add("9F78", new TagLocaton(188));
            eccTable.Add("9F6D", new TagLocaton(189));

            //双币特有数据
            eccTable.Add("DF71", new TagLocaton(191));
            eccTable.Add("DF77", new TagLocaton(192));
            eccTable.Add("DF78", new TagLocaton(193));
            eccTable.Add("DF76", new TagLocaton(194));
            eccTable.Add("DF72", new TagLocaton(195));

            sm_eccTable = new Dictionary<string, TagLocaton>();
            sm_eccTable.Add("93", new TagLocaton(180, 5));
            sm_eccTable.Add("9F46", new TagLocaton(184, 5));
            sm_eccTable.Add("90", new TagLocaton(164, 5));
            sm_eccTable.Add("8F", new TagLocaton(162, 5));
            sm_eccTable.Add("9F10", new TagLocaton(155, 5));


            //快速借记/贷记(qPBOC/qUICS）部分
            qpbocTable = new Dictionary<string, TagLocaton>();
            qpbocTable.Add("82", new TagLocaton(198));
            qpbocTable.Add("9F68", new TagLocaton(199));
            qpbocTable.Add("9F6C", new TagLocaton(200));

            //快速借记/贷记(qPBOC/qUICS)发卡行通用数据
            qpbocTable.Add("9F63", new TagLocaton(204));
            qpbocTable.Add("9F10", new TagLocaton(205, 5));           
            qpbocTable.Add("8F", new TagLocaton(207, 5));            
            qpbocTable.Add("90", new TagLocaton(209, 5));           
            qpbocTable.Add("92", new TagLocaton(211));
            qpbocTable.Add("9F32", new TagLocaton(212));

            //快速借记/贷记(qPBOC/qUICS)卡或持卡人特殊数据
            qpbocTable.Add("9F6B", new TagLocaton(214));
            qpbocTable.Add("93", new TagLocaton(215, 5));            
            qpbocTable.Add("9F4A", new TagLocaton(217));
            qpbocTable.Add("9F45", new TagLocaton(218));
            qpbocTable.Add("9F46", new TagLocaton(219, 5));         
            qpbocTable.Add("9F47", new TagLocaton(221));
            qpbocTable.Add("9F48", new TagLocaton(222));

            //qPBOC/UICS扩展部分
            qpbocTable.Add("DF62", new TagLocaton(224));

            sm_qpbocTable = new Dictionary<string, TagLocaton>();
            sm_qpbocTable.Add("9F10", new TagLocaton(206, 5));
            sm_qpbocTable.Add("8F", new TagLocaton(208, 5));
            sm_qpbocTable.Add("90", new TagLocaton(210, 5));
            sm_qpbocTable.Add("9F46", new TagLocaton(220, 5));
            sm_qpbocTable.Add("93", new TagLocaton(216, 5));
        }

        public bool WriteToFile(TagType type)
        {
            IExcelOp op = new ExcelOp();
            if(!op.OpenExcel(tagFile,OpExcelType.Modify))
            {
                return false;
            }
            var tagDict = TagDict.GetInstance();
            switch(type)
            {
                case TagType.ContactDC_DES:
                    foreach(var item in contactTable)
                    {
                        var tag = tagDict.GetTag(item.Key);
                        if(string.IsNullOrEmpty(tag))
                        {
                            tag = "无";
                        }
                        op.ModifyCell(shettNmae, item.Value.Column, item.Value.Row, tag);
                    }
                    break;
                case TagType.ContactDC_SM:
                    foreach (var item in sm_contactTable)
                    {
                        var tag = tagDict.GetTag(item.Key);
                        if (string.IsNullOrEmpty(tag))
                        {
                            tag = "无";
                        }
                        op.ModifyCell(shettNmae, item.Value.Column, item.Value.Row, tag);
                    }
                    break;
                case TagType.ContactlessDC_DES:
                    foreach (var item in contactlessTable)
                    {
                        var tag = tagDict.GetTag(item.Key);
                        if (string.IsNullOrEmpty(tag))
                        {
                            tag = "无";
                        }
                        op.ModifyCell(shettNmae, item.Value.Column, item.Value.Row, tag);
                    }
                    break;
                case TagType.ECC_DES:
                    foreach (var item in eccTable)
                    {
                        var tag = tagDict.GetTag(item.Key);
                        if (string.IsNullOrEmpty(tag))
                        {
                            tag = "无";
                        }
                        op.ModifyCell(shettNmae, item.Value.Column, item.Value.Row, tag);
                    }
                    break;
                case TagType.ECC_SM:
                    foreach (var item in sm_eccTable)
                    {
                        var tag = tagDict.GetTag(item.Key);
                        if (string.IsNullOrEmpty(tag))
                        {
                            tag = "无";
                        }
                        op.ModifyCell(shettNmae, item.Value.Column, item.Value.Row, tag);
                    }
                    break;
                case TagType.QPBOC_DES:
                    foreach (var item in qpbocTable)
                    {
                        var tag = tagDict.GetTag(item.Key);
                        if (string.IsNullOrEmpty(tag))
                        {
                            tag = "无";
                        }
                        op.ModifyCell(shettNmae, item.Value.Column, item.Value.Row, tag);
                    }
                    break;
                case TagType.QPBOC_SM:
                    foreach (var item in sm_qpbocTable)
                    {
                        var tag = tagDict.GetTag(item.Key);
                        if (string.IsNullOrEmpty(tag))
                        {
                            tag = "无";
                        }
                        op.ModifyCell(shettNmae, item.Value.Column, item.Value.Row, tag);
                    }
                    break;
            }
            op.Close();
            return true;
        }
    }
}

﻿using System;
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
    }

    public enum TransCategory
    {
        Contact = 0,
        Contactless = 1
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
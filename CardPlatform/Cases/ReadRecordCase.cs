﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Business;
using CplusplusDll;

namespace CardPlatform.Cases
{
    public class ReadRecordCase : CaseBase
    {
        protected override void Load()
        {
            Step = "ReadRecord";
            base.Load();
        }
    }
}

using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Business
{
    public class BusinessBase
    {
        public List<TLV> ParseAndSave(string response)
        {
            List<TLV> arrTLV = DataParse.ParseTLV(response);
            TagDict tagDict = TagDict.GetInstance();
            tagDict.SetTags(arrTLV);

            return arrTLV;
        }


    }
}

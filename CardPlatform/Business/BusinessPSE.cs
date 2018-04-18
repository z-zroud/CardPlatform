﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CplusplusDll;
using CardPlatform.Cases;

namespace CardPlatform.Business
{
    public class BusinessPSE : BusinessBase
    {
        private Dictionary<string, List<TLV>> aidTags;

        public BusinessPSE()
        {
            aidTags = new Dictionary<string, List<TLV>>();
        }

        /// <summary>
        /// 选择PSE
        /// </summary>
        /// <returns></returns>
        public List<string> SelectPSE()
        {
            ApduResponse response = new ApduResponse();
            response = APDU.SelectCmd(Constant.PSE);
            if(response.SW != 0x9000)
            {
                return new List<string>();
            }
            ParseAndSave(response.Response);

            IExcuteCase cases = new PSECases();
            cases.ExcuteCase(response);

            //获取AID列表
            List<string> Aids = new List<string>();
            int SFI;
            if(int.TryParse(TagDict.GetInstance().GetTag("88"),out SFI))
            {
                string aid = string.Empty;
                int recordNo = 1;
                do
                {                 
                    aid = ReadPSERecord(SFI, recordNo);
                    recordNo++;
                    if(!string.IsNullOrWhiteSpace(aid))
                    {
                        Aids.Add(aid);
                    }
                }
                while (!string.IsNullOrWhiteSpace(aid));
               
            }
            return Aids;
        }

        /// <summary>
        /// 读取PSE DIR
        /// </summary>
        /// <param name="SFI"></param>
        /// <param name="recordNo"></param>
        /// <returns></returns>
        protected string ReadPSERecord(int SFI, int recordNo)
        {
            ApduResponse response = new ApduResponse();
            response = APDU.ReadRecordCmd(SFI, recordNo);
            if(response.SW != 0x9000)
            {
                return string.Empty;
            }
            List<TLV> arrTLV = DataParse.ParseTLV(response.Response);
            var aid = from tlv in arrTLV where tlv.Tag == "4F" select tlv.Value;
            aidTags.Add(aid.First(), arrTLV);

            return aid.First();
        }
    }
}

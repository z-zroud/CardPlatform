using System;
using System.Collections.Generic;
using System.Linq;
using CardPlatform.Common;
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
            SaveTags(TransactionStep.SelectPSE,response.Response);

            IExcuteCase stepCase = new PSECases() { CurrentApp = Constant.APP_PSE};
            stepCase.Excute(BatchNo,TransCfg.CurrentApp,TransactionStep.SelectPSE, response);

            //获取AID列表
            List<string> Aids = new List<string>();
            int SFI;
            if(int.TryParse(TransactionTag.GetInstance().GetTag("88"),out SFI))
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

            //IExcuteCase stepCase = CaseFactory.GetCaseInstance(Constant.APP_PSE, Constant.STEP_READ_PSE_DIR);
            IExcuteCase stepCase = new PSEDirCase() { CurrentApp = Constant.APP_PSE };
            if (response.SW != 0x9000 && response.SW != 0x6A83)
            {
                caseObj.TraceInfo(Config.TipLevel.Failed, "ReadPSERecord", "读到最后一条记录后再读下一条应返回6A83");
                return string.Empty;
            }
            if(response.SW == 0x6A83)
            {
                return string.Empty;
            }
            stepCase.Excute(BatchNo, TransCfg.CurrentApp, TransactionStep.ReadPSEDir,response);

            List<TLV> arrTLV = DataParse.ParseTLV(response.Response);
            var aid = from tlv in arrTLV where tlv.Tag == "4F" select tlv.Value;
            aidTags.Add(aid.First(), arrTLV);

            return aid.First();
        }
    }
}

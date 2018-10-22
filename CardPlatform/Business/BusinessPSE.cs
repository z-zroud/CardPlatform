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
        public BusinessPSE()
        {
        }

        /// <summary>
        /// 选择PSE
        /// </summary>
        /// <returns></returns>
        public List<string> SelectPSE()
        {
            log.TraceLog("选择PSE交易流程检测...");
            ApduResponse response = new ApduResponse();
            response = APDU.SelectCmd(Constant.PSE);
            if(response.SW != 0x9000)
            {
                return null;
            }
            var tlvs = DataParse.ParseTLV(response.Response);
            businessUtil.ShowTlvLog(tlvs);
            businessUtil.SaveTags(TransactionStep.SelectPSE, tlvs);

            IExcuteCase stepCase = new PSECases();
            stepCase.Excute(BatchNo, transCfg.CurrentApp,TransactionStep.SelectPSE, response);

            //获取AID列表
            List<string> cardAids = new List<string>();
            int SFI;
            if(int.TryParse(TransactionTag.GetInstance().GetTag("88"),out SFI))
            {
                int recordNo = 1;
                var aids = new List<string>();
                do
                {                 
                    aids = ReadPSERecord(SFI, recordNo);
                    recordNo++;
                    if(aids != null)
                    {
                        foreach (var aid in aids)
                        {
                            if (!string.IsNullOrWhiteSpace(aid))
                            {
                                cardAids.Add(aid);
                            }
                        }
                    }
                }
                while (aids != null);
               
            }
            return cardAids;
        }

        /// <summary>
        /// 读取PSE DIR
        /// </summary>
        /// <param name="SFI"></param>
        /// <param name="recordNo"></param>
        /// <returns></returns>
        protected List<string> ReadPSERecord(int SFI, int recordNo)
        {
            log.TraceLog(LogLevel.Info, "读取PSE记录流程检测...");
            ApduResponse response = new ApduResponse();
            response = APDU.ReadRecordCmd(SFI, recordNo);

            IExcuteCase stepCase = new PSEDirCase();
            if (response.SW != 0x9000 && response.SW != 0x6A83)
            {
                caseObj.TraceInfo(Config.TipLevel.Failed, "ReadPSERecord", "读到最后一条记录后再读下一条应返回6A83");
                return null;
            }
            if(response.SW == 0x6A83)
            {
                return null;
            }
            
            var tlvs = DataParse.ParseTLV(response.Response);
            businessUtil.ShowTlvLog(tlvs);
            var aids = from tlv in tlvs where tlv.Tag == "4F" select tlv.Value;
            stepCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.ReadPSEDir, response);
            return aids.ToList();
        }
    }
}

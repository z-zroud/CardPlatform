using CplusplusDll;
using System.Collections.Generic;

namespace CardPlatform.Business
{
    public class BusinessBase
    {
        public BusinessBase()
        {
            KeyType = TransKeyType.MDK;
            //AFLs = new List<AFL>();
        }

        public TransKeyType KeyType { get; set; }
        public string TransDesACKey { get; set; }
        public string TransDesMACKey { get; set; }
        public string TransDesENCKey { get; set; }
        public string TransSMACKey { get; set; }
        public string TransSMMACKey { get; set; }
        public string TransSMENCKey { get; set; }

        protected bool doDesTrans = false;
        protected bool doSMTrans = false;

        //protected List<AFL> AFLs;

        public void SetTransDESKeys(string acKey,string macKey,string encKey)
        {
            TransDesACKey = acKey;
            TransDesMACKey = macKey;
            TransDesENCKey = encKey;
        }

        public void SetTransSMKeys(string acKey,string macKey,string encKey)
        {
            TransSMACKey = acKey;
            TransSMMACKey = macKey;
            TransSMENCKey = encKey;
        }

        public virtual void DoTrans(string aid, bool doDesTrans, bool doSMTrans)
        {

        }

        protected List<TLV> ParseAndSave(string response)
        {
            List<TLV> arrTLV = DataParse.ParseTLV(response);
            TagDict tagDict = TagDict.GetInstance();
            tagDict.SetTags(arrTLV);

            return arrTLV;
        }

        //protected List<AFL> ParseAFL(string response)
        //{
        //    AFLs = DataParse.ParseAFL(response);
        //    return AFLs;
        //}

        protected virtual ApduResponse SelectAid(string aid)
        {
            return APDU.SelectCmd(aid);
        }

        protected virtual ApduResponse GPO(string pdol)
        {
            var response = APDU.GPOCmd(pdol);
            //AFLs = DataParse.ParseAFL(response.Response);

            return response;
        }

        protected virtual List<ApduResponse> ReadRecords(List<AFL> afls)
        {
            var responses = new List<ApduResponse>();
            foreach(var afl in afls)
            {
                var resp = APDU.ReadRecordCmd(afl.SFI, afl.RecordNo);
                responses.Add(resp);
            }
            return responses;
        }


    }
}

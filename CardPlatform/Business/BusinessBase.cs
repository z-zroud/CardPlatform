using CardPlatform.Cases;
using CardPlatform.ViewModel;
using CplusplusDll;
using System.Collections.Generic;
using System.Reflection;

namespace CardPlatform.Business
{
    public class BusinessBase
    {
        private ViewModelLocator locator;
        public BusinessBase()
        {
            KeyType = TransKeyType.MDK;
            locator = new ViewModelLocator();
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
        protected string toBeSignAppData;
        protected string aid;

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
            this.doDesTrans = doDesTrans;
            this.doSMTrans = doSMTrans;
            this.aid = aid;
        }

        protected bool ParseAndSave(string response)
        {
            bool result = false;
            if (DataParse.IsTLV(response))
            {
                var arrTLV = DataParse.ParseTLV(response);
                TagDict tagDict = TagDict.GetInstance();
                tagDict.SetTags(arrTLV);
                result = true;
            }
            else
            {
                IExcuteCase cases = new CaseBase();
                var caseNo = MethodBase.GetCurrentMethod().Name;
                cases.TraceInfo(Config.CaseLevel.Failed, caseNo, "解析TLV格式失败");
            }

            return result;
        }


        protected virtual ApduResponse SelectAid(string aid)
        {
            return APDU.SelectCmd(aid);
        }

        protected virtual ApduResponse GPO(string pdol)
        {
            var response = APDU.GPOCmd(pdol);
            return response;
        }

        protected virtual List<ApduResponse> ReadRecords(List<AFL> afls)
        {
            var responses = new List<ApduResponse>();
            foreach(var afl in afls)
            {
                var resp = APDU.ReadRecordCmd(afl.SFI, afl.RecordNo);
                responses.Add(resp);
                if(afl.IsSignedRecordNo)
                {
                    var tlvs = DataParse.ParseTLV(resp.Response);
                    foreach(var tlv in tlvs)
                    {
                        if(tlv.IsTemplate && tlv.Tag == "70") //用于脱机数据认证中的签名数据
                        {
                            toBeSignAppData += tlv.Value;
                            break;
                        }
                    }
                    
                }
            }
            return responses;
        }

        protected virtual ApduResponse GAC1(int acType, string CDOL1)
        {
            string CDOL1Data = string.Empty;
            var tls = DataParse.ParseTL(CDOL1);
            foreach(var tl in tls)
            {
                CDOL1Data += locator.Terminal.TermianlSettings.GetTag(tl.Tag);
            }
            return APDU.GACCmd(acType, CDOL1Data);
        }

        protected virtual ApduResponse GAC2(int acType, string CDOL2)
        {
            string CDOL2Data = string.Empty;
            var tls = DataParse.ParseTL(CDOL2);
            foreach (var tl in tls)
            {
                CDOL2Data += locator.Terminal.TermianlSettings.GetTag(tl.Tag);
            }

            return APDU.GACCmd(acType, CDOL2Data);
        }
    }
}

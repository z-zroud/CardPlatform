using CardPlatform.Cases;
using CardPlatform.Config;
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
        protected AlgorithmCategory curTransAlgorithmCategory = AlgorithmCategory.DES;    //default
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

        /// <summary>
        /// 进行SDA验证
        /// </summary>
        /// <returns></returns>
        protected virtual bool SDA(ref string issuerPublicKey)
        {
            int result;
            IExcuteCase excuteCase = new CaseBase();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (aid.Length < 10 || aid.Length > 16)
            {
                excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "应用AID长度为{0}不在规范内", aid.Length);
                return false;
            }

            //获取CA公钥
            TagDict tagDict = TagDict.GetInstance();
            string rid = aid.Substring(0, 10);
            string CAIndex = tagDict.GetTag("8F");
            if (CAIndex.Length != 2)
            {
                excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取CA 索引,请检查8F是否存在");
                return false;
            }
            string CAPublicKey = Authencation.GenCAPublicKey(CAIndex, rid);
            if (string.IsNullOrWhiteSpace(CAPublicKey))
            {
                excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取CA公钥，请检查RID及索引是否正确");
                return false;
            }

            string issuerPublicCert = tagDict.GetTag("90");
            string PAN = tagDict.GetTag("5A");
            string signedStaticAppData = tagDict.GetTag("93");
            string AIP = tagDict.GetTag("82");

            if (curTransAlgorithmCategory == AlgorithmCategory.DES) //DES算法
            {
                //获取发卡行公钥
                string issuerPublicKeyRemainder = tagDict.GetTag("92");
                string issuerExp = tagDict.GetTag("9F32");
                issuerPublicKey = Authencation.GenDesIssuerPublicKey(CAPublicKey, issuerPublicCert, issuerPublicKeyRemainder, issuerExp);
                if (string.IsNullOrWhiteSpace(issuerPublicKey))
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取发卡行公钥，请检查tag90,92,9F32是否存在");
                    return false;
                }

                //验证hash签名
                result = Authencation.DES_SDA(issuerPublicKey, issuerExp, signedStaticAppData, toBeSignAppData, AIP);
                if (result != 0)
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "静态数据认证失败! 返回码: {0}", result);
                    return false;
                }
            }
            else //SM算法
            {
                issuerPublicKey = Authencation.GenSMIssuerPublicKey(CAPublicKey, issuerPublicCert, PAN);
                if (string.IsNullOrWhiteSpace(issuerPublicKey))
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取发卡行公钥，请检查tag90,5A是否存在");
                    return false;
                }

                result = Authencation.SM_SDA(issuerPublicKey, toBeSignAppData, signedStaticAppData, AIP);
                if (result != 0)
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "SM算法 静态数据认证失败! 返回码: {0}", result);
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// 进行DDA/fDDA脱机数据认证
        /// </summary>
        /// <param name="issuerPublicKey"></param>
        /// <param name="tag9F4B"></param>
        /// <param name="ddolData"></param>
        /// <returns></returns>
        protected virtual bool DDA(string issuerPublicKey, string tag9F4B, string ddolData)
        {
            TagDict tagDict = TagDict.GetInstance();
            IExcuteCase excuteCase = new CaseBase();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string iccPublicCert = tagDict.GetTag("9F46");
            string PAN = tagDict.GetTag("5A");
            string AIP = tagDict.GetTag("82");
            if (curTransAlgorithmCategory == AlgorithmCategory.DES)
            {
                //获取IC卡公钥
                string iccPublicKeyRemainder = tagDict.GetTag("9F48");
                string iccPublicKeyExp = tagDict.GetTag("9F47");
                string iccPublicKey = Authencation.GenDesICCPublicKey(issuerPublicKey, iccPublicCert, iccPublicKeyRemainder, toBeSignAppData, iccPublicKeyExp, AIP);
                if (string.IsNullOrWhiteSpace(iccPublicKey))
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取IC卡公钥，请确保tag9F46,9F48,9F47是否存在");
                    return false;
                }

                //校验动态签名的hash值
                var result = Authencation.DES_DDA(iccPublicKey, iccPublicKeyExp, tag9F4B, ddolData);
                if (result != 0)
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "动态数据认证失败! 返回码: {0}", result);
                    return false;
                }
            }
            else
            {
                string iccPublicKey = Authencation.GenSMICCPublicKey(issuerPublicKey, iccPublicCert, toBeSignAppData, AIP, PAN);
                if (string.IsNullOrWhiteSpace(iccPublicKey))
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "无法获取IC卡公钥");
                    return false;
                }
                var result = Authencation.SM_DDA(iccPublicKey, tag9F4B, ddolData);
                if (result != 0)
                {
                    excuteCase.TraceInfo(CaseLevel.Failed, caseNo, "SM算法 动态数据认证失败! 返回码: {0}", result);
                    return false;
                }
            }
            return true;
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

        /// <summary>
        /// 根据MDK/UDK生成过程密钥
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        protected string GenSessionKey(string key, TransKeyType transKeyType, AlgorithmCategory algorithmCategory)
        {
            string acSessionKey = string.Empty;
            var tagDict = TagDict.GetInstance();
            string ATC = tagDict.GetTag("9F36");
            if (transKeyType == TransKeyType.MDK)
            {
                string cardAcct = tagDict.GetTag("5A");
                string cardSeq = tagDict.GetTag("5F34");
                string UDKACKey = Authencation.GenUdk(key, cardAcct, cardSeq, (int)algorithmCategory);
                acSessionKey = Authencation.GenUdkSessionKey(UDKACKey, ATC, (int)algorithmCategory);
            }
            else
            {
                acSessionKey = Authencation.GenUdkSessionKey(key, ATC, (int)algorithmCategory);
            }

            return acSessionKey;
        }

        /// <summary>
        /// 校验发卡行自定义数据
        /// </summary>
        /// <returns></returns>
        protected bool CheckTag9F10Mac()
        {
            var tagDict = TagDict.GetInstance();
            string tag9F10 = tagDict.GetTag("9F10");
            string tag9F10Mac = tag9F10.Substring(tag9F10.Length - 8);
            string ATC = tagDict.GetTag("9F36");
            string data = string.Empty;

            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (tag9F10.Substring(18, 2) == "01") //暂时只支持IDD为01类型
            {
                var tag9F79 = tag9F10.Substring(20, 10);
                data = ATC + tag9F79 + "00";
            }

            string macSessionKey = string.Empty;
            string cardAcct = tagDict.GetTag("5A");
            string cardSeq = tagDict.GetTag("5F34");
            string mac = string.Empty;

            if (curTransAlgorithmCategory == AlgorithmCategory.DES)
            {
                macSessionKey = GenSessionKey(TransDesMACKey, KeyType, curTransAlgorithmCategory);
            }
            else
            {
                macSessionKey = GenSessionKey(TransSMMACKey, KeyType, curTransAlgorithmCategory);               
            }
            mac = Authencation.GenTag9F10Mac(macSessionKey, data, (int)curTransAlgorithmCategory);
            if (mac == tag9F10Mac)
            {
                return true;
            }
            return false;
        }

        /// <summary>
        /// 交易应用密文是否正确
        /// </summary>
        /// <param name="AC"></param>
        /// <returns></returns>
        protected bool CheckAC(string AC)
        {
            string acSessionKey = string.Empty;
            if (curTransAlgorithmCategory == AlgorithmCategory.DES)
            {
                acSessionKey = GenSessionKey(TransDesACKey, KeyType, curTransAlgorithmCategory);
            }
            else
            {
                acSessionKey = GenSessionKey(TransSMACKey, KeyType, curTransAlgorithmCategory);
            }

            string tag9F02 = locator.Terminal.TermianlSettings.GetTag("9F02");  //授权金额
            string tag9F03 = locator.Terminal.TermianlSettings.GetTag("9F03");  //其他金额
            string tag9F1A = locator.Terminal.TermianlSettings.GetTag("9F1A");  //终端国家代码
            string tag95 = locator.Terminal.TermianlSettings.GetTag("95");      //终端验证结果           
            string tag5F2A = locator.Terminal.TermianlSettings.GetTag("5F2A");  //交易货币代码
            string tag9A = locator.Terminal.TermianlSettings.GetTag("9A");      //交易日期
            string tag9C = locator.Terminal.TermianlSettings.GetTag("9C");      //交易类型
            string tag9F37 = locator.Terminal.TermianlSettings.GetTag("9F37");  //不可预知数
            string tag82 = TagDict.GetInstance().GetTag("82");
            string tag9F36 = TagDict.GetInstance().GetTag("9F36");

            string input = tag9F02 + tag9F03 + tag9F1A + tag95 + tag5F2A + tag9A + tag9C + tag9F37 + tag82 + tag9F36;
            //Authencation.
            return true;
        }
    }
}

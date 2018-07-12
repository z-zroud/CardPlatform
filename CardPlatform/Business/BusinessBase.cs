using CardPlatform.Cases;
using CardPlatform.Config;
using CardPlatform.Common;
using CardPlatform.ViewModel;
using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Reflection;

namespace CardPlatform.Business
{
    public class RequirementData
    {
        public RequirementData(string tag, int len, TipLevel level,string desc)
        {
            Tag = tag;
            Len = len;
            Level = level;
            Desc = desc;
        }

        public string Tag { get; set; }
        public int Len { get; set; }
        public TipLevel Level { get; set; }
        public string Desc { get; set; }
    }

    /// <summary>
    /// 所有应用交易的基础类
    /// </summary>
    public class BusinessBase
    {
        private ViewModelLocator locator;
        public BusinessBase()
        {
            KeyType = TransKeyType.MDK;
            locator = new ViewModelLocator();
            caseObj = new CaseBase();
        }

        public TransKeyType KeyType { get; set; }       //定义界面中输入的是MDK/UDK类型
        public string TransDesACKey { get; set; }       //DES_AC (MDK/UDK) 由KeyType决定
        public string TransDesMACKey { get; set; }      //DES_MAC
        public string TransDesENCKey { get; set; }      //DES_ENC
        public string TransSMACKey { get; set; }        //SM_AC
        public string TransSMMACKey { get; set; }       //SM_MAC
        public string TransSMENCKey { get; set; }       //SM_ENC
        public bool IsContactTrans { get; set; }    //需要此参数来判断写入到个人化信息表中的内容

        protected bool doDesTrans = false;  //是否执行DES算法的交易
        protected bool doSMTrans = false;   //是否执行国密算法
        protected string toBeSignAppData;   //当前交易流程认证数据
        protected string aid;       //当前应用AID
        protected AlgorithmCategory curTransAlgorithmCategory = AlgorithmCategory.DES;    //default 当前交易流程使用的算法
        protected IExcuteCase caseObj;  //定义了基本的case,涉及到程序逻辑，不在case配置文件中配置
        public static string PersoFile; //个人化信息表Excel文件
        

        //检查AIP支持的功能
        #region Check AIP support functions
        private int GetFirstByteOfAIP(string AIP)
        {
            if (AIP.Length != 4)
            {
                return 0;
            }
            return Convert.ToInt32(AIP.Substring(0, 2), 16);
        }
        public bool IsSupportSDA(string AIP)
        {
            var firstByte = GetFirstByteOfAIP(AIP);
            if ((firstByte & 0x40) == 0x40)
            {
                return true;
            }
            return false;
        }

        public bool IsSupportDDA(string AIP)
        {
            var firstByte = GetFirstByteOfAIP(AIP);
            if ((firstByte & 0x20) == 0x20)
            {
                return true;
            }
            return false;
        }

        public bool IsSupportCardHolderVerify(string AIP)
        {
            var firstByte = GetFirstByteOfAIP(AIP);
            if ((firstByte & 0x10) == 0x10)
            {
                return true;
            }
            return false;
        }

        public bool IsSupportTerminalRiskManagement(string AIP)
        {
            var firstByte = GetFirstByteOfAIP(AIP);
            if ((firstByte & 0x08) == 0x08)
            {
                return true;
            }
            return false;
        }

        public bool IsSupportIssuerAuth(string AIP)
        {
            var firstByte = GetFirstByteOfAIP(AIP);
            if ((firstByte & 0x04) == 0x04)
            {
                return true;
            }
            return false;
        }

        public bool IsSupportCDA(string AIP)
        {
            var firstByte = GetFirstByteOfAIP(AIP);
            if ((firstByte & 0x01) == 0x01)
            {
                return true;
            }
            return false;
        }
        #endregion

        public TipLevel GetTipLevel(bool result, TipLevel level = TipLevel.Failed)
        {
            return result == true ? TipLevel.Sucess : level;
        }

        /// <summary>
        /// 设置DES应用秘钥
        /// </summary>
        /// <param name="acKey"></param>
        /// <param name="macKey"></param>
        /// <param name="encKey"></param>
        public void SetTransDESKeys(string acKey,string macKey,string encKey)
        {
            TransDesACKey = acKey;
            TransDesMACKey = macKey;
            TransDesENCKey = encKey;
        }

        /// <summary>
        /// 设置国密应用秘钥
        /// </summary>
        /// <param name="acKey"></param>
        /// <param name="macKey"></param>
        /// <param name="encKey"></param>
        public void SetTransSMKeys(string acKey,string macKey,string encKey)
        {
            TransSMACKey = acKey;
            TransSMMACKey = macKey;
            TransSMENCKey = encKey;
        }

        /// <summary>
        /// 执行交易的入口
        /// </summary>
        /// <param name="aid"></param>
        /// <param name="doDesTrans"></param>
        /// <param name="doSMTrans"></param>
        public virtual void DoTrans(string aid, bool doDesTrans, bool doSMTrans)
        {
            this.doDesTrans = doDesTrans;
            this.doSMTrans = doSMTrans;
            this.aid = aid;
        }

        /// <summary>
        /// 此函数定义了所有应用都应用GetData命令获取的数据
        /// </summary>
        public virtual void GetRequirementData()
        {

        }

        /// <summary>
        /// 解析TLV结构，并保存到数据字典中
        /// </summary>
        /// <param name="response"></param>
        /// <returns></returns>
        protected bool SaveTags(TransactionStep step,string response)
        {
            bool result = false;
            if (DataParse.IsTLV(response))
            {
                var arrTLV = DataParse.ParseTLV(response);
                TransactionTag tagDict = TransactionTag.GetInstance();
                tagDict.SetTags(step,arrTLV);
                result = true;
            }
            else
            {
                IExcuteCase cases = new CaseBase();
                var caseNo = MethodBase.GetCurrentMethod().Name;
                cases.TraceInfo(TipLevel.Failed, caseNo, "解析TLV格式失败,数据为{0}", response);
            }

            return result;
        }


        /// <summary>
        /// 选择应用，基类中的所有执行流程仅包含基本命令的交互，
        /// 不涉及应用逻辑的检查
        /// </summary>
        /// <param name="aid"></param>
        /// <returns></returns>
        protected virtual ApduResponse SelectAid(string aid)
        {
            return APDU.SelectCmd(aid);
        }

        /// <summary>
        /// 发送GPO命令
        /// </summary>
        /// <param name="pdol"></param>
        /// <returns></returns>
        protected virtual ApduResponse GPO(string pdolData)
        {
            return APDU.GPOCmd(pdolData);
        }

        /// <summary>
        /// 根据AFL结构，读取SFI指定的记录文件
        /// </summary>
        /// <param name="afls"></param>
        /// <returns></returns>
        protected virtual List<ApduResponse> ReadRecords(List<AFL> afls)
        {
            toBeSignAppData = string.Empty; //每次读取记录时，置空
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
        /// 获取发卡行公钥
        /// </summary>
        /// <returns></returns>
        protected string GetIssuerPublicKey()
        {
            string issuerPublicKey = string.Empty;
            IExcuteCase excuteCase = new CaseBase();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (aid.Length < 10 || aid.Length > 16)
            {
                excuteCase.TraceInfo(TipLevel.Failed, caseNo, "应用AID长度为{0}不在规范内", aid.Length);
                return issuerPublicKey;
            }

            //获取CA公钥
            TransactionTag tagDict = TransactionTag.GetInstance();
            string rid = aid.Substring(0, 10);
            string CAIndex = tagDict.GetTag("8F");
            if (CAIndex.Length != 2)
            {
                excuteCase.TraceInfo(TipLevel.Failed, caseNo, "无法获取CA 索引,请检查8F是否存在");
                return issuerPublicKey;
            }
            string CAPublicKey = Authencation.GenCAPublicKey(CAIndex, rid);
            if (string.IsNullOrWhiteSpace(CAPublicKey))
            {
                excuteCase.TraceInfo(TipLevel.Failed, caseNo, "无法获取CA公钥，请检查RID及索引是否正确");
                return issuerPublicKey;
            }

            string issuerPublicCert = tagDict.GetTag("90");
            string PAN = tagDict.GetTag("5A");
            string signedStaticAppData = tagDict.GetTag("93");
            string AIP = tagDict.GetTag("82");
            string expiryDate = tagDict.GetTag(TransactionStep.ReadRecord, "5F24");

            if (curTransAlgorithmCategory == AlgorithmCategory.DES) //DES算法
            {
                //获取发卡行公钥
                string issuerPublicKeyRemainder = tagDict.GetTag("92");
                string issuerExp = tagDict.GetTag("9F32");
                var result = Authencation.GenDesIssuerPublicKey(CAPublicKey, issuerPublicCert, issuerPublicKeyRemainder, issuerExp, PAN, expiryDate, out issuerPublicKey);
                CheckPublicKeyResult(result, "发卡行公钥");
                if (string.IsNullOrWhiteSpace(issuerPublicKey))
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "无法获取发卡行公钥，请检查tag90,92,9F32是否存在");
                    return issuerPublicKey;
                }
            }
            else //SM算法
            {
                var result = Authencation.GenSMIssuerPublicKey(CAPublicKey, issuerPublicCert, PAN, expiryDate,out issuerPublicKey);
                CheckSMPublicKeyResult(result, "发卡行公钥");
                if (string.IsNullOrWhiteSpace(issuerPublicKey))
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "无法获取发卡行公钥，请检查tag90,5A是否存在");
                    return issuerPublicKey;
                }
            }
            return issuerPublicKey;
        }

        /// <summary>
        /// 获取IC卡公钥
        /// </summary>
        /// <param name="issuerPublicKey"></param>
        /// <returns></returns>
        protected string GetIccPublicKey(string issuerPublicKey)
        {
            string iccPublicKey = string.Empty;
            TransactionTag tagDict = TransactionTag.GetInstance();
            IExcuteCase excuteCase = new CaseBase();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string iccPublicCert = tagDict.GetTag("9F46");
            string PAN = tagDict.GetTag("5A");
            string AIP = tagDict.GetTag("82");
            string expiryDate = tagDict.GetTag(TransactionStep.ReadRecord, "5F24");
            if (curTransAlgorithmCategory == AlgorithmCategory.DES)
            {
                //获取IC卡公钥
                string iccPublicKeyRemainder = tagDict.GetTag("9F48");
                string iccPublicKeyExp = tagDict.GetTag("9F47");
                var result = Authencation.GenDesICCPublicKey(issuerPublicKey, iccPublicCert, iccPublicKeyRemainder, toBeSignAppData, iccPublicKeyExp, AIP,PAN, expiryDate, out iccPublicKey);
                CheckPublicKeyResult(result, "IC卡公钥");
                if (string.IsNullOrWhiteSpace(iccPublicKey))
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "无法获取IC卡公钥，请确保tag9F46,9F48,9F47是否存在");
                    return iccPublicKey;
                }
            }
            else
            {
                var result = Authencation.GenSMICCPublicKey(issuerPublicKey, iccPublicCert, toBeSignAppData, AIP, PAN, expiryDate,out iccPublicKey);
                CheckSMPublicKeyResult(result, "IC卡公钥");
                if (string.IsNullOrWhiteSpace(iccPublicKey))
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "无法获取IC卡公钥");
                    return iccPublicKey;
                }
            }
            return iccPublicKey;
        }
        /// <summary>
        /// 进行SDA验证
        /// </summary>
        /// <returns></returns>
        protected virtual bool SDA()
        {
            int result;
            IExcuteCase excuteCase = new CaseBase();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            TransactionTag tagDict = TransactionTag.GetInstance();
            string signedStaticAppData = tagDict.GetTag("93");
            string AIP = tagDict.GetTag("82");
            string issuerPublicKey = GetIssuerPublicKey();

            if (curTransAlgorithmCategory == AlgorithmCategory.DES) //DES算法
            {
                string issuerExp = tagDict.GetTag("9F32");
                //验证hash签名
                result = Authencation.DES_SDA(issuerPublicKey, issuerExp, signedStaticAppData, toBeSignAppData, AIP);
                if (!CheckOfflineResult(result,"SDA") )
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "静态数据认证失败! 返回码: {0}", result);
                    return false;
                }
            }
            else //SM算法
            {
                result = Authencation.SM_SDA(issuerPublicKey, toBeSignAppData, signedStaticAppData, AIP);
                if (!CheckSMOfflineResult(result, "SDA"))
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "SM算法 静态数据认证失败! 返回码: {0}", result);
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
            TransactionTag tagDict = TransactionTag.GetInstance();
            IExcuteCase excuteCase = new CaseBase();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var iccPublicKey = GetIccPublicKey(issuerPublicKey);
            if (curTransAlgorithmCategory == AlgorithmCategory.DES)
            {
                //获取IC卡公钥
                string iccPublicKeyRemainder = tagDict.GetTag("9F48");
                string iccPublicKeyExp = tagDict.GetTag("9F47");

                //校验动态签名的hash值
                var result = Authencation.DES_DDA(iccPublicKey, iccPublicKeyExp, tag9F4B, ddolData);
                if (!CheckOfflineResult(result,"DDA"))
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "动态数据认证失败! 返回码: {0}", result);
                    return false;
                }
            }
            else
            {
                var result = Authencation.SM_DDA(iccPublicKey, tag9F4B, ddolData);
                if (!CheckSMOfflineResult(result,"DDA"))
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "SM算法 动态数据认证失败! 返回码: {0}", result);
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// 发送第一次GAC
        /// </summary>
        /// <param name="acType"></param>
        /// <param name="CDOL1"></param>
        /// <returns></returns>
        protected virtual ApduResponse FirstGAC(int acType, string CDOL1)
        {
            string CDOL1Data = string.Empty;
            var tls = DataParse.ParseTL(CDOL1);
            foreach(var tl in tls)
            {
                CDOL1Data += locator.Terminal.TermianlSettings.GetTag(tl.Tag);
            }
            return APDU.GACCmd(acType, CDOL1Data);
        }

        /// <summary>
        /// 发送第二次GAC
        /// </summary>
        /// <param name="acType"></param>
        /// <param name="CDOL2"></param>
        /// <returns></returns>
        protected virtual ApduResponse SecondGAC(int acType, string cdol2)
        {
            string cdol2Data = string.Empty;
            var tls = DataParse.ParseTL(cdol2);
            foreach (var tl in tls)
            {
                cdol2Data += locator.Terminal.TermianlSettings.GetTag(tl.Tag);
            }

            return APDU.GACCmd(acType, cdol2Data);
        }

        /// <summary>
        /// 根据MDK/UDK生成过程密钥
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        protected string GenSessionKey(string key, TransKeyType transKeyType, AlgorithmCategory algorithmCategory)
        {
            string acSessionKey = string.Empty;
            var tagDict = TransactionTag.GetInstance();
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
            var tagDict = TransactionTag.GetInstance();
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
                if (string.IsNullOrEmpty(TransDesMACKey) || TransDesMACKey.Length != 32)
                    return false;
                macSessionKey = GenSessionKey(TransDesMACKey, KeyType, curTransAlgorithmCategory);
            }
            else
            {
                if (string.IsNullOrEmpty(TransSMMACKey) || TransSMMACKey.Length != 32)
                    return false;
                macSessionKey = GenSessionKey(TransSMMACKey, KeyType, curTransAlgorithmCategory);               
            }
            mac = Authencation.GenTag9F10Mac(macSessionKey, data, (int)curTransAlgorithmCategory);
            if (mac == tag9F10Mac)
            {
                return true;
            }
            return false;
        }

        protected string GenAC()
        {
            string udkACKey = string.Empty;
            var tagDict = TransactionTag.GetInstance();
            string ATC = tagDict.GetTag("9F36");
            if (KeyType == TransKeyType.MDK)
            {
                string cardAcct = tagDict.GetTag("5A");
                string cardSeq = tagDict.GetTag("5F34");
                udkACKey = Authencation.GenUdk(TransDesACKey, cardAcct, cardSeq);
            }
            else
            {
                udkACKey = TransDesACKey;
            }

            string tag9F02 = locator.Terminal.TermianlSettings.GetTag("9F02");  //授权金额
            string tag9F03 = locator.Terminal.TermianlSettings.GetTag("9F03");  //其他金额
            string tag9F1A = locator.Terminal.TermianlSettings.GetTag("9F1A");  //终端国家代码
            string tag95 = locator.Terminal.TermianlSettings.GetTag("95");      //终端验证结果           
            string tag5A = locator.Terminal.TermianlSettings.GetTag("5F2A");  //交易货币代码
            string tag9A = locator.Terminal.TermianlSettings.GetTag("9A");      //交易日期
            string tag9C = locator.Terminal.TermianlSettings.GetTag("9C");      //交易类型
            string tag9F37 = locator.Terminal.TermianlSettings.GetTag("9F37");  //不可预知数
            string tag82 = TransactionTag.GetInstance().GetTag("82");
            string tag9F36 = TransactionTag.GetInstance().GetTag("9F36");
            string tag9F10 = TransactionTag.GetInstance().GetTag("9F10");
            var customData = tag9F10.Substring(6);

            string input = tag9F02 + tag9F03 + tag9F1A + tag95 + tag5A + tag9A + tag9C + tag9F37 + tag82 + tag9F36 + customData;
            int zeroCount = input.Length % 16;
            if (zeroCount != 0)
            {
                input.PadRight(zeroCount, '0');
            }
            var mac = Authencation.GenEMVAC(udkACKey, input);
            //Authencation.
            return "";
        }

        /// <summary>
        /// 交易应用密文是否正确
        /// </summary>
        /// <param name="AC"></param>
        /// <returns></returns>
        protected bool CheckAC(string AC)
        {
            string udkACKey = string.Empty;
            var tagDict = TransactionTag.GetInstance();
            string ATC = tagDict.GetTag("9F36");
            if (KeyType == TransKeyType.MDK)
            {
                string cardAcct = tagDict.GetTag("5A");
                string cardSeq = tagDict.GetTag("5F34");
                udkACKey = Authencation.GenUdk(TransDesACKey, cardAcct, cardSeq);
            }
            else
            {
                udkACKey = TransDesACKey;
            }

            string tag9F02 = locator.Terminal.TermianlSettings.GetTag("9F02");  //授权金额
            string tag9F03 = locator.Terminal.TermianlSettings.GetTag("9F03");  //其他金额
            string tag9F1A = locator.Terminal.TermianlSettings.GetTag("9F1A");  //终端国家代码
            string tag95 = locator.Terminal.TermianlSettings.GetTag("95");      //终端验证结果           
            string tag5A = locator.Terminal.TermianlSettings.GetTag("5F2A");  //交易货币代码
            string tag9A = locator.Terminal.TermianlSettings.GetTag("9A");      //交易日期
            string tag9C = locator.Terminal.TermianlSettings.GetTag("9C");      //交易类型
            string tag9F37 = locator.Terminal.TermianlSettings.GetTag("9F37");  //不可预知数
            string tag82 = TransactionTag.GetInstance().GetTag("82");
            string tag9F36 = TransactionTag.GetInstance().GetTag("9F36");
            string tag9F10 = TransactionTag.GetInstance().GetTag("9F10");
            var customData = tag9F10.Substring(6);

            string input = tag9F02 + tag9F03 + tag9F1A + tag95 + tag5A + tag9A + tag9C + tag9F37 + tag82 + tag9F36 + customData;
            int zeroCount = input.Length % 16;
            if (zeroCount != 0)
            {
                input.PadRight(zeroCount, '0');
            }
            var mac = Authencation.GenEMVAC(udkACKey, input);
            //Authencation.
            return true;
        }

        protected void CheckPublicKeyResult(int result, string publicKeyName)
        {
            bool check01 = true;
            bool check02 = true;
            bool check04 = true;
            bool check08 = true;
            bool check10 = true;
            bool check100 = true;
            if ((result & 0x01) == 0x01) check01 = false;
            if ((result & 0x02) == 0x02) check02 = false;
            if ((result & 0x04) == 0x04) check04 = false;
            if ((result & 0x08) == 0x08) check08 = false;
            if ((result & 0x10) == 0x10) check10 = false;
            if ((result & 0x100) == 0x100) check100 = false;

            caseObj.TraceInfo(GetTipLevel(check01), "OfflineDataAuthencation", "检测({0})恢复数据是否解密成功",publicKeyName);
            caseObj.TraceInfo(GetTipLevel(check02), "OfflineDataAuthencation", "检测({0})恢复数据收尾格式是否正确", publicKeyName);
            caseObj.TraceInfo(GetTipLevel(check04), "OfflineDataAuthencation", "检测({0})恢复数据hash值是否正确", publicKeyName);
            caseObj.TraceInfo(GetTipLevel(check08), "OfflineDataAuthencation", "检测({0})证书是否失效", publicKeyName);
            caseObj.TraceInfo(GetTipLevel(check10), "OfflineDataAuthencation", "检测({0})恢复数据中主账号是否与卡片主账号匹配", publicKeyName);
            caseObj.TraceInfo(GetTipLevel(check100), "OfflineDataAuthencation", "检测({0})证书有效期是否晚于或等于应用有效期", publicKeyName);
        }

        protected bool CheckOfflineResult(int result, string offlineType)
        {
            bool check01 = true;
            bool check02 = true;
            bool check04 = true;
            bool check80 = true;
            if ((result & 0x01) == 0x01) check01 = false;
            if ((result & 0x02) == 0x02) check02 = false;
            if ((result & 0x04) == 0x04) check04 = false;
            if ((result & 0x80) == 0x80) check80 = false;

            caseObj.TraceInfo(GetTipLevel(check01), "OfflineDataAuthencation", "检测({0})恢复数据是否解密成功", offlineType);
            caseObj.TraceInfo(GetTipLevel(check02), "OfflineDataAuthencation", "检测({0})恢复数据收尾格式是否正确", offlineType);
            caseObj.TraceInfo(GetTipLevel(check04), "OfflineDataAuthencation", "检测({0})签名Hash是否正确", offlineType);
            caseObj.TraceInfo(GetTipLevel(check80), "OfflineDataAuthencation", "检测({0})脱机认证是否成功", offlineType);

            return check80;
        }

        protected void CheckSMPublicKeyResult(int result, string publicKeyName)
        {
            bool check01 = true;
            bool check02 = true;
            bool check04 = true;
            bool check08 = true;
            bool check10 = true;
            bool check20 = true;
            if ((result & 0x01) == 0x01) check01 = false;
            if ((result & 0x02) == 0x02) check02 = false;
            if ((result & 0x04) == 0x04) check04 = false;
            if ((result & 0x08) == 0x08) check08 = false;
            if ((result & 0x10) == 0x10) check10 = false;
            if ((result & 0x20) == 0x20) check20 = false;

            caseObj.TraceInfo(GetTipLevel(check01), "OfflineDataAuthencation", "检测({0})证书格式是否正确", publicKeyName);
            caseObj.TraceInfo(GetTipLevel(check02), "OfflineDataAuthencation", "检测({0})恢复数据中主账号是否与卡片主账号匹配", publicKeyName);
            caseObj.TraceInfo(GetTipLevel(check04), "OfflineDataAuthencation", "检测({0})证书中算法标识是否正确", publicKeyName);
            caseObj.TraceInfo(GetTipLevel(check08), "OfflineDataAuthencation", "检测({0})证书是否失效", publicKeyName);
            caseObj.TraceInfo(GetTipLevel(check10), "OfflineDataAuthencation", "检测({0})证书有效期是否晚于或等于应用有效期", publicKeyName);
            caseObj.TraceInfo(GetTipLevel(check20), "OfflineDataAuthencation", "检测({0})公钥长度是否正确", publicKeyName);
        }

        protected bool CheckSMOfflineResult(int result, string offlineType)
        {
            bool check01 = true;
            bool check02 = true;
            bool check80 = true;
            if ((result & 0x01) == 0x01) check01 = false;
            if ((result & 0x02) == 0x02) check02 = false;
            if ((result & 0x80) == 0x80) check80 = false;

            caseObj.TraceInfo(GetTipLevel(check01), "OfflineDataAuthencation", "检测({0})签名数据格式是否正确", offlineType);
            caseObj.TraceInfo(GetTipLevel(check02), "OfflineDataAuthencation", "检测({0})签名数据长度是否正确", offlineType);
            caseObj.TraceInfo(GetTipLevel(check80), "OfflineDataAuthencation", "检测({0})脱机认证是否成功", offlineType);

            return check80;
        }
    }
}

using CardPlatform.Cases;
using CardPlatform.Config;
using CardPlatform.Common;
using CardPlatform.ViewModel;
using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Reflection;
using CardPlatform.Helper;
using GalaSoft.MvvmLight.Threading;
using CardPlatform.Models;

namespace CardPlatform.Business
{
    //一个用于GetData获取数据的结构描述
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
        protected static readonly ViewModelLocator  locator         = new ViewModelLocator();
        protected static readonly TransactionConfig transCfg        = TransactionConfig.GetInstance();
        protected static readonly TransactionTag    transTags       = TransactionTag.GetInstance();
        protected static readonly IExcuteCase       caseObj         = new CaseBase();
        protected static readonly Log               log             = Log.CreateLog(Constant.LogPath);
        protected TVR                               tvr             = TVR.GetInstance();
        protected string                            needSignAppData;    //当前交易流程认证数据
        protected string                            currentAid;         //当前应用AID

        public int BatchNo { get; set; }                //一次检测中重复跑该交易的序号
        public bool IsContactTrans { get; set; }        //需要此参数来判断写入到个人化信息表中的内容
        public string PersoFile { get; set; }           //个人化信息表Excel文件


        public BusinessBase()
        {
            tvr.Clear();    //需要在每次交易之前清除上次TVR结果
        }

        public TipLevel GetTipLevel(bool result, TipLevel level = TipLevel.Failed)
        {
            return result == true ? TipLevel.Sucess : level;
        }

        /// <summary>
        /// 执行交易的入口
        /// </summary>
        /// <param name="aid"></param>
        /// <param name="doDesTrans"></param>
        /// <param name="doSMTrans"></param>
        public virtual void DoTransaction(string aid)
        {
            CaseBase.ResetCase();
            if(transCfg.CurrentApp == AppType.PSE ||
                transCfg.CurrentApp == AppType.PPSE)
            {
                transTags.Clear();    //做交易之前，需要将tag清空，避免与上次交易重叠
            }
            else
            {
                transTags.ClearExceptPSEandPPSE();  //不需要删除PSE或PPSE应用，后续应用需要用到PSE做关联性检测
            }
            
            currentAid = aid;
        }
        
        /// <summary>
        /// 填写送检表
        /// </summary>
        public void WriteTagToSongJianFile()
        {
            var songJianFile = new SongJianHelper(PersoFile);
            if (!string.IsNullOrEmpty(PersoFile))
            {
                songJianFile.WriteToFile(transCfg.CurrentApp, transCfg.Algorithm, transCfg.TransType);
            }
        }

        /// <summary>
        /// 开始检测，实际的检测流程由传入的回调函数完成.
        /// </summary>
        /// <param name="DoActualTransaction"></param>
        public void DoTransaction(Func<bool> DoActualTransaction)
        {
            if(transCfg.Algorithm == AlgorithmType.DES)
            {
                locator.Terminal.SetTag("DF69", "00");
            }
            else
            {
                locator.Terminal.SetTag("DF69", "01");
            }
            bool isSucess = DoActualTransaction();
            WriteTagToSongJianFile();
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
            {
                TransResultModel transactionResult = new TransResultModel(transCfg.TransType);
                if (isSucess)
                {
                    transactionResult.Result = TransResult.Sucess;
                }
                else
                {
                    transactionResult.Result = TransResult.Failed;
                }
                locator.Transaction.TransResult.Add(transactionResult);
            });
        }

        /// <summary>
        /// 此函数定义了所有应用用GetData命令获取的数据
        /// </summary>
        public virtual void GetRequirementData()
        {
            ///...get basic data
        }

        protected string GetDolData(string dol)
        {
            string data = string.Empty;
            if (!string.IsNullOrEmpty(dol))
            {
                var tls = DataParse.ParseTL(dol);

                foreach (var tl in tls)
                {
                    var terminalData = locator.Terminal.GetTag(tl.Tag);
                    if (string.IsNullOrEmpty(terminalData))
                    {
                        log.TraceLog(LogLevel.Error, "终端设置中不存在tag{0},请检查终端设置是否正确.", tl.Tag);
                        return null;
                    }
                    if (terminalData.Length != tl.Len * 2)
                    {
                        log.TraceLog(LogLevel.Error, "tag{0} 终端设置长度[{1}]与tag9F38中指定长度[{2}]不一致", tl.Tag, terminalData.Length / 2, tl.Len);
                        return null;
                    }
                    data += terminalData;
                }
            }
            return data;
        }

        protected void ParseTlvToLog(List<TLV> tlvs)
        {
            foreach(var tlv in tlvs)
            {
                string info = string.Empty;
                string prefixPadding = info.PadLeft(tlv.Level * 4, ' ');
                if(tlv.IsTemplate)
                {
                    info = prefixPadding + "<" + tlv.Tag + ">";
                }
                else
                {
                    info = prefixPadding + "[" + tlv.Tag + "]=" + tlv.Value;
                }
                log.TraceLog(LogLevel.Info, info);
            }
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
            var resp= APDU.SelectCmd(aid);
            if(resp.SW == 0x9000)
            {
                var tlvs = DataParse.ParseTLV(resp.Response);
                ParseTlvToLog(tlvs);
            }
            return resp;
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
            needSignAppData = string.Empty; //每次读取记录时，置空
            var responses = new List<ApduResponse>();
            foreach(var afl in afls)
            {
                var resp = APDU.ReadRecordCmd(afl.SFI, afl.RecordNo);
                responses.Add(resp);
                var tlvs = DataParse.ParseTLV(resp.Response);
                ParseTlvToLog(tlvs);
                if (afl.IsSignedRecordNo)
                {
                    foreach (var tlv in tlvs)
                    {
                        if(tlv.IsTemplate && tlv.Tag == "70") //用于脱机数据认证中的签名数据
                        {
                            needSignAppData += tlv.Value;
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
            if (currentAid.Length < 10 || currentAid.Length > 16)
            {
                excuteCase.TraceInfo(TipLevel.Failed, caseNo, "应用AID长度为{0}不在规范内", currentAid.Length);
                return issuerPublicKey;
            }

            //获取CA公钥
            TransactionTag tagDict = TransactionTag.GetInstance();
            string rid = currentAid.Substring(0, 10);
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

            if (transCfg.Algorithm == AlgorithmType.DES) //DES算法
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
            if (transCfg.Algorithm == AlgorithmType.DES)
            {
                //获取IC卡公钥
                string iccPublicKeyRemainder = tagDict.GetTag("9F48");
                string iccPublicKeyExp = tagDict.GetTag("9F47");
                var result = Authencation.GenDesICCPublicKey(issuerPublicKey, iccPublicCert, iccPublicKeyRemainder, needSignAppData, iccPublicKeyExp, AIP,PAN, expiryDate, out iccPublicKey);
                CheckPublicKeyResult(result, "IC卡公钥");
                if (string.IsNullOrWhiteSpace(iccPublicKey))
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "无法获取IC卡公钥，请确保tag9F46,9F48,9F47是否存在");
                    return iccPublicKey;
                }
            }
            else
            {
                var result = Authencation.GenSMICCPublicKey(issuerPublicKey, iccPublicCert, needSignAppData, AIP, PAN, expiryDate,out iccPublicKey);
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

            if (transCfg.Algorithm == AlgorithmType.DES) //DES算法
            {
                string issuerExp = tagDict.GetTag("9F32");
                //验证hash签名
                result = Authencation.DES_SDA(issuerPublicKey, issuerExp, signedStaticAppData, needSignAppData, AIP);
                if (!CheckOfflineResult(result,"SDA") )
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "静态数据认证失败! 返回码: {0}", result);
                    return false;
                }
            }
            else //SM算法
            {
                result = Authencation.SM_SDA(issuerPublicKey, needSignAppData, signedStaticAppData, AIP);
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
            if (transCfg.Algorithm == AlgorithmType.DES)
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
                CDOL1Data += locator.Terminal.GetTag(tl.Tag);
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
                cdol2Data += locator.Terminal.GetTag(tl.Tag);
            }

            return APDU.GACCmd(acType, cdol2Data);
        }

        /// <summary>
        /// 根据MDK/UDK生成过程密钥
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        protected string GenSessionKey(string key, AppKeyType appKeyType, AlgorithmType algorithmType)
        {
            string acSessionKey = string.Empty;
            var tagDict = TransactionTag.GetInstance();
            string ATC = tagDict.GetTag("9F36");
            if (appKeyType == AppKeyType.MDK)
            {
                string cardAcct = tagDict.GetTag("5A");
                string cardSeq = tagDict.GetTag("5F34");
                string UDKACKey = Authencation.GenUdk(key, cardAcct, cardSeq, (int)algorithmType);
                acSessionKey = Authencation.GenUdkSessionKey(UDKACKey, ATC, (int)algorithmType);
            }
            else
            {
                acSessionKey = Authencation.GenUdkSessionKey(key, ATC, (int)algorithmType);
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

            if (transCfg.Algorithm == AlgorithmType.DES)
            {
                if (string.IsNullOrEmpty(transCfg.TransDesMacKey) || transCfg.TransDesMacKey.Length != 32)
                    return false;
                macSessionKey = GenSessionKey(transCfg.TransDesMacKey, transCfg.KeyType, transCfg.Algorithm);
            }
            else
            {
                if (string.IsNullOrEmpty(transCfg.TransSmMacKey) || transCfg.TransSmMacKey.Length != 32)
                    return false;
                macSessionKey = GenSessionKey(transCfg.TransSmMacKey, transCfg.KeyType, transCfg.Algorithm);               
            }
            mac = Authencation.GenTag9F10Mac(macSessionKey, data, (int)transCfg.Algorithm);
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
            if (transCfg.KeyType == AppKeyType.MDK)
            {
                string cardAcct = tagDict.GetTag("5A");
                string cardSeq = tagDict.GetTag("5F34");
                udkACKey = Authencation.GenUdk(transCfg.TransDesAcKey, cardAcct, cardSeq);
            }
            else
            {
                udkACKey = transCfg.TransDesAcKey;
            }

            string tag9F02 = locator.Terminal.GetTag("9F02");  //授权金额
            string tag9F03 = locator.Terminal.GetTag("9F03");  //其他金额
            string tag9F1A = locator.Terminal.GetTag("9F1A");  //终端国家代码
            string tag95 = locator.Terminal.GetTag("95");      //终端验证结果           
            string tag5A = locator.Terminal.GetTag("5F2A");  //交易货币代码
            string tag9A = locator.Terminal.GetTag("9A");      //交易日期
            string tag9C = locator.Terminal.GetTag("9C");      //交易类型
            string tag9F37 = locator.Terminal.GetTag("9F37");  //不可预知数
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
            if (transCfg.KeyType == AppKeyType.MDK)
            {
                string cardAcct = tagDict.GetTag("5A");
                string cardSeq = tagDict.GetTag("5F34");
                udkACKey = Authencation.GenUdk(transCfg.TransDesAcKey, cardAcct, cardSeq);
            }
            else
            {
                udkACKey = transCfg.TransDesAcKey;
            }

            string tag9F02 = locator.Terminal.GetTag("9F02");  //授权金额
            string tag9F03 = locator.Terminal.GetTag("9F03");  //其他金额
            string tag9F1A = locator.Terminal.GetTag("9F1A");  //终端国家代码
            string tag95 = locator.Terminal.GetTag("95");      //终端验证结果           
            string tag5A = locator.Terminal.GetTag("5F2A");  //交易货币代码
            string tag9A = locator.Terminal.GetTag("9A");      //交易日期
            string tag9C = locator.Terminal.GetTag("9C");      //交易类型
            string tag9F37 = locator.Terminal.GetTag("9F37");  //不可预知数
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

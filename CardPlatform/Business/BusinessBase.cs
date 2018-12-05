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
using UtilLib;

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
        protected static readonly BusinessUtil      businessUtil    = new BusinessUtil();
        protected static readonly TransactionConfig transCfg        = TransactionConfig.GetInstance();
        protected static readonly TransactionTag    transTags       = TransactionTag.GetInstance();
        protected static readonly IExcuteCase       caseObj         = new CaseBase();
        protected static readonly Log               log             = Log.CreateLog(Constant.LogPath);
        protected TVR                               tvr             = TVR.GetInstance();
        protected string                            needSignAppData;    //当前交易流程认证数据
        protected string                            currentAid;         //当前应用AID

        public int                                  BatchNo         { get; set; }       //一次检测中重复跑该交易的序号
        public bool                                 IsContactTrans  { get; set; }       //需要此参数来判断写入到个人化信息表中的内容
        public string                               PersoInfoFile   { get; set; }       //个人化信息表Excel文件


        public BusinessBase()
        {
            tvr.Clear();    //需要在每次交易之前清除上次TVR结果
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
            var songJianFile = new SongJianHelper(PersoInfoFile);
            if (!string.IsNullOrEmpty(PersoInfoFile))
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
            //设置通用的终端属性
            if(transCfg.Algorithm == AlgorithmType.DES)
            {
                locator.Terminal.SetTag("DF69", "00");
            }
            else
            {
                locator.Terminal.SetTag("DF69", "01");
            }
            bool isSucess = DoActualTransaction();  //开始执行交易流程检测
            WriteTagToSongJianFile();   //填写个人化信息表
            //将交易结果显示到UI中
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
            {
                locator.Transaction.TransResult.Tag5A = transTags.GetTag("5A");
                locator.Transaction.TransResult.Tag50 = Utils.BcdToStr(transTags.GetTag("50"));
                locator.Transaction.TransResult.Tag5F20 = Utils.BcdToStr(transTags.GetTag("5F20"));
                string tag5F24 = transTags.GetTag("5F24");
                string tag5F25 = transTags.GetTag("5F25");
                if (!string.IsNullOrEmpty(tag5F24))
                    locator.Transaction.TransResult.Tag5F24 = "20" + tag5F24.Substring(0, 2) + "-" + tag5F24.Substring(2, 2) + "-" + tag5F24.Substring(4);
                if(!string.IsNullOrEmpty(tag5F25))
                    locator.Transaction.TransResult.Tag5F25 = "20" + tag5F25.Substring(0, 2) + "-" + tag5F25.Substring(2, 2) + "-" + tag5F25.Substring(4);
                locator.Transaction.TransResult.Tag5F34 = transTags.GetTag("5F34");
                var version = transTags.GetTag("9F08");
                if(transCfg.CurrentApp == AppType.VISA)
                {
                    if (version == "0096")
                        locator.Transaction.TransResult.Tag9F08 = "VISA 1.5";
                    else if (version == "00A0")
                        locator.Transaction.TransResult.Tag9F08 = "VISA 1.6";
                    else
                        locator.Transaction.TransResult.Tag9F08 = "未知的VISA版本";
                }
                
            });
        }

        ///// <summary>
        ///// 此函数定义了所有应用用GetData命令获取的数据
        ///// </summary>
        //public virtual void GetRequirementData()
        //{
        //    ///...get basic data
        //}

        /// <summary>
        /// 选择应用，基类中的所有执行流程仅包含基本命令的交互，
        /// 不涉及应用逻辑的检查
        /// </summary>
        /// <param name="aid"></param>
        /// <returns></returns>
        public virtual ApduResponse ApplicationSelection(string aid)
        {
            var resp= APDU.SelectCmd(aid);
            if(resp.SW == 0x9000)
            {
                var tlvs = DataParse.ParseTLV(resp.Response);
                businessUtil.ShowTlvLog(tlvs);
            }
            return resp;
        }

        /// <summary>
        /// 发送GPO命令
        /// </summary>
        /// <param name="pdol"></param>
        /// <returns></returns>
        public virtual ApduResponse InitiateApplicationProcessing(string pdolData)
        {
            //return APDU.GPOCmd(pdolData);
            var resp = APDU.GPOCmd(pdolData);
            if(resp.SW == 0x9000)
            {
                var tlvs = DataParse.ParseTLV(resp.Response);
                if (resp.Response.Substring(0,2) == "80")
                {
                    log.TraceLog(LogLevel.Info, "<{0}>", tlvs[0].Tag);
                    log.TraceLog(LogLevel.Info, "\t[82]={0}", tlvs[0].Value.Substring(0, 4));
                    log.TraceLog(LogLevel.Info, "\t[94]={0}", tlvs[0].Value.Substring(4));
                }
                else
                {
                    businessUtil.ShowTlvLog(tlvs);
                }
            }
            return resp;
        }

        /// <summary>
        /// 根据AFL结构，读取SFI指定的记录文件
        /// </summary>
        /// <param name="afls"></param>
        /// <returns></returns>
        public virtual List<ApduResponse> ReadApplicationData(List<AFL> afls)
        {
            needSignAppData = string.Empty; //每次读取记录时，置空
            var responses = new List<ApduResponse>();
            foreach(var afl in afls)
            {
                var resp = APDU.ReadRecordCmd(afl.SFI, afl.RecordNo);
                responses.Add(resp);
                var tlvs = DataParse.ParseTLV(resp.Response);
                businessUtil.ShowTlvLog(tlvs);
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
        /// 处理限制,
        /// </summary>
        /// <returns></returns>
        protected int ProcessingRestrictions()
        {
            log.TraceLog(LogLevel.Info, "========================= Processing Restrictions  =========================");
            var handleLimitationCase = new ProcessRestrictionCase();
            handleLimitationCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.ProcessRestriction, null);
            return 0;
        }

        /// <summary>
        /// 持卡人认证
        /// </summary>
        /// <returns></returns>
        protected int CardHolderVerification()
        {
            log.TraceLog(LogLevel.Info, "========================= Cardholder Verification  =========================");
            var cardHolderVerifyCase = new CardHolderVerifyCase();
            cardHolderVerifyCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.CardHolderVerify, null);
            string tag8E = transTags.GetTag(TransactionStep.ReadRecord, "8E");
            locator.Terminal.SetTag("9F34", tag8E.Substring(10, 6), "持卡人验证方法结果"); //总是以第一个CVM作为验证结果
            return 0;
        }

        /// <summary>
        /// 终端风险管理为大额交易提供了发卡行授权，确保卡片交易可以周期性的
        /// 进行联机处理。
        /// </summary>
        /// <returns></returns>
        protected int TerminalRiskManagement()
        {
            log.TraceLog(LogLevel.Info, "========================= Terminal Risk Management  =========================");
            var terminalRishManagementCase = new TerminalRiskManagementCase();
            terminalRishManagementCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.TerminalRiskManagement, null);
            return 0;
        }

        /// <summary>
        /// 进行SDA验证
        /// </summary>
        /// <returns></returns>
        protected virtual bool SDA()
        {
            bool result;
            IExcuteCase excuteCase = new CaseBase();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string tag93 = transTags.GetTag("93");
            string tag82 = transTags.GetTag("82");
            string issuerPublicKey = GetIssuerPublicKey();

            if (transCfg.Algorithm == AlgorithmType.DES) //DES算法
            {
                string issuerExp = transTags.GetTag("9F32");
                //验证hash签名
                result = businessUtil.DES_SDA(issuerPublicKey, issuerExp, tag93, needSignAppData, tag82);
                if (!result)
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "静态数据认证失败! 返回码: {0}", result);
                    return false;
                }
            }
            else //SM算法
            {
                result = businessUtil.SM_SDA(issuerPublicKey, needSignAppData, tag93, tag82);
                if (!result)
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "SM算法 静态数据认证失败! 返回码: {0}", result);
                    return false;
                }
            }
            caseObj.TraceInfo(TipLevel.Sucess, caseNo, "检测DES DDA脱机数据认证是否成功");
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
            IExcuteCase excuteCase = new CaseBase();
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var iccPublicKey = GetIccPublicKey(issuerPublicKey);
            
            if (transCfg.Algorithm == AlgorithmType.DES)
            {
                //获取IC卡公钥
                string iccPublicKeyRemainder = transTags.GetTag("9F48");
                string iccPublicKeyExp = transTags.GetTag("9F47");

                //校验动态签名的hash值
                var result = businessUtil.DES_DDA(iccPublicKey, iccPublicKeyExp, tag9F4B, ddolData);
                if (!result)
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "动态数据认证失败! 返回码: {0}", result);
                    return false;
                }
            }
            else
            {
                var result = businessUtil.SM_DDA(iccPublicKey, tag9F4B, ddolData);
                if (!result)
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "SM算法 动态数据认证失败! 返回码: {0}", result);
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// 获取发卡行公钥
        /// </summary>
        /// <returns></returns>
        protected string GetIssuerPublicKey()
        {
            string issuerPublicKey = string.Empty;
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (currentAid.Length < 10 || currentAid.Length > 16)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "应用AID长度为{0}不在规范内", currentAid.Length);
                return string.Empty;
            }

            //获取CA公钥
            string rid = currentAid.Substring(0, 10);
            string CAIndex = transTags.GetTag("8F");
            if (CAIndex.Length != 2)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "无法获取CA 索引,请检查8F是否存在");
                return string.Empty;
            }
            string CAPublicKey = Authencation.GenCAPublicKey(CAIndex, rid);
            if (string.IsNullOrWhiteSpace(CAPublicKey))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "无法获取CA公钥，请检查RID及索引是否正确");
                return string.Empty;
            }

            string issuerPublicCert = transTags.GetTag("90");
            string tag5A = transTags.GetTag("5A");
            string tag5F24 = transTags.GetTag(TransactionStep.ReadRecord, "5F24");

            if (transCfg.Algorithm == AlgorithmType.DES) //DES算法
            {
                //获取发卡行公钥
                string issuerPublicKeyRemainder = transTags.GetTag("92");
                string issuerExp = transTags.GetTag("9F32");
                issuerPublicKey = businessUtil.GenDesIssuerPublicKey(CAPublicKey, issuerPublicCert, issuerPublicKeyRemainder, issuerExp, tag5A, tag5F24);
                if (string.IsNullOrWhiteSpace(issuerPublicKey))
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "无法获取发卡行公钥，请检查tag90,92,9F32是否存在");
                    return string.Empty;
                }
            }
            else //SM算法
            {
                issuerPublicKey = businessUtil.GenSMIssuerPublicKey(CAPublicKey, issuerPublicCert, tag5A, tag5F24);
                if (string.IsNullOrWhiteSpace(issuerPublicKey))
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "无法获取发卡行公钥，请检查tag90,5A是否存在");
                    return string.Empty;
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
            string tag5A = tagDict.GetTag("5A");
            string tag82 = tagDict.GetTag("82");
            string tag5F24 = tagDict.GetTag(TransactionStep.ReadRecord, "5F24");
            if (transCfg.Algorithm == AlgorithmType.DES)
            {
                //获取IC卡公钥
                string iccPublicKeyRemainder = tagDict.GetTag("9F48");
                string iccPublicKeyExp = tagDict.GetTag("9F47");
                string ipkExp = tagDict.GetTag("9F32");
                iccPublicKey = businessUtil.GenDesICCPublicKey(issuerPublicKey,
                    ipkExp,
                    iccPublicCert,
                    iccPublicKeyExp,
                    iccPublicKeyRemainder,
                    needSignAppData,
                    tag82,
                    tag5A,
                    tag5F24);
                if (string.IsNullOrWhiteSpace(iccPublicKey))
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "无法获取IC卡公钥，请确保tag9F46,9F48,9F47是否存在");
                    return iccPublicKey;
                }
            }
            else
            {
                var result = businessUtil.GenSMICCPublicKey(issuerPublicKey, iccPublicCert, needSignAppData, tag82, tag5A, tag5F24);
                if (string.IsNullOrWhiteSpace(iccPublicKey))
                {
                    excuteCase.TraceInfo(TipLevel.Failed, caseNo, "无法获取IC卡公钥");
                    return iccPublicKey;
                }
            }
            return iccPublicKey;
        }


        /// <summary>
        /// 发送第一次GAC
        /// </summary>
        /// <param name="acType"></param>
        /// <param name="CDOL1"></param>
        /// <returns></returns>
        protected virtual ApduResponse GAC(int acType, string cdol)
        {
            var cdolData = businessUtil.GetDolData(cdol);
            return APDU.GACCmd(acType, cdolData);
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
            return true;
        }
    }
}

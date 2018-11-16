using CardPlatform.Cases;
using CardPlatform.Common;
using CardPlatform.Config;
using CardPlatform.Models;
using CardPlatform.ViewModel;
using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Business
{
    public class BusinessUtil
    {
        protected static readonly Log log = Log.CreateLog(Constant.LogPath);
        protected static readonly ViewModelLocator locator = new ViewModelLocator();
        protected static readonly IExcuteCase caseObj = new CaseBase();
        protected static readonly BusinessUtil businessUtil = new BusinessUtil();
        protected static readonly TransactionConfig transCfg = TransactionConfig.GetInstance();
        protected static readonly TransactionTag transTags = TransactionTag.GetInstance();

        public void ShowTlvLog(List<TLV> tlvs)
        {
            foreach (var tlv in tlvs)
            {
                string info = string.Empty;
                string prefixPadding = info.PadLeft(tlv.Level * 4, ' ');
                if (tlv.IsTemplate)
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
        public void SaveTags(TransactionStep step, List<TLV> tlvs)
        {
            TransactionTag tagDict = TransactionTag.GetInstance();
            tagDict.SetTags(step, tlvs);
        }

        public void SaveTag(TransactionStep step, string tag, string value)
        {
            TransactionTag tagDict = TransactionTag.GetInstance();
            tagDict.SetTag(step, tag, value);
        }

        /// <summary>
        /// 获取DOL数据
        /// </summary>
        /// <param name="dol"></param>
        /// <returns></returns>
        public string GetDolData(string dol)
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
                        return string.Empty;
                    }
                    if (terminalData.Length != tl.Len * 2)
                    {
                        log.TraceLog(LogLevel.Error, "tag{0} 终端设置长度[{1}]与DOL中指定长度[{2}]不一致", tl.Tag, terminalData.Length / 2, tl.Len);
                        return string.Empty;
                    }
                    data += terminalData;
                }
            }
            return data;
        }

        public bool DES_SDA(string issuerPublicKey, string ipkExponent, string tag93, string sigStaticData, string tag82)
        {
            string recoveryData = Authencation.GenRecoveryData(issuerPublicKey, ipkExponent, tag93);
            if (string.IsNullOrEmpty(recoveryData))
            {
                log.TraceLog("无法解签tag93,请检查发卡行公钥或者tag93是否正确");
                return false;
            }
            log.TraceLog("解签tag93恢复数据:{0}", recoveryData);
            if (recoveryData.Substring(0, 4) != "6A03" || recoveryData.Substring(recoveryData.Length - 2) != "BC")
            {
                log.TraceLog("恢复数据首尾格式不正确.");
                return false;
            }
            string hash = recoveryData.Substring(recoveryData.Length - 42, 40);
            string hashInput = recoveryData.Substring(2, recoveryData.Length - 44) + sigStaticData + tag82;
            string hashOuput = Authencation.GetHash(hashInput);
            log.TraceLog("参与hash计算的数据:{0}", hashInput);
            log.TraceLog("恢复数据hash:{0}", hash);
            log.TraceLog("计算的hash：{0}", hashOuput);
            if (hash != hashOuput)
            {
                log.TraceLog("恢复数据中的hash值与签名数据生成的hash比对失败");
                return false;
            }
            locator.Terminal.SetTag("9F45", recoveryData.Substring(6, 4),"从SDA恢复数据中获取的数据验证代码");
            return true;
        }

        public bool SM_SDA(string issuerPublicKey, string sigStaticData, string tag93, string tag82)
        {
            if (tag93.Substring(0, 2) != "13")
            {
                log.TraceLog("tag93签名数据格式不正确");
                return false;
            }
            string signedData = tag93.Substring(6);
            string toSignData = tag93.Substring(0, 6) + sigStaticData + tag82;
            int result = Authencation.SM2Verify(issuerPublicKey, toSignData, signedData);
            if (result != 0)
            {
                log.TraceLog("tag93签名数据与参与签名数据生成的签名不一致");
                return false;
            }
            return true;
        }

        public bool DES_DDA(string iccPublicKey, string iccExponent, string tag9F4B, string dynamicData)
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string recoveryData = Authencation.GenRecoveryData(iccPublicKey, iccExponent, tag9F4B);
            TipLevel level = TipLevel.Sucess;
            if (string.IsNullOrEmpty(recoveryData))
            {
                caseObj.TraceInfo(level, caseNo, "DES算法 DDA脱机数据认证 解签tag9F4B失败");
                return false;
            }
                
            log.TraceLog("解签tag9F4B恢复数据:{0}", recoveryData);

            if ((recoveryData.Substring(0, 4) != "6A95" && recoveryData.Substring(0, 4) != "6A05") || recoveryData.Substring(recoveryData.Length - 2) != "BC")
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "恢复数据格式不正确");
            }

            string hash = recoveryData.Substring(recoveryData.Length - 42, 40);
            string hashInput = recoveryData.Substring(2, recoveryData.Length - 44) + dynamicData;
            string hashOutput = Authencation.GetHash(hashInput);
            log.TraceLog("参与hash计算的数据:{0}", hashInput);
            log.TraceLog("恢复数据hash:{0}", hash);
            log.TraceLog("计算的hash：{0}", hashOutput);
            if (hash != hashOutput)
            {
                caseObj.TraceInfo(TipLevel.Failed,caseNo,"检测恢复数据中的hash值与签名数据生成的hash比对是否一致");
                return false;
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Sucess, caseNo, "检测恢复数据中的hash值与签名数据生成的hash比对是否一致");
            }
            int icCardDynamicDataLen = Convert.ToInt32(recoveryData.Substring(8, 2), 16);
            string tag9F4C = recoveryData.Substring(10, icCardDynamicDataLen * 2);
            locator.Terminal.SetTag("9F4C", tag9F4C, "从DDA中获取的IC卡动态数据");
            return true;
        }

        public bool SM_DDA(string iccPublicKey, string tag9F4B, string dynamicData)
        {
            if (tag9F4B.Substring(0, 2) != "15")
            {
                log.TraceLog("解签tag9F4B失败，请检查IC公钥是否正确");
                return false;
            }
            int iccDynamicAppDataLen = Convert.ToInt32(tag9F4B.Substring(2, 2), 16) * 2;
            if (tag9F4B.Length < 4 + iccDynamicAppDataLen)
            {
                return false;
            }
            string signedData = tag9F4B.Substring(4, iccDynamicAppDataLen);
            string toSignData = tag9F4B.Substring(0, 4 + iccDynamicAppDataLen) + dynamicData;
            int result = Authencation.SM2Verify(iccPublicKey, toSignData, signedData);
            if (result != 0)
            {
                log.TraceLog("tag9F4B签名数据与参与签名数据生成的签名不一致");
                return false;
            }
            return true;
        }

        public string GenDesIssuerPublicKey(string caPublicKey,
            string issuerPublicCert,
            string ipkRemainder,
            string ipkExp,
            string tag5A,
            string tag5F24)
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string recoveryData = Authencation.GenRecoveryData(caPublicKey, ipkExp, issuerPublicCert);
            if (string.IsNullOrEmpty(recoveryData))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "无法通过发卡行公钥证书获取恢复数据");
                return string.Empty;
            }
            if (recoveryData.Substring(0, 4) != "6A02" || recoveryData.Substring(recoveryData.Length - 2) != "BC")
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "通过发卡行公钥证书得到的恢复数据格式不正确");
                return string.Empty;
            }
            if(recoveryData.Substring(22,4) != "0101")
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "检测通过发卡行公钥证书得到的算法标识是否正确");
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Sucess, caseNo, "检测通过发卡行公钥证书得到的算法标识是否正确");
            }
            string hash = recoveryData.Substring(recoveryData.Length - 42, 40);
            string hashInput = recoveryData.Substring(2, recoveryData.Length - 44) + ipkRemainder + ipkExp;
            string hashOutput = Authencation.GetHash(hashInput);
            log.TraceLog("参与hash计算的数据:{0}", hashInput);
            log.TraceLog("恢复数据hash:{0}", hash);
            log.TraceLog("计算的hash：{0}", hashOutput);
            if (hash != hashOutput)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "检测恢复数据中的hash值与签名数据生成的hash比对是否一致");
                return string.Empty;
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Sucess, caseNo, "检测恢复数据中的hash值与签名数据生成的hash比对是否一致");
            }
            string bin = recoveryData.Substring(4, 8);
            bin = bin.Substring(0, bin.IndexOf('F'));
            if (tag5A.Substring(0,bin.Length) != bin || bin.Length < 3)
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "检测恢复得到的主账号和从IC卡读出的应用主账号是否一致");
            else
                caseObj.TraceInfo(TipLevel.Sucess, caseNo, "检测恢复得到的主账号和从IC卡读出的应用主账号是否一致");
            int today = 0;
            int nExpiryDate = 0;
            int nTag5F24 = 0;
            string expiryDate = recoveryData.Substring(14, 2) + recoveryData.Substring(12, 2);
            int.TryParse(DateTime.Now.ToString("yyMM"), out today);
            int.TryParse(expiryDate, out nExpiryDate);
            int.TryParse(tag5F24.Substring(0,4), out nTag5F24);
            if (nExpiryDate < today)
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "检测证书是否失效,证书失效日期为:{0}", expiryDate);
            else
                caseObj.TraceInfo(TipLevel.Sucess, caseNo, "检测证书是否失效,证书失效日期为:{0}", expiryDate);
            if (nExpiryDate < nTag5F24)
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "检测证书有效期是否比应用有效期早,证书有效期为{0}，应用有效期为{1}", expiryDate, tag5F24);
            else
                caseObj.TraceInfo(TipLevel.Sucess, caseNo, "检测证书有效期是否比应用有效期早,证书有效期为{0}，应用有效期为{1}", expiryDate, tag5F24);
            int issuerLen = Convert.ToInt32(recoveryData.Substring(26, 2), 16) * 2;
            if(caPublicKey.Length < issuerLen)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "检测CA公钥模需要大于或等于发卡行公钥");
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Sucess, caseNo, "检测CA公钥模需要大于或等于发卡行公钥");
            }
            if (issuerLen <= caPublicKey.Length - 72)
            {
                return recoveryData.Substring(30, issuerLen);
            }
            return recoveryData.Substring(30, recoveryData.Length - 72) + ipkRemainder;
        }

        public string GenDesICCPublicKey(string issuerPublicKey,
            string ipkExp,
            string iccPublicCert,
            string iccExp,
            string iccRemainder,
            string sigStaticData,
            string tag82,
            string tag5A,
            string tag5F24)
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            string recoveryData = Authencation.GenRecoveryData(issuerPublicKey, ipkExp, iccPublicCert);
            if (string.IsNullOrEmpty(recoveryData))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "无法通过IC卡公钥证书获取恢复数据");
                return string.Empty;
            }
            if (recoveryData.Substring(0, 4) != "6A04" || recoveryData.Substring(recoveryData.Length - 2) != "BC")
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "检测通过发IC卡公钥证书得到的恢复数据格式是否正确");
                return string.Empty;
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Sucess, caseNo, "检测通过发IC卡公钥证书得到的恢复数据格式是否正确");
            }
            string hash = recoveryData.Substring(recoveryData.Length - 42, 40);
            string hashInput = recoveryData.Substring(2, recoveryData.Length - 44) + iccRemainder + iccExp + sigStaticData + tag82;
            string hashOutput = Authencation.GetHash(hashInput);
            log.TraceLog("参与hash计算的数据:{0}", hashInput);
            log.TraceLog("恢复数据hash:{0}", hash);
            log.TraceLog("计算的hash：{0}", hashOutput);
            if (hash != hashOutput)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "检测恢复数据中的hash值与签名数据生成的hash比对是否一致");
                return string.Empty;
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Sucess, caseNo, "检测恢复数据中的hash值与签名数据生成的hash比对是否一致");
            }
            string account = recoveryData.Substring(4, 20);
            account = account.Substring(0, account.IndexOf('F'));
            if (account != tag5A)
                caseObj.TraceInfo(TipLevel.Failed,caseNo,"检测恢复得到的主账号和从IC卡读出的应用主账号是否一致");
            else
                caseObj.TraceInfo(TipLevel.Sucess, caseNo, "检测恢复得到的主账号和从IC卡读出的应用主账号是否一致");
            int today = 0;
            int nExpiryDate = 0;
            int nTag5F24 = 0;
            string expiryDate = recoveryData.Substring(26, 2) + recoveryData.Substring(24, 2);
            int.TryParse(DateTime.Now.ToString("yyMM"), out today);
            int.TryParse(expiryDate, out nExpiryDate);
            int.TryParse(tag5F24.Substring(0, 4), out nTag5F24);
            if (nExpiryDate < today)
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "检测证书已失效,证书失效日期为:{0}", expiryDate);
            else
                caseObj.TraceInfo(TipLevel.Sucess, caseNo, "检测证书已失效,证书失效日期为:{0}", expiryDate);
            if (nExpiryDate < nTag5F24)
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "检测证书有效期比应用有效期早,证书有效期为{0}，应用有效期为{1}", expiryDate, tag5F24) ;
            else
                caseObj.TraceInfo(TipLevel.Sucess, caseNo, "检测证书有效期比应用有效期早,证书有效期为{0}，应用有效期为{1}", expiryDate, tag5F24);
            int iccLen = Convert.ToInt32(recoveryData.Substring(38, 2), 16) * 2;
            if(issuerPublicKey.Length < iccLen)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "检测发卡行公钥长度是否大于或等于IC卡公钥长度");
            }
            else
            {
                caseObj.TraceInfo(TipLevel.Sucess, caseNo, "检测发卡行公钥长度是否大于或等于IC卡公钥长度");
            }
            if (iccLen <= issuerPublicKey.Length - 84)
            {
                return recoveryData.Substring(42, iccLen);
            }
            return recoveryData.Substring(42, recoveryData.Length - 84) + iccRemainder;
        }

        public string GenSMIssuerPublicKey(string caPublicKey,
            string issuerPublicCert,
            string tag5A,
            string tag5F24)
        {

            if (issuerPublicCert.Substring(0, 2) != "12")
            {
                return string.Empty;
            }
            return string.Empty;
        }

        public string GenSMICCPublicKey(string issuerPublicKey,
                string iccPublicCert,
                string needAuthStaticData,
                string tag82,
                string tag5A,
                string tag5F24)
        {
            return string.Empty;
        }

       
    }
}

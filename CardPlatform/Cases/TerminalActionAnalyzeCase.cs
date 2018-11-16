﻿using System;
using System.Collections.Generic;
using System.Reflection;
using CardPlatform.Common;
using CardPlatform.Config;
using CardPlatform.Models;
using CplusplusDll;


namespace CardPlatform.Cases
{
    public class TerminalActionAnalyzeCase : CaseBase
    {
        private ApduResponse response;
        private List<TLV> tlvs;

        public TerminalActionAnalyzeCase()
        {
            response = new ApduResponse();
            tlvs = new List<TLV>();
        }

        public override void Excute(int batchNo, AppType app, TransactionStep step, object srcData)
        {
            response = (ApduResponse)srcData;
            if(!DataParse.IsTLV(response.Response))
            {
                TraceInfo(TipLevel.Failed, "TLV分解", "无法解析GAC1返回的TLV数据");
            }
            tlvs = DataParse.ParseTLV(response.Response);
            base.Excute(batchNo,app,step, srcData);
            CheckTemplateTag(tlvs);           
        }

        /// <summary>
        /// 检测指令返回数据是否以80模板开头
        /// </summary>
        /// <returns></returns>
        public TipLevel TerminalActionAnalyze_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (tlvs.Count < 1 || tlvs[0].Tag != "80")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测指令返回数据是否以77模板开头
        /// </summary>
        /// <returns></returns>
        public TipLevel TerminalActionAnalyze_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (tlvs.Count < 1 || tlvs[0].Tag != "77")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测返回数据tag9F27的规范性
        /// </summary>
        /// <returns></returns>
        public TipLevel TerminalActionAnalyze_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F27 = TransactionTag.GetInstance().GetTag(TransactionStep.TerminalActionAnalyze, "9F27");
            if (string.IsNullOrEmpty(tag9F27))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[GAC1返回数据缺少tag9F27]");
            }
            if (tag9F27.Length != 2)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[GAC1返回数据tag9F27长度错误]");
            }
            if (tag9F27 != "80")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[GAC1返回数据tag9F27应为80]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测返回数据tag9F36的规范性
        /// </summary>
        /// <returns></returns>
        public TipLevel TerminalActionAnalyze_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F36 = TransactionTag.GetInstance().GetTag(TransactionStep.TerminalActionAnalyze, "9F36");
            if (string.IsNullOrEmpty(tag9F36))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[GAC1返回数据缺少tag9F36]");
            }
            if (tag9F36.Length != 4)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[GAC1返回数据tag9F36长度错误]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测返回数据tag9F26的规范性
        /// </summary>
        /// <returns></returns>
        public TipLevel TerminalActionAnalyze_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F26 = TransactionTag.GetInstance().GetTag(TransactionStep.TerminalActionAnalyze, "9F26");
            if (string.IsNullOrEmpty(tag9F26))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[GAC1返回数据缺少tag9F26]");
            }
            if (tag9F26.Length != 16)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[GAC1返回数据tag9F67长度错误]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测返回数据tag9F10的规范性
        /// </summary>
        /// <returns></returns>
        public TipLevel TerminalActionAnalyze_006()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F10 = TransactionTag.GetInstance().GetTag(TransactionStep.TerminalActionAnalyze, "9F10");
            if (string.IsNullOrEmpty(tag9F10))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[GAC1返回数据缺少tag9F10]");
            }
            if (tag9F10.Length > 64 || tag9F10.Length < 8)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[GAC1返回数据tag9F10长度错误]");
            }
            if (tag9F10.Substring(0, 2) != "06")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[GAC1返回数据tag9F10第一字节长度错误]");
            }
            var cvn = Convert.ToInt32(tag9F10.Substring(4, 2), 16);
            if (cvn != 0x0A && cvn != 0x12 && cvn != 0x16)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[GAC1返回数据tag9F10 CVN 不正确]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F10中MAC的正确性
        /// </summary>
        public TipLevel TerminalActionAnalyze_007()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (Check9F10Mac())
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F26的值与终端计算的mac一致性
        /// </summary>
        public TipLevel TerminalActionAnalyze_008()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            if (TransactionConfig.GetInstance().CurrentApp == AppType.VISA)
            {
                if (CheckVisaAc())
                {
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
            }
            else if (TransactionConfig.GetInstance().CurrentApp == AppType.MC)
            {
                if (CheckMcAc())
                {
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
            }
            else
            {
                if (CheckPbocAc())
                {
                    return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
                }
            }
            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
        }

        public TipLevel TerminalActionAnalyze_009()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            string tag9F4C = locator.Terminal.GetTag("9F4C");
            string key = transConfig.TransIdnKey;
            if (string.IsNullOrEmpty(key))
            {

                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "IDN key不存在");
            }
            if (string.IsNullOrEmpty(tag9F4C))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "无法获取IC卡动态数据，请确认DDA是否成功?");
            }
            string atc = TransactionTag.GetInstance().GetTag(TransactionStep.TerminalActionAnalyze, "9F36");
            if (string.IsNullOrEmpty(atc))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "无法获取 ATC");
            }
            var calcIccDynamicData = Algorithm.Des3Encrypt(key, atc + "000000000000");
            if (calcIccDynamicData != tag9F4C)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "tag9F4C:{0},计算得到的IC卡动态数据为:{1}", tag9F4C, calcIccDynamicData);
            }
            return TipLevel.Sucess;
        }
    }
}

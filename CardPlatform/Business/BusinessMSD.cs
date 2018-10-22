using System;
using System.Collections.Generic;
using System.Linq;
using CardPlatform.ViewModel;
using CardPlatform.Cases;
using System.Reflection;
using CplusplusDll;
using CardPlatform.Config;
using CardPlatform.Models;
using CardPlatform.Common;


namespace CardPlatform.Business
{
    public class BusinessMSD : BusinessBase
    {
        /// <summary>
        /// 开始UICS交易流程
        /// </summary>
        /// <param name="aid"></param>
        /// <param name="doDesTrans"></param>
        /// <param name="doSMTrans"></param>
        public override void DoTransaction(string aid)
        {
            base.DoTransaction(aid);
            locator.Terminal.SetTag("9F7A", "00", "电子现金支持指示器(这里走借贷记交易流程)");
            locator.Terminal.SetTag("9C", "00", "交易类型");
            locator.Terminal.SetTag("9F66", "46000000", "终端交易属性");
            DoTransaction(DoTransactionEx);
        }

        private bool DoTransactionEx()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            if (!ApplicationSelection(currentAid))
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "选择应用失败，交易流程终止");
                return false;
            }
            var ret = InitiateApplicationProcessing();
            if (!ret)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "GPO命令发送失败，交易流程终止");
                return false;
            }
            string tag94 = transTags.GetTag("94");
            if (!string.IsNullOrEmpty(tag94))
            {
                var afls = DataParse.ParseAFL(tag94);
                if (!ReadApplicationData(afls))
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "读取应用记录失败，交易流程终止");
                    return false;
                }
            }
            GetRequirementData();   //在脱机之前先进行必要数据的获取
            return true;
        }

        /// <summary>
        /// 选择应用,只要发送选择AID命令成功返回0x9000,则表示成功
        /// </summary>
        /// <param name="aid"></param>
        /// <returns></returns>
        public new bool ApplicationSelection(string aid)
        {
            log.TraceLog("执行应用选择流程...");
            bool result = false;
            ApduResponse resp = base.ApplicationSelection(aid);
            if (resp.SW == 0x9000)
            {
                var tlvs = DataParse.ParseTLV(resp.Response);
                businessUtil.SaveTags(TransactionStep.SelectApp, tlvs);
                var selectAppCase = new SelectAppCase();
                selectAppCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.SelectApp, resp);
                result = true;
            }
            else
            {
                var caseNo = MethodBase.GetCurrentMethod().Name;
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "选择应用{0}失败,SW={1}", aid, resp.SW);
            }
            return result;
        }

        /// <summary>
        /// 应用初始化
        /// </summary>
        /// <param name="pdol"></param>
        /// <returns></returns>
        protected bool InitiateApplicationProcessing()
        {
            log.TraceLog("开始执行应用初始化流程检测...");
            var afls = new List<AFL>();
            var caseNo = MethodBase.GetCurrentMethod().Name;

            string tag9F38 = transTags.GetTag(TransactionStep.SelectApp, "9F38");
            var pdolData = businessUtil.GetDolData(tag9F38);
            var resp = base.InitiateApplicationProcessing(pdolData);
            if (resp.SW != 0x9000)
            {
                caseObj.TraceInfo(TipLevel.Failed, caseNo, "GPO命令失败，SW={0}", resp.SW);
                return false;
            }
            var tlvs = DataParse.ParseTLV(resp.Response);
            businessUtil.SaveTags(TransactionStep.GPO, tlvs);

            var gpoCase = new GPOCase();
            gpoCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.GPO, resp);
            return true;
        }

        /// <summary>
        /// 读记录
        /// </summary>
        /// <param name="afls"></param>
        /// <returns></returns>
        protected new bool ReadApplicationData(List<AFL> afls)
        {
            log.TraceLog("开始执行读记录数据流程检测...");
            var caseNo = MethodBase.GetCurrentMethod().Name;

            List<AFL> magAfls = new List<AFL>();
            foreach(var afl in afls)
            {
                if (afl.RecordNo == 1 && afl.SFI == 1)
                    magAfls.Add(afl);
            }
            var resps = base.ReadApplicationData(magAfls);

            foreach (var resp in resps)
            {
                if (resp.SW != 0x9000)
                {
                    caseObj.TraceInfo(TipLevel.Failed, caseNo, "读取应用记录失败,SW={0}", resp.SW);
                    return false;
                }
                var tlvs = DataParse.ParseTLV(resp.Response);
                businessUtil.SaveTags(TransactionStep.ReadRecord, tlvs);
            }

            IExcuteCase readRecordCase = new ReadRecordCase();
            readRecordCase.Excute(BatchNo, transCfg.CurrentApp, TransactionStep.ReadRecord, resps);
            return true;
        }


        public void GetRequirementData()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;

            RequirementData[] tagStandards =
            {
                new RequirementData("C4",3,TipLevel.Failed,""),  //如果执行频度检查，需要此应用货币代码
                new RequirementData("C5",3,TipLevel.Failed,""),  //如果支持发卡行认证，需要ADA应用缺省行为
                new RequirementData("C3",3,TipLevel.Failed,""),  //如果执行国际货币频度检查，需要此连续脱机交易限制数(国际-货币)
                new RequirementData("D1",25,TipLevel.Failed,""),  //如果执行国际国际频度检查，需要此连续脱机交易限制数(国际-国家)
                new RequirementData("D3",18,TipLevel.Failed,""),  //如果支持发卡行认证，需要此发卡行认证指示位
                new RequirementData("D5",6,TipLevel.Failed,""),  //如果支持卡片频度检查，需要此发卡行国家代码
                new RequirementData("D6",2,TipLevel.Failed,""),  //如果执行卡片频度检查，需要此连续脱机交易下限
                new RequirementData("9F17",1,TipLevel.Failed,""),  //如果无法联机，卡片风险管理需要此连续脱机交易上限做出拒绝交易
                new RequirementData("9F14",1,TipLevel.Failed,""),  //累计脱机交易金额上限
                new RequirementData("9F23",1,TipLevel.Failed,""),  //连续脱机交易限制数
                new RequirementData("CA",6,TipLevel.Failed,""),  //累计脱机交易金额(双货币)
                new RequirementData("CB",6,TipLevel.Failed,""),  //第二应用货币代码
                new RequirementData("C7",1,TipLevel.Failed,""),  //交易日志格式
                new RequirementData("C8",2,TipLevel.Failed,""),
                new RequirementData("C9",2,TipLevel.Failed,""),  //应用交易计数器
                new RequirementData("9F4F",26,TipLevel.Failed,""),  //如果卡片或终端执行频度检查，或新卡检查，需要此上次联机应用交易计数器
                new RequirementData("9F7E",48,TipLevel.Failed,""),  //如果支持脱机PIN,需要此PIN尝试计数器
                new RequirementData("C6",1,TipLevel.Failed,""),  //如果支持脱机PIN,需要此PIN尝试计数器
               
            };
            for (int i = 0; i < tagStandards.Length; i++)
            {
                var resp = APDU.GetDataCmd(tagStandards[i].Tag);
                if (resp.SW != 0x9000)
                {
                    caseObj.TraceInfo(tagStandards[i].Level, caseNo, "{0},缺少Tag{1},", tagStandards[i].Desc, tagStandards[i].Tag);
                }
                else
                {
                    var tlvs = DataParse.ParseTLV(resp.Response);
                    var tlv = from tmp in tlvs where tmp.Tag == tagStandards[i].Tag select tmp;

                    if (tagStandards[i].Len != 0)
                    {
                        if (tlv.First().Len != tagStandards[i].Len)
                        {
                            caseObj.TraceInfo(tagStandards[i].Level, caseNo, "tag[{0}]长度不匹配，标准规范为[{1}],实际长度为[{2}]", tagStandards[i].Tag, tagStandards[i].Len, tlv.First().Len);
                        }
                    }
                    transTags.SetTag(TransactionStep.GetData, tlv.First().Tag, tlv.First().Value); //保存
                }
            }
        }
    }
}

using CardPlatform.Config;
using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using UtilLib;
using CardPlatform.Common;
using CardPlatform.Models;

namespace CardPlatform.Cases
{
    public class PSEDirCase : CaseBase
    {
        private ApduResponse response;
        private List<TLV> tlvs;

        public PSEDirCase()
        {
            response = new ApduResponse();
            tlvs = new List<TLV>();
        }

        public override void Excute(int batchNo, AppType app, TransactionStep step, object srcData)
        {
            response = (ApduResponse)srcData;
            tlvs = DataParse.ParseTLV(response.Response);
            base.Excute(batchNo,app,step, srcData);           
            CheckTemplateTag(tlvs);
        }

        /// <summary>
        /// 检测响应数据是否以70开头
        /// </summary>
        public TipLevel PSEDIR_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            if (response.Response.Length < 2 || response.Response.Substring(0, 2) != "70")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            else
            {
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
        }

        /// <summary>
        /// 检测响应数据70模板是否仅包含61模板
        /// </summary>
        public TipLevel PSEDIR_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            foreach(var item in tlvs)
            {
                if(item.Level == 1)
                {
                    if(item.Tag != "61")
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                    }
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 61中是否包含4F（必选包含）4F的长度是否在5～16字节之间
        /// </summary>
        public TipLevel PSEDIR_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            for(int i = 0; i < transConfig.Aids.Count; i++)
            {
                var template61 = CaseUtil.GetSubTags("61", i + 1, tlvs);
                if(!CaseUtil.HasTag("4F",template61))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
                foreach (var item in template61)
                {
                    if (item.Tag == "4F")
                    {
                        if (item.Len < 5 || item.Len > 16)
                        {
                            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        }
                    }
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 50必须包含在61模板内，50的长度是否在1～16字节之间
        /// </summary>
        public TipLevel PSEDIR_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            for(int i = 0; i < transConfig.Aids.Count; i++)
            {
                var template61 = CaseUtil.GetSubTags("61", i + 1, tlvs);
                if (!CaseUtil.HasTag("50", template61))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
                foreach (var item in template61)
                {
                    if (item.Tag == "50")
                    {
                        var value = Utils.BcdToStr(item.Value);
                        if (item.Len > 16 || item.Len == 0 ||
                            !CaseUtil.IsAlphaAndBlank(value))
                        {
                            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        }
                    }
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 61中可包含的其他Tag有 50.9F12、87、73，其他Tag不能出现
        /// </summary>
        public TipLevel PSEDIR_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            List<string> tags = new List<string>() { "4F","50", "9F12", "87", "73" };
            for(int i = 0; i < transConfig.Aids.Count; i++)
            {
                var template61 = CaseUtil.GetSubTags("61", i + 1, tlvs);
                foreach (var item in template61)
                {
                    if (!tags.Contains(item.Tag))
                    {
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[不能包含tag{0}]",item.Tag);
                    }
                }
            }

            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 9F12的长度应在1～16字节之间,BCD码显示
        /// </summary>
        public TipLevel PSEDIR_006()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            for (int i = 0; i < transConfig.Aids.Count; i++)
            {
                var template61 = CaseUtil.GetSubTags("61", tlvs);
                foreach (var item in template61)
                {
                    if (item.Tag == "9F12")
                    {
                        string value = UtilLib.Utils.BcdToStr(item.Value);
                        if (item.Len == 0 ||
                            item.Len > 16 ||
                            !CaseUtil.IsAlphaAndBlank(value))
                        {
                            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        }
                    }
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 87的长度是否为1字节,值的合规性检测,多个应用下,tag87必须存在
        /// </summary>
        public TipLevel PSEDIR_007()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var transCfg = TransactionConfig.GetInstance();
            for (int i = 0; i < transCfg.Aids.Count; i++)
            {
                var template61 = CaseUtil.GetSubTags("61", i + 1, tlvs);
                if (!CaseUtil.HasTag("87", template61))
                {
                    return TraceInfo(caseItem.Level, caseNo, "如果存在多个应用，tag87必须存在");
                }
                foreach (var item in template61)
                {
                    if (item.Tag == "87")
                    {
                        var value = Convert.ToInt16(item.Value, 16);
                        if (item.Len != 1 ||
                            !((value >= 0 && value <= 0xF) ||
                            (value > 0x80 && value <= 0x8F))
                            )
                        {
                            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        }
                    }
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F12存在的情况下，则tag9F11必须存在
        /// </summary>
        public TipLevel PSEDIR_008()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            for(int i = 0; i < transConfig.Aids.Count; i++)
            {
                var template61 = CaseUtil.GetSubTags("61", tlvs);
                foreach (var item in template61)
                {
                    if (item.Tag == "9F12")
                    {
                        string tag9F11 = TransactionTag.GetInstance().GetTag(TransactionStep.SelectPSE, "9F11");
                        if (string.IsNullOrEmpty(tag9F11))
                        {
                            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        }
                    }
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测4F和50的一致性
        /// </summary>
        public TipLevel PSEDIR_009()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            for(int i = 0; i < transConfig.Aids.Count; i++)
            {
                var template61 = CaseUtil.GetSubTags("61", i + 1, tlvs);
                var tag4F = CaseUtil.GetTag("4F", template61);
                var tag50 = CaseUtil.GetTag("50", template61);
                var value = Utils.BcdToStr(tag50);
                if(tag4F == "A000000333010101")
                {
                    if (tag50 != "556E696F6E506179204465626974")
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag50={0}]", value);
                }else if(tag4F == "A000000333010102")
                {
                    if (tag50 != "556E696F6E50617920437265646974")
                        return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag50={0}]", value);
                }else if(tag4F == "A0000000031010")
                {
                    //if(value.Contains("Visa"))
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// PSE DIR tag重复性检测
        /// </summary>
        public TipLevel PSEDIR_010()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var transCfg = TransactionConfig.GetInstance();
            for (int i = 0; i < transCfg.Aids.Count; i++)
            {
                var template61 = CaseUtil.GetSubTags("61", i + 1, tlvs);
                if (CaseUtil.HasDuplexTag(template61))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            int count = 0;
            foreach(var tlv in tlvs)
            {
                if(tlv.Tag == "70")
                {
                    count++;
                }
            }
            if(count > 1)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[包含多个70模板]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测模板73是否只包含9F0A
        /// </summary>
        /// <returns></returns>
        public TipLevel PSEDIR_011()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            List<string> tags = new List<string>() { "9F0A"};
            for (int i = 0; i < transConfig.Aids.Count; i++)
            {
                if(CaseUtil.HasTag("73",tlvs))
                {
                    var template73 = CaseUtil.GetSubTags("73", i + 1, tlvs);
                    foreach (var item in template73)
                    {
                        if (!tags.Contains(item.Tag))
                        {
                            return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                        }
                    }
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测DIR文件中的AID是否都能选择成功
        /// </summary>
        /// <returns></returns>
        public TipLevel PSEDIR_012()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);
            foreach(var aid in transConfig.Aids)
            {
                var resp = APDU.SelectCmd(aid);
                if(resp.SW != 0x9000)
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
    }
}

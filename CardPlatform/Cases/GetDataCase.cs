using CardPlatform.Common;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Config;
using CplusplusDll;

namespace CardPlatform.Cases
{
    public class GetDataCase : CaseBase
    {

        /// <summary>
        /// 取数据
        /// </summary>
        /// <param name="tag"></param>
        /// <returns></returns>
        public string GetData(string tag)
        {
            var resp = APDU.GetDataCmd(tag);
            if(resp.SW != 0x9000)
            {
                return string.Empty;
            }
            if(!DataParse.IsTLV(resp.Response))
            {
                TraceInfo(TipLevel.Failed, "GetData", "取数据tag{0},响应数据TLV分解有误resp={1}", tag, resp.Response);
                return string.Empty;
            }
            var tlvs = DataParse.ParseTLV(resp.Response);
            if(tlvs.Count != 1 || tlvs[0].Tag != tag)
            {
                return string.Empty;
            }
            TransactionTag.GetInstance().SetTag(TransactionStep.GetData, tag, tlvs[0].Value);
            return tlvs[0].Value;
        }

        public List<TLV> GetDatas(string tag)
        {
            var resp = APDU.GetDataCmd(tag);
            if (resp.SW != 0x9000)
            {
                return new List<TLV>();
            }
            if (!DataParse.IsTLV(resp.Response))
            {
                TraceInfo(TipLevel.Failed, "GetData", "取数据tag{0},响应数据TLV分解有误resp={1}", tag, resp.Response);
                return new List<TLV>();
            }
            var tlvs = DataParse.ParseTLV(resp.Response);
            TransactionTag.GetInstance().SetTags(TransactionStep.GetData, tlvs);
            return tlvs;
        }

        public TipLevel GetData_9F36()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F36 = GetData("9F36");
            if(string.IsNullOrEmpty(tag9F36))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F13()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F13 = GetData("9F13");
            if (string.IsNullOrEmpty(tag9F13))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F17()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F17 = GetData("9F17");
            if (string.IsNullOrEmpty(tag9F17))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }



        public TipLevel GetData_9F52()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F52 = GetData("9F52");
            if (string.IsNullOrEmpty(tag9F52))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F53()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F53 = GetData("9F53");
            if (string.IsNullOrEmpty(tag9F53))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F54()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F54 = GetData("9F54");
            if (string.IsNullOrEmpty(tag9F54))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F51()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F51 = GetData("9F51");
            if (string.IsNullOrEmpty(tag9F51))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F56()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F56 = GetData("9F56");
            if (string.IsNullOrEmpty(tag9F56))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F57()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F57 = GetData("9F57");
            if (string.IsNullOrEmpty(tag9F57))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F58()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F58 = GetData("9F58");
            if (string.IsNullOrEmpty(tag9F58))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F59()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F59 = GetData("9F59");
            if (string.IsNullOrEmpty(tag9F59))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F5C()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F5C = GetData("9F5C");
            if (string.IsNullOrEmpty(tag9F5C))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F5D()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F5D = GetData("9F5D");
            if (string.IsNullOrEmpty(tag9F5D))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F5E()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F5E = GetData("9F5E");
            if (string.IsNullOrEmpty(tag9F5E))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F72()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F72 = GetData("9F72");
            if (string.IsNullOrEmpty(tag9F72))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F73()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F73 = GetData("9F73");
            if (string.IsNullOrEmpty(tag9F73))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F75()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F75 = GetData("9F75");
            if (string.IsNullOrEmpty(tag9F75))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F76()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F76 = GetData("9F76");
            if (string.IsNullOrEmpty(tag9F76))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F77()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F77 = GetData("9F77");
            if (string.IsNullOrEmpty(tag9F77))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F78()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F78 = GetData("9F78");
            if (string.IsNullOrEmpty(tag9F78))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F79()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F79 = GetData("9F79");
            if (string.IsNullOrEmpty(tag9F79))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            if(tag9F79 != "000000000000")
            {
                return TraceInfo(TipLevel.Warn, caseNo, "卡片余额不为0,tag9F79为{0}",tag9F79);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F4F()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F4F = GetData("9F4F");
            if (string.IsNullOrEmpty(tag9F4F))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F68()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F68 = GetData("9F68");
            if (string.IsNullOrEmpty(tag9F68))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F6B()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F6B = GetData("9F6B");
            if (string.IsNullOrEmpty(tag9F6B))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F6C()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F6C = GetData("9F6C");
            if (string.IsNullOrEmpty(tag9F6C))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F6D()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F6D = GetData("9F6D");
            if (string.IsNullOrEmpty(tag9F6D))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F6E()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F6E = GetData("9F6E");
            if (string.IsNullOrEmpty(tag9F6E))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_9F7C()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F7C = GetData("9F7C");
            if (string.IsNullOrEmpty(tag9F7C))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_BF55()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tlvs = GetDatas("BF55");
            if (tlvs.Count == 0)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_BF56()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tlvs = GetDatas("BF56");
            if (tlvs.Count == 0)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_BF57()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tlvs = GetDatas("BF57");
            if (tlvs.Count == 0)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_BF58()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tlvs = GetDatas("BF58");
            if (tlvs.Count == 0)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        public TipLevel GetData_BF5B()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tlvs = GetDatas("BF5B");
            if (tlvs.Count == 0)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[无法通过取命令获取数据]");
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }


        /// <summary>
        /// 检测Tag9F79小于等于tag9F77, tag9F78小于等于tag9F77
        /// </summary>
        public TipLevel GetData_001()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F79 = GetData("9F79");
            var tag9F78 = GetData("9F78");
            var tag9F77 = GetData("9F77");
            if(!string.IsNullOrEmpty(tag9F77) ||
                !string.IsNullOrEmpty(tag9F78) ||
                !string.IsNullOrEmpty(tag9F79))
            {
                return TipLevel.Sucess;
            }
            int nTag9F79 = Convert.ToInt32(tag9F79);
            int nTag9F78 = Convert.ToInt32(tag9F78);
            int nTag9F77 = Convert.ToInt32(tag9F77);

            if(nTag9F79 > nTag9F77 || nTag9F78 > nTag9F77)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F6D设置的合理性，数值不宜过大，一般默认为0
        /// </summary>
        public TipLevel GetData_002()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F6D = TransactionTag.GetInstance().GetTag(TransactionStep.GetData, "9F6D");
            if(string.IsNullOrEmpty(tag9F6D))
            {
                return TraceInfo(caseItem.Level, caseNo, "无法通过GetData命令获取tag9F6D");
            }
            if(tag9F6D != "000000000000")
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F68卡片附加处理选项的规范性
        /// </summary>
        public TipLevel GetData_003()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F68 = TransactionTag.GetInstance().GetTag(TransactionStep.GetData, "9F68");
            if (string.IsNullOrEmpty(tag9F68))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag5F28与9F57发卡行国家代码的一致性
        /// </summary>
        public TipLevel GetData_004()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag5F28 = TransactionTag.GetInstance().GetTag("5F28");
            var tag9F57 = TransactionTag.GetInstance().GetTag(TransactionStep.GetData, "9F57");
            if (string.IsNullOrEmpty(tag9F57))
            {
                return TraceInfo(TipLevel.Tip, caseNo, caseItem.Description + "卡片缺少tag9F57，如果支持卡片频度检测，该数据必须存在");
            }

            if (tag5F28 != tag9F57)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "tag5F28值为:{0},tag9F57值为:{1}",tag5F28,tag9F57);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测Tag9F42与9F51应用货币代码的一致性
        /// </summary>
        public TipLevel GetData_005()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F42 = TransactionTag.GetInstance().GetTag("9F42");
            var tag9F51 = TransactionTag.GetInstance().GetTag(TransactionStep.GetData, "9F51");
            if (string.IsNullOrEmpty(tag9F42))
            {
                var tag8E = TransactionTag.GetInstance().GetTag(TransactionStep.ReadRecord, "8E");
                if (!string.IsNullOrEmpty(tag8E) && tag8E.Substring(0,16) != "0000000000000000")
                    return TraceInfo(TipLevel.Warn, caseNo, "读数据中缺少tag9F42,如果8E中X,Y金额不为0，该数据必须存在");
                return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
            }
            
            if (string.IsNullOrEmpty(tag9F51))
            {
                return TraceInfo(caseItem.Level, caseNo, "卡片缺少tag9F51,如果只选频度检测，该数据必须存在");
            }
            caseItem.Description += "[tag9F42=" + tag9F42 + "]";
            caseItem.Description += "[tag9F51=" + tag9F51 + "]";
            if (tag9F42 != tag9F51)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F54存在或卡片支持国际频度检测时，tag9F51必须存在
        /// </summary>
        /// <returns></returns>
        public TipLevel GetData_006()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F54 = TransactionTag.GetInstance().GetTag(TransactionStep.GetData, "9F54");
            var tag9F51 = TransactionTag.GetInstance().GetTag(TransactionStep.GetData, "9F51");
            if(!string.IsNullOrEmpty(tag9F54))
            {
                if(string.IsNullOrEmpty(tag9F51))
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description);
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }

        /// <summary>
        /// 检测tag9F5A，卡片所属区域，货币代码、国家代码与tag9F42,tag5F28的一致性
        /// </summary>
        /// <returns></returns>
        public TipLevel GetData_007()
        {
            var caseNo = MethodBase.GetCurrentMethod().Name;
            var caseItem = GetCaseItem(caseNo);

            var tag9F51 = TransactionTag.GetInstance().GetTag("9F51");
            var tag9F54 = TransactionTag.GetInstance().GetTag("9F54");
            var tag5F28 = TransactionTag.GetInstance().GetTag("5F28");
            var tag9F57 = TransactionTag.GetInstance().GetTag("9F57");
            var tag9F5A = TransactionTag.GetInstance().GetTag("9F5A");
            if(string.IsNullOrEmpty(tag9F5A))
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[数据缺失tag9F5A]");
            }
            if(tag9F5A.Length != 10)
            {
                return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[tag9F5A长度有误]");
            }
            if(tag9F5A.Substring(0,2) != "40")
            {
                return TraceInfo(caseItem.Level, caseNo, "亚洲区域，tag9F5A第一个字节必须为40");
            }
            if(!string.IsNullOrEmpty("9F51"))
            {
                if(tag9F5A.Substring(2,4) != tag9F51)
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[货币代码与tag9F51不匹配]");
                }
            }else if(!string.IsNullOrEmpty("9F54"))
            {
                if (tag9F5A.Substring(2, 4) != tag9F54)
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[货币代码与tag9F54不匹配]");
                }
            }

            if (!string.IsNullOrEmpty("5F28"))
            {
                if (tag9F5A.Substring(6, 4) != tag5F28)
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[国家代码与tag5F28不匹配]");
                }
            }
            else if (!string.IsNullOrEmpty("9F57"))
            {
                if (tag9F5A.Substring(6, 4) != tag9F57)
                {
                    return TraceInfo(caseItem.Level, caseNo, caseItem.Description + "[国家代码与tag9F57不匹配]");
                }
            }
            return TraceInfo(TipLevel.Sucess, caseNo, caseItem.Description);
        }
    }
}

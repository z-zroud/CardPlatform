using CplusplusDll;
using System;
using System.Collections.Generic;

namespace CardPlatform.Common
{


    /// <summary>
    /// 存储交易流程中卡片返回的所有tag
    /// </summary>
    public class TransactionTag
    {
        private static TransactionTag TransTagsObj;
        private Dictionary<TransactionStep, Dictionary<string,string>> transTags;

        private TransactionTag()
        {
            transTags = new Dictionary<TransactionStep, Dictionary<string,string>>();
        }

        public static TransactionTag GetInstance()
        {
            if(TransTagsObj == null)
            {
                TransTagsObj = new TransactionTag();
            }
            return TransTagsObj;
        }

        /// <summary>
        /// 清空tag集合
        /// </summary>
        public void Clear()
        {
            transTags.Clear();
        }

        public void ClearExceptPSEandPPSE()
        {
            foreach(var item in transTags)
            {
                if(item.Key != TransactionStep.SelectPSE &&
                    item.Key != TransactionStep.ReadPSEDir &&
                    item.Key != TransactionStep.SelectPPSE)
                {
                    item.Value.Clear();
                }
            }
        }

        /// <summary>
        /// 保存TLV格式的list集合
        /// </summary>
        /// <param name="arrTLV"></param>
        public void SetTags(TransactionStep step, List<TLV> arrTLV)
        {
            foreach(var item in arrTLV)
            {
                if(!item.IsTemplate)
                {
                    if(transTags.ContainsKey(step))
                    {
                        if (transTags[step].ContainsKey(item.Tag))
                            transTags[step][item.Tag] = item.Value;
                        else
                            transTags[step].Add(item.Tag, item.Value);
                    }
                    else
                    {
                        var tagDict = new Dictionary<string, string>();
                        tagDict.Add(item.Tag, item.Value);
                        transTags.Add(step, tagDict);
                    }                    
                }
            }
        }

        /// <summary>
        /// 保存一个tag值
        /// </summary>
        /// <param name="tag"></param>
        /// <param name="value"></param>
        public void SetTag(TransactionStep step, string tag,string value)
        {
            if (transTags.ContainsKey(step))
            {
                if (transTags[step].ContainsKey(tag))
                    transTags[step][tag] = value;
                else
                    transTags[step].Add(tag, value);
            }
            else
            {
                var tagDict = new Dictionary<string, string>();
                tagDict.Add(tag, value);
                transTags.Add(step, tagDict);
            }
        }

        /// <summary>
        /// 获取指定的tag值
        /// </summary>
        /// <param name="tag"></param>
        /// <returns></returns>
        public string GetTag(TransactionStep step, string tag)
        {
            string value = string.Empty;
            if(transTags.ContainsKey(step))
            {
                transTags[step].TryGetValue(tag, out value);
            }

            return value;
        }

        public string GetTag(string tag)
        {
            foreach(var dict in transTags)
            {
                foreach(var transTag in dict.Value)
                {
                    if (transTag.Key == tag)
                        return transTag.Value;
                }
            }
            return string.Empty;
        }
    }
}

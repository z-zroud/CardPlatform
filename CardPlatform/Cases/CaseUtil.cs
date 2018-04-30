using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;

namespace CardPlatform.Cases
{
    public static class CaseUtil
    {
        /// <summary>
        /// 比较数据是否以startBuffer开头
        /// </summary>
        /// <param name="buffer"></param>
        /// <param name="startBuffer"></param>
        /// <returns></returns>
        public static bool RespStartWith(string buffer,string startBuffer)
        {
            if(buffer.Length < startBuffer.Length)
            {
                return false;
            }
            if(buffer.StartsWith(startBuffer))
            {
                return true;
            }
            return false;
        }

        /// <summary>
        /// 判断buffer中是否包含指定tag集合
        /// </summary>
        /// <param name="buffer"></param>
        /// <param name="tags"></param>
        /// <returns></returns>
        public static bool RespContainsTag(string buffer, params string[] tags)
        {
            if(DataParse.IsTLV(buffer))
            {
                var tlvItems = DataParse.ParseTLV(buffer);
                foreach(var tag in tags)
                {
                    var item = from tlv in tlvItems where tlv.Tag == tag select tlv;
                    if(item == null || item.Count() != 1)
                    {
                        return false;
                    }
                }
                return true;
            }
            return false;
        }


        /// <summary>
        /// 数据中是否包含除containTags以外的其他tag
        /// </summary>
        /// <param name="buffer"></param>
        /// <param name="containTags"></param>
        /// <param name="otherTags"></param>
        /// <returns></returns>
        public static bool RespHasOtherTag(string buffer, List<string> containTags, out List<string> otherTags)
        {
            bool result = false;
            otherTags = new List<string>();
            if (DataParse.IsTLV(buffer))
            {
                var tlvItems = DataParse.ParseTLV(buffer);               
                foreach (var tlv in tlvItems)
                {
                    if(!containTags.Contains(tlv.Tag))
                    {
                        otherTags.Add(tlv.Tag);
                        result = true;
                    }
                }                
            }
            return result;
        }

        /// <summary>
        /// tag长度是否符合期望长度
        /// </summary>
        /// <param name="tag"></param>
        /// <param name="min"></param>
        /// <param name="max"></param>
        /// <returns></returns>
        public static bool IsExpectedLen(string tag, int min, int max)
        {
            if(tag.Length >= min && tag.Length < max)
            {
                return true;
            }
            return false;
        }

        /// <summary>
        /// 是否包含tag为空值
        /// </summary>
        /// <param name="buffer"></param>
        /// <param name="tag"></param>
        /// <returns></returns>
        public static bool HasEmptyTag(string buffer, out List<string> tag)
        {
            bool result = false;
            tag = new List<string>();
            var tlvsItem = DataParse.ParseTLV(buffer);
            foreach(var tlv in tlvsItem)
            {
                if(tlv.Len == 0)
                {
                    tag.Add(tlv.Tag);
                    result = true;
                }
            }

            return result;
        }

        /// <summary>
        /// 判断buffer中的tag是否有重复的出现
        /// </summary>
        /// <param name="buffer"></param>
        /// <param name="tags"></param>
        /// <returns></returns>
        public static bool IsUniqTag(string buffer, out List<string> tags)
        {
            tags = new List<string>();
            bool result = false;
            var tlvItems = DataParse.ParseTLV(buffer);
            Dictionary<string, TLV> tagsDict = new Dictionary<string, TLV>();
            foreach (var item in tlvItems)
            {
                if (tagsDict.ContainsKey(item.Tag))
                {
                    tags.Add(item.Tag);
                    result = true;
                }
                else
                {
                    tagsDict.Add(item.Tag, item);
                }
            }
            return result;
        } 

        /// 判断字符串是否为数字字母组合
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        public static bool IsAlpha(string str)
        {
            string alphaNum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
            foreach (var c in str)
            {
                if (!alphaNum.Contains(c))
                    return false;
            }

            return true;
        }
    }
}

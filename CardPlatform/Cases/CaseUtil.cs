using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;

namespace CardPlatform.Cases
{
    public class OrderedTag
    {
        public string tag { get; set; }
        public int level { get; set; }
    }

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

        public static bool RespConstainsTagByOrder(string buffer, params OrderedTag[] tags)
        {
            //if (DataParse.IsTLV(buffer))
            //{
            //    var tlvItems = DataParse.ParseTLV(buffer);
            //    var item = from tlv in tlvItems where tlv.Level == tag.level select tlv;
            //    foreach (var tag in tags)
            //    {
                    
                    
            //    }
            //    return true;
            //}
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
            var ascii = UtilLib.Utils.BcdToStr(str);
            string alphaNum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ";
            foreach (var c in ascii)
            {
                if (!alphaNum.Contains(c))
                    return false;
            }

            return true;
        }

        /// <summary>
        /// 判断是否为数字
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        public static bool IsDigit(string str)
        {
            string num = "0123456789";
            foreach (var c in num)
            {
                if (!num.Contains(c))
                    return false;
            }

            return true;
        }

        /// <summary>
        /// 判断tag57格式是否正确
        /// </summary>
        /// <param name="tag57"></param>
        /// <returns></returns>
        public static bool IsCorrectTag57Format(string tag57)
        {
            string format1 = "0123456789D";
            string format2 = "0123456789DF";
            string format3 = "0123456789";
            int len = tag57.Length;
            if(len % 2 != 0)
            {
                if(tag57[len - 1] != 'F')
                {
                    return false;   //奇数位需补F
                }
                foreach(var c in tag57)
                {
                    if(!format2.Contains(c))
                    {
                        return false;   //数字必须是format2集合里面
                    }
                }
            }
            else
            {
                foreach (var c in tag57)
                {
                    if (!format1.Contains(c))
                    {
                        return false;   //数字必须是format1集合里面
                    }
                }
            }

            int indexD = tag57.IndexOf('D');
            if(indexD < 17 || indexD > 20)
            {
                return false;   //字母D需在第17-20字节之间
            }
            string account = tag57.Substring(0, indexD);
            foreach(var c in account)
            {
                if(!format3.Contains(c))
                {
                    return false;   //账号必须是数字
                }
            }
            string MM = tag57.Substring(indexD + 2, 2);
            int month = int.Parse(MM);
            if(month > 12 || month < 1)
            {
                return false;   //失效月份检查
            }
            if(tag57[indexD + 4] != '2' || tag57[indexD + 4] != '6')
            {
                return false;   //服务码必须以2或6开头
            }

            return true;
        }
    }
}

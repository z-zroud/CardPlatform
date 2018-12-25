using CardPlatform.Common;
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

    public enum LenType
    {
        Fixed,
        Range
    }

    public class TagLenAttribute
    {
        public TagLenAttribute(LenType type,int min, int max, int fixedLen)
        {
            this.type = type;
            this.max = max;
            this.min = min;
            this.fixedLen = fixedLen;
        }
        public LenType type { get; set; }
        public int min { get; set; }
        public int max { get; set; }
        public int fixedLen { get; set; }
    }

    public class TagLenInfo
    {
        public static Dictionary<string, TagLenAttribute> tagLenInfoDict = new Dictionary<string, TagLenAttribute>();
        static TagLenInfo()
        {
            tagLenInfoDict.Add("4F", new TagLenAttribute(LenType.Range, 5, 16, 0));
            tagLenInfoDict.Add("50", new TagLenAttribute(LenType.Range, 1, 16, 0));
            tagLenInfoDict.Add("57", new TagLenAttribute(LenType.Range, 1, 19, 0));
            tagLenInfoDict.Add("5A", new TagLenAttribute(LenType.Range, 8, 10, 0));
            tagLenInfoDict.Add("82", new TagLenAttribute(LenType.Fixed, 0, 0, 2));
            tagLenInfoDict.Add("84", new TagLenAttribute(LenType.Range, 5, 16, 0));
            tagLenInfoDict.Add("87", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("88", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("8C", new TagLenAttribute(LenType.Range, 0, 252, 0));
            tagLenInfoDict.Add("8D", new TagLenAttribute(LenType.Range, 0, 252, 0));
            tagLenInfoDict.Add("8E", new TagLenAttribute(LenType.Range, 0, 252, 0));
            tagLenInfoDict.Add("8F", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("90", new TagLenAttribute(LenType.Range, 1, 2048, 0));
            tagLenInfoDict.Add("92", new TagLenAttribute(LenType.Range, 1, 2048, 0));
            tagLenInfoDict.Add("93", new TagLenAttribute(LenType.Range, 1, 1024, 0));
            tagLenInfoDict.Add("94", new TagLenAttribute(LenType.Range, 4, 32, 0));
            tagLenInfoDict.Add("5F20", new TagLenAttribute(LenType.Range, 2, 26, 0));
            tagLenInfoDict.Add("5F24", new TagLenAttribute(LenType.Fixed, 0, 0, 3));
            tagLenInfoDict.Add("5F25", new TagLenAttribute(LenType.Fixed, 0, 0, 3));
            tagLenInfoDict.Add("5F28", new TagLenAttribute(LenType.Fixed, 0, 0, 2));
            tagLenInfoDict.Add("5F2D", new TagLenAttribute(LenType.Range, 2, 8, 0));
            tagLenInfoDict.Add("5F30", new TagLenAttribute(LenType.Fixed, 0, 0, 2));
            tagLenInfoDict.Add("5F34", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("5F55", new TagLenAttribute(LenType.Fixed, 0, 0, 2));
            tagLenInfoDict.Add("9F07", new TagLenAttribute(LenType.Fixed, 0, 0, 2));
            tagLenInfoDict.Add("9F08", new TagLenAttribute(LenType.Fixed, 0, 0, 2));
            tagLenInfoDict.Add("9F0D", new TagLenAttribute(LenType.Fixed, 0, 0, 5));
            tagLenInfoDict.Add("9F0E", new TagLenAttribute(LenType.Fixed, 0, 0, 5));
            tagLenInfoDict.Add("9F0F", new TagLenAttribute(LenType.Fixed, 0, 0, 5));
            tagLenInfoDict.Add("9F10", new TagLenAttribute(LenType.Range, 7, 32, 0));
            tagLenInfoDict.Add("9F11", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("9F12", new TagLenAttribute(LenType.Range, 1, 16, 0));
            tagLenInfoDict.Add("9F13", new TagLenAttribute(LenType.Fixed, 0, 0, 2));
            tagLenInfoDict.Add("9F14", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("9F23", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("9F1F", new TagLenAttribute(LenType.Range, 0, 100, 0));
            tagLenInfoDict.Add("9F26", new TagLenAttribute(LenType.Fixed,0,0,8));
            tagLenInfoDict.Add("9F27", new TagLenAttribute(LenType.Fixed,0,0,1));
            tagLenInfoDict.Add("9F32", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("9F36", new TagLenAttribute(LenType.Fixed, 0, 0, 2));
            tagLenInfoDict.Add("9F38", new TagLenAttribute(LenType.Range, 1, 100, 0));
            tagLenInfoDict.Add("9F42", new TagLenAttribute(LenType.Fixed, 0, 0, 2));
            tagLenInfoDict.Add("9F44", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("9F46", new TagLenAttribute(LenType.Range, 1, 2048, 0));
            tagLenInfoDict.Add("9F47", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("9F48", new TagLenAttribute(LenType.Range, 1, 2048, 0));
            tagLenInfoDict.Add("9F49", new TagLenAttribute(LenType.Fixed, 0, 0, 3));
            tagLenInfoDict.Add("9F4A", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("9F4B", new TagLenAttribute(LenType.Range, 1, 2048, 0));
            tagLenInfoDict.Add("9F4D", new TagLenAttribute(LenType.Fixed, 0, 0, 2));
            tagLenInfoDict.Add("9F51", new TagLenAttribute(LenType.Fixed, 0, 0, 2));
            tagLenInfoDict.Add("9F52", new TagLenAttribute(LenType.Fixed, 0, 0, 6));
            tagLenInfoDict.Add("9F53", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("9F54", new TagLenAttribute(LenType.Fixed, 0, 0, 6));
            tagLenInfoDict.Add("9F56", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("9F57", new TagLenAttribute(LenType.Fixed, 0, 0, 2));
            tagLenInfoDict.Add("9F58", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("9F59", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("9F5A", new TagLenAttribute(LenType.Fixed, 0, 0, 5));
            tagLenInfoDict.Add("9F5C", new TagLenAttribute(LenType.Fixed, 0, 0, 6));
            tagLenInfoDict.Add("9F5E", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("9F63", new TagLenAttribute(LenType.Fixed, 0, 0, 16));
            tagLenInfoDict.Add("9F68", new TagLenAttribute(LenType.Fixed, 0, 0, 4));
            tagLenInfoDict.Add("9F69", new TagLenAttribute(LenType.Fixed, 0, 0, 7));
            tagLenInfoDict.Add("9F6C", new TagLenAttribute(LenType.Fixed,0,0,2));
            tagLenInfoDict.Add("9F6E", new TagLenAttribute(LenType.Fixed,0,0,4));
            tagLenInfoDict.Add("9F72", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("9F73", new TagLenAttribute(LenType.Fixed, 0, 0, 6));
            tagLenInfoDict.Add("9F77", new TagLenAttribute(LenType.Fixed, 0, 0, 6));
            tagLenInfoDict.Add("9F78", new TagLenAttribute(LenType.Fixed, 0, 0, 6));
            tagLenInfoDict.Add("9F79", new TagLenAttribute(LenType.Fixed, 0, 0, 6));
            tagLenInfoDict.Add("9F7C", new TagLenAttribute(LenType.Range,0,32,0));
            tagLenInfoDict.Add("DF01", new TagLenAttribute(LenType.Fixed, 0, 0, 2));
            tagLenInfoDict.Add("DF11", new TagLenAttribute(LenType.Fixed, 0, 0, 6));
            tagLenInfoDict.Add("DF21", new TagLenAttribute(LenType.Fixed, 0, 0, 6));
            tagLenInfoDict.Add("DF31", new TagLenAttribute(LenType.Fixed, 0, 0, 6));
            tagLenInfoDict.Add("DF41", new TagLenAttribute(LenType.Fixed, 0, 0, 6));
            tagLenInfoDict.Add("DF4D", new TagLenAttribute(LenType.Fixed, 0, 0, 2));
            tagLenInfoDict.Add("DF51", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("DF61", new TagLenAttribute(LenType.Fixed, 0, 0, 1));
            tagLenInfoDict.Add("DF71", new TagLenAttribute(LenType.Fixed, 0, 0, 6));
        }
        public static TagLenAttribute GetLenInfo(string tag)
        {
            if (tagLenInfoDict.ContainsKey(tag))
                return tagLenInfoDict[tag];
            return null;
        }
        public static bool CheckTagLen(string tag,string value)
        {
            if(tagLenInfoDict.ContainsKey(tag))
            {
                var tagLenAttr = tagLenInfoDict[tag];
                var tagLen = value.Length / 2;
                if(tagLenAttr.type == LenType.Fixed)
                {
                    if (tagLen == tagLenAttr.fixedLen)
                        return true;
                }
                else
                {
                    if (tagLen >= tagLenAttr.min && tagLen <= tagLenAttr.max)
                        return true;
                }
            }
            return false;
        }
    }

    public static class CaseUtil
    {
        private static Log log = Log.CreateLog(Constant.LogPath);

        public static int GetDuplexTemplate(string parentTag, string template, List<TLV> tags)
        {
            int level = 0;
            bool start = false; //用于什么时候保存tag
            var subTags = new List<TLV>();
            int count = 0;
            foreach (var item in tags)
            {
                if (item.Tag == parentTag)
                {
                    level = item.Level; //level保存父模板的层次，则子tag为父模板的下一级，直到遍历结束或者遇到和父模板层级一样的模板
                    start = true;
                    continue;
                }
                if (start && 
                    item.Level == level + 1 && 
                    item.IsTemplate && 
                    item.Tag == template)
                {
                    subTags.Add(item);
                    count++;
                    continue;
                }
                if (start && item.Level == level)
                {
                    start = false;
                }
            }
            return count;
        }
        public static List<TLV> GetSubTags(string parentTag, int count, List<TLV> tags)
        {
            int level = 0;
            bool start = false; //用于什么时候保存tag
            var subTags = new List<TLV>();
            int whichOne = 0;
            foreach (var item in tags)
            {
                if (item.Tag == parentTag)
                {
                    level = item.Level; //level保存父模板的层次，则子tag为父模板的下一级，直到遍历结束或者遇到和父模板层级一样的模板
                    start = true;
                    whichOne++;
                    continue;
                }
                if (start && 
                    item.Level == level + 1 && 
                    whichOne == count)
                {
                    subTags.Add(item);
                    continue;
                }
                if (start && item.Level == level)
                {
                    start = false;
                }
            }
            return subTags;
        }
        /// <summary>
        /// 此函数只查找第一个父节点，并收集该父节点的子节点
        /// 若存在多个相同的父节点，只返回第一个父节点的子节点
        /// </summary>
        /// <param name="parentTag"></param>
        /// <param name="tags"></param>
        /// <returns></returns>
        public static List<TLV> GetSubTags(string parentTag,List<TLV> tags)
        {
            int level = 0;
            bool start = false; //用于什么时候保存tag
            var subTags = new List<TLV>();
            foreach (var item in tags)
            {
                if (item.Tag == parentTag)
                {
                    level = item.Level; //level保存父模板的层次，则子tag为父模板的下一级，直到遍历结束或者遇到和父模板层级一样的模板
                    start = true;
                    continue;
                }
                if (start && item.Level == level + 1)
                {
                    subTags.Add(item);
                    continue;
                }
                if(start && item.Level == level)
                {
                    start = false;
                    break;
                }
            }
            return subTags;
        }

        /// <summary>
        /// 从tags集合中获取指定的tag
        /// </summary>
        /// <param name="tag"></param>
        /// <param name="tags"></param>
        /// <returns></returns>
        public static string GetTag(string tag, List<TLV> tags)
        {
            foreach(var item in tags)
            {
                if (item.Tag == tag)
                    return item.Value;
            }
            return string.Empty;
        }

        
        public static bool HasTag(string tag,List<TLV> tags)
        {
            foreach (var item in tags)
            {
                if (item.Tag == tag)
                    return true;
            }
            return false;
        }

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
            if(tag.Length >= min && tag.Length <= max)
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
            bool result = true;
            var tlvItems = DataParse.ParseTLV(buffer);
            Dictionary<string, TLV> tagsDict = new Dictionary<string, TLV>();
            foreach (var item in tlvItems)
            {
                if (tagsDict.ContainsKey(item.Tag))
                {
                    tags.Add(item.Tag);
                    result = false;
                }
                else
                {
                    tagsDict.Add(item.Tag, item);
                }
            }
            return result;
        } 

        /// <summary>
        /// 判断某个模板下是否有tag重复
        /// </summary>
        /// <param name="parentTemplate"></param>
        /// <param name="tags"></param>
        /// <returns></returns>
        public static bool HasDuplexTag(string parentTemplate, List<TLV> tags)
        {
            var subTags = GetSubTags(parentTemplate, tags);
            bool result = false;
            Dictionary<string, TLV> tagsDict = new Dictionary<string, TLV>();
            foreach (var item in subTags)
            {
                if (tagsDict.ContainsKey(item.Tag))
                {
                    result = true;
                    break;
                }
                else
                {
                    tagsDict.Add(item.Tag, item);
                }
            }
            return result;
        }

        public static bool HasDuplexTag(List<TLV> tags)
        {
            bool result = false;
            Dictionary<string, TLV> tagsDict = new Dictionary<string, TLV>();
            foreach (var item in tags)
            {
                if (tagsDict.ContainsKey(item.Tag))
                {
                    result = true;
                    break;
                }
                else
                {
                    tagsDict.Add(item.Tag, item);
                }
            }
            return result;
        }

        /// <summary>
        /// 检查srcTags中是否包含重复的dstTags,若存在，保存到返回的列表中
        /// </summary>
        /// <param name="srcTags"></param>
        /// <param name="dstTags"></param>
        /// <returns></returns>
        public static List<string> HasDuplexTag(List<TLV> srcTags, List<string> dstTags)
        {
            var results = new List<string>();
            foreach(var dstTag in dstTags)
            {
                int count = 0;
                foreach(var srcTag in srcTags)
                {
                    if (srcTag.Tag == dstTag)
                        count++;
                }
                if (count > 1)
                    results.Add(dstTag);
            }
            return results;
        }

        /// 判断字符串是否为数字字母组合
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        public static bool IsAlpha(string ascii)
        {
            //var ascii = UtilLib.Utils.BcdToStr(str);
            string alphaNum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
            foreach (var c in ascii)
            {
                if (!alphaNum.Contains(c))
                    return false;
            }

            return true;
        }

        public static bool IsAlphaAndBlank(string ascii)
        {
            string alphaNum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ";
            foreach (var c in ascii)
            {
                if (!alphaNum.Contains(c))
                    return false;
            }

            return true;
        }

        public static bool IsContainSlash(string ascii)
        {
            string alphaNum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 /";
            foreach (var c in ascii)
            {
                if (!alphaNum.Contains(c))
                    return false;
            }
            if(!ascii.Contains("/"))
            {
                return false;
            }

            return true;
        }

        public static bool IsLowerCaseAlpha(string ascii)
        {
            string alphaNum = "abcdefghijklmnopqrstuvwxyz";
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
        /// 判断是否为数字字母
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        public static bool IsAplhaDigit(string str)
        {
            string alphaNum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ";
            foreach (var c in str)
            {
                if (!alphaNum.Contains(c))
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
            string format2 = "0123456789DF";
            string format3 = "0123456789";
            

            int indexD = tag57.IndexOf('D');
            log.TraceLog("字母D需在第17-20字节之间,当前字母D的位置为:{0}",indexD);
            if (indexD < 16 || indexD > 19)
            {
                return false;   //字母D需在第17-20字节之间//卡号长度检测
            }
            string account = tag57.Substring(0, indexD);
            log.TraceLog("账号必须是数字,当前账号为:{0}",account);
            foreach (var c in account)
            {
                if (!format3.Contains(c))
                {
                    
                    return false;   //账号必须是数字
                }
            }

            log.TraceLog("tag57必须是0123456789DF中的数值,且F只能出现一次并在最末尾，当前tag57为：{0}",tag57);
            foreach (var c in tag57)
            {
                if(!format2.Contains(c))
                {
                    return false;   //数字必须是format2集合里面
                }
                if (tag57.Substring(0, tag57.Length - 1).Contains("F"))
                    return false;
            }
            indexD++;
            string MM = tag57.Substring(indexD + 2, 2);
            int month = int.Parse(MM);
            log.TraceLog("tag57 失效月份检查，失效月份为: 【{0}】", MM);
            if (month > 12 || month < 1)
            {
                return false;   //失效月份检查
            }
            log.TraceLog("tag57 服务码检查失败 服务码必须以2或6开头.当前服务码以 【{0}】开头", tag57[indexD + 4]);
            if (tag57[indexD + 4] != '2' && tag57[indexD + 4] != '6')
            {
                return false;   //服务码必须以2或6开头
            }

            return true;
        }
    }
}

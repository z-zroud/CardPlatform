using CplusplusDll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Business
{
    /// <summary>
    /// 存储交易流程中卡片返回的所有tag
    /// </summary>
    public class TagDict
    {
        private static TagDict TagDictObj;
        private Dictionary<string, string> tagDict;

        private TagDict()
        {
            tagDict = new Dictionary<string, string>();
        }

        public static TagDict GetInstance()
        {
            if(TagDictObj == null)
            {
                TagDictObj = new TagDict();
            }
            return TagDictObj;
        }

        /// <summary>
        /// 保存TLV格式的list集合
        /// </summary>
        /// <param name="arrTLV"></param>
        public void SetTags(List<TLV> arrTLV)
        {
            foreach(var item in arrTLV)
            {
                if(!item.IsTemplate)
                {
                    if(tagDict.ContainsKey(item.Tag))
                    {
                        tagDict[item.Tag] = item.Value;
                    }
                    else
                    {
                        tagDict.Add(item.Tag, item.Value);
                    }
                    
                }
            }
        }

        /// <summary>
        /// 保存一个tag值
        /// </summary>
        /// <param name="tag"></param>
        /// <param name="value"></param>
        public void SetTag(string tag,string value)
        {
            if (tagDict.ContainsKey(tag))
            {
                tagDict[tag] = value;
            }
            else
            {
                tagDict.Add(tag, value);
            }
        }

        /// <summary>
        /// 获取指定的tag值
        /// </summary>
        /// <param name="tag"></param>
        /// <returns></returns>
        public string GetTag(string tag)
        {
            string value;
            bool hasExisted = tagDict.TryGetValue(tag, out value);
            if (!hasExisted)
                return string.Empty;
            return value;
        }
    }
}

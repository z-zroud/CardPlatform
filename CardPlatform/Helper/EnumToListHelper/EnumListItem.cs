using System;

namespace CardPlatform.Helper.EnumToListHelper
{
    [Serializable()]
    public class EnumListItem
    {
        public object Value { get; set; }

        public string DisplayValue { get; set; }
    }
}
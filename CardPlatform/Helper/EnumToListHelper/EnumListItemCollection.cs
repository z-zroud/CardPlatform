using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Resources;

namespace CardPlatform.Helper.EnumToListHelper
{
    [Serializable()]
    public class EnumListItemCollection<T> : ObservableCollection<EnumListItem> where T : struct 
    {
        readonly ResourceManager resourceManager;
        readonly CultureInfo cultureInfo;
        readonly Type enumType;
        readonly Type resourceType;
   
        public EnumListItemCollection() : this(CultureInfo.CurrentUICulture)
        {
        }
  
        public EnumListItemCollection(CultureInfo cultureInfo)
        {
            if (!typeof(T).IsEnum)
                throw new NotSupportedException(String.Format("{0} is not Enum!", typeof(T).Name));

            enumType = typeof(T);
            this.cultureInfo = cultureInfo;

            
            resourceType = GetResourceTypeFromEnumType();
            if (resourceType != null)
                resourceManager = new ResourceManager(resourceType.FullName, resourceType.Assembly);
          
            foreach (T item in Enum.GetValues(enumType))
                Add(new EnumListItem() { Value = item, DisplayValue = GetEnumDisplayValue(item) });
        }

        Type GetResourceTypeFromEnumType()
        {
            var manifestResourceName = this.enumType.Assembly.GetManifestResourceNames().FirstOrDefault(t => t.Contains(this.enumType.Name)); 
            if (!String.IsNullOrEmpty(manifestResourceName))
                return Type.GetType(manifestResourceName.Replace(".resources", String.Empty), (a) => this.enumType.Assembly, (a,n,i) => this.enumType.Assembly.GetType(n, false, i));
            return null;
        }

        String GetEnumDisplayValue(T item)
        {
            var value = default(String);
            if (resourceManager != null)
                value = resourceManager.GetString(item.ToString(), cultureInfo);

            if (value == null)
            {
                var descriptionAttribute = (item as Enum).GetAttribute<DescriptionAttribute>();
                if (descriptionAttribute == null) 
                    return item.ToString();
                return descriptionAttribute.Description;
            }
            return value;
        }
    }
}
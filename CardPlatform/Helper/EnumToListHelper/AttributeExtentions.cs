using System;
using System.Linq;
using System.Linq.Expressions;

namespace CardPlatform.Helper.EnumToListHelper
{
    public static class AttributeExtentions
    {
        public static TAttribute GetAttribute<TAttribute>(this Enum enumValue) where TAttribute : Attribute
        {
            var memberInfo = enumValue.GetType()
                                      .GetMember(enumValue.ToString())
                                      .FirstOrDefault();

            if (memberInfo != null)
                return (TAttribute)memberInfo.GetCustomAttributes(typeof(TAttribute), false).FirstOrDefault();
            return null;
        }

        public static TAttribute GetAttribute<TAttribute, T>(this T sender, Expression<Func<T, object>> propertyExpression) where TAttribute : Attribute
        {
            var propertyInfo = ((MemberExpression)propertyExpression.Body).Member;
            if (propertyInfo != null)
                return (TAttribute)propertyInfo.GetCustomAttributes(typeof(TAttribute), true).FirstOrDefault();
            return null;
        }
    }
}

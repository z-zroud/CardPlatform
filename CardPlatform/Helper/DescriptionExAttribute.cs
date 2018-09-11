using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Helper
{
    [AttributeUsage(AttributeTargets.All, AllowMultiple = false, Inherited = false)]
    public class DescriptionExAttribute : Attribute
    {
        private string description;
        public DescriptionExAttribute(string desc)
        {
            description = desc;
        }
        public string Description => description;
        public bool Hide { get; set; }
    }
}

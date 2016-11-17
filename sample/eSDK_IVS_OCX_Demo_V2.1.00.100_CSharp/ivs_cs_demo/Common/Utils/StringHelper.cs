using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;

namespace ivs_cs_demo
{
    public  class StringHelper
    {
        public static string FindLanguageResource(string key)
        {
            var value = Application.Current.TryFindResource(key)!=null? string.Format(Application.Current.TryFindResource(key).ToString(),key):
                        (Application.Current.TryFindResource("CommonMistake") ?? key);
            return value.ToString();
        }
    }
}

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
//using ICSharpCode.Core.Services;

namespace ivs_cs_demo
{
    public class LogService
    {
        public static void Info(object message)
        {
            ServiceManager.Instance.Info(message);
        }

        public static void Error(object message)
        {
            ServiceManager.Instance.Error(message);
        }
        public static void Error(object message,object para)
        {
            ServiceManager.Instance.Error(message, para);
        }
    }
}

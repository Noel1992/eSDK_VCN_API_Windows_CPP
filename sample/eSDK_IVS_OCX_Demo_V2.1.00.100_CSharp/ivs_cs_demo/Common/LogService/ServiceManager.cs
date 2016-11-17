using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Diagnostics;

namespace ivs_cs_demo
{
    public class ServiceManager
    {
        static ServiceManager instance = new ServiceManager();
        public static ServiceManager Instance { get { return instance; } }
        string basePath = string.Empty;
        FileStream LogFileStream = null;
        StreamWriter LogFileWriter = null;        
        public void Info(object message)
        {
            LogFileWriter.WriteLine("Info:" + message);
            LogFileWriter.Flush();
        }
        public void Error(object message)
        {
            LogFileWriter.WriteLine("Error:" + message);
            LogFileWriter.Flush();
        }
        public void Error(object message,object para)
        {
            LogFileWriter.WriteLine("Error:" + message + para);
            LogFileWriter.Flush();
        }

        public ServiceManager()
        {
            basePath = System.Windows.Forms.Application.StartupPath.Substring(0, System.Windows.Forms.Application.StartupPath.LastIndexOf("\\"));
            if (!System.IO.Directory.Exists(basePath + "\\log\\CU"))
            {
                System.IO.Directory.CreateDirectory(basePath + "\\log\\CU");
            }
            LogFileStream = new FileStream(basePath + "\\log\\CU\\cu.txt", FileMode.OpenOrCreate, FileAccess.ReadWrite, FileShare.ReadWrite);
            LogFileWriter = new StreamWriter(LogFileStream, Encoding.Default);
        }
    }
}

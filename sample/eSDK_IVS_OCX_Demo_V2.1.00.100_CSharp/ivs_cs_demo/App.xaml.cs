using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;
using System.Globalization;
using System.Windows.Forms;
using System.Threading;
using System.IO;
namespace ivs_cs_demo
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : System.Windows.Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            bool createdNew;
            Mutex mutex = new Mutex(true, Environment.MachineName, out createdNew);
            //只能启动一个客户端
            if (!createdNew)
            {
                System.Windows.Forms.Application.Exit();
            }
               
            base.OnStartup(e);
            //载入语言
            LoadLanguage();

            //0-启动上下文环境验证
            if (!CheckEnvironment())
                return;
        }

        /// <summary>
        /// 验证启动上下文环境
        /// </summary>
        /// <returns></returns>
        bool CheckEnvironment()
        {

            if (Environment.Version < new Version(4, 0, 30319))
            {
                System.Windows.Forms.MessageBox.Show("This version of eSpaceStartUp requires .NET 4.0. You are using: " + Environment.Version, "SharpDevelop");
                return false;
            }
            string windir = Environment.GetFolderPath(Environment.SpecialFolder.Windows, Environment.SpecialFolderOption.DoNotVerify);
            if (Environment.GetEnvironmentVariable("WINDIR") != windir)
            {
                Environment.SetEnvironmentVariable("WINDIR", windir);
            }
            return true;
        }
       
        /// <summary>
        /// 程序启动时根据CultureInfo载入不同的语言
        /// 如：中文 currentCultureInfo.Name = zh-CN
        /// 程序自动载入zh-CN.xaml
        /// </summary>
        private void LoadLanguage()
        {
            CultureInfo currentCultureInfo = CultureInfo.CurrentCulture;
            List<ResourceDictionary> langRdList = new List<ResourceDictionary>();
            try
            {
                langRdList.Add(
                System.Windows.Application.LoadComponent(
                new Uri("/Resources/Language/" + currentCultureInfo.Name + ".xaml", UriKind.Relative)) as ResourceDictionary);

                string[] strFiles = Directory.GetFiles(System.Windows.Forms.Application.StartupPath.Substring(0,System.Windows.Forms.Application.StartupPath.IndexOf("\\bin")) + "\\Resources\\Style");   
                foreach(string strFile in strFiles)  
                {
                    langRdList.Add(System.Windows.Application.LoadComponent(
                     new Uri("/Resources/Style/" + strFile.Substring(strFile.LastIndexOf("\\") + 1), UriKind.Relative)) as ResourceDictionary);
                }

                strFiles = Directory.GetFiles(System.Windows.Forms.Application.StartupPath.Substring(0, System.Windows.Forms.Application.StartupPath.IndexOf("\\bin")) + "\\Resources\\ResultCode");
                foreach (string strFile in strFiles)
                {
                    langRdList.Add(System.Windows.Application.LoadComponent(
                     new Uri("/Resources/ResultCode/" + strFile.Substring(strFile.LastIndexOf("\\") + 1), UriKind.Relative)) as ResourceDictionary);
                }
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
            }
            if (langRdList.Count>0)
            {
                if (this.Resources.MergedDictionaries.Count > 0)
                {
                    this.Resources.MergedDictionaries.Clear();
                }
                foreach(ResourceDictionary langRd in langRdList)  
                {
                    this.Resources.MergedDictionaries.Add(langRd);
                }
               
            }
        }

    }
}

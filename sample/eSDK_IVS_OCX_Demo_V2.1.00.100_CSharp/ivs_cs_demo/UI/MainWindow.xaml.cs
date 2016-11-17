#region 文件头注释
/*******************************************************************************
// 版权所有  2013 华为技术有限公司
// 程 序 集：ivs_cs_demo
// 文 件 名：MainWindow.cs
// 文件说明：主画面 
// 版    本：
// 创建人：  00206574
// 创建日期：2013-07-15
// 修改人： 
// 修改标识：
// 修改日期：
// 修改说明：
*******************************************************************************/
#endregion
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Interop;
using System.Diagnostics;

namespace ivs_cs_demo
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        HwndSource hs;
        HwndSourceHook hook;
        public MainWindow()
        {        
            #region 设置全屏
            InitializeComponent();
            this.WindowState = System.Windows.WindowState.Normal;
            this.Topmost = false;
            this.MinHeight = 850;
            this.MinWidth = 850;
            this.WindowStartupLocation = System.Windows.WindowStartupLocation.CenterScreen;
            this.Top = 10;
            this.PlayOcx.Visibility = System.Windows.Visibility.Collapsed;
            #endregion
            this.Closing += new System.ComponentModel.CancelEventHandler(MainWindow_Closing);
            this.Loaded += new RoutedEventHandler(MainWindow_Loaded);
        }

        void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            hook = new HwndSourceHook(WndProc);
            hs = PresentationSource.FromVisual(this) as HwndSource;
            if (null == hs)
            {
                return;
            }
            hs.AddHook(hook);
        }
        private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            if (msg == 0x0232)//WM_EXITSIZEMOVE
            {
                OcxHelper.OCX.IVS_OCX_RefreshWnd(0);
            }
            return (System.IntPtr)wParam;
        }
        void MainWindow_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Process.GetCurrentProcess().Kill();
        }
    }
}

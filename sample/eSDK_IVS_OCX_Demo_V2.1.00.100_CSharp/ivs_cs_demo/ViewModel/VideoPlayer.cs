#region 文件头注释
/*******************************************************************************
// 版权所有  2013 华为技术有限公司
// 程 序 集：ivs_cs_demo
// 文 件 名：VideoPlayer.cs
// 文件说明：视频播放
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
using System.Windows.Forms.Integration;
using System.Windows.Forms;
using AxIVS_OCXPlayer;

namespace ivs_cs_demo
{
    public class VideoPlayer
    {
        public frmLocalOCX OCX { get; set; }
        protected readonly WindowsFormsHost formsHost;
        public UIElement OcxElement
        {
            get { return formsHost; }
        }

        public VideoPlayer()
        {
            formsHost = new WindowsFormsHost();
            InitLocalOcx();
            formsHost.Unloaded += new RoutedEventHandler(formsHost_Unloaded);
        }

        void formsHost_Unloaded(object sender, RoutedEventArgs e)
        {
            var ctrl = formsHost.Child;
            if (ctrl != null)
            {
                ctrl.Visible = false;
                OCX.SetOcxVisible(false);
            }
            formsHost.Visibility = Visibility.Hidden;
        }

        private void InitLocalOcx()
        {
            OCX = new frmLocalOCX();
            OCX.TopLevel = OCX.TopMost = false;
            formsHost.AllowDrop = true;
            OCX.Show();
            formsHost.Child = OCX;
            formsHost.Loaded += new RoutedEventHandler(formsHost_Loaded_Local);
        }

        void formsHost_Loaded_Local(object sender, RoutedEventArgs e)
        {
            if (formsHost.Visibility == Visibility.Hidden)
            {
                formsHost.Visibility = Visibility.Visible;
                var ctrl = formsHost.Child;
                if (ctrl != null)
                {
                    ctrl.Visible = true;
                }
            }

            try
            {
                formsHost.Visibility = Visibility.Visible;

                ((frmLocalOCX)OCX).OcxPlayer.IVS_OCX_Windows_Event += new AxIVS_OCXPlayer._DIVS_OCXPlayerEvents_IVS_OCX_Windows_EventEventHandler(OcxPlayerBase_IVS_OCX_Windows_Event);
            }
            catch (Exception ex)
            {
                LogService.Error(ex);
            }
        }

        public _DIVS_OCXPlayerEvents_IVS_OCX_Windows_EventEventHandler OCXWindowsEvent { get; set; }
        public void OcxPlayerBase_IVS_OCX_Windows_Event(object sender, _DIVS_OCXPlayerEvents_IVS_OCX_Windows_EventEvent e)
        {
            if (OCXWindowsEvent != null)
            {
                OCXWindowsEvent(sender, e);
            }
        }
    }
}

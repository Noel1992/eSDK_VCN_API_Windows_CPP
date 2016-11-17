using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ivs_cs_demo
{
   public class OcxHelper
    {
      static public MainWindow MainWin
       {
           get {return (MainWindow)System.Windows.Application.Current.MainWindow; }
       }
      static public frmLocalOCX OCX
       {
           get { return (frmLocalOCX)((MainWindow)System.Windows.Application.Current.MainWindow).PlayOcx.playOcxViewModel.Player.OCX; }
       }
      static public void InitOcxPaneState(int wndId)
      {
          //OcxHelper.OCX.IVS_OCX_StopBackupPlayBack(wndId);
          OcxHelper.OCX.IVS_OCX_StopLocalPlayBack(wndId);
          OcxHelper.OCX.IVS_OCX_StopLocalRecord(wndId);
          OcxHelper.OCX.IVS_OCX_StopPlatformPlayBack(wndId);
          OcxHelper.OCX.IVS_OCX_StopPUPlayBack(wndId);
          OcxHelper.OCX.IVS_OCX_StopRealPlay(wndId);
      }
    }
}

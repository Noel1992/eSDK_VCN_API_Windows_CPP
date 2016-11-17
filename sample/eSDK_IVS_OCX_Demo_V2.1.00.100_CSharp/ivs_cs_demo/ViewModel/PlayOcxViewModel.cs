#region 文件头注释
/*******************************************************************************
// 版权所有  2013 华为技术有限公司
// 程 序 集：ivs_cs_demo
// 文 件 名：PlayOcxViewModel.cs
// 文件说明：OCX视图 
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
using AxIVS_OCXPlayer;

namespace ivs_cs_demo
{
   public class PlayOcxViewModel
   {
       public  VideoPlayer Player { get; protected set; }
       public  UIElement OcxElement { get { return Player.OcxElement; } }

       public event EventHandler OcxMsgHandlerEvent;
        public PlayOcxViewModel()
        {
            Player = new VideoPlayer();
            Player.OCXWindowsEvent = Player_PlayErrorEvent;
        }

        void Player_PlayErrorEvent(object sender, _DIVS_OCXPlayerEvents_IVS_OCX_Windows_EventEvent e)
        {
            if (OcxMsgHandlerEvent != null)
            {
                OcxMsgHandlerEvent(sender, new OCX_Windows_EventEventArgs(e));
            } 
        }
      
    }
   public class OCX_Windows_EventEventArgs : EventArgs
   {
       public _DIVS_OCXPlayerEvents_IVS_OCX_Windows_EventEvent Data { get; set; }
       public OCX_Windows_EventEventArgs(_DIVS_OCXPlayerEvents_IVS_OCX_Windows_EventEvent e)
       {
           Data = e;
       }
   }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Input;

namespace ivs_cs_demo
{
   public  class LayOutManageViewModel
    {
       public ICommand SetDisplayScaleCommand { get; set; }
       public ICommand FullScreenCommand { get; set; }
       public ICommand QuitFullScreenCommand { get; set; }
       public ICommand SetToolbarCommand { get; set; }
       public ICommand Layout1Command { get; set; }
       public ICommand Layout6Command { get; set; }
       public ICommand Layout9Command { get; set; }
       public ICommand GetActivePanIDCommand { get; set; }
       public ICommand GetFreePanIDCommand { get; set; }
       public ICommand GetActivePanCameraCodeCommand { get; set; }
 

        public LayOutManageUserControls layOutManageUserControls { get; set; }

        public LayOutManageViewModel(LayOutManageUserControls window)
       {
           layOutManageUserControls = window;

           SetDisplayScaleCommand = new DelegateCommand(SetDisplayScaleProcess);
           FullScreenCommand = new DelegateCommand(FullScreenProcess);
           QuitFullScreenCommand = new DelegateCommand(QuitFullScreenProcess);
           SetToolbarCommand = new DelegateCommand(SetToolbarProcess);
           Layout1Command = new DelegateCommand(Layout1Process);
           Layout6Command = new DelegateCommand(Layout6Process);
           Layout9Command = new DelegateCommand(Layout9Process);
           GetActivePanIDCommand = new DelegateCommand(GetActivePanIDProcess);
           GetFreePanIDCommand = new DelegateCommand(GetFreePanIDProcess);
           GetActivePanCameraCodeCommand = new DelegateCommand(GetActivePanCameraCodeProcess);
       }

        private void SetDisplayScaleProcess()
        {
            try
            {
                int result = OcxHelper.OCX.IVS_OCX_SetDisplayScale(Convert.ToInt32(layOutManageUserControls.DisplayScale.SelectedIndex + 1));//Source = 1,Full = 2
                layOutManageUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_SetDisplayScale(Convert.ToInt32(layOutManageUserControls.DisplayScale.Text.ToString()));";

                if (result == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("SetDisplayScale") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }   
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }

        private void FullScreenProcess()
        {
            try
            {
                int result = OcxHelper.OCX.IVS_OCX_FullScreenDisplay();
                layOutManageUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_FullScreenDisplay();";

                if (result == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("FullScreen") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }   
              
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }

        private void QuitFullScreenProcess()
        {
            try
            {
                int result = OcxHelper.OCX.IVS_OCX_NormalScreenDisplay();
                layOutManageUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_NormalScreenDisplay();";

                if (result == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("QuitFullScreen") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                } 
          
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }

        private void SetToolbarProcess()
        {
            try
            {
                uint toolBar =  (layOutManageUserControls.LocalRecord.IsChecked == true ? (uint)LivePlayerToolType.LocalRecord : 0)|
                                (layOutManageUserControls.Zoom.IsChecked == true ? (uint)LivePlayerToolType.Zoom : 0)|
                                (layOutManageUserControls.PTZ.IsChecked == true ? (uint)LivePlayerToolType.PTZ : 0)|
                                (layOutManageUserControls.Snapshot.IsChecked == true ? (uint)LivePlayerToolType.Snapshot : 0)|
                                (layOutManageUserControls.AlarmWin.IsChecked == true ? (uint)LivePlayerToolType.AlarmWin : 0)|
                                (layOutManageUserControls.Bookmark.IsChecked == true ? (uint)LivePlayerToolType.Bookmark : 0);
                int result = OcxHelper.OCX.IVS_OCX_SetToolbar(toolBar);
                System.Windows.Interop.WindowInteropHelper helper = new System.Windows.Interop.WindowInteropHelper(System.Windows.Application.Current.MainWindow);
                OcxHelper.OCX.IVS_OCX_RefreshWnd((uint)helper.Handle.ToInt32());

                layOutManageUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_SetToolbar();";

                if (result == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("SetToolbar") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }   
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }

        private void Layout1Process()
        {
            try
            {
                int result = OcxHelper.OCX.IVS_OCX_SetWndLayout((uint)PlayerLayoutType.Layout11);
                layOutManageUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_SetWndLayout((uint)PlayerLayoutType.Layout11);";

                if (result == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("Layout1") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }

        private void Layout6Process()
        {
            try
            {
                int result = OcxHelper.OCX.IVS_OCX_SetWndLayout((uint)PlayerLayoutType.Layout63);
                layOutManageUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_SetWndLayout((uint)PlayerLayoutType.Layout63);";

                if (result == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("Layout6") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }

        private void Layout9Process()
        {
            try
            {
                int result = OcxHelper.OCX.IVS_OCX_SetWndLayout((uint)PlayerLayoutType.Layout92);
                layOutManageUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_SetWndLayout((uint)PlayerLayoutType.Layout92);";

                if (result == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("Layout9") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }

        private void GetActivePanIDProcess()
        {
            try
            {
                int result =(int)OcxHelper.OCX.IVS_OCX_GetSelectWnd();
                layOutManageUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_GetSelectWnd();";

                if (0<result&&result <65)
                {
                    layOutManageUserControls.ActivePanID.Text = result.ToString();
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("GetActivePanID") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    layOutManageUserControls.ActivePanID.Text = string.Empty;
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }  
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }

        private void GetFreePanIDProcess()
        {
            try
            {
                int result = OcxHelper.OCX.IVS_OCX_GetFreeWnd();
                layOutManageUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_GetFreeWnd();";

                if (0 < result && result < 65)
                {
                    layOutManageUserControls.FreePanID.Text = result.ToString();
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("GetFreePanID") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    layOutManageUserControls.FreePanID.Text = string.Empty;
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }  
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }

        private void GetActivePanCameraCodeProcess()
        {
            try
            {
                string result = OcxHelper.OCX.IVS_OCX_GetCameraByWnd((uint)OcxHelper.OCX.IVS_OCX_GetSelectWnd());
                layOutManageUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_GetCameraByWnd((uint)OcxHelper.OCX.IVS_OCX_GetSelectWnd());";

                if (result != "2" )
                {
                    layOutManageUserControls.ActivePanCameraCode.Text = result;
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("GetActivePanCameraCode") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    layOutManageUserControls.ActivePanCameraCode.Text = string.Empty;
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }
    }
}

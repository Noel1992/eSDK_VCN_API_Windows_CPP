#region 文件头注释
/*******************************************************************************
// 版权所有  2013 华为技术有限公司
// 程 序 集：ivs_cs_demo
// 文 件 名：LiveVideoPtzViewModel.cs
// 文件说明：实况、云台控制视图 
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
using System.Windows.Input;
using System.Windows;

namespace ivs_cs_demo
{
   public  class LiveVideoPtzViewModel
    {
       public ICommand GetCameraListCommand { get; set; }
       public ICommand RealPlayCommand { get; set; }
       public ICommand RealPlayStopCommand { get; set; }
       public ICommand AddPresetCommand { get; set; }
       public ICommand QueryPresetCommand { get; set; }
       public ICommand DeletePresetCommand { get; set; }
       public ICommand InvokePresetCommand { get; set; }
       public ICommand Copy { get; set; }
       PTZBusiness business=null;

       public LiveVideoPtzUserControls liveVideoPtzUserControls { get; set; }
       public LiveVideoPtzViewModel(LiveVideoPtzUserControls widow)
       {
           liveVideoPtzUserControls = widow;

           GetCameraListCommand = new DelegateCommand(GetCameraListProcess);
           RealPlayCommand = new DelegateCommand(RealPlayProcess);
           RealPlayStopCommand = new DelegateCommand(RealPlayStopProcess);
           AddPresetCommand = new DelegateCommand(AddPresetProcess);
           QueryPresetCommand = new DelegateCommand(QueryPresetProcess);
           DeletePresetCommand = new DelegateCommand(DeletePresetProcess);
           InvokePresetCommand = new DelegateCommand(InvokePresetProcess);
           Copy = new DelegateCommand(CopyProcess);

           business = new PTZBusiness();

       }

       private void GetCameraListProcess()
       {
           try
           {
               DeviceInfoBusiness bussiness = new DeviceInfoBusiness();
               List<VideoEntity> deviceList = bussiness.LoadDeviceInfoByUser();
               liveVideoPtzUserControls.CameraList.ItemsSource = deviceList;

               liveVideoPtzUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_GetDeviceList((uint)IVSDeviceType.DEVICE_TYPE_CAMERA, (uint)1, (uint)PageSize);";
           }
           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
               OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
               return;
           }
       }
       private void RealPlayProcess()
       {
           try
           {
               int wndID = Convert.ToInt32(liveVideoPtzUserControls.PlayPane.Text.ToString());
               OcxHelper.OCX.IVS_OCX_StopRealPlay(wndID);
               int resultCode = OcxHelper.OCX.IVS_OCX_StartRealPlay((liveVideoPtzUserControls.CameraList.SelectedItem as VideoEntity).CameraCode, wndID, -1, liveVideoPtzUserControls.PlayParameter.Text);
               liveVideoPtzUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_StartRealPlay((liveVideoPtzUserControls.CameraList.SelectedItem as VideoEntity).CameraCode, wndID, -1);";
               if (resultCode == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("RealPlay") + StringHelper.FindLanguageResource("Success");
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(resultCode.ToString());
               }
           }
           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
               OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
               return;
           }

       }
       private void RealPlayStopProcess()
       {
           try
           {
               int wndID = Convert.ToInt32(liveVideoPtzUserControls.PlayPane.Text.ToString());
               int resultCode = OcxHelper.OCX.IVS_OCX_StopRealPlay(wndID);
               liveVideoPtzUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_StopRealPlay(wndID);";
               if (resultCode == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("RealPlayStop") + StringHelper.FindLanguageResource("Success");
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(resultCode.ToString());
               }
           }
           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
               OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
               return;
           }
       }
       private void AddPresetProcess()
       {
           try
           {
               if (string.IsNullOrEmpty(liveVideoPtzUserControls.PresetName.Text.ToString()))
               {
                   return;
               }
               VideoEntity camera = liveVideoPtzUserControls.CameraList.SelectedItem as VideoEntity;
               string resultValue = OcxHelper.OCX.IVS_OCX_AddPTZPreset(camera.CameraCode, liveVideoPtzUserControls.PresetName.Text.ToString());
               liveVideoPtzUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_AddPTZPreset(camera.CameraCode, liveVideoPtzUserControls.PresetName.Text.ToString());";
               OcxResult result = XmlHelper.ProcessOcxResponseMsg(resultValue);
               if (result.ResultCode != "0")
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ResultCode);
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("AddPreset") + StringHelper.FindLanguageResource("Success");
               }
           }
           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
               OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
               return;
           }
       }
       private void QueryPresetProcess()
       {
           try
           {
               VideoEntity camera = liveVideoPtzUserControls.CameraList.SelectedItem as VideoEntity;
               var resultValue = OcxHelper.OCX.IVS_OCX_GetPTZPresetList(camera.CameraCode);
               liveVideoPtzUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_GetPTZPresetList(CameraCode);";
               OcxResult result = XmlHelper.ProcessOcxResponseMsg(resultValue);
               if (result.ResultCode != "0")
               {
                   throw new Exception(result.ResultCode);
               }

               var returnCameraCode = result.BodyXmlNode.SelectSingleNode("CameraCode").InnerText;
               List<PTZPresetEntity> preSetLocationEntityList = XmlHelper.DeserializeToList<PTZPresetEntity>(result.BodyXmlNode.SelectSingleNode("PresetPTZList"));

               foreach (PTZPresetEntity Preset in preSetLocationEntityList)
               {
                   Preset.CameraCode = camera.CameraCode;
               }
               liveVideoPtzUserControls.PresetList.ItemsSource = preSetLocationEntityList;

               if (result.ResultCode != "0")
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ResultCode);
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("QueryPreset") + StringHelper.FindLanguageResource("Success");
               }
           }
           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
               OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
               return;
           }
       }

       private void DeletePresetProcess()
       {
          PTZPresetEntity pTZPreset= liveVideoPtzUserControls.PresetList.SelectedItem as PTZPresetEntity;

          if (pTZPreset ==null)
          {
              return;
          }
          var resultValue = OcxHelper.OCX.IVS_OCX_DelPTZPreset(pTZPreset.CameraCode, (uint)(pTZPreset.PresetIndex));
          liveVideoPtzUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_DelPTZPreset(pTZPreset.CameraCode, (uint)(pTZPreset.PresetIndex));";
          if (resultValue != 0)
          {
              OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(resultValue.ToString());
          }
          else
          {
              OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("DeletePreset") + StringHelper.FindLanguageResource("Success");
          }
          QueryPresetProcess();
       }
       private void InvokePresetProcess()
       {
           PTZPresetEntity pTZPreset = liveVideoPtzUserControls.PresetList.SelectedItem as PTZPresetEntity;

           var resultValue = business.SendPtz(PtzOperationType.PTZ_PREFAB_BIT_RUN, pTZPreset.PresetIndex, 5, pTZPreset.CameraCode);
           liveVideoPtzUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_PtzControl(strCameracode, (int)operation, param1.ToString(), param2.ToString())";
           if (resultValue != 0)
           {
               OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(resultValue.ToString());
           }
           else
           {
               OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("InvokePreset") + StringHelper.FindLanguageResource("Success");
           }
       }
       public void PtzProcess(PtzOperationType operaction)
       {
           try
           {
               int resultCode = business.SendPtz(operaction, 2, 5, OcxHelper.OCX.IVS_OCX_GetCameraByWnd(Convert.ToUInt32(liveVideoPtzUserControls.PlayPane.Text.ToString())));
               liveVideoPtzUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_PtzControl(strCameracode, (int)operation, param1.ToString(), param2.ToString())";
               if (operaction == PtzOperationType.PTZ_STOP)
               {
                   return;
               }
               if (resultCode == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("PtzOp") + StringHelper.FindLanguageResource("Success");
               }
               else if (resultCode == 1)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("PtzLocked");
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(resultCode.ToString());
               }
           }
           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
               OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
               return;
           }
       }
       private void CopyProcess()
       {
           try
           {
               Clipboard.SetDataObject((liveVideoPtzUserControls.CameraList.SelectedItem as VideoEntity).CameraCode);
           }
           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
           }
       }
    }
}

#region 文件头注释
/*******************************************************************************
// 版权所有  2013 华为技术有限公司
// 程 序 集：ivs_cs_demo
// 文 件 名：RecordViewModel.cs
// 文件说明：录像视图 
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

namespace ivs_cs_demo
{
   public class RecordViewModel
   {
       public ICommand StartPLATRecordCommand { get; set; }
       public ICommand StopPLATRecordCommand { get; set; }
       public ICommand StartPURecordCommand { get; set; }
       public ICommand StopPURecordCommand { get; set; }
       public ICommand StartLocalRecordCommand { get; set; }
       public ICommand StopLocalRecordCommand { get; set; }

       public RecordUserControls recordUserControls { get; set; }
       public RecordViewModel(RecordUserControls window)
       {
           recordUserControls = window;

           StartPLATRecordCommand = new DelegateCommand(StartPLATRecordProcess);
           StopPLATRecordCommand = new DelegateCommand(StopPLATRecordProcess);
           StartPURecordCommand = new DelegateCommand(StartPURecordProcess);
           StopPURecordCommand = new DelegateCommand(StopPURecordProcess);
           StartLocalRecordCommand = new DelegateCommand(StartLocalRecordProcess);
           StopLocalRecordCommand = new DelegateCommand(StopLocalRecordProcess);

           recordUserControls.LocalFileNameTextBox.Text = @"D:\\";
       }

       private void StartPLATRecordProcess()
       {
           try
            {
                int result = OcxHelper.OCX.IVS_OCX_StartPlatformRecord(recordUserControls.CameraCodeTextBox.Text.ToString(), UInt32.Parse(recordUserControls.RecordTimeTextBox.Text.ToString()));
                recordUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_StartPlatformRecord(recordUserControls.CameraCodeTextBox.Text.ToString(),UInt32.Parse(recordUserControls.RecordTimeTextBox.Text.ToString()));";

                if (result == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StartPLATRecord") + StringHelper.FindLanguageResource("Success");
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

       private void StopPLATRecordProcess()
       {
           try
           {
               int result = OcxHelper.OCX.IVS_OCX_StopPlatformRecord(recordUserControls.CameraCodeTextBox.Text.ToString());
               recordUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_StopPlatformRecord(recordUserControls.CameraCodeTextBox.Text.ToString());";

               if (result == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StopPLATRecord") + StringHelper.FindLanguageResource("Success");
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

       private void StartPURecordProcess()
       {
           try
           {
               int result = OcxHelper.OCX.IVS_OCX_StartPURecord(recordUserControls.PUCameraCodeTextBox.Text.ToString(), UInt32.Parse(recordUserControls.PURecordTimeTextBox.Text.ToString()));
               recordUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_StartPURecord(recordUserControls.CameraCodeTextBox.Text.ToString(),UInt32.Parse(recordUserControls.RecordTimeTextBox.Text.ToString()));";

               if (result == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StartPURecord") + StringHelper.FindLanguageResource("Success");
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

       private void StopPURecordProcess()
       {
           try
           {
               int result = OcxHelper.OCX.IVS_OCX_StopPURecord(recordUserControls.PUCameraCodeTextBox.Text.ToString());
               recordUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_StopPURecord(recordUserControls.CameraCodeTextBox.Text.ToString());";

               if (result == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StopPURecord") + StringHelper.FindLanguageResource("Success");
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

       private void StartLocalRecordProcess()
       {
           try
           {
               string xmlString = recordUserControls.RecordParaTextBox.Text;
               LocalRecordParam localRecordParam = XmlHelper.DeserializeToEntity<LocalRecordParam>(xmlString);
               localRecordParam.SavePath = recordUserControls.LocalFileNameTextBox.Text.ToString();

               int result = (int)OcxHelper.OCX.IVS_OCX_StartLocalRecord(localRecordParam, int.Parse(recordUserControls.WndIDTextBox.Text.ToString()));
               recordUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_StartLocalRecord(recordUserControls.RecordParaTextBox.Text.ToString(), int.Parse(recordUserControls.WndIDTextBox.Text.ToString()));";

               if (result == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StartLocalRecord") + StringHelper.FindLanguageResource("Success");
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

       private void StopLocalRecordProcess()
       {
           try
           {
               int result = (int)OcxHelper.OCX.IVS_OCX_StopLocalRecord(int.Parse(recordUserControls.WndIDTextBox.Text.ToString()));
               recordUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_StopLocalRecord(int.Parse(recordUserControls.WndIDTextBox.Text.ToString()));";

               if (result == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StopLocalRecord") + StringHelper.FindLanguageResource("Success");
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
    }
}

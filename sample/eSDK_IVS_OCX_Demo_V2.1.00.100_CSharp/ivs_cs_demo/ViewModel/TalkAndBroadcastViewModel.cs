#region 文件头注释
/*******************************************************************************
// 版权所有  2013 华为技术有限公司
// 程 序 集：ivs_cs_demo
// 文 件 名：TalkAndBroadcastViewModel.cs
// 文件说明：对讲与广播视图 
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
using System.Xml;
using System.Windows;

namespace ivs_cs_demo
{
    public class TalkAndBroadcastViewModel
    {
        public TalkAndBroadcastUserControls talkAndBroadcastUserControls { get; set; }

        public ICommand StartVoiceTalkbackCommand { get; set; }
        public ICommand StopVoiceTalkbackCommand { get; set; }
        public ICommand AddBroadcastDeviceCommand { get; set; }
        public ICommand DeleteBroadcastDeviceCommand { get; set; }
        public ICommand StartRealBroadcastCommand { get; set; }
        public ICommand StopRealBroadcastCommand { get; set; }
        public ICommand StartFileBroadcastCommand { get; set; }
        public ICommand StopFileBroadcastCommand { get; set; }
        public ICommand Copy { get; set; }

        public TalkAndBroadcastViewModel(TalkAndBroadcastUserControls window)
        {
            talkAndBroadcastUserControls = window;

            StartVoiceTalkbackCommand = new DelegateCommand(StartVoiceTalkbackProcess);
            StopVoiceTalkbackCommand = new DelegateCommand(StopVoiceTalkbackProcess);
            AddBroadcastDeviceCommand = new DelegateCommand(AddBroadcastDeviceProcess);
            DeleteBroadcastDeviceCommand = new DelegateCommand(DeleteBroadcastDeviceProcess);
            StartRealBroadcastCommand = new DelegateCommand(StartRealBroadcastProcess);
            StopRealBroadcastCommand = new DelegateCommand(StopRealBroadcastProcess);
            StartFileBroadcastCommand = new DelegateCommand(StartFileBroadcastProcess);
            StopFileBroadcastCommand = new DelegateCommand(StopFileBroadcastProcess);
            Copy = new DelegateCommand(CopyProcess);
        }
        public void StartVoiceTalkbackProcess()
        {
            try
            { 
                TalkbackParam para = new TalkbackParam();
                para.ProtocolType = (talkAndBroadcastUserControls.ProtocolType.SelectedIndex + 1).ToString();
                para.DirectFirst =(bool)talkAndBroadcastUserControls.DirectFirst.IsChecked ? "1" :"0";

                var requestXml = XmlHelper.SerializeToXml(para);
                requestXml = XmlHelper.GetContentXml(requestXml);

                string responseMsg = OcxHelper.OCX.IVS_OCX_StartVoiceTalkback(requestXml, talkAndBroadcastUserControls.TalkCameraCode.Text.ToString());
                talkAndBroadcastUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_StartVoiceTalkback(requestXml, talkAndBroadcastUserControls.TalkCameraCode.Text.ToString());";
                
                var ocxResult = XmlHelper.ProcessOcxResponseMsg(responseMsg);

                if (ocxResult.ResultCode != "0")
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(ocxResult.ResultCode.ToString());
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StartVoiceTalkback") + StringHelper.FindLanguageResource("Success");
                    var node = ocxResult.BodyXmlNode.SelectSingleNode("TalkbackHandle");
                    talkAndBroadcastUserControls.TalkbackHandle.Text = node.InnerText.ToString();
                }
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }
        public void StopVoiceTalkbackProcess()
        {
            try
            {
                int result = 0;
                result = OcxHelper.OCX.IVS_OCX_StopVoiceTalkback(uint.Parse(talkAndBroadcastUserControls.TalkbackHandle.Text.ToString()));

                if (result == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StopVoiceTalkback") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }
                talkAndBroadcastUserControls.eSDKCodeTextBlock.Text = @"var result = OcxHelper.OCX.IVS_OCX_StopVoiceTalkback(uint.Parse(talkAndBroadcastUserControls.TalkbackHandle.Text.ToString()));";
            }

            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }
        public void AddBroadcastDeviceProcess()
        {
            try
            {
                int result = 0;
                result = OcxHelper.OCX.IVS_OCX_AddBroadcastDevice(talkAndBroadcastUserControls.BroadcastDeviceCode.Text.ToString());

                if (result == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("AddBroadcastDevice") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }
                GetBroadcastDeviceList();
                talkAndBroadcastUserControls.eSDKCodeTextBlock.Text = @"var result = OcxHelper.OCX.IVS_OCX_AddBroadcastDevice(talkAndBroadcastUserControls.TalkCameraCode.Text.ToString());";
            }

            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }
        private void GetBroadcastDeviceList()
        {
            try
            {
                string responseMsg = OcxHelper.OCX.IVS_OCX_GetBroadcastCameraList();

                var ocxResult = XmlHelper.ProcessOcxResponseMsg(responseMsg);

                if (ocxResult.ResultCode == "0")
                {
                    List<VideoEntity> list = new List<VideoEntity>();

                    XmlNode node = ocxResult.BodyXmlNode;
                    XmlNodeList nodeList = node.SelectNodes("BroadcastList/CameraCode");

                    foreach (XmlNode xmlNode in nodeList)
                    {
                        VideoEntity video = new VideoEntity();
                        video.CameraCode = xmlNode.InnerText;
                        list.Add(video);
                    }
                    talkAndBroadcastUserControls.BroadcastDeviceList.ItemsSource = list;
                }
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                return;
            }
        }
        public void DeleteBroadcastDeviceProcess()
        {
            try
            {
                int result = 0;
                result = OcxHelper.OCX.IVS_OCX_DeleteBroadcastDevice(talkAndBroadcastUserControls.BroadcastDeviceCode.Text.ToString());

                if (result == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("DeleteBroadcastDevice") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }
                GetBroadcastDeviceList();

                talkAndBroadcastUserControls.eSDKCodeTextBlock.Text = @"var result = OcxHelper.OCX.IVS_OCX_DeleteBroadcastDevice(talkAndBroadcastUserControls.TalkCameraCode.Text.ToString());";
            }

            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }
        public void StartRealBroadcastProcess()
        {
            try
            {
                int result = OcxHelper.OCX.IVS_OCX_StartRealBroadcast();
                talkAndBroadcastUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_StartRealBroadcast();";

                if (result != 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StartRealBroadcast") + StringHelper.FindLanguageResource("Success");
                }
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }
        public void StopRealBroadcastProcess()
        {
            try
            {
                int result = OcxHelper.OCX.IVS_OCX_StopRealBroadcast();
                talkAndBroadcastUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_StopRealBroadcast();";

                if (result != 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StopRealBroadcast") + StringHelper.FindLanguageResource("Success");
                }
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }
        public void StartFileBroadcastProcess()
        {
            try
            {
                int result = OcxHelper.OCX.IVS_OCX_StartFileBroadcast(talkAndBroadcastUserControls.FileName.Text.ToString(),(uint)talkAndBroadcastUserControls.CycleType.SelectedIndex);
                talkAndBroadcastUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_StartFileBroadcast(talkAndBroadcastUserControls.FileName.Text.ToString(),(uint)talkAndBroadcastUserControls.CycleType.SelectedIndex);";

                if (result != 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StartFileBroadcast") + StringHelper.FindLanguageResource("Success");
                }
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }
        public void StopFileBroadcastProcess()
        {
            try
            {
                int result = OcxHelper.OCX.IVS_OCX_StopFileBroadcast();
                talkAndBroadcastUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_StopFileBroadcast();";

                if (result != 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StopFileBroadcast") + StringHelper.FindLanguageResource("Success");
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
                Clipboard.SetDataObject((talkAndBroadcastUserControls.BroadcastDeviceList.SelectedItem as VideoEntity).CameraCode);
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
            }
        }
    }
    
}

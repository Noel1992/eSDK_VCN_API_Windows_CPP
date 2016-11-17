#region 文件头注释
/*******************************************************************************
// 版权所有  2013 华为技术有限公司
// 程 序 集：ivs_cs_demo
// 文 件 名：DownloadAndCaptureViewModel.cs
// 文件说明：下载抓拍视图 
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
using AxIVS_OCXPlayer;
using System.IO;
using System.Threading;
using System.ComponentModel;
using System.Windows.Threading;
using System.Windows.Forms;

namespace ivs_cs_demo
{
    public class DownloadAndCaptureViewModel
    {
        public DownloadAndCaptureUserControls downloadAndCaptureUserControls { get; set; }

        public ICommand StartPlatformDownloadCommand { get; set; }
        public ICommand StartPUDownloadCommand { get; set; }
        public ICommand StopPlatformDownloadCommand { get; set; }
        public ICommand StopPUDownloadCommand { get; set; }
        public ICommand DownloadPauseCommand { get; set; }
        public ICommand DownloadResumeCommand { get; set; }
        public ICommand GetDownloadInfoCommand { get; set; }
        public ICommand LocalSnapshotCommand { get; set; }

        public DownloadAndCaptureViewModel(DownloadAndCaptureUserControls window)
        {
            this.downloadAndCaptureUserControls = window;

            StartPlatformDownloadCommand = new DelegateCommand(StartPlatformDownloadCommandProcess);
            StartPUDownloadCommand = new DelegateCommand(StartPUDownloadCommandProcess);
            StopPlatformDownloadCommand = new DelegateCommand(StopPlatformDownloadCommandProcess);
            StopPUDownloadCommand = new DelegateCommand(StopPUDownloadCommandProcess);
            DownloadPauseCommand = new DelegateCommand(DownloadPauseCommandProcess);
            DownloadResumeCommand = new DelegateCommand(DownloadResumeCommandProcess);
            GetDownloadInfoCommand = new DelegateCommand(GetDownloadInfoCommandProcess);
            LocalSnapshotCommand = new DelegateCommand(LocalSnapshotCommandProcess);

        }

        public void StartPlatformDownloadCommandProcess()
        {
            try
            {
                RecordDownloadParam recordDownloadParam = XmlHelper.DeserializeToEntity<RecordDownloadParam>(downloadAndCaptureUserControls.DownLoadPara.Text);
                recordDownloadParam.SavePath = downloadAndCaptureUserControls.SaveFileName.Text.ToString();
                if (!downloadAndCaptureUserControls.CameraCode.Text.Contains("#"))
                {
                    return;
                }
                
                    recordDownloadParam.NVRCode = downloadAndCaptureUserControls.CameraCode.Text.Split('#')[1];
                
        

                var requestXml = XmlHelper.SerializeToXml(recordDownloadParam);
                requestXml = "<?xml version='1.0' encoding='utf-8'?>" + XmlHelper.GetContentXml(requestXml);

                DateTime realStartTime = DateTime.ParseExact(downloadAndCaptureUserControls.StartTime.Text.ToString(), "yyyyMMddHHmmss", Thread.CurrentThread.CurrentCulture);
                string startTime = realStartTime.ToUniversalTime().ToString("yyyyMMddHHmmss");

                DateTime realEndTime = DateTime.ParseExact(downloadAndCaptureUserControls.EndTime.Text.ToString(), "yyyyMMddHHmmss", Thread.CurrentThread.CurrentCulture);
                string endTime = realEndTime.ToUniversalTime().ToString("yyyyMMddHHmmss");

                string responseMsg = OcxHelper.OCX.IVS_OCX_StartPlatformDownload(downloadAndCaptureUserControls.CameraCode.Text.ToString(), startTime, endTime, requestXml);
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_StartPlatformDownload(downloadAndCaptureUserControls.CameraCode.Text.ToString(), downloadAndCaptureUserControls.StartTime.Text.ToString(), downloadAndCaptureUserControls.EndTime.Text.ToString(), requestXml);";

                var ocxResult = XmlHelper.ProcessOcxResponseMsg(responseMsg);
                if (ocxResult.ResultCode != "0")
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(ocxResult.ResultCode.ToString());
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StartPlatformDownload") + StringHelper.FindLanguageResource("Success");
                    var node = ocxResult.BodyXmlNode.SelectSingleNode("DownloadHandle");
                    downloadAndCaptureUserControls.DownLoadHandle.Text = node.InnerText.ToString();
                }
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @"OcxHelper.OCX.IVS_OCX_StartPlatformDownload(downloadAndCaptureUserControls.CameraCode.Text.ToString(), startTime, endTime, requestXml);";
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }
        public void StartPUDownloadCommandProcess()
        {
            try
            {
                RecordDownloadParam recordDownloadParam = XmlHelper.DeserializeToEntity<RecordDownloadParam>(downloadAndCaptureUserControls.DownLoadPara.Text);
                recordDownloadParam.SavePath = downloadAndCaptureUserControls.SaveFileName.Text.ToString();
               
                DateTime realStartTime = DateTime.ParseExact(downloadAndCaptureUserControls.StartTime.Text.ToString(), "yyyyMMddHHmmss", Thread.CurrentThread.CurrentCulture);
                string startTime = realStartTime.ToUniversalTime().ToString("yyyyMMddHHmmss");

                DateTime realEndTime = DateTime.ParseExact(downloadAndCaptureUserControls.EndTime.Text.ToString(), "yyyyMMddHHmmss", Thread.CurrentThread.CurrentCulture);
                string endTime = realEndTime.ToUniversalTime().ToString("yyyyMMddHHmmss");

                var requestXml = XmlHelper.SerializeToXml(recordDownloadParam);
                requestXml = "<?xml version='1.0' encoding='utf-8'?>" + XmlHelper.GetContentXml(requestXml);

                string responseMsg = OcxHelper.OCX.IVS_OCX_StartPUDownload(downloadAndCaptureUserControls.CameraCode.Text.ToString(), startTime, endTime, requestXml);
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_StartPUDownload(downloadAndCaptureUserControls.CameraCode.Text.ToString(), downloadAndCaptureUserControls.StartTime.Text.ToString(), downloadAndCaptureUserControls.EndTime.Text.ToString(), requestXml);";

                var ocxResult = XmlHelper.ProcessOcxResponseMsg(responseMsg);
                if (ocxResult.ResultCode != "0")
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(ocxResult.ResultCode.ToString());
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StartPUDownload") + StringHelper.FindLanguageResource("Success");
                    var node = ocxResult.BodyXmlNode.SelectSingleNode("DownloadHandle");
                    downloadAndCaptureUserControls.DownLoadHandle.Text = node.InnerText.ToString();
                }
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @"OcxHelper.OCX.IVS_OCX_StartPUDownload(downloadAndCaptureUserControls.CameraCode.Text.ToString(), downloadAndCaptureUserControls.StartTime.Text.ToString(), downloadAndCaptureUserControls.EndTime.Text.ToString(), requestXml);";
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }
        public void StopPlatformDownloadCommandProcess()
        {
            try
            {
                int downLoadHandle = Convert.ToInt32(downloadAndCaptureUserControls.DownLoadHandle.Text.ToString());
                int resultCode = OcxHelper.OCX.IVS_OCX_StopPlatformDownload((uint)downLoadHandle);
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_StopPlatformDownload(downLoadHandle);";

                if (resultCode == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StopPlatformDownload") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(resultCode.ToString());
                }
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @"OcxHelper.OCX.IVS_OCX_StopPlatformDownload((uint)downLoadHandle);";
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }
        public void StopPUDownloadCommandProcess()
        {
            try
            {
                int downLoadHandle = Convert.ToInt32(downloadAndCaptureUserControls.DownLoadHandle.Text.ToString());
                int resultCode = OcxHelper.OCX.IVS_OCX_StopPUDownload((uint)downLoadHandle);
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_StopPUDownload(downLoadHandle);";

                if (resultCode == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StopPUDownload") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(resultCode.ToString());
                }
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @"OcxHelper.OCX.IVS_OCX_StopPUDownload((uint)downLoadHandle);";
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }
        public void DownloadPauseCommandProcess()
        {
            try
            {
                int downLoadHandle = Convert.ToInt32(downloadAndCaptureUserControls.DownLoadHandle.Text.ToString());
                int resultCode = OcxHelper.OCX.IVS_OCX_DownloadPause((uint)downLoadHandle);
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_DownloadPause(downLoadHandle);";

                if (resultCode == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("DownloadPause") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(resultCode.ToString());
                }
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @"OcxHelper.OCX.IVS_OCX_DownloadPause((uint)downLoadHandle);";
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }
        public void DownloadResumeCommandProcess()
        {
            try
            {
                int downLoadHandle = Convert.ToInt32(downloadAndCaptureUserControls.DownLoadHandle.Text.ToString());
                int resultCode = OcxHelper.OCX.IVS_OCX_DownloadResume((uint)downLoadHandle);
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_DownloadResume(downLoadHandle);";

                if (resultCode == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("DownloadResume") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(resultCode.ToString());
                }
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @"OcxHelper.OCX.IVS_OCX_DownloadResume((uint)downLoadHandle);";
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }
        public void GetDownloadInfoCommandProcess()
        {
            try
            {             
                uint downLoadHandle = Convert.ToUInt32(downloadAndCaptureUserControls.DownLoadHandle.Text.ToString());
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.OCX.IVS_OCX_GetDownloadInfo(downLoadHandle);";
                bool fristExe = true;
                DispatcherTimer timer = new DispatcherTimer();
                //初始化定时器
                timer.Interval = TimeSpan.FromSeconds(1);
                timer.IsEnabled = true;
                timer.Tick += new EventHandler((sender, args) => 
                {
                    string responseMsg = OcxHelper.OCX.IVS_OCX_GetDownloadInfo(downLoadHandle);
                    var ocxResult = XmlHelper.ProcessOcxResponseMsg(responseMsg);

                    if (ocxResult.ResultCode != "0")
                    {
                        timer.IsEnabled = false;
                        timer.Stop();
                    }
                    else if (fristExe == true)
                    {
                        OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("GetDownloadInfo") + StringHelper.FindLanguageResource("Success");
                    }

                    if (ocxResult.ResultCode == "0")
                    {
                        var tempNode = XmlHelper.DeserializeToList<RecordDownloadInfoEntity>(ocxResult.BodyXmlNode);
                        
                        downloadAndCaptureUserControls.RecordList.ItemsSource = tempNode;
                        if (tempNode[0].Progress == 100)
                        {
                            timer.IsEnabled = false;
                            timer.Stop();
                        }
                        fristExe = false;
                    }
                });            
             
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @"OcxHelper.OCX.IVS_OCX_GetDownloadInfo(downLoadHandle);";
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return;
            }
        }
        public void LocalSnapshotCommandProcess()
        {
            try
            {
                CaptureConfigEntity reqEntity = new CaptureConfigEntity();
                reqEntity.CaptureDownloadPath = downloadAndCaptureUserControls.SnapshotFielName.Text.ToString();
                reqEntity.CapturePath = downloadAndCaptureUserControls.SnapshotFielName.Text.ToString();
                reqEntity.NameRule = NameRuleType.NameRule1;
                reqEntity.SnapshotCount = 1;
                reqEntity.SnapshotFormat = downloadAndCaptureUserControls.PicType.SelectedIndex == 0 ? SnapshotFormat.Jpg : SnapshotFormat.Bmp;
                reqEntity.SnapshotInterval = 1;
                reqEntity.SnapshotMode = "0";

                string msgReq = XmlHelper.SerializeToXml(reqEntity);
                //OcxHelper.OCX.IVS_OCX_SetLocalCaptureConfig(msgReq);
                int wndID = Convert.ToInt32(downloadAndCaptureUserControls.WndID.Text.ToString());
                int resultCode = OcxHelper.OCX.IVS_OCX_LocalSnapshot(wndID);
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @"int resultCode = OcxHelper.IVS_OCX_LocalSnapshot(wndID);";

                if (resultCode == 0)
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("LocalSnapshot") + StringHelper.FindLanguageResource("Success");
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(resultCode.ToString());
                }
                downloadAndCaptureUserControls.eSDKCodeTextBlock.Text = @" OcxHelper.OCX.IVS_OCX_SetLocalCaptureConfig(msgReq);OcxHelper.OCX.IVS_OCX_LocalSnapshot(wndID);";
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

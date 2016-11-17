#region 文件头注释
/*******************************************************************************
// 版权所有  2013 华为技术有限公司
// 程 序 集：ivs_cs_demo
// 文 件 名：ReplayViewModel.cs
// 文件说明：回放视图
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
using System.Threading;
using System.Windows;

namespace ivs_cs_demo
{
    public class ReplayViewModel
    {
        public ICommand QuerytCommand { get; set; }
        public ICommand StopLocalReplayCommand { get; set; }
        public ICommand StartLocalReplayCommand { get; set; } 
        public ICommand PauseReplayCommand { get; set; }
        public ICommand ResumeReplayCommand { get; set; }
        public ICommand FrameReplayCommand { get; set; }
        public ICommand FastReplayCommand { get; set; }
        public ICommand BackReplayCommand { get; set; }
        public ICommand PlayBackTimeCommand { get; set; }
        public ICommand Copy { get; set; }

       public ReplayUserControls replayUserControls { get; set; }
       public ReplayViewModel(ReplayUserControls window)
       {
           replayUserControls = window;

           QuerytCommand = new DelegateCommand(QueryProcess);
           StopLocalReplayCommand = new DelegateCommand(StopLocalReplayProcess);
           StartLocalReplayCommand = new DelegateCommand(StartLocalReplayProcess);
           PauseReplayCommand = new DelegateCommand(PauseReplayProcess);
           ResumeReplayCommand = new DelegateCommand(ResumeReplayProcess);
           FrameReplayCommand = new DelegateCommand(FrameReplayProcess);
           FastReplayCommand = new DelegateCommand(FastReplayProcess);
           BackReplayCommand = new DelegateCommand(BackReplayProcess);
           PlayBackTimeCommand = new DelegateCommand(PlayBackTimeProcess);
           Copy = new DelegateCommand(CopyProcess);
       }
       private void QueryProcess()
       {
           try
           {
               var result = new RecordQueryResultEntity();

               PageInfo pageInfo = new PageInfo();
               pageInfo.FromIndex = 1;
               pageInfo.PageSize = 200;
               pageInfo.ToIndex = 200;
               pageInfo.QueryCondition = new QueryCondition();

               QueryField startTime = new QueryField();//开始时间
               startTime.Field = "FromTime";
               startTime.QueryType = QueryType.Exact;

               DateTime realFromTime = DateTime.ParseExact(replayUserControls.StartTime.Text.ToString(), "yyyyMMddHHmmss", Thread.CurrentThread.CurrentCulture);
               startTime.Value =  DateTimeExtensions.ToDstUtcTime(realFromTime, true).ToString("yyyyMMddHHmmss");
               pageInfo.QueryCondition.QueryFieldList.Add(startTime);

               QueryField endTime = new QueryField();//结束时间
               endTime.Field = "ToTime";
               endTime.QueryType = QueryType.Exact;

               DateTime realEndTime = DateTime.ParseExact(replayUserControls.EndTime.Text.ToString(), "yyyyMMddHHmmss", Thread.CurrentThread.CurrentCulture);
               endTime.Value = DateTimeExtensions.ToDstUtcTime(realEndTime, false).ToString("yyyyMMddHHmmss");
               pageInfo.QueryCondition.QueryFieldList.Add(endTime);

               QueryField cameraCode = new QueryField();//摄像机编号
               cameraCode.Field = "CameraCode";
               cameraCode.QueryType = QueryType.Exact;
               cameraCode.Value = replayUserControls.CameraCode.Text.ToString();
               pageInfo.QueryCondition.QueryFieldList.Add(cameraCode);

               QueryField recordMethod = new QueryField();//录像方式
               recordMethod.Field = "RecordMethod";
               recordMethod.QueryType = QueryType.Exact;
               recordMethod.Value = replayUserControls.RecordType.SelectedIndex.ToString();
               pageInfo.QueryCondition.QueryFieldList.Add(recordMethod);

               QueryField recordType = new QueryField();//录像类型
               recordType.Field = "RecordType";
               recordType.QueryType = QueryType.Exact;
               recordType.Value = "111";
               pageInfo.QueryCondition.QueryFieldList.Add(recordType);

               QueryField queryType = new QueryField();//查询方式
               queryType.Field = "QueryType";
               queryType.QueryType = QueryType.Exact;
               queryType.Value = "0";
               pageInfo.QueryCondition.QueryFieldList.Add(queryType);

               QueryField alarmTypeList = new QueryField();//告警类型列表
               alarmTypeList.Field = "AlarmTypeList";
               alarmTypeList.QueryType = QueryType.Exact;
               alarmTypeList.Value = "ALARM_TYPE_DI,ALARM_NO_SIGNAL,ALARM_SHIELD,ALARM_TYPE_MOVE_DECTION,ALARM_VIDEO_G,ALARM_REVERSE_DETECTION,ALARM_INTRUSION,ALARM_TRIP_LINE,ALARM_OBJECT_ABANDONED,ALARM_OBJECT_REMOVAL,ALARM_PRESENCE,ALARM_PATH,ALARM_DIRECTION_MOVE,ALARM_SPEED,ALARM_SPEED_MONITOR,ALARM_OBJECT_WANDER,ALARM_DENSITY_MONITOR,ALARM_FACE_RECOGNITION_WARNING,ALARM_FACE_RECOGNITION,ALARM_FACE_CAPTURE,ALARM_MAU_VQD_BRIGHTNESS,ALARM_MAU_VQD_DARKNESS,ALARM_MAU_VQD_BLUR,ALARM_MAU_VQD_SNOWNOISE,ALARM_MAU_VQD_COLORCAST,ALARM_MAU_VQD_STRIPENOISE,ALARM_MAU_VQD_FRAMEFROZEN,ALARM_MAU_VQD_SHAKE,ALARM_MAU_VQD_CAMERACOVERD,ALARM_MAU_VIDEO_LOST,EXSYS_ALARM_EMERGENCY_ON,EXSYS_ALARM_FIRE_ALERT_ON,EXSYS_ALARM_ROBBERY_ON,EXSYS_ALARM_STEAL_ON,EXSYS_ALARM_ALERT_ON,EXSYS_ALARM_MONITOR_ON,EXSYS_ALARM_FAILURE_ON,EXSYS_ALARM_DEFENCE_ON,EXSYS_ALARM_REMOTE_CONTROL_ON,EXSYS_ALARM_ENTER_EXIT_ON,EXSYS_ALARM_STATUS_ON,EXSYS_ALARM_SHUTDOWN_ON,EXSYS_ALARM_BYPASS_ON,EXSYS_ALARM_TEST_ON,EXSYS_ALARM_USER_OPERATION_ON,EXSYS_ALARM_ERROR_ON,SCADA_ALARM";
               pageInfo.QueryCondition.QueryFieldList.Add(alarmTypeList);

               QueryField operatorID = new QueryField();//执行查询操作的用户ID
               operatorID.Field = "OperatorID";
               operatorID.QueryType = QueryType.Exact;
               operatorID.Value = LoginInfo.CurrentLoginInfo.UserId;
               pageInfo.QueryCondition.QueryFieldList.Add(operatorID);

               QueryField operatorDomain = new QueryField();//
               operatorDomain.Field = "OperatorDomain";
               operatorDomain.QueryType = QueryType.Exact;
               operatorDomain.Value = LoginInfo.CurrentLoginInfo.OperatorDomain;
               pageInfo.QueryCondition.QueryFieldList.Add(operatorDomain);

               var requestXml = XmlHelper.SerializeToXml(pageInfo);
               requestXml = XmlHelper.GetContentXml(requestXml);

               var responseMsg = OcxHelper.OCX.IVS_OCX_QueryRecordList(requestXml);

               var ocxResult = XmlHelper.ProcessOcxResponseMsg(responseMsg);

               if (ocxResult.ResultCode != "0")
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(ocxResult.ResultCode.ToString());
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("Query") + StringHelper.FindLanguageResource("Success");
               }

               var node = ocxResult.BodyXmlNode.SelectSingleNode("PageInfo");

               if (node != null)
               {
                   var tempNode = XmlHelper.DeserializeToEntity<PageInfo>(node);
                   result.PageInfo = tempNode;
               }

               node = ocxResult.BodyXmlNode.SelectSingleNode("RecordDataList");

               if (node != null)
               {
                   var tempNode = XmlHelper.DeserializeToList<RecordDataInfoEntity>(node);

                   foreach (RecordDataInfoEntity info in tempNode)
                   {
                       info.CameraCode = replayUserControls.CameraCode.Text.ToString();
 
                       info.LocalStartTime = DateTime.ParseExact(info.StartTime, "yyyyMMddHHmmss", Thread.CurrentThread.CurrentCulture).ToLocalTime().ToString();
                       info.LocalEndTime = DateTime.ParseExact(info.EndTime, "yyyyMMddHHmmss", Thread.CurrentThread.CurrentCulture).ToLocalTime().ToString();
                   }
                   replayUserControls.RecordList.ItemsSource = tempNode;
               }
               else
               {
                   replayUserControls.RecordList.ItemsSource = null;
               }
               replayUserControls.eSDKCodeTextBlock.Text = @"var responseMsg = OcxHelper.OCX.IVS_OCX_QueryRecordList(requestXml);";

           }
           catch (Exception ex)
           {
               LogService.Error("Server Record Search Error Code:" + ex.ToString());             
           }
            
       }

       private void StartLocalReplayProcess()
       {
           try
           {
               OcxHelper.InitOcxPaneState(int.Parse(replayUserControls.WndID.Text.ToString()));
               PlaybackParam para = new PlaybackParam();
               para.StartTime = "";
               para.EndTime = "";
               para.ProtocolType = "2";
               para.Speed = "1";
               para.pPWD = "";
               var requestXml = XmlHelper.SerializeToXml(para);
               requestXml = XmlHelper.GetContentXml(requestXml);

               int result = 0;
               result = OcxHelper.OCX.IVS_OCX_StartLocalPlayBack(int.Parse(replayUserControls.WndID.Text.ToString()), replayUserControls.LocalFileName.Text.ToString(), requestXml);
               if (result == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StartLocalReplay") + StringHelper.FindLanguageResource("Success");
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
               }
               replayUserControls.eSDKCodeTextBlock.Text = @"var result = OcxHelper.OCX.IVS_OCX_StartLocalPlayBack(int.Parse(replayUserControls.WndID.Text.ToString()), replayUserControls.LocalFileName.Text.ToString(), requestXml);";
           }
           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
               OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
               return;
           }

       }
       private void StopLocalReplayProcess()
       {
           try
           {
               int result = 0;           
               result = OcxHelper.OCX.IVS_OCX_StopLocalPlayBack(int.Parse(replayUserControls.WndID.Text.ToString()));
              
               if (result == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StopLocalReplay") + StringHelper.FindLanguageResource("Success");
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
               }
               replayUserControls.eSDKCodeTextBlock.Text = @"var result = OcxHelper.OCX.IVS_OCX_StopLocalPlayBack(int.Parse(replayUserControls.WndID.Text.ToString()));";
           }

           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
               OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
               return;
           }
       }

       private void PauseReplayProcess()
       {
           try
           {
               int result = 0;
               result = OcxHelper.OCX.IVS_OCX_PlayBackPause((int)OcxHelper.OCX.IVS_OCX_GetSelectWnd());
               if (result == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("PauseReplay") + StringHelper.FindLanguageResource("Success");
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
               }
               replayUserControls.eSDKCodeTextBlock.Text = @"var result = OcxHelper.OCX.IVS_OCX_PlayBackPause((int)OcxHelper.OCX.IVS_OCX_GetSelectWnd());";

           }

           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
               OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
               return;
           }
       }
       private void ResumeReplayProcess()
       {
           try
           {
               int result = 0;
               result = OcxHelper.OCX.IVS_OCX_PlayBackResume((int)OcxHelper.OCX.IVS_OCX_GetSelectWnd());
               if (result == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("ResumeReplay") + StringHelper.FindLanguageResource("Success");
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
               }
               replayUserControls.eSDKCodeTextBlock.Text = @"var result = OcxHelper.OCX.IVS_OCX_PlayBackResume((int)OcxHelper.OCX.IVS_OCX_GetSelectWnd());";
           }

           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
               OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
               return;
           }
       }
       private void FrameReplayProcess()
       {
           try
           {
               int result = 0;
               result = OcxHelper.OCX.IVS_OCX_PlaybackFrameStepForward((int)OcxHelper.OCX.IVS_OCX_GetSelectWnd());
               if (result == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("FrameReplay") + StringHelper.FindLanguageResource("Success");
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
               }
               replayUserControls.eSDKCodeTextBlock.Text = @"var result = OcxHelper.OCX.IVS_OCX_PlaybackFrameStepForward((int)OcxHelper.OCX.IVS_OCX_GetSelectWnd());";
           }

           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
               OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
               return;
           }
       }
       private void FastReplayProcess()
       {
           try
           {
               int result = 0;
               result = OcxHelper.OCX.IVS_OCX_SetPlayBackSpeed((int)OcxHelper.OCX.IVS_OCX_GetSelectWnd(),4);
               if (result == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("FastReplay") + StringHelper.FindLanguageResource("Success");
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
               }
               replayUserControls.eSDKCodeTextBlock.Text = @"var result = OcxHelper.OCX.IVS_OCX_SetPlayBackSpeed((int)OcxHelper.OCX.IVS_OCX_GetSelectWnd(),4);";
           }

           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
               OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
               return;
           }
       }
       private void BackReplayProcess()
       {
           try
           {
               int result = 0;
               result = OcxHelper.OCX.IVS_OCX_SetPlayBackSpeed((int)OcxHelper.OCX.IVS_OCX_GetSelectWnd(), -4);
               if (result == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("BackReplay") + StringHelper.FindLanguageResource("Success");
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
               }
               replayUserControls.eSDKCodeTextBlock.Text = @"var result = OcxHelper.OCX.IVS_OCX_SetPlayBackSpeed((int)OcxHelper.OCX.IVS_OCX_GetSelectWnd(),-4);";
           }

           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
               OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
               return;
           }
           
       }
       public void StartReplayProcess()
       {
           try
           {
               RecordDataInfoEntity record = replayUserControls.RecordList.SelectedItem as RecordDataInfoEntity;
               replayUserControls.RecordList.CommitEdit();//刷新数据的编辑
               OcxHelper.InitOcxPaneState(Convert.ToInt32(record.WndID));

               PlaybackParam para = new PlaybackParam();
               para.StartTime = record.StartTime;
               para.EndTime = record.EndTime;
               para.NVRCode = record.NvrCode;
               para.ProtocolType = "2";
               para.Speed = "1";

               var requestXml = XmlHelper.SerializeToXml(para);
               requestXml = XmlHelper.GetContentXml(requestXml);

               int result = 0;
               if (replayUserControls.RecordType.SelectedIndex == 0)
               {
                   result = OcxHelper.OCX.IVS_OCX_StartPlatformPlayBack(record.CameraCode, requestXml, Convert.ToInt32(record.WndID));
               }
               else
               {
                   result = OcxHelper.OCX.IVS_OCX_StartPUPlayBack(record.CameraCode, requestXml, Convert.ToInt32(record.WndID));
               }
               if (result == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StartReplay") + StringHelper.FindLanguageResource("Success");
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
               }
               replayUserControls.eSDKCodeTextBlock.Text = @"var result = OcxHelper.OCX.IVS_OCX_StartPlatformPlayBack(inrecord.CameraCode, requestXml, Convert.ToInt32(record.WndID));";
           }
           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
               OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
               return;
           }

       }
       public void StopReplayProcess()
       {
           try
           {
               int result = 0;
               RecordDataInfoEntity record = replayUserControls.RecordList.SelectedItem as RecordDataInfoEntity;
               replayUserControls.RecordList.CommitEdit();//刷新数据的编辑

               if (replayUserControls.RecordType.SelectedIndex == 0)
               {
                   result = OcxHelper.OCX.IVS_OCX_StopPlatformPlayBack(Convert.ToInt32(record.WndID));
               }
               else
               {
                   result = OcxHelper.OCX.IVS_OCX_StopPUPlayBack(Convert.ToInt32(record.WndID));
               }
               if (result == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("StopReplay") + StringHelper.FindLanguageResource("Success");
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
               }
               replayUserControls.eSDKCodeTextBlock.Text = @"var result = OcxHelper.OCX.IVS_OCX_StopBackupPlayBack(Convert.ToInt32(record.WndID));";

           }

           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
               OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
               return;
           }
       }


       private void PlayBackTimeProcess()
       {
           try
           {
               int result = 0;
               result = OcxHelper.OCX.IVS_OCX_SetPlayBackTime((int)OcxHelper.OCX.IVS_OCX_GetSelectWnd(), UInt32.Parse(replayUserControls.Speed.Text));
               if (result == 0)
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("PlayBackTime") + StringHelper.FindLanguageResource("Success");
               }
               else
               {
                   OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource(result.ToString());
               }
               replayUserControls.eSDKCodeTextBlock.Text = @"var result = OcxHelper.OCX.IVS_OCX_SetPlayBackTime((int)OcxHelper.OCX.IVS_OCX_GetSelectWnd(), UInt32.Parse(replayUserControls.Speed.Text));";
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
               RecordDataInfoEntity record =(replayUserControls.RecordList.SelectedItem as RecordDataInfoEntity);
               if (record != null)
               {
                   OcxHelper.MainWin.downloadAndCaptureUserControls.CameraCode.Text = record.CameraCode;
                   OcxHelper.MainWin.downloadAndCaptureUserControls.StartTime.Text = Convert.ToDateTime(record.LocalStartTime).ToString("yyyyMMddHHmmss");
                   OcxHelper.MainWin.downloadAndCaptureUserControls.EndTime.Text = Convert.ToDateTime(record.LocalEndTime).ToString("yyyyMMddHHmmss");
               }
           }
           catch (Exception ex)
           {
               LogService.Error(ex.ToString());
           }
       }
    }
}

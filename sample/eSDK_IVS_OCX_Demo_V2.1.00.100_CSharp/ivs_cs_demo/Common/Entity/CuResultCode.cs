using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ivs_cs_demo
{
    public class CuResultCode
    {
        #region 公共
        public static readonly string EnterFullDisplayFaild = "999102601";
        public static readonly string EnterNormalDisplayFaild = "999102602";
        public static readonly string OcxException = "999102699";
        #endregion

        #region 录像回放错误码

        public static readonly string SearchServerRecordFaild = "999119601";
        public static readonly string SearchLocalRecordFaild = "999119602";
        public static readonly string SearchRecordBookmarkFaild = "999119603";
        public static readonly string AddRecordBookmarkFaild = "999119604";
        public static readonly string DeleteRecordBookmarkFaild = "999119605";
        public static readonly string ModifyRecordBookmarkFaild = "999119606";
        public static readonly string QueryMotionDataFaild = "999119607";
        public static readonly string StartPlatformRecordDownloadFaild = "999119608";
        public static readonly string StopPlatformRecordDownloadFaild = "999119609";
        public static readonly string StartPuRecordDownloadFaild = "999119610";
        public static readonly string StopPuRecordDownloadFaild = "999119611";
        public static readonly string StartBackupRecordDownloadFaild = "999119612";
        public static readonly string StopBackupRecordDownloadFaild = "999119613";
        public static readonly string StartRecoverRecordDownloadFaild = "999119614";
        public static readonly string StopRecoverRecordDownloadFaild = "999119615";
        public static readonly string PauseRecordDownloadFaild = "999119616";
        public static readonly string ResumeRecordDownloadFaild = "999119617";
        public static readonly string GetRecordDownloadInfoFaild = "999119618";
        public static readonly string LockRecordFaild = "999119619";
        public static readonly string UnlockRecordFaild = "999119620";
        public static readonly string ModifyRecordLockDescFaild = "999119621";
        public static readonly string StartRestoreRecordDownloadFaild = "999119622";
        public static readonly string StopRestoreRecordDownloadFaild = "999119623";
        public static readonly string GetRecordThumbnailFaild = "999119624";

        public static readonly string StartPlatReplayFaild = "999119625";
        public static readonly string StopPlatReplayFaild = "999119626";
        public static readonly string PauseReplayFaild = "999119627";
        public static readonly string ResumeReplayFaild = "999119628";
        public static readonly string SetReplayTimeFaild = "999119629";
        public static readonly string SetReplaySpeedFaild = "999119630";
        public static readonly string SingleFrameReplayFaild = "999119631";
        public static readonly string StartSyncReplayFaild = "999119632";
        public static readonly string StopSyncReplayFaild = "999119633";
        public static readonly string StartPuReplayFaild = "999119634";
        public static readonly string StopPuReplayFaild = "999119635";
        public static readonly string StartBackupReplayFaild = "999119636";
        public static readonly string StopBackupReplayFaild = "999119637";
        public static readonly string StartLocalReplayFaild = "999119638";
        public static readonly string StopLocalReplayFaild = "999119639";
        public static readonly string StartRestoreReplayFaild = "999119640";
        public static readonly string StopRestoreReplayFaild = "999119641";
        public static readonly string GetReplayTimeFaild = "999119642";
        public static readonly string SwitchWatermarkFaild = "999119643";
        #endregion

        #region 备份管理错误码
        public static readonly string GetBackupInfoFaild = "999125601";
        public static readonly string SetBackupInfoFaild = "999125602";
        public static readonly string AddBackupTaskFaild = "999125603";
        #endregion

        #region 告警管理错误码

        public static readonly string QueryAlarmTypeFaild = "999120601";
        public static readonly string QueryAlarmInfoFaild = "999120602";

        #endregion



    }
}

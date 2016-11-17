#region 文件头注释
/*******************************************************************************
// 版权所有  2013 华为技术有限公司
// 程 序 集：ivs_cs_demo
// 文 件 名：frmLocalOCX.cs
// 文件说明：播放控件视图 
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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Globalization;

namespace ivs_cs_demo
{
    public partial class frmLocalOCX : Form
    {
        public AxIVS_OCXPlayer.AxIVS_OCXPlayer OcxPlayer { get { return axIVS_OCXPlayer; } }
        public frmLocalOCX()
        {
            InitializeComponent();
            this.FormBorderStyle = FormBorderStyle.None;
            this.ShowInTaskbar = false;
            this.Dock = DockStyle.Fill;

            this.FormClosed += new FormClosedEventHandler(frmLocalOCX_FormClosed);
            this.FormClosing += new FormClosingEventHandler(frmLocalOCX_FormClosing);
        }

        void frmLocalOCX_FormClosing(object sender, FormClosingEventArgs e)
        {
            axIVS_OCXPlayer.Dispose();
        }

        void frmLocalOCX_FormClosed(object sender, FormClosedEventArgs e)
        {
            LogService.Info("frmLocalOCX_FormClosed");
        }

        protected override void WndProc(ref System.Windows.Forms.Message m)
        {
            base.WndProc(ref m);
            switch (m.Msg)
            {
                case 528:
                    if (m.WParam.ToInt32() == 516)
                    {
                        if (MouseRightButtonDown != null)
                        {
                            MouseRightButtonDown();
                        }
                    }
                    break;
            }
        }

        public event Action MouseRightButtonDown;

        public int IVS_OCX_GetMouseWnd()
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_GetMouseWnd();
                LogService.Info("frmLocalOCX IVS_OCX_GetMouseWnd result is " + result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }

        public int IVS_OCX_SetDisplayScale(int ulDisplayScale)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_SetDisplayScale(ulDisplayScale);
                LogService.Info("frmLocalOCX IVS_OCX_SetDisplayScale result is " + result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }

        public int IVS_OCX_SetTitlebar(int show)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_SetToolbar((uint)show);
                LogService.Info("frmLocalOCX IVS_OCX_SetTitlebar result is " + result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }

        public int IVS_OCX_ShowTitlebar(bool show)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_ShowTitlebar(show);
                LogService.Info("frmLocalOCX IVS_OCX_ShowTitlebar result is " + result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }

        public int IVS_OCX_ShowToolbar(bool show)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_ShowToolbar(show);
                LogService.Info("frmLocalOCX IVS_OCX_ShowToolbar result is " + result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }

        public int IVS_OCX_SetWndLayout(uint lWindowsNum)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_SetWndLayout(lWindowsNum);
                LogService.Info("frmLocalOCX IVS_OCX_SetWndLayout result is " + result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }

        public uint IVS_OCX_SetActiveWnd(uint wndId)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_SetActiveWnd(wndId);
                LogService.Info("frmLocalOCX IVS_OCX_SetActiveWnd result is " + result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return 0;
            }
        }

        public uint IVS_OCX_GetSelectWnd()
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_GetSelectWnd();
                LogService.Info("frmLocalOCX IVS_OCX_GetSelectWnd result is " + result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return 0;
            }
        }

        //public int IVS_OCX_EnableGPU(int bEnable)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_EnableGPU(bEnable);
        //        LogService.Info("frmLocalOCX IVS_OCX_EnableGPU result is " + result.ToString());
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}

        public int IVS_OCX_Cleanup()
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_Cleanup();
                LogService.Info("frmLocalOCX IVS_OCX_Cleanup result is " + result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }

        public int IVS_OCX_Init()
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_Init();
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error(ex);
                return -1;
            }
        }

        public int IVS_OCX_SetToolbar(uint pToolbar)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_SetToolbar(pToolbar);
                LogService.Info("frmLocalOCX IVS_OCX_SetToolbar result is " + result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }

        public int IVS_OCX_StartRealPlay(string pCameraCode, int lWndId, int stream = -1, string mediaParaXml="")
        {
            try
            {
                //if (mediaParaXml == "")
                //{
                //    mediaParaXml = GetMediaPara(pCameraCode, stream).ToString();
                //    LogService.Info(mediaParaXml + " " + pCameraCode + " " + lWndId.ToString());
                //}
                var result = OcxPlayer.IVS_OCX_StartRealPlay(mediaParaXml, pCameraCode, lWndId);
                LogService.Info(result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }

        }
        //private LocalParameterSetEntity localParam;
        //public LocalParameterSetEntity LocalParam
        //{
        //    get
        //    {
        //        string localNetWorkXml = OcxPlayer.IVS_OCX_GetLocalNetWorkConfig();
        //        if (!string.IsNullOrWhiteSpace(localNetWorkXml))
        //        {
        //            localParam = XmlHelper.DeserializeToEntity<LocalParameterSetEntity>(localNetWorkXml);
        //        }
        //        if (localParam == null) localParam = new LocalParameterSetEntity();

        //        return localParam;
        //    }
        //}
        //public MediaPara GetMediaPara(string cameraCode, int stream = -1)
        //{
        //    MediaPara mediaPara;
        //    LocalParameterSetEntity localParameter = LocalParam;
        //    if (localParameter == null)
        //    {
        //        mediaPara = new MediaPara
        //        {
        //            StreamType = stream == -1 ? 0 : stream,
        //        };
        //    }
        //    else
        //    {
        //        mediaPara = new MediaPara
        //        {
        //            StreamType = stream == -1 ? int.Parse(localParameter.StreamType) : stream,
        //            ProtoclType = (int)localParameter.ProtocolType,
        //            MultiCase = (int)localParameter.MultiFlag,
        //        };
        //    }
        //    mediaPara.DirectFirst = IVS_OCX_GetLocalCameraLinkConfig(cameraCode);
        //    return mediaPara;
        //}

        public int IVS_OCX_SetDeviceName(uint ulWindowID, string pDevName)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_SetDeviceName(ulWindowID, pDevName);
                LogService.Info("frmLocalOCX IVS_OCX_SetDeviceName result is " + result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }

        public int IVS_OCX_GetFreeWnd()
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_GetFreeWnd();
                LogService.Info("frmLocalOCX IVS_OCX_GetFreeWnd result is " + result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }

        //public string IVS_OCX_GetMediaInfo(uint ulWndID)
        //{
        //    try
        //    {
        //        LogService.Info("IVS_OCX_GetMediaInfo input " + ulWndID.ToString());
        //        var result = OcxPlayer.IVS_OCX_GetMediaInfo(ulWndID);
        //        LogService.Info("IVS_OCX_GetMediaInfo result " + result.ToString());
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
        //        return "";
        //    }
        //}




        public uint IVS_OCX_StartLocalRecord(LocalRecordParam pRecordPara, int lWndID)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_StartLocalRecord(string.Format("<Content>{0}</Content>", XmlHelper.SerializeToXml(pRecordPara)), lWndID);
                LogService.Info("frmLocalOCX IVS_OCX_StartLocalRecord result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return 0;
            }
        }

        public int IVS_OCX_StopLocalRecord(int lWndID)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_StopLocalRecord(lWndID);
                LogService.Info("frmLocalOCX IVS_OCX_StopLocalRecord result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }

        public int IVS_OCX_StopRealPlay(int lWndID)
        {
            try
            {
                LogService.Info("IVS_OCX_GetMediaInfo input " + lWndID.ToString());
                var result = OcxPlayer.IVS_OCX_StopRealPlay(lWndID);
                LogService.Info("IVS_OCX_GetMediaInfo result " + result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }

        public int IVS_OCX_SetWndDrag(uint ulWndID, uint ulEnable)
        {
            return OcxPlayer.IVS_OCX_SetWndDrag(ulWndID, ulEnable);
        }

        #region 画图接口

        //public int IVS_OCX_EnableDraw(uint ulWindowID)
        //{
        //    return OcxPlayer.IVS_OCX_EnableDraw(ulWindowID);
        //}

        //public int IVS_OCX_DisableDraw(uint ulWindowID)
        //{
        //    return OcxPlayer.IVS_OCX_DisableDraw(ulWindowID);
        //}

        //public int IVS_OCX_SetGraphics(uint ulWindowID, string pGraphicsXml)
        //{
        //    return OcxPlayer.IVS_OCX_SetGraphics(ulWindowID, pGraphicsXml);
        //}

        public string IVS_OCX_GetGraphics(uint ulWindowID)
        {
            return OcxPlayer.IVS_OCX_GetGraphics(ulWindowID);
        }

        //public string IVS_OCX_GetGraphicsByPercent(uint ulWindowID)
        //{
        //    return OcxPlayer.IVS_OCX_GetGraphicsByPercent(ulWindowID);
        //}

        //public int IVS_OCX_SetGraphicsByPercent(uint ulWindowID, string pGraphicsXml)
        //{
        //    return OcxPlayer.IVS_OCX_SetGraphicsByPercent(ulWindowID, pGraphicsXml);
        //}

        //public string IVS_OCX_GetGraphicsByGroup(uint ulWindowID, uint ulGroupId)
        //{
        //    return OcxPlayer.IVS_OCX_GetGraphicsByGroup(ulWindowID, ulGroupId);
        //}

        //public int IVS_OCX_DeleteGraphicsSelected(uint ulWindowID)
        //{
        //    return OcxPlayer.IVS_OCX_DeleteGraphicsSelected(ulWindowID);
        //}

        //public int IVS_OCX_DeleteGraphics(uint ulWindowID)
        //{
        //    return OcxPlayer.IVS_OCX_DeleteGraphics(ulWindowID);
        //}

        //public int IVS_OCX_DeleteGraphicsByGroup(uint ulWindowID, uint ulGroupId)
        //{
        //    return OcxPlayer.IVS_OCX_DeleteGraphicsByGroup(ulWindowID, ulGroupId);
        //}

        //public int IVS_OCX_SelectGraphicByGroup(uint ulWindowID, uint ulGroupId)
        //{
        //    return OcxPlayer.IVS_OCX_SelectGraphicByGroup(ulWindowID, ulGroupId);
        //}

        //public int IVS_OCX_SelectCancel(uint ulWindowID)
        //{
        //    return OcxPlayer.IVS_OCX_SelectCancel(ulWindowID);
        //}

        //public int IVS_OCX_DrawGraphics(uint ulWindowID, string pGraphicsXml)
        //{
        //    return OcxPlayer.IVS_OCX_DrawGraphics(ulWindowID, pGraphicsXml);
        //}

        //public int IVS_OCX_StopDrawGraphics(uint ulWindowID, uint ulGraphicGroup)
        //{
        //    return OcxPlayer.IVS_OCX_StopDrawGraphics(ulWindowID, ulGraphicGroup);
        //}

        //public int IVS_OCX_SetRelationShip(uint ulWindowID, uint ulGraphicA, uint ulGraphicB, uint ulRelationShipType)
        //{
        //    return OcxPlayer.IVS_OCX_SetRelationShip(ulWindowID, ulGraphicA, ulGraphicB, ulRelationShipType);
        //}

        //public int IVS_OCX_IALocusOverly(uint ulWinID, uint ulRuleType, uint ulStartOverly)
        //{
        //    return OcxPlayer.IVS_OCX_IALocusOverly(ulWinID, ulRuleType, ulStartOverly);
        //}

        public int IVS_OCX_SetIAToolbar(uint dwToolbarBtn)
        {
            return OcxPlayer.IVS_OCX_SetIAToolbar(dwToolbarBtn);
        }

        public string IVS_OCX_QueryDeviceList(uint ulDeviceType, string pReqGetList)
        {
            return OcxPlayer.IVS_OCX_QueryDeviceList(ulDeviceType, pReqGetList);
        }


        #endregion

        //public uint IVS_OCX_Set3DPTZ(uint ulWindowID, uint ulStart3D)
        //{
        //    try
        //    {
        //        LogService.Info("IVS_OCX_Set3DPTZ" + ulWindowID.ToString() + " " + ulStart3D.ToString());
        //        uint result = OcxPlayer.IVS_OCX_Set3DPTZ(ulWindowID, ulStart3D);
        //        LogService.Info("IVS_OCX_Set3DPTZ result " + result.ToString());
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
        //        return 0;
        //    }
        //}

        //public int IVS_OCX_SetLocalPTZConfig(string ptzparam)
        //{
        //    try
        //    {
        //        LogService.Info("IVS_OCX_SetLocalPTZConfig" + ptzparam);
        //        uint result = OcxPlayer.IVS_OCX_SetLocalPTZConfig(ptzparam);
        //        LogService.Info("IVS_OCX_SetLocalPTZConfig result " + result.ToString());
        //        return (int)result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
        //        return 0;
        //    }
        //}

        public string IVS_OCX_PtzControl(string pCameraCode, int lOperateCode, string pControlPara1, string pControlPara2)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_PtzControl(pCameraCode, lOperateCode, pControlPara1, pControlPara2);
                LogService.Info("IVS_OCX_PtzControl result " + result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return "";
            }
        }

        //public int IVS_OCX_SetTVWallWndLayout(uint ulTVWallID)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_SetTVWallWndLayout(ulTVWallID);
        //        LogService.Info("frmLocalOCX IVS_OCX_SetTVWallWndLayout result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}

        //public int IVS_OCX_SetTVWallPaneStatus(uint ulTVWallID, uint ulPaneID, uint ulPaneStatus)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_SetTVWallPaneStatus(ulTVWallID, ulPaneID, ulPaneStatus);
        //        LogService.Info("frmLocalOCX IVS_OCX_SetTVWallPaneStatus result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}

        //public int IVS_OCX_StartRealPlayTVWall(uint ulTVWallID, string pCameraCode, string pRealplayParam, uint ulWndID)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_StartRealPlayTVWall(ulTVWallID, pCameraCode, pRealplayParam, ulWndID);
        //        LogService.Info("frmLocalOCX IVS_OCX_StartRealPlayTVWall result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}

        //public int IVS_OCX_StopRealPlayTVWall(uint ulTVWallID, uint ulWndID)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_StopRealPlayTVWall(ulTVWallID, ulWndID);
        //        LogService.Info("frmLocalOCX IVS_OCX_StopRealPlayTVWall result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}

        //public int IVS_OCX_StartPlatformPlayBackTVWall(uint ulTVWallID, string pCameraCode, string pPlaybackParam, uint ulWndID)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_StartPlatformPlayBackTVWall(ulTVWallID, pCameraCode, pPlaybackParam, ulWndID);
        //        LogService.Info("frmLocalOCX IVS_OCX_StartPlatformPlayBackTVWall result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}


        public int IVS_OCX_SetWndType(uint ulWndID, uint ulType)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_SetWndType(ulWndID, ulType);
                LogService.Info("frmLocalOCX IVS_OCX_SetWndType result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }

        public int IVS_OCX_GetWndType(uint ulWndID)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_GetWndType(ulWndID);
                LogService.Info("frmLocalOCX IVS_OCX_GetWndType result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }




        //public int IVS_OCX_SetCameraSwitchWnd(uint ulWndID, uint ulStyle)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_SetCameraSwitchWnd(ulWndID, ulStyle);
        //        LogService.Info("frmLocalOCX IVS_OCX_SetCameraSwitchWnd result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}




        public int IVS_OCX_Logout()
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_Logout();
                LogService.Info("frmLocalOCX IVS_OCX_Logout result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }



        public void IVS_OCX_RefreshWnd(uint itype)
        {
            try
            {
                OcxPlayer.IVS_OCX_RefreshWnd(itype);
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
            }
        }

        //public string IVS_OCX_GetDecoderList()
        //{
        //    try
        //    {
        //        return OcxPlayer.IVS_OCX_GetDecoderList();
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
        //        return string.Empty;
        //    }
        //}


        public int IVS_OCX_Login(string pUserName, string pPWD, string pServerIP, string pPort, uint ulClientType)
        {
            try
            {
                int result = OcxPlayer.IVS_OCX_Login(pUserName, pPWD, pServerIP, pPort, (uint)0);
                if (result == 0)
                {
                    string xml = OcxPlayer.IVS_OCX_GetUserID();
                    OcxResult ocxResult = XmlHelper.ProcessOcxResponseMsg(xml);

                    string DomainXml = OcxPlayer.IVS_OCX_GetDomainRoute();
                    IEnumerable<DomainEntity>  domainList =
                     XmlHelper.DeserializeToList<DomainEntity>(XmlHelper.ProcessOcxResponseMsg(DomainXml).BodyXmlNode.SelectSingleNode("DomainRoutes"));
                   
                    string operatorDomain = domainList.FirstOrDefault(p => string.IsNullOrEmpty(p.SuperDomain)).DomainCode;
                    LoginInfo.CurrentLoginInfo = new LoginInfo(pUserName, pPWD, pServerIP, pPort, ocxResult.BodyXmlNode.InnerText, operatorDomain);
                }
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return 0;
            }
        }


        //public int IVS_OCX_StopPlatformPlayBackTVWall(uint ulTVWallID, uint ulWndID)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_StopPlatformPlayBackTVWall(ulTVWallID, ulWndID);
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}


        public void SetOcxVisible(bool isvisible)
        {
            try
            {
                OcxPlayer.Visible = isvisible;
                OcxPlayer.Show();
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
            }
        }



        public int IVS_OCX_LocalSnapshot(int ulWinID)
        {
            try
            {

                return OcxPlayer.IVS_OCX_LocalSnapshot(ulWinID);
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>" + ex.StackTrace);
                return -1;
            }
        }




        //public string IVS_OCX_GetPanelWndID(int tvwallId, int panelId)
        //{
        //    try
        //    {
        //        return OcxPlayer.IVS_OCX_GetPaneWndID((uint)tvwallId, (uint)panelId);
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("IVS_OCX_GetPanelWndId Error == >>" + ex.StackTrace);
        //        return string.Empty;
        //    }
        //}




        //public void IVS_OCX_ShowPaneBar(int wndID, int isVisible)
        //{
        //    try
        //    {
        //        OcxPlayer.IVS_OCX_ShowPaneBar((uint)wndID, (uint)isVisible);
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("IVS_OCX_ShowPaneBar Error == >>" + ex.StackTrace);
        //    }
        //}

        //public string IVS_OCX_SetPaneEMap(int wndID)
        //{
        //    try
        //    {
        //        return OcxPlayer.IVS_OCX_SetPaneEMap((uint)wndID);
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("IVS_OCX_SetPaneEMap Error == >>" + ex.StackTrace);
        //        return string.Empty;
        //    }
        //}



        //public int IVS_OCX_SetWarnState(uint ulWndID, uint ulWarnState, uint ulWarnSeconds)
        //{
        //    try
        //    {
        //        return OcxPlayer.IVS_OCX_SetWarnState(ulWndID, ulWarnState, ulWarnSeconds);
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("IVS_OCX_SetWarnState Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}


        //public int IVS_OCX_StartBackupPlayBackTVWall(uint ulTVWallID, string pDomainCode, string pNvrCode, string pCameraCode, string pPlaybackParam, uint ulWndID)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_StartBackupPlayBackTVWall(ulTVWallID, pDomainCode, pNvrCode, pCameraCode, pPlaybackParam, (int)ulWndID);
        //        LogService.Info("EivsPlayerWindowSubprocessProxy IVS_OCX_StartBackupPlayBackTVWall result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("EivsPlayerWindowSubprocessProxy Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}

        //public int IVS_OCX_StopBackuplayBackTVWall(uint ulTVWallID, uint ulWndID)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_StopBackuplayBackTVWall(ulTVWallID, (int)ulWndID);
        //        LogService.Info("EivsPlayerWindowSubprocessProxy IVS_OCX_StopBackuplayBackTVWall result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("EivsPlayerWindowSubprocessProxy Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}

        //public int IVS_OCX_StartPUPlayBackTVWall(uint ulTVWallID, string pCameraCode, string pPlaybackParam, uint ulWndID)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_StartPUPlayBackTVWall(ulTVWallID, pCameraCode, pPlaybackParam, (int)ulWndID);
        //        LogService.Info("EivsPlayerWindowSubprocessProxy IVS_OCX_StartPUPlayBackTVWall result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("EivsPlayerWindowSubprocessProxy Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}

        //public int IVS_OCX_StopPUPlayBackTVWall(uint ulTVWallID, uint ulWndID)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_StopPUPlayBackTVWall(ulTVWallID, (int)ulWndID);
        //        LogService.Info("EivsPlayerWindowSubprocessProxy IVS_OCX_StopPUPlayBackTVWall result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("EivsPlayerWindowSubprocessProxy Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}

        //public int IVS_OCX_StartSyncPlayTVWall(uint ulTVWallID, string pSyncPlayInfo)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_StartSyncPlayTVWall(ulTVWallID, pSyncPlayInfo);
        //        LogService.Info("EivsPlayerWindowSubprocessProxy IVS_OCX_StartSyncPlayTVWall result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("EivsPlayerWindowSubprocessProxy Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}

        //public int IVS_OCX_StopSyncPlayTVWall(uint ulTVWallID)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_StopSyncPlayTVWall(ulTVWallID);
        //        LogService.Info("EivsPlayerWindowSubprocessProxy IVS_OCX_StopSyncPlayTVWall result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("EivsPlayerWindowSubprocessProxy Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}

        //public int IVS_OCX_PlayBackPauseTVWall(uint ulTVWallID, uint ulWndID)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_PlayBackPauseTVWall(ulTVWallID, (int)ulWndID);
        //        LogService.Info("EivsPlayerWindowSubprocessProxy IVS_OCX_PlayBackPauseTVWall result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("EivsPlayerWindowSubprocessProxy Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}

        //public int IVS_OCX_PlayBackResumeTVWall(uint ulTVWallID, uint ulWndID)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_PlayBackResumeTVWall(ulTVWallID, (int)ulWndID);
        //        LogService.Info("EivsPlayerWindowSubprocessProxy IVS_OCX_PlayBackResumeTVWall result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("EivsPlayerWindowSubprocessProxy Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}

        //public int IVS_OCX_PlaybackFrameStepForwardTVWall(uint ulTVWallID, uint ulWndID)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_PlaybackFrameStepForwardTVWall(ulTVWallID, (int)ulWndID);
        //        LogService.Info("EivsPlayerWindowSubprocessProxy IVS_OCX_PlaybackFrameStepForwardTVWall result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("EivsPlayerWindowSubprocessProxy Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}
        //public int IVS_OCX_GetLocalCameraLinkConfig(string code)
        //{
        //    try
        //    {
        //        uint result = OcxPlayer.IVS_OCX_GetLocalCameraLinkConfig(code);
        //        LogService.Info("IVS_OCX_GetLocalCameraLinkConfig  result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return (int)result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("IVS_OCX_GetLocalCameraLinkConfig Error == >>" + ex.StackTrace);
        //        return -1;
        //    }
        //}

        public string IVS_OCX_AddPTZPreset(string pCameraCode, string pPresetName)
        {
            try
            {
                string result = OcxPlayer.IVS_OCX_AddPTZPreset(pCameraCode, pPresetName);
                LogService.Info("IVS_OCX_AddPTZPreset result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("IVS_OCX_AddPTZPreset Error == >>" + ex.StackTrace);
                return string.Empty;
            }
        }

        public int IVS_OCX_DelPTZPreset(string pCameraCode, uint ulPresetIndex)
        {
            try
            {
                int result = OcxPlayer.IVS_OCX_DelPTZPreset(pCameraCode, ulPresetIndex);
                LogService.Info("IVS_OCX_DelPTZPreset result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("IVS_OCX_DelPTZPreset Error == >>" + ex.StackTrace);
                return -1;
            }
        }

        public string IVS_OCX_GetPTZPresetList(string pCameraCode)
        {
            try
            {
                string result = OcxPlayer.IVS_OCX_GetPTZPresetList(pCameraCode);
                LogService.Info("IVS_OCX_GetPTZPresetList result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("IVS_OCX_GetPTZPresetList Error == >>" + ex.StackTrace);
                return string.Empty;
            }
        }

        public string IVS_OCX_GetDomainRoute()
        {
            try
            {
                string result = OcxPlayer.IVS_OCX_GetDomainRoute();
                LogService.Info("IVS_OCX_GetDomainRoute result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("IVS_OCX_GetDomainRoute Error == >>" + ex.StackTrace);
                return string.Empty;
            }
        }
        public int IVS_OCX_SetLanguage(string pLanguage)
        {
            try
            {
                int result = OcxPlayer.IVS_OCX_SetLanguage(pLanguage);
                LogService.Info("IVS_OCX_SetLanguage result is " + result.ToString(CultureInfo.InvariantCulture));
                return -1;
            }
            catch (Exception ex)
            {
                LogService.Error("IVS_OCX_GetDomainRoute Error == >>" + ex.StackTrace);
                return -1;
            }
        }
    
        #region 录像回放
        public int IVS_OCX_StartPlatformPlayBack(string pCameraCode, string pPlaybackParam, int lWndID)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_StartPlatformPlayBack(pCameraCode, pPlaybackParam, lWndID);
                LogService.Info("frmLocalOCX IVS_OCX_StartPlatformPlayBack result is " + result.ToString(CultureInfo.InvariantCulture));

                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.StartPlatReplayFaild);
            }
        }

        public int IVS_OCX_StopPlatformPlayBack(int lWndID)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_StopPlatformPlayBack(lWndID);
                LogService.Info("frmLocalOCX IVS_OCX_StopPlatformPlayBack result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.StopPlatReplayFaild);
            }
        }

        public string IVS_OCX_GetPlayBackTime(int lWndID)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_GetPlayBackTime(lWndID);
                LogService.Info("IVS_OCX_GetMediaInfo result " + result.ToString());
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return ex.Message.ToString();
            }
        }


        public int IVS_OCX_SetPlayBackTime(int lWndID, uint ulTime)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_SetPlayBackTime(lWndID, ulTime);
                LogService.Info("frmLocalOCX IVS_OCX_StopLocalRecord result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.SetReplayTimeFaild);
            }
        }

        //public int IVS_OCX_StartDisasterRecoveryPlayBack(string pCameraCode, string pPlaybackParam, int lWndID)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_StartDisasterRecoveryPlayBack(pCameraCode, pPlaybackParam, lWndID);
        //        LogService.Info("frmLocalOCX IVS_OCX_StartDisasterRecoveryPlayBack result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("frmLocalOCX Error == >>", ex);
        //        return int.Parse(CuResultCode.StartRestoreReplayFaild);
        //    }
        //}

        //public int IVS_OCX_StopDisasterRecoveryPlayBack(int lWndID)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_StopDisasterRecoveryPlayBack(lWndID);
        //        LogService.Info("frmLocalOCX IVS_OCX_StopDisasterRecoveryPlayBack result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("frmLocalOCX Error == >>", ex);
        //        return int.Parse(CuResultCode.StopRestoreReplayFaild);
        //    }
        //}

        //public int IVS_OCX_StartBackupPlayBack(string pDomainCode, string pNVRCode, string pCameraCode, string pPlaybackParam, int lWndID)
        //{
        //    try
        //    {
        //        return OcxPlayer.IVS_OCX_StartBackupPlayBack(pDomainCode, pNVRCode, pCameraCode, pPlaybackParam, lWndID);
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("IVS_OCX_StartBackupPlayBack Error == >>", ex);
        //        return int.Parse(CuResultCode.StartBackupReplayFaild);
        //    }
        //}

        //public int IVS_OCX_StopBackupPlayBack(int lWndID)
        //{
        //    try
        //    {
        //        return OcxPlayer.IVS_OCX_StopBackupPlayBack(lWndID);
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("IVS_OCX_StopBackupPlayBack Error == >>", ex);
        //        return int.Parse(CuResultCode.StopBackupReplayFaild);
        //    }
        //}

        //public int IVS_OCX_VerifyWatermark(int lWndID, uint ulVerifyWatermark)
        //{
        //    try
        //    {
        //        return OcxPlayer.IVS_OCX_VerifyWatermark(lWndID, ulVerifyWatermark);
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("IVS_OCX_VerifyWatermark Error == >>", ex);
        //        return int.Parse(CuResultCode.SwitchWatermarkFaild);
        //    }
        //}

        public int IVS_OCX_StartLocalPlayBack(int lWndID, string pFileName, string pPlaybackParam)
        {
            try
            {
                return OcxPlayer.IVS_OCX_StartLocalPlayBack(lWndID, pFileName, pPlaybackParam);
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.StartLocalReplayFaild);
            }
        }

        public int IVS_OCX_StopLocalPlayBack(int lWndID)
        {
            try
            {
                return OcxPlayer.IVS_OCX_StopLocalPlayBack(lWndID);
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.StopLocalReplayFaild);
            }
        }

        public string IVS_OCX_QueryRecordList(string pUnifiedQuery)
        {
            try
            {
                return OcxPlayer.IVS_OCX_QueryRecordList(pUnifiedQuery);
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                throw new Exception(CuResultCode.SearchServerRecordFaild);
            }
        }

        public int IVS_OCX_FullScreenDisplay()
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_FullScreenDisplay();
                LogService.Info("frmLocalOCX IVS_OCX_FullScreenDisplay result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.EnterFullDisplayFaild);
            }
        }

        public int IVS_OCX_NormalScreenDisplay()
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_NormalScreenDisplay();
                LogService.Info("frmLocalOCX IVS_OCX_NormalScreenDisplay result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.EnterNormalDisplayFaild);
            }
        }

        public int IVS_OCX_PlayBackPause(int lWndID)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_PlayBackPause(lWndID);
                LogService.Info("frmLocalOCX IVS_OCX_PlayBackPause result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.PauseReplayFaild);
            }
        }

        public int IVS_OCX_PlayBackResume(int lWndID)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_PlayBackResume(lWndID);
                LogService.Info("frmLocalOCX IVS_OCX_PlayBackResume result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.ResumeReplayFaild);
            }
        }

        public int IVS_OCX_PlaybackFrameStepForward(int lWndID)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_PlaybackFrameStepForward(lWndID);
                LogService.Info("frmLocalOCX IVS_OCX_PlaybackFrameStepForward result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.SingleFrameReplayFaild);
            }
        }

        public int IVS_OCX_PlaybackFrameStepBackward(int lWndID)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_PlaybackFrameStepBackward(lWndID);
                LogService.Info("frmLocalOCX IVS_OCX_PlaybackFrameStepBackward result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.SingleFrameReplayFaild);
            }
        }

        public int IVS_OCX_SetPlayBackSpeed(int lWndID, float fSpeed)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_SetPlayBackSpeed(lWndID, fSpeed);
                LogService.Info("frmLocalOCX IVS_OCX_SetPlayBackSpeed result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.SetReplaySpeedFaild);
            }
        }

        public int IVS_OCX_StartSyncPlay(string pSyncPlayInfo)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_StartSyncPlay(pSyncPlayInfo);
                LogService.Info("frmLocalOCX IVS_OCX_StartSyncPlay result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.StartSyncReplayFaild);
            }
        }

        public int IVS_OCX_StopSyncPlay()
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_StopSyncPlay();
                LogService.Info("frmLocalOCX IVS_OCX_StopSyncPlay result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.StopSyncReplayFaild);
            }
        }

        public int IVS_OCX_StartPUPlayBack(string pCameraCode, string pPlaybackParam, int lWndID)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_StartPUPlayBack(pCameraCode, pPlaybackParam, lWndID);
                LogService.Info("frmLocalOCX IVS_OCX_StartPUPlayBack result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.StartPuReplayFaild);
            }
        }

        public int IVS_OCX_StopPUPlayBack(int lWndID)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_StopPUPlayBack(lWndID);
                LogService.Info("frmLocalOCX IVS_OCX_StopPUPlayBack result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.StopPuReplayFaild);
            }
        }

        public int IVS_OCX_SetVolume(int lWndID, uint ulVolumeValue)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_SetVolume(lWndID, ulVolumeValue);
                LogService.Info("frmLocalOCX IVS_OCX_SetVolume result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.OcxException);
            }
        }

        public uint IVS_OCX_GetVolume(int lWndID)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_GetVolume(lWndID);
                LogService.Info("frmLocalOCX IVS_OCX_GetVolume result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return uint.Parse(CuResultCode.OcxException);
            }
        }

        public uint IVS_OCX_PlaySound(int lWndID)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_PlaySound(lWndID);
                LogService.Info("frmLocalOCX IVS_OCX_PlaySound result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return uint.Parse(CuResultCode.OcxException);
            }
        }
        public int IVS_OCX_GetVersion()
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_GetVersion();
                LogService.Info("frmLocalOCX IVS_OCX_PlaySound result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.OcxException);
            }
        }
        public int IVS_OCX_ChangePWD(string pOldPWD, string pNewPWD)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_ChangePWD(pOldPWD, pNewPWD);
                LogService.Info("frmLocalOCX IVS_OCX_ChangePWD result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return int.Parse(CuResultCode.OcxException);
            }
        }

        //public string IVS_OCX_Aes256CbcDecode(string PWD)
        //{
        //    try
        //    {
        //        var result = OcxPlayer.IVS_OCX_Aes256CbcDecode(PWD);
        //        LogService.Info("frmLocalOCX IVS_OCX_Aes256CbcDecode result is " + result.ToString(CultureInfo.InvariantCulture));
        //        return result;
        //    }
        //    catch (Exception ex)
        //    {
        //        LogService.Error("frmLocalOCX Error == >>", ex);
        //        return (CuResultCode.OcxException);
        //    }
        //}
        public string IVS_OCX_GetCameraByWnd(uint ulWndID)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_GetCameraByWnd(ulWndID);
                LogService.Info("frmLocalOCX IVS_OCX_GetCameraByWnd result is " + result.ToString(CultureInfo.InvariantCulture));
                if (result == "1")
                {
                    return string.Empty;
                }
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return (CuResultCode.OcxException);
            }
        }

        public string IVS_OCX_GetDeviceList(uint ulDeviceType, uint ulFromIndex, uint ulToIndex)
        {
            try
            {
                var result = OcxPlayer.IVS_OCX_GetDeviceList(ulDeviceType, ulFromIndex, ulToIndex);
                LogService.Info("frmLocalOCX IVS_OCX_GetDeviceList result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return (CuResultCode.OcxException);
            }
        }
        public int IVS_OCX_StartPlatformRecord(string pCameraCode, uint ulRecordTime)
        {
            try
            {
                int result = OcxPlayer.IVS_OCX_StartPlatformRecord(pCameraCode, ulRecordTime);
                LogService.Info("frmLocalOCX IVS_OCX_StartPlatformRecord result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return -1;
            }
        }
        public int IVS_OCX_StopPlatformRecord(string pCameraCode)
        {
            try
            {
                int result = OcxPlayer.IVS_OCX_StopPlatformRecord(pCameraCode);
                LogService.Info("frmLocalOCX IVS_OCX_StopPlatformRecord result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return -1;
            }
        }
        public int IVS_OCX_StartPURecord(string pCameraCode, uint ulRecordTime)
        {
            try
            {
                int result = OcxPlayer.IVS_OCX_StartPURecord(pCameraCode, ulRecordTime);
                LogService.Info("frmLocalOCX IVS_OCX_StartPURecord result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return -1;
            }
        }
        public int IVS_OCX_StopPURecord(string pCameraCode)
        {
            try
            {
                int result = OcxPlayer.IVS_OCX_StopPURecord(pCameraCode);
                LogService.Info("frmLocalOCX IVS_OCX_StopPURecord result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return -1;
            }
        }
        public string IVS_OCX_StartVoiceTalkback(string pTalkParam, string pCameraCode)
        {
            try
            {
                string result = OcxPlayer.IVS_OCX_StartVoiceTalkback(pTalkParam,pCameraCode);
                LogService.Info("frmLocalOCX IVS_OCX_StartVoiceTalkback result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return string.Empty;
            }
        }
         public int IVS_OCX_StopVoiceTalkback(uint ulTalkbackHandle)
        {
            try
            {
                int result = OcxPlayer.IVS_OCX_StopVoiceTalkback(ulTalkbackHandle);
                LogService.Info("frmLocalOCX IVS_OCX_StopVoiceTalkback result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return -1;
            }
        }

         public int IVS_OCX_AddBroadcastDevice(string pCameraCode)
        {
            try
            {
                int result = OcxPlayer.IVS_OCX_AddBroadcastDevice(pCameraCode);
                LogService.Info("frmLocalOCX IVS_OCX_AddBroadcastDevice result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return -1;
            }
        }
         public int IVS_OCX_DeleteBroadcastDevice(string pCameraCode)
         {
             try
             {
                 int result = OcxPlayer.IVS_OCX_DeleteBroadcastDevice(pCameraCode);
                 LogService.Info("frmLocalOCX IVS_OCX_DeleteBroadcastDevice result is " + result.ToString(CultureInfo.InvariantCulture));
                 return result;
             }
             catch (Exception ex)
             {
                 LogService.Error("frmLocalOCX Error == >>", ex);
                 return -1;
             }
         }
         public int IVS_OCX_StartRealBroadcast()
        {
            try
            {
                int result = OcxPlayer.IVS_OCX_StartRealBroadcast();
                LogService.Info("frmLocalOCX IVS_OCX_StartRealBroadcast result is " + result.ToString(CultureInfo.InvariantCulture));
                return result;
            }
            catch (Exception ex)
            {
                LogService.Error("frmLocalOCX Error == >>", ex);
                return -1;
            }
        }
         public int IVS_OCX_StopRealBroadcast()
         {
             try
             {
                 int result = OcxPlayer.IVS_OCX_StopRealBroadcast();
                 LogService.Info("frmLocalOCX IVS_OCX_StopRealBroadcast result is " + result.ToString(CultureInfo.InvariantCulture));
                 return result;
             }
             catch (Exception ex)
             {
                 LogService.Error("frmLocalOCX Error == >>", ex);
                 return -1;
             }
         }
         public int IVS_OCX_StartFileBroadcast(string pFileName, uint ulCycle)
         {
             try
             {
                 int result = OcxPlayer.IVS_OCX_StartFileBroadcast(pFileName,ulCycle);
                 LogService.Info("frmLocalOCX IVS_OCX_StartFileBroadcast result is " + result.ToString(CultureInfo.InvariantCulture));
                 return result;
             }
             catch (Exception ex)
             {
                 LogService.Error("frmLocalOCX Error == >>", ex);
                 return -1;
             }
         }
         public int IVS_OCX_StopFileBroadcast()
         {
             try
             {
                 int result = OcxPlayer.IVS_OCX_StopFileBroadcast();
                 LogService.Info("frmLocalOCX IVS_OCX_StopFileBroadcast result is " + result.ToString(CultureInfo.InvariantCulture));
                 return result;
             }
             catch (Exception ex)
             {
                 LogService.Error("frmLocalOCX Error == >>", ex);
                 return -1;
             }
         }

         public string IVS_OCX_StartPlatformDownload(string pCameraCode, string pStartTime, string pEndTime, string pDownloadParamXml)
         {
             try
             {
                 string result = OcxPlayer.IVS_OCX_StartPlatformDownload(pCameraCode, pStartTime, pEndTime, pDownloadParamXml);
                 LogService.Info("frmLocalOCX IVS_OCX_StartPlatformDownload result is " + result.ToString(CultureInfo.InvariantCulture));
                 return result;
             }
             catch (Exception ex)
             {
                 LogService.Error("frmLocalOCX Error == >>", ex);
                 return string.Empty;
             }
         }

         public int IVS_OCX_StopPlatformDownload(uint ulDownloadHandle)
         {
             try
             {
                 int result = OcxPlayer.IVS_OCX_StopPlatformDownload(ulDownloadHandle);
                 LogService.Info("frmLocalOCX IVS_OCX_StopPlatformDownload result is " + result.ToString(CultureInfo.InvariantCulture));
                 return result;
             }
             catch (Exception ex)
             {
                 LogService.Error("frmLocalOCX Error == >>", ex);
                 return -1;
             }
         }

         public string IVS_OCX_StartPUDownload(string pCameraCode, string pStartTime, string pEndTime, string pDownloadParamXml)
         {
             try
             {
                 string result = OcxPlayer.IVS_OCX_StartPUDownload(pCameraCode, pStartTime, pEndTime, pDownloadParamXml);
                 LogService.Info("frmLocalOCX IVS_OCX_StartPUDownload result is " + result.ToString(CultureInfo.InvariantCulture));
                 return result;
             }
             catch (Exception ex)
             {
                 LogService.Error("frmLocalOCX Error == >>", ex);
                 return string.Empty;
             }
         }
         public int IVS_OCX_StopPUDownload(uint ulDownloadHandle)
         {
             try
             {
                 int result = OcxPlayer.IVS_OCX_StopPUDownload(ulDownloadHandle);
                 LogService.Info("frmLocalOCX IVS_OCX_StopPUDownload result is " + result.ToString(CultureInfo.InvariantCulture));
                 return result;
             }
             catch (Exception ex)
             {
                 LogService.Error("frmLocalOCX Error == >>", ex);
                 return -1;
             }
         }
         public int IVS_OCX_DownloadResume(uint ulDownloadHandle)
         {
             try
             {
                 int result = OcxPlayer.IVS_OCX_DownloadResume(ulDownloadHandle);
                 LogService.Info("frmLocalOCX IVS_OCX_DownloadResume result is " + result.ToString(CultureInfo.InvariantCulture));
                 return result;
             }
             catch (Exception ex)
             {
                 LogService.Error("frmLocalOCX Error == >>", ex);
                 return -1;
             }
         }
          public int IVS_OCX_DownloadPause(uint ulDownloadHandle)
         {
             try
             {
                 int result = OcxPlayer.IVS_OCX_DownloadPause(ulDownloadHandle);
                 LogService.Info("frmLocalOCX IVS_OCX_DownloadPause result is " + result.ToString(CultureInfo.InvariantCulture));
                 return result;
             }
             catch (Exception ex)
             {
                 LogService.Error("frmLocalOCX Error == >>", ex);
                 return -1;
             }
         }
          public string IVS_OCX_GetDownloadInfo(uint ulDownloadHandle)
          {
              try
              {
                  string result = OcxPlayer.IVS_OCX_GetDownloadInfo(ulDownloadHandle);
                  LogService.Info("frmLocalOCX IVS_OCX_GetDownloadInfo result is " + result.ToString(CultureInfo.InvariantCulture));
                  return result;
              }
              catch (Exception ex)
              {
                  LogService.Error("frmLocalOCX Error == >>", ex);
                  return string.Empty;
              }
          }

          //public uint IVS_OCX_SetLocalCaptureConfig(string pCaptureParm)
          //{
          //    try
          //    {
          //        uint result = OcxPlayer.IVS_OCX_SetLocalCaptureConfig(pCaptureParm);
          //        LogService.Info("frmLocalOCX IVS_OCX_SetLocalCaptureConfig result is " + result.ToString(CultureInfo.InvariantCulture));
          //        return result;
          //    }
          //    catch (Exception ex)
          //    {
          //        LogService.Error("frmLocalOCX Error == >>", ex);
          //        return (uint)2;
          //    }
          //}

          public string IVS_OCX_GetBroadcastCameraList()
          {
              try
              {
                  string result = OcxPlayer.IVS_OCX_GetBroadcastCameraList();
                  LogService.Info("frmLocalOCX IVS_OCX_GetBroadcastCameraList result is " + result.ToString(CultureInfo.InvariantCulture));
                  return result;
              }
              catch (Exception ex)
              {
                  LogService.Error("frmLocalOCX Error == >>", ex);
                  return string.Empty;
              }
          }

          public void IVS_OCX_SetLocalCaptureConfig(uint ulRefreshType)
          {
              try
              {
                  OcxPlayer.IVS_OCX_RefreshWnd(ulRefreshType);
                  LogService.Info("frmLocalOCX IVS_OCX_RefreshWnd result is ");
              }
              catch (Exception ex)
              {
                  LogService.Error("frmLocalOCX Error == >>", ex);
              }
          }

        #endregion
                
    }
}

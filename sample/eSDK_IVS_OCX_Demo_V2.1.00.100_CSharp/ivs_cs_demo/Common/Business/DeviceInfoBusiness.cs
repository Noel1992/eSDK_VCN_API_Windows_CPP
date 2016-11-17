using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace ivs_cs_demo
{
    public class DeviceInfoBusiness
    {
        const int PageSize = 300;
        private const int ShowPageSize = 50;
        public bool IsInManagement { get; set; }

        public List<VideoEntity> LoadDeviceInfoByUser()
        {
            List<VideoEntity> deviceList = new List<VideoEntity>();
            try
            {
                string resultXml = OcxHelper.OCX.IVS_OCX_GetDeviceList((uint)IVSDeviceType.DEVICE_TYPE_CAMERA, (uint)1, (uint)PageSize);

                OcxResult ocxResult = XmlHelper.ProcessOcxResponseMsg(resultXml);
                if (ocxResult.ResultCode != "0")
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("GetCameraList") + StringHelper.FindLanguageResource(ocxResult.ResultCode);
                }
                else
                {
                    OcxHelper.MainWin.OperationInfo.Content = StringHelper.FindLanguageResource("GetCameraList") + StringHelper.FindLanguageResource("Success");
                    XmlNode node = ocxResult.BodyXmlNode;
                    XmlNodeList nodeList = node.SelectNodes("VideoInChanList/VideoInChanInfo");

                    foreach (XmlNode xmlNode in nodeList)
                    {
                        VideoEntity video = new VideoEntity();
                        video.CameraCode = xmlNode.SelectSingleNode("CameraCode").InnerXml.ToString();
                        video.AliasName = xmlNode.SelectSingleNode("CameraName").InnerXml.ToString();

                        string cameraType = xmlNode.SelectSingleNode("CameraType").InnerXml.ToString();
                        video.CameraType = cameraType.Equals("FIX") ? CameraType.FIX : cameraType.Equals("PTZ") ? CameraType.PTZ : cameraType.Equals("DOME") ?
                            CameraType.DOME : cameraType.Equals("HALFDOME") ? CameraType.HALFDOME : cameraType.Equals("DOME") ? CameraType.DOME : CameraType.None;

                        video.IsOnline = xmlNode.SelectSingleNode("IsOnline").InnerXml.Equals("ON") ? true : false;
                        deviceList.Add(video);
                    }
                }
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                OcxHelper.MainWin.OperationInfo.Content = ex.ToString();
                return null;
            }
            return deviceList;
        }

    }
}

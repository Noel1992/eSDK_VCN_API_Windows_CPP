using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ivs_cs_demo
{
    public class PtzService
    {
        public int SendPtz(PtzOperationType operation, int param1, int param2, string code)
        {
            string strCameracode = code;

            if (string.IsNullOrEmpty(strCameracode))
            {
                strCameracode = string.Empty;
                LogService.Info("Search request info:==>>CameraCode is null.");
                return -1;
            }
            LogService.Info("Search request info:==>>CameraCode:" + strCameracode + ",PtzOperationType:" + operation.ToString() + ",Param1:" + param1.ToString() + ",Param2:" + param2.ToString());
            var resultValue = OcxHelper.OCX.IVS_OCX_PtzControl(strCameracode, (int)operation, param1.ToString(), param2.ToString());

            OcxResult result = XmlHelper.ProcessOcxResponseMsg(resultValue);
            var lockStatus = string.Empty;

            if (result.BodyXmlNode.SelectSingleNode("LockStatus") != null)
            {
                lockStatus = result.BodyXmlNode.SelectSingleNode("LockStatus").InnerText;
            }
            if (result.ResultCode != "0")
            {
                throw new Exception(result.ResultCode + "," + lockStatus);
            }
            LogService.Info("IVS_OCX_PtzControl response info:==>>" + resultValue);

            return int.Parse(lockStatus.ToString());
        }

    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ivs_cs_demo
{
    /// <summary>
    /// 通用设备类型（查询设备时获取）
    /// </summary>
    public enum IVSDeviceType
    {
        /// <summary>
        /// 主设备
        /// </summary>
        DEVICE_TYPE_MAIN = 1,
        /// <summary>
        /// 摄像头设备
        /// </summary>
        DEVICE_TYPE_CAMERA = 2,
        /// <summary>
        /// 语音设备
        /// </summary>
        DEVICE_TYPE_VOICE = 3,
        /// <summary>
        /// 告警设备（同时获取告警输入、告警输出）
        /// </summary>
        DEVICE_TYPE_ALARM = 4,
        /// <summary>
        /// 串口设备
        /// </summary>
        DEVICE_TYPE_SERIAL = 5,

        /// <summary>
        /// 跟DEVICE_TYPE_ALARM通一个值，主要用于区分告警输入和输出
        /// </summary>
        DEVICE_TYPE_ALARMIN = 6,
        /// <summary>
        /// 跟DEVICE_TYPE_ALARM通一个值，主要用于区分告警输入和输出
        /// </summary>
        DEVICE_TYPE_ALARMOUT = 7,
        DEVICE_TYPE_EXTENAL_ALARM = 9,
        /// <summary>
        /// 跟DEVICE_TYPE_EXTENAL_ALARM通一个值，主要用于区分告警输入和输出
        /// </summary>
        DEVICE_TYPE_EXTENAL_ALARMIN = 10,
        /// <summary>
        /// 跟DEVICE_TYPE_EXTENAL_ALARM通一个值，主要用于区分告警输入和输出
        /// </summary>
        DEVICE_TYPE_EXTENAL_ALARMOUT = 11,
    }
}

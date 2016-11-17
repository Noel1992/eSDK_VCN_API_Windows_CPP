using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    /// <summary>
    /// 摄像机类型的枚举
    /// </summary>
    public enum CameraType
    {
        None = 0,
        All = 1,

        /// <summary>
        /// 固定摄像机
        /// </summary>
        [XmlEnum("FIX")]
        FIX = 1 << 1,
        /// <summary>
        /// 云台枪机
        /// </summary>
        [XmlEnum("PTZ")]
        PTZ = 1 << 2,
        /// <summary>
        /// 球机
        /// </summary>
        [XmlEnum("DOME")]
        DOME = 1 << 3,
        /// <summary>
        /// 半球
        /// </summary>
        [XmlEnum("FIX_DOME")]
        HALFDOME = 1 << 4
    }
}

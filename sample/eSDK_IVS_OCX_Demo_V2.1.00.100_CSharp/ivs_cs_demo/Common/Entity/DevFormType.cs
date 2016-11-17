using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    /// <summary>
    /// 设备形态
    /// </summary>
    public enum DevFormType
    {
        /// <summary>
        /// 空
        /// </summary>
        [XmlEnum("")]
        None,
        All,
        /// <summary>
        /// IPC
        /// </summary>
        [XmlEnum("IPC")]
        IPC,
        /// <summary>
        /// DVS
        /// </summary>
        [XmlEnum("DVS")]
        DVS,
        /// <summary>
        /// DVR
        /// </summary>
        [XmlEnum("DVR")]
        DVR,
        /// <summary>
        /// eNVR
        /// </summary>
        [XmlEnum("eNVR")]
        eNVR
    }
}

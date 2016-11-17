using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.Threading;

namespace ivs_cs_demo
{
    [Serializable]
    [XmlRoot("RecordDataInfo")]
    public class RecordDataInfoEntity
    {

        /// 录像方式（存储位置）：
        ///0-平台
        ///1-前端
        [XmlElement(ElementName = "RecordMethod")]
        public StorageType RecordMethod { get; set; }

        [XmlElement(ElementName = "NVRCode")]
        public string NvrCode { get; set; }

        [XmlElement(ElementName = "MBUDomain")]
        public string MbuDomain { get; set; }

        /// <summary>
        /// 告警类型
        /// </summary>
        [XmlElement(ElementName = "AlarmType")]
        public string AlarmType { get; set; }

        /// <summary>
        /// 录像起始时间：yyyyMMddHHmmss
        /// </summary>
        [XmlElement(ElementName = "StartTime")]
        public string StartTime { get; set; }

        /// <summary>
        /// 录像结束时间： yyyyMMddHHmmss
        /// </summary>
        [XmlElement(ElementName = "EndTime")]
        public string EndTime { get; set; }

        /// <summary>
        /// 是否完整录像：
        ///0：完整
        ///1：不完整（抽帧一次）
        ///2：不完整（抽帧二次）
        ///3：不完整（抽帧三次）
        /// </summary>
        [XmlElement(ElementName = "IsFullRecord")]
        public string IsFullRecord { get; set; }


        /// <summary>
        /// 前端录像回放文件名，非前端录像类型不携带
        /// </summary>
        [XmlElement(ElementName = "FileName")]
        public string FileName { get; set; }

        /// <summary>
        /// 开始时间
        /// </summary>
        public DateTime StartTimeUTC { get { return DateTime.ParseExact(StartTime, "yyyyMMddHHmmss", Thread.CurrentThread.CurrentCulture); } }

        /// <summary>
        /// 结束时间
        /// </summary>
        public DateTime EndTimeUTC { get { return DateTime.ParseExact(EndTime, "yyyyMMddHHmmss", Thread.CurrentThread.CurrentCulture); } }


        [XmlElement(ElementName = "CameraCode")]
        public string CameraCode { get; set; }


        public string LocalStartTime { get; set; }

        public string LocalEndTime { get; set; }

        public string WndID { get; set; }
    }

    /// <summary>
    /// 存储位置类型
    /// </summary>
    public enum StorageType
    {
        /// <summary>
        /// 平台
        /// </summary>
        [XmlEnum("0")]
        Plat = 0,
        /// <summary>
        /// 前端
        /// </summary>
        [XmlEnum("1")]
        Pu = 1,
        /// <summary>
        /// 备份
        /// </summary>
        [XmlEnum("2")]
        Backup = 2,
        /// <summary>
        /// 容灾
        /// </summary>
        [XmlEnum("3")]
        Restore = 3,
        /// <summary>
        /// 本地
        /// </summary>
        [XmlEnum("100")]
        Local = 100,
        /// <summary>
        /// 无
        /// </summary>
        [XmlEnum("-1")]
        None
    }
}

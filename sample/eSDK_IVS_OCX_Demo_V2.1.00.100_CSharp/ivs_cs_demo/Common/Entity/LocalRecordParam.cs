using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    [XmlRoot("RecordParam")]
    public class LocalRecordParam 
    {
        [XmlElement(ElementName = "RecordFormat")]
        public RecordFormatType RecordFormat { get; set; }
        [XmlElement(ElementName = "SplitterType")]
        public FileSeparateType SplitterType { get; set; }
        [XmlElement(ElementName = "SplitterValue")]
        public double SplitterValue { get; set; }
        [XmlElement("DiskWarningValue")]
        public double DiskWarningValue { get; set; }
        [XmlElement("StopRecordValue")]
        public double StopRecordValue { get; set; }
        [XmlElement(ElementName = "RecordTime")]
        public double RecordTime { get; set; }
        [XmlElement(ElementName = "NameRule")]
        public NameRuleType NameRule { get; set; }
        [XmlElement(ElementName = "SavePath")]
        public string SavePath { get; set; }

        public LocalRecordParam()
        {
            //RecordFormat = RecordFormatType.Mp4;
            //SplitterType = FileSeparateType.Time;
            //SplitterValue = 600;
            //DiskWarningValue = 1024;
            //StopRecordValue = 1024;
            //RecordTime = 600;
            //NameRule = NameRuleType.NameRule1;
            //SavePath = System.Windows.Forms.Application.StartupPath + "LocalRecord";
        }
    }
    public enum RecordFormatType
    {
        [XmlEnum("1")]
        Mp4 = 1
    }


    /// <summary>
    /// 下载速度
    /// 1:时间
    /// 2:容量
    /// </summary>
    public enum FileSeparateType
    {
        /// <summary>
        /// 时间
        /// </summary>
        [XmlEnum("1")]
        Time = 1,

        /// <summary>
        /// 容量
        /// </summary>
        [XmlEnum("2")]
        Capacity = 2
    }
    /// <summary>
    /// 0-摄像机名_序号_时间(YYYY-MM-DD-hh-mm-ss)[默认值]；
    /// 1-摄像机名_时间(YYYY-MM-DD-hh-mm-ss)_序号；
    /// 2-时间(YYYY-MM-DD-hh-mm-ss)_摄像机名_序号
    /// </summary>
    public enum NameRuleType
    {
        /// <summary>
        /// 1-摄像机名_序号_时间(YYYY-MM-DD-hh-mm-ss)；
        /// </summary>
        [XmlEnum("1")]
        NameRule1 = 1,

        /// <summary>
        /// 2-摄像机名_时间(YYYY-MM-DD-hh-mm-ss)_序号；
        /// </summary>
        [XmlEnum("2")]
        NameRule2,

        /// <summary>
        /// 3-时间(YYYY-MM-DD-hh-mm-ss)_摄像机名_序号
        /// </summary>
        [XmlEnum("3")]
        NameRule3,
    }
}

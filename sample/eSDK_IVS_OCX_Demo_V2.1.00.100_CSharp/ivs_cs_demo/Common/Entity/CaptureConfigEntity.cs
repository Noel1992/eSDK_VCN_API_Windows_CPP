using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    [Serializable]
    [XmlRoot(ElementName = "CaptureConfig")]
    public class CaptureConfigEntity
    {
        /// <summary>
        /// 本地抓拍存储路径
        /// </summary>
        [XmlElement(ElementName = "CapturePath")]
        public virtual string CapturePath { get; set; }

        /// <summary>
        /// 抓拍下载路径
        /// </summary>
        [XmlElement(ElementName = "CaptureDownloadPath")]
        public virtual string CaptureDownloadPath { get; set; }

        /// <summary>
        /// 抓拍形式
        /// </summary>
        [XmlElement(ElementName = "SnapshotMode")]
        public virtual string SnapshotMode { get; set; }

        /// <summary>
        /// 抓拍数量（连续抓拍时设置）
        /// </summary>
        [XmlElement(ElementName = "SnapshotCount")]
        public virtual int SnapshotCount { get; set; }

        /// <summary>
        /// 抓拍间隔（连续抓拍时设置，单位为秒）
        /// </summary>
        [XmlElement(ElementName = "SnapshotInterval")]
        public virtual int SnapshotInterval { get; set; }

        /// <summary>
        /// 抓拍保存格式
        /// </summary>
        [XmlElement(ElementName = "SnapshotFormat")]
        public virtual SnapshotFormat SnapshotFormat { get; set; }

        /// <summary>
        /// 抓拍命名规则
        /// </summary>
        [XmlElement(ElementName = "NameRule")]
        public virtual NameRuleType NameRule { get; set; }
    }
}

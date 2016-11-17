using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    /// <summary>
    /// 抓拍保存格式
    /// 1-jpg
    /// 2-bmp
    /// </summary>
    public enum SnapshotFormat
    {
        /// <summary>
        /// 抓拍保存为jpg
        /// </summary>
        [XmlEnum("1")]
        Jpg = 1,

        /// <summary>
        /// 抓拍保存为bmp
        /// </summary>
        [XmlEnum("2")]
        Bmp = 2
    }
}

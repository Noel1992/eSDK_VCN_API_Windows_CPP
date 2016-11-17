using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    public enum RecordDownloadStatusType
    {
        /// <summary>
        /// 暂停
        /// </summary>
        [XmlEnum("0")]
        Pause = 1,

        /// <summary>
        /// 下载中
        /// </summary>
        [XmlEnum("1")]
        Download = 0,

        /// <summary>
        /// 启动中
        /// </summary>
        Starting = 2,

        /// <summary>
        /// 排队中
        /// </summary>
        InQueue = 3,

        /// <summary>
        /// 异常停止
        /// </summary>
        Exception = 4,

        /// <summary>
        /// 下载成功
        /// </summary>
        Success = 10
    }
}

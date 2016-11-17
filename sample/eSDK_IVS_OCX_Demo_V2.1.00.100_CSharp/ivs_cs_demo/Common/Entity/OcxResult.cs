using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace ivs_cs_demo
{
    /// <summary>
    /// 调用Ocx接口，返回的结果
    /// </summary>
    public class OcxResult
    {
        /// <summary>
        /// 错误码枚举
        /// </summary>
        public string ResultCode { get; set; }

        /// <summary>
        /// 响应消息
        /// </summary>
        public XmlNode BodyXmlNode { get; set; }
    }
}

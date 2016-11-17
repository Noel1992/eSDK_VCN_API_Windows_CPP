using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    [Serializable]
    [XmlRoot("DomainRoute")]
    public class DomainEntity
    {
        /// 域名称
        /// </summary>
        [XmlElement("DomainName")]
        public virtual string DomainName { get; set; }

        /// <summary>
        /// 域编码
        /// </summary>
        [XmlElement("DomainCode")]
        public virtual string DomainCode { get; set; }

        /// <summary>
        /// 域IP
        /// </summary>
        [XmlElement("DomainIp")]
        public virtual string DomainIp { get; set; }

        /// <summary>
        /// 域端口
        /// </summary>
        [XmlElement("DomainPort")]
        public virtual string DomainPort { get; set; }

        /// <summary>
        /// 上级域编码
        /// </summary>
        [XmlElement("SuperDomain")]
        public virtual string SuperDomain { get; set; }


        /// <summary>
        /// 是否本域 1-本域
        /// </summary>
        [XmlElement("IsLocalDomain")]
        public virtual string IsLocalDomain { get; set; }

                /// </summary>
        [XmlElement("IsProxy")]
        public virtual string IsProxy { get; set; }

        [XmlElement("DomainType")]
        public virtual string DomainType { get; set; }
    }
}

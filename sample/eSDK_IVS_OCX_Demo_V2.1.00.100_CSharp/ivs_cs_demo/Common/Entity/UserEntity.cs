using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    [Serializable]
    [XmlRoot("UserEntity")]
    public class UserEntity
    {    
        [XmlElement("username")]
        public virtual string username { get; set; }
        [XmlElement("userpassword")]
        public virtual string userpassword { get; set; }
        [XmlElement("ip")]
        public virtual string ip { get; set; }
        [XmlElement("port")]
        public virtual string port { get; set; }
    }
}

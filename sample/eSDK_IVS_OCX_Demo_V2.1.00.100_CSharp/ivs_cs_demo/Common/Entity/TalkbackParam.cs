using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    [Serializable]
    [XmlRoot("TalkbackParam")]
    public class TalkbackParam
    {
        [XmlElement("ProtocolType")]
        public string ProtocolType
        {
            get;
            set;
        }
        [XmlElement("DirectFirst")]
        public string DirectFirst
        {
            get;
            set;
        }
    }
}

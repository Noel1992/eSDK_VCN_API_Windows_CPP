using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    [XmlRoot("PlaybackParam")]
    public class PlaybackParam
    {
        [XmlElement(ElementName = "ProtocolType")]
        public string ProtocolType { get; set; }

         [XmlElement(ElementName = "StartTime")]
        public string StartTime { get; set; }

         [XmlElement(ElementName = "EndTime")]
        public string EndTime { get; set; }

         [XmlElement(ElementName = "Speed")]
        public string Speed { get; set; }

         [XmlElement(ElementName = "pPWD")]
         public string pPWD { get; set; }

         [XmlElement(ElementName = "NVRCode")]
         public string NVRCode { get; set; }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    [Serializable]
    [XmlRoot("PresetPTZ")]
    public class PTZPresetEntity
    {
        [XmlElement(ElementName = "CameraCode")]
        public string CameraCode { get; set; }

        [XmlElement(ElementName = "Name")]
        public string PTZPresetName { get; set; }

        [XmlElement(ElementName = "Index")]
        public int PresetIndex { get; set; }

        
    }
}

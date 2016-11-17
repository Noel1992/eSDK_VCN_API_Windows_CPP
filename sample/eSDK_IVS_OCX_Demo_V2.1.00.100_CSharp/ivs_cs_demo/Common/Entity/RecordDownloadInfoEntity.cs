using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    [XmlRoot("DownloadInfo")]
    public class RecordDownloadInfoEntity
    {
        [XmlElement("Status")]
        public RecordDownloadStatusType Status { get; set; }

        [XmlElement("Speed")]
        public int Speed { get; set; }

        [XmlElement("Size")]
        public long Size { get; set; }

        [XmlElement("TimeLeft")]
        public long TimeLeft { get; set; }

        [XmlElement("Progress")]
        public int Progress { get; set; }

        [XmlElement("ProgressPer")]
        public string ProgressPer
        {
            get { return Progress + "%";}
            set { }
        }
    }
}

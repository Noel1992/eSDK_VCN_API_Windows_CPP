using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    [XmlRoot("DownloadParam")]
    public class RecordDownloadParam 
    {
        [XmlElement(ElementName = "RecordFormat")]
        public RecordFormatType RecordFormat { get; set; }

        [XmlElement(ElementName = "SplitterType")]
        public FileSeparateType SplitterType { get; set; }

        [XmlElement(ElementName = "SplitterValue")]
        public double SplitterValue { get; set; }

        [XmlElement(ElementName = "DiskWarningValue")]
        public double DiskWarningValue { get; set; }

        [XmlElement(ElementName = "StopRecordValue")]
        public double StopRecordValue { get; set; }

        [XmlElement(ElementName = "NameRule")]
        public NameRuleType NameRule { get; set; }

        [XmlElement(ElementName = "EncryptRecord")]
        public string EncryptRecord { get; set; }

        [XmlElement(ElementName = "RecordPWD")]
        public string RecordPWD { get; set; }

        [XmlElement(ElementName = "SavePath")]
        public string SavePath { get; set; }

        [XmlElement(ElementName = "SaveFileName")]
        public string SaveFileName { get; set; }

        [XmlElement(ElementName = "DownloadSpeed")]
        public string DownloadSpeed { get; set; }

        [XmlElement(ElementName = "NVRCode")]
        public string NVRCode { get; set; }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    [Serializable]
    [XmlRoot("RealplayParam")]
    public class MediaPara
    {
        private int _decoderCompany = 1;

        private int _streamType = 1;

        private int _protoclType = 1;

        private int _directFirst = 0;

        private int _multiCase = 0;

        private int _videoEncodeType = 1;

        private int _audioEncodeType = 1;
        //[XmlElement("PROTOCOL_TYPE")]
        //public int ProtoclType { get; set; }
        //[XmlElement("DIRECT_FIRST")]
        //public int DirectFirst { get; set; }
        //[XmlElement("MULTICAST")]
        //public int MultiCast { get; set; }
        //[XmlElement("VIDEO_ENCODE_TYPE")]
        //public int VideoEncodeType { get; set; }
        //[XmlElement("AUDIO_ENCODE_TYPE")]
        //public int AudioEncodeType { get; set; }

        [XmlElement("DECODER_COMPANY")]
        public int DecoderCompany { get { return _decoderCompany; } set { _decoderCompany = value; } }

        [XmlElement("StreamType")]
        public int StreamType
        {
            get { return _streamType; }
            set { _streamType = value; }
        }
        [XmlElement("ProtocolType")]
        public int ProtoclType
        {
            get { return _protoclType; }
            set { _protoclType = value; }
        }
        [XmlElement("DirectFirst")]
        public int DirectFirst
        {
            get { return _directFirst; }
            set { _directFirst = value; }
        }
        [XmlElement("Multicast")]
        public int MultiCase
        {
            get { return _multiCase; }
            set { _multiCase = value; }
        }
        [XmlElement("VIDEO_ENCODE_TYPE")]
        public int VideoEncodeType
        {
            get { return _videoEncodeType; }
            set { _videoEncodeType = value; }
        }
        [XmlElement("AUDIO_ENCODE_TYPE")]
        public int AudioEncodeType
        {
            get { return _audioEncodeType; }
            set { _audioEncodeType = value; }
        }

        public override string ToString()
        {
            var sb = new StringBuilder("<?xml version='1.0' encoding='utf-8'?><Content><RealplayParam>");
            //sb.Append("<DECODER_COMPANY>").Append(DecoderCompany).Append("</DECODER_COMPANY>");
            sb.Append("<StreamType>").Append(StreamType).Append("</StreamType>");
            sb.Append("<ProtocolType>").Append(ProtoclType).Append("</ProtocolType>");
            sb.Append("<DirectFirst>").Append(DirectFirst).Append("</DirectFirst>");
            sb.Append("<Multicast>").Append(MultiCase).Append("</Multicast>");
            //sb.Append("<VIDEO_ENCODE_TYPE>").Append(VideoEncodeType).Append("</VIDEO_ENCODE_TYPE>");
            //sb.Append("<AUDIO_ENCODE_TYPE>").Append(AudioEncodeType).Append("</AUDIO_ENCODE_TYPE>");
            sb.Append("</RealplayParam></Content>");
            return sb.ToString();
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    [Serializable]
    [XmlRoot("VideoInChanInfo")]
    public class VideoEntity
    {
        /// <summary>
        /// 摄像机图标
        /// </summary>
        [XmlElement(ElementName = "CameraType")]
        public virtual string CameraPicPath
        {
            get
            {
                if (CameraType == ivs_cs_demo.CameraType.FIX)
                {
                    if (IsOnline)
                    {
                        return @"/Images/FixedBoltOn.png";
                    }
                    else
                    {
                        return @"/Images/FixedBoltOff.png";
                    }
                }
                else if (CameraType != ivs_cs_demo.CameraType.None)
                {
                    if (IsOnline)
                    {
                        return @"/Images/BallMachineOn.png";
                    }
                    else
                    {
                        return @"/Images/BallMachineOff.png";
                    }
                }
                return string.Empty;
            }


        }

        /// <summary>
        /// 摄像机类型
        /// </summary>
        [XmlElement(ElementName = "CameraType")]
        public virtual CameraType CameraType
        {
            get;
            set;
        }

        [XmlElement(ElementName = "IsOnline")]
        public virtual bool IsOnline
        {
            get;
            set;
        }

        /// <summary>
        /// 摄像机编码
        /// 即视频通道编码
        /// </summary>
        [XmlElement(ElementName = "CameraCode")]
        public virtual string CameraCode { get; set; }

        /// <summary>
        /// 主设备编码
        /// </summary>
        [XmlElement("ParentCode")]
        public string DevCode { get; set; }

        /// <summary>
        /// 摄像机所属域
        /// </summary>
        [XmlIgnore]
        public string DomainCode
        {
            get
            {
                if (string.IsNullOrEmpty(domainCode))
                {
                    string[] codes = CameraCode.Split('#');
                    if (codes.Length >= 2)
                    {
                        domainCode = codes[codes.Length - 1];
                    }
                }
                return domainCode;
            }
        }
        /// <summary>
        /// 外域设备，原始域编码
        /// </summary>
        [XmlIgnore]
        public string SourceDomainCode
        {
            get
            {
                if (IsExDomain)
                {
                    string[] codes = CameraCode.Split('#');
                    if (codes.Length > 2)
                    {
                        return codes[1];
                    }
                }
                return string.Empty;
            }
        }

        private string domainCode;

        /// <summary>
        /// 通道流数目
        /// 最大长度4字符，正整数
        /// </summary>
        [XmlElement(ElementName = "StreamNum")]
        public virtual int StreamNum { get; set; }


        [XmlElement(ElementName = "NVRCode")]
        public virtual string NVRCode { get; set; }

        /// <summary>
        /// 设备添加时间
        /// </summary>
        [XmlElement(ElementName = "DevCreateTime")]
        public virtual string DevCreateTime { get; set; }

        #region  << 原先属性 >>

        /// <summary>
        /// 分组编码
        /// </summary>
        [XmlElement(ElementName = "GroupID")]
        public virtual string GroupCode { get; set; }

        /// <summary>
        /// 分组名称
        /// </summary>
        //[XmlElement(ElementName = "GPR_NAME")]
        [XmlIgnore]
        public virtual string GroupName { get; set; }

        [XmlElement(ElementName = "DevVendorType")]
        public virtual string DevVendorType { get; set; }

        [XmlElement(ElementName = "DevModelType")]
        public virtual string DevModelType { get; set; }

        [XmlElement(ElementName = "DevFormType")]
        public virtual DevFormType DevFormType { get; set; }

        //[XmlElement(ElementName = "DEV_NAME")]
        [XmlIgnore]
        public virtual string MainDevName { get; set; }

        #endregion

        [XmlIgnore]
        public string AliasName { get; set; }

        /// <summary>
        /// 是否为外部域 0-否  1-是
        /// </summary>
        [XmlElement(ElementName = "IsExDomian")]
        public string IsExDomainString { get; set; }
        /// <summary>
        /// 是否为外部域
        /// </summary>
        [XmlIgnore]
        public bool IsExDomain
        {
            get
            {
                return IsExDomainString == "1";
            }
            set
            {
                IsExDomainString = value ? "1" : "0";
            }
        }

        [XmlIgnore]
        public string DeviceCode
        {
            get { return CameraCode; }
            set { CameraCode = value; }
        }

        [XmlIgnore]
        public string CameraNo { get; set; }

        [XmlIgnore]
        public string GroupID
        {
            get { return GroupCode; }
            set { GroupCode = value; }
        }

        [XmlIgnore]
        public string ConnctionCode { get; set; }


        [XmlIgnore]
        public string Manufacture { get; set; }

        [XmlIgnore]
        public string Address { get; set; }
        [XmlIgnore]
        public string SimpleCameraCode { get { return CameraCode.Split('#')[0]; } }

        private bool isSupportWaterMark = true;
        [XmlIgnore]
        public bool IsSupportWaterMark { get { return isSupportWaterMark; } set { isSupportWaterMark = value; } }

        [XmlElement(ElementName = "DevIP")]
        public string DeviceIP { get; set; }
    }
}

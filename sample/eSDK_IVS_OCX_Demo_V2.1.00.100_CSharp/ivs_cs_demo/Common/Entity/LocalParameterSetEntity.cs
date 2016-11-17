using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    /// <summary>
    /// 本地网络参数配置信息
    /// </summary>
    [Serializable]
    [XmlRoot("NetSetInfo")]
    public class LocalParameterSetEntity
    {

        #region ///--属性--///
        /// <summary>
        /// 用户指定的码流类型，多码流时使用：0-不指定，1-主码流，2-辅码流1，3-辅码流2，
        /// </summary>
        [XmlElement(ElementName = "StreamType")]
        public virtual string StreamType { get; set; }

        /// <summary>
        /// 是否支持组播 1-支持 0-不支持
        /// </summary>
        [XmlElement(ElementName = "MultiFlag")]
        public virtual MultiFlagEnum MultiFlag { get; set; }

        /// <summary>
        /// 媒体流传输协议：1-UDP 2-TCP
        /// </summary>
        [XmlElement(ElementName = "ProtocolType")]
        public virtual ProtocolTypeEnum ProtocolType { get; set; }

        /// <summary>
        /// 支持自动适应分辨率: 1-支持 0-不支持
        /// </summary>
        [XmlElement(ElementName = "SupportAutoResolution")]
        public virtual AutoResolutionEnum AutoResolution { get; set; }

        /// <summary>
        /// 启用GPU加速：1-启用 0-不启用
        /// </summary>
        [XmlElement(ElementName = "GpuSeedUp")]
        public virtual GPUSpeedUpEnum GPUSpeedUp { get; set; }

        public virtual bool GpuEnabled { get; set; }

        /// <summary>
        /// 登录时窗口显示模式：0-默认模式 1-全屏模式 2-一般模式 3-视频模式
        /// </summary>
        [XmlElement(ElementName = "WinViewModel")]
        public virtual ScreenModelEnum ScreenModel { get; set; }


        [XmlElement("IsEnableNetkeyboard")]
        public virtual bool IsEnableNetkeyboard { get; set; }

        [XmlElement("NetkeyboardPort")]
        public virtual string NetkeyboardPort { get; set; }

        public List<string> NetkeyboardVendorList { get; set; }

        [XmlElement("NetkeyboardVendor")]
        public virtual string NetkeyboardVendor { get; set; }

        [XmlElement("CurrentLanguage")]
        public virtual string CurrentLanguage { get; set; }

        #endregion
        public LocalParameterSetEntity()
        {
            StreamType = "1";//默认是1，
            ProtocolType = ProtocolTypeEnum.TCPProtocolTypeChecked;
            NetkeyboardVendorList = new List<string>() { StringHelper.FindLanguageResource("DAHUA"), StringHelper.FindLanguageResource("TIANDIWEIYE") };//新增厂商
        }
    }

    /// <summary>
    /// 是否支持组播
    /// </summary>
    public enum MultiFlagEnum
    {
        /// <summary>
        /// 不支持组播
        /// </summary>
        [XmlEnum("0")]
        MultiFlagUnChecked = 0,

        /// <summary>
        /// 支持组播
        /// </summary>
        [XmlEnum("1")]
        MultiFlagChecked = 1
    }
    /// <summary>
    /// 媒体流传输协议
    /// </summary>
    public enum ProtocolTypeEnum
    {
        /// <summary>
        /// TCP
        /// </summary>
        [XmlEnum("2")]
        TCPProtocolTypeChecked = 2,

        /// <summary>
        /// UDP
        /// </summary>
        [XmlEnum("1")]
        UDPProtocolTypeChecked = 1
    }

    /// <summary>
    /// 支持自动适应分辨率
    /// </summary>
    public enum AutoResolutionEnum
    {
        /// <summary>
        /// 不支持
        /// </summary>
        [XmlEnum("0")]
        AutoResolutionUnChecked = 0,

        /// <summary>
        /// 支持
        /// </summary>
        [XmlEnum("1")]
        AutoResolutionChecked = 1
    }

    /// <summary>
    /// 启用GPU加速
    /// </summary>
    public enum GPUSpeedUpEnum
    {
        /// <summary>
        /// 不启用
        /// </summary>
        [XmlEnum("0")]
        GPUSpeedUpUnChecked = 0,

        /// <summary>
        /// 启用
        /// </summary>
        [XmlEnum("1")]
        GPUSpeedUpChecked = 1
    }

    /// <summary>
    /// 登录时窗口显示模式
    /// </summary>
    public enum ScreenModelEnum
    {
        /// <summary>
        /// 默认模式
        /// </summary>
        [XmlEnum("0")]
        DefaultModelChecked = 0,

        /// <summary>
        /// 全屏模式
        /// </summary>
        [XmlEnum("1")]
        FullScreenModelChecked = 1,

        /// <summary>
        /// 一般模式
        /// </summary>
        [XmlEnum("2")]
        NormalModelChecked = 2,

        /// <summary>
        /// 视频模式
        /// </summary>
        [XmlEnum("3")]
        VideoModelChecked = 3
    }
}

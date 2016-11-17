using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    public  class LoginInfo
    {
        /// <summary>
        ///登录账号
        /// </summary>
        [XmlElement(ElementName = "UserName")]
        public string LoginName
        {
            set;
            get;
        }

        /// <summary>
        ///登录密码
        /// </summary>
        [XmlElement(ElementName = "Password")]
        public string LoginPassword
        {
            set;
            get;
        }

        /// <summary>
        ///客户端登录绑定的地址
        /// </summary>
        [XmlElement(ElementName = "IP")]
        public string LoginIp
        {
            set;
            get;
        }

        /// <summary>
        ///客户端登录绑定的端口
        /// </summary>
        [XmlElement(ElementName = "Port")]
        public string LoginPort
        {
            set;
            get;
        }

        /// <summary>
        /// 用户ID
        /// </summary>
        [XmlIgnore]
        public string UserId
        {
            set;
            get;
        }

        /// <summary>
        /// OperatorDomain
        /// </summary>
        [XmlIgnore]
        public string OperatorDomain
        {
            set;
            get;
        }

        public LoginInfo(string pUserName, string pPWD, string pServerIP, string pPort, string UserId,string OperatorDomain)
        {
            this.LoginName = pUserName;
            this.LoginPassword = pPWD;
            this.LoginIp = pServerIP;
            this.LoginPort = pPort;
            this.UserId = UserId;
            this.OperatorDomain = OperatorDomain;
        }
      static  public LoginInfo CurrentLoginInfo { get; set; }
    }
}

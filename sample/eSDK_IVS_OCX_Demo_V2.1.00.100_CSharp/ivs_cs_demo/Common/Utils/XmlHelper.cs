using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    public  class XmlHelper
    {
        static public OcxResult ProcessOcxResponseMsg(string responseMsg)
        {
            var ocxResult = new OcxResult()
            {
                ResultCode = "0"
            };

            var doc = new XmlDocument();
            try
            {
                doc.LoadXml(responseMsg);
            }
            catch (Exception)
            {
                ocxResult.ResultCode = "ServerResponseFailed";
            }

            XmlNode content = doc.SelectSingleNode("Content");
            if (content != null)
            {
                XmlNode resultCode = content.SelectSingleNode("ResultCode");
                if (resultCode != null)
                {
                    ocxResult.ResultCode = resultCode.InnerText;
                    content.RemoveChild(resultCode);
                }
                else
                {
                    //响应消息体不存在RESULT_CODE节点
                    ocxResult.ResultCode = "ServerResponseFailed";
                }
            }
            else
            {
                //响应消息体不存在CV_CONTENT节点
                ocxResult.ResultCode = "ServerResponseFailed";
            }
            ocxResult.BodyXmlNode = content;

            return ocxResult;
        }

        /// <summary>
        /// 反序列化xml为对象
        /// </summary>
        /// <param name="xml">对象序列化后的Xml字符串</param>
        /// <returns></returns>
        public static T DeserializeToEntity<T>(string xml)
        {
            using (var sr = new StringReader(xml))
            {
                var xz = new XmlSerializer(typeof(T));
                try
                {
                    return (T)xz.Deserialize(sr);
                }
                catch (Exception xmle)
                {
                    LogService.Error(xmle.Message, xmle);
                    return default(T);
                }
            }
        }

        /// <summary>
        /// 反序列化xml为List泛型
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="node">XmlNode</param>
        /// <returns></returns>
        public static List<T> DeserializeToList<T>(XmlNode node)
        {
            var list = new List<T>();
            try
            {
                if (node != null)
                {
                    list.AddRange(from XmlNode subNode in node.ChildNodes
                                  where subNode.NodeType != XmlNodeType.Comment
                                  select DeserializeToEntity<T>(subNode.OuterXml));
                }
            }
            catch (NullReferenceException nulle)
            {
                LogService.Error(nulle.Message, nulle);
            }
            catch (XmlException xmle)
            {
                LogService.Error(xmle.Message, xmle);
            }
            return list;
        }
        public static string SerializeToXml<T>(T t)
        {
            var xs = new XmlSerializer(typeof(T), new Type[] { t.GetType() });
            using (TextWriter tw = new StringWriterWithEncoding(Encoding.UTF8))
            {
                using (XmlWriter xw = new XmlTextWriter(tw))
                {
                    var xsn = new XmlSerializerNamespaces();
                    xsn.Add(String.Empty, String.Empty);
                    xs.Serialize(xw, t, xsn);
                    var s = tw.ToString();
                    return s.Replace("<?xml version=\"1.0\" encoding=\"utf-8\"?>", ""); //目前需要这个头 先留着 biankejia
                }
            }
        }
        public class StringWriterWithEncoding : StringWriter
        {
            private readonly Encoding myEncoding;

            public StringWriterWithEncoding(Encoding encoding)
            {
                myEncoding = encoding;
            }

            public override Encoding Encoding
            {
                get { return myEncoding; }
            }
        }
        const string xmlHeader = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
        public static string GetContentXml(string xmlContent)
        {
            if (string.IsNullOrEmpty(xmlContent))
                return string.Empty;
            else
            {
                StringBuilder sb = new StringBuilder(xmlContent);
                int index = xmlContent.IndexOf(xmlHeader, System.StringComparison.Ordinal);
                if (index != -1)
                {
                    sb.Insert(index + xmlHeader.Length, "<Content>");
                }
                else
                {
                    sb.Insert(0, "<Content>");
                }
                sb.Append("</Content>");
                return sb.ToString();
            }
        }
        /// <summary>
        /// 反序列化xml为对象
        /// </summary>
        /// <param name="node"> </param>
        /// <returns></returns>
        public static T DeserializeToEntity<T>(XmlNode node)
        {
            return DeserializeToEntity<T>(node.OuterXml);
        }

        public static List<UserEntity> GetAllUser()
        {
            try
            {
                List<UserEntity> list = new List<UserEntity>();
                string xml = File.ReadAllText(System.Windows.Forms.Application.StartupPath + "\\LogInfo.xml", Encoding.UTF8);

                var doc = new XmlDocument();
                doc.LoadXml(xml);

                XmlNode content = doc.SelectSingleNode(@"Content/userlist");
                list = DeserializeToList<UserEntity>(content);
                return list;
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
                return null;
            }
        }

        public static void AddUser(UserEntity user)
        {
            try
            {
                XmlDocument xmlDoc = new XmlDocument(); 
                xmlDoc.Load(System.Windows.Forms.Application.StartupPath + "\\LogInfo.xml");

                XmlNodeList list = xmlDoc.SelectNodes(@"Content/userlist/UserEntity");

                foreach (XmlNode node in list)
                {
                    if (node.FirstChild.InnerText == user.username)
                    {
                        return;
                    }
                }

                XmlNode root = xmlDoc.SelectSingleNode(@"Content/userlist");

                XmlElement userEntity = xmlDoc.CreateElement("UserEntity");//创建一个<UserEntity>节点

                XmlElement username = xmlDoc.CreateElement("username");
                XmlElement userpassword = xmlDoc.CreateElement("userpassword");
                XmlElement ip = xmlDoc.CreateElement("ip");
                XmlElement port = xmlDoc.CreateElement("port");

                userEntity.AppendChild(username);
                userEntity.AppendChild(userpassword);
                userEntity.AppendChild(ip);
                userEntity.AppendChild(port);

                username.InnerText = user.username;
                //userpassword.InnerText = user.userpassword;
                ip.InnerText = user.ip;
                port.InnerText = user.port;

                root.AppendChild(userEntity);


                xmlDoc.Save(System.Windows.Forms.Application.StartupPath + "\\LogInfo.xml");
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
            }
        }
    }
}

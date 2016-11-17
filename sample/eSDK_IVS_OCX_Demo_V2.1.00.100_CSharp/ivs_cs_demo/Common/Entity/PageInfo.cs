using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace ivs_cs_demo
{
    /// <summary>
    /// 分页信息
    /// </summary>
    [Serializable]
    [XmlRoot("PageInfo")]
    public class PageInfo
    {
        private const int DefaultPageSize = 100;

        public PageInfo()
        {
            FromIndex = 1;
            PageSize = 30;
            ToIndex = PageSize;

        }

        /// <summary>
        /// 总的符合条件的记录数
        /// </summary>
        [XmlElement(ElementName = "RealNum")]
        public UInt32 RealNum { get; set; }

        /// <summary>
        /// 从第几条记录开始返回，第1条记录的索引值为1
        /// </summary>
        [XmlElement(ElementName = "FromIndex")]
        public UInt32 FromIndex { get; set; }

        [XmlIgnore]
        public UInt32 PageSize { get; set; }

        /// <summary>
        /// 截止第几条记录
        /// </summary>
        [XmlElement(ElementName = "ToIndex")]
        public UInt32 ToIndex { get; set; }

        /// <summary>
        /// 查询条件
        /// </summary>
        [XmlElement(ElementName = "QueryCond")]
        public QueryCondition QueryCondition { get; set; }

        /// <summary>
        /// 排序条件
        /// </summary>
        [XmlElement(ElementName = "OrderCond")]
        public OrderCondition OrderCondition { get; set; }

        public void AddPage(uint p)
        {
            uint size = PageSize * p;
            this.FromIndex += size;
            this.ToIndex += size;
        }

        public override string ToString()
        {
            string query = "";
            foreach (QueryField field in QueryCondition.QueryFieldList)
            {
                query = "Field：";
                query += field.Field;
                query += "/t" + "Value：";
                query += field.Value;
                query += "/t" + "QueryType：";
                query += field.QueryType;
                query += "\n";
            }
            return "Page Info:\n"
                   + "RealNum：" + RealNum + "\n"
                   + "FromIndex：" + FromIndex + "\n"
                   + "ToIndex：" + ToIndex + "\n"
                   + "query：" + query + "\n"
                   + "OrderField：" + OrderCondition.OrderField + "\n"
                   + "Order：" + OrderCondition.Order;
        }

        public static PageInfo DefaultPage
        {
            get { return new PageInfo { FromIndex = 1, PageSize = DefaultPageSize, ToIndex = DefaultPageSize }; }
        }
    }

    [Serializable]
    [XmlRoot("PageInfo")]
    public class PageInfoRequest
    {
        public PageInfoRequest()
        {
            FromIndex = 1;
            PageSize = 30;
            ToIndex = PageSize;

        }

        /// <summary>
        /// 从第几条记录开始返回，第1条记录的索引值为1
        /// </summary>
        [XmlElement(ElementName = "FromIndex")]
        public UInt32 FromIndex
        {
            get;
            set;
        }

        [XmlIgnore]
        public UInt32 PageSize { get; set; }

        /// <summary>
        /// 截止第几条记录
        /// </summary>
        [XmlElement(ElementName = "ToIndex")]
        public UInt32 ToIndex
        {
            get;
            set;
        }

        /// <summary>
        /// 查询条件
        /// </summary>
        [XmlElement(ElementName = "QueryCond")]
        public QueryCondition QueryCondition
        {
            get;
            set;
        }

        /// <summary>
        /// 排序条件
        /// </summary>
        [XmlElement(ElementName = "OrderCond")]
        public OrderCondition OrderCondition
        {
            get;
            set;
        }
    }

    /// <summary>
    /// 排序条件
    /// </summary>
    [Serializable]
    [XmlRoot("OrderCond")]
    public class OrderCondition
    {
        /// <summary>
        /// 排序字段
        /// </summary>
        [XmlElement(ElementName = "OrderField")]
        public string OrderField
        {
            get;
            set;
        }

        /// <summary>
        /// 升序还是降序：
        ///UP-升序
        ///DOWN-降序
        /// </summary>
        [XmlElement(ElementName = "Order")]
        public string Order
        {
            get;
            set;
        }
    }

    /// <summary>
    /// 升序还是降序：
    ///UP-升序
    ///DOWN-降序
    /// </summary>
    public class OrderBy
    {
        /// <summary>
        /// 升序
        /// </summary>
        public static string UP
        {
            get
            {
                return "UP";
            }
        }

        /// <summary>
        /// 降序
        /// </summary>
        public static string DOWN
        {
            get
            {
                return "DOWN";
            }
        }
    } 
    
    /// <summary>
    /// 查询条件
    /// </summary>
    [Serializable]
    [XmlRoot("QueryCond")]
    public class QueryCondition
    {
        public QueryCondition()
        {
            QueryFieldList = new List<QueryField>();
        }

        /// <summary>
        /// 查询字段列表
        /// </summary>
        [XmlElement(ElementName = "QueryField")]
        public List<QueryField> QueryFieldList
        {
            get;
            set;
        }
    }
    /// <summary>
    /// 单个查询字段
    /// </summary>
    [Serializable]
    [XmlRoot("QueryField")]
    public struct QueryField
    {
        /// <summary>
        /// 查询字段
        /// </summary>
        [XmlElement(ElementName = "Field")]
        public string Field
        {
            get;
            set;
        }

        /// <summary>
        /// 查询字段的值
        /// </summary>
        [XmlElement(ElementName = "Value")]
        public string Value
        {
            get;
            set;
        }

        /// <summary>
        /// 查询类型：
        /// EXACT-精确
        /// INEXACT-模糊
        /// </summary>
        [XmlElement(ElementName = "QueryType")]
        public QueryType QueryType
        {
            get;
            set;
        }
    }
    public enum QueryType
    {
        /// <summary>
        /// 精确
        /// </summary>
        [XmlEnum(Name = "EXACT")]
        Exact,

        /// <summary>
        /// 模糊
        /// </summary>
        [XmlEnum(Name = "INEXACT")]
        Inexact,
    }
}

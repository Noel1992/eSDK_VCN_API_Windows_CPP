using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ivs_cs_demo
{
    /// <summary>
    /// 设备信息
    /// </summary>
    [Serializable]
    public class RecordQueryResultEntity
    {
        /// <summary>
        /// 分页信息
        /// </summary>
        public PageInfo PageInfo { get; set; }

        /// <summary>
        /// 摄像机编码
        /// </summary>
        public string CameraCode { get; set; }

        /// <summary>
        /// 摄像机名称
        /// </summary>
        public string CameraName { get; set; }

        /// <summary>
        /// 录像记录列表
        /// </summary>
        public List<RecordDataInfoEntity> RecordDataList { get; set; }
    }
}

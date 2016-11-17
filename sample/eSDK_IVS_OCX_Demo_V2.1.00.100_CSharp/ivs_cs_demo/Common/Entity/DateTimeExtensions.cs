using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ivs_cs_demo
{
    /// <summary>
    /// string的扩展方法
    /// </summary>
    public static class DateTimeExtensions
    {
        /// <summary>
        /// 转UTC扩展方法
        /// </summary>
        /// <param name="key"></param>
        /// <param name="isForStart"></param>
        /// <returns></returns>
        public static DateTime ToDstUtcTime(this DateTime key, bool isForStart)
        {
            var dstInfo = System.TimeZone.CurrentTimeZone.GetDaylightChanges(key.Year);
            var start = dstInfo.Start;
            var end = dstInfo.End;
            var span = dstInfo.Delta;

            if (Math.Abs(span.TotalSeconds - 0d) < 0.00000001) return key.ToUniversalTime();

            if (key >= start && key < start.Add(span))
            {
                return start.Add(span).ToUniversalTime();
            }

            if (key >= end && key < end.Add(span))
            {
                if (isForStart)
                {
                    return key.ToUniversalTime().AddSeconds(-span.TotalSeconds);
                }
            }

            return key.ToUniversalTime();
        }

        public static string ToStringDst(this DateTime key)
        {
            var sb = new StringBuilder();
            sb.Append(key.ToString("G"));
            if (key.IsDaylightSavingTime())
            {
                sb.Append(" DST");
            }
            return sb.ToString();
        }
    }
}

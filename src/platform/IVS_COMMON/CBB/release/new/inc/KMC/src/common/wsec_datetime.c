/* 如下pc lint告警可忽略 */
/*lint -e506 -e522 -e533 -e534 -e632 -e633 -e634 -e638 -e639 -e641 -e665 -e830 -e960 */

#include "wsec_type.h"
#include "wsec_pri.h"
#include "time.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#ifdef CBB_INNER_TEST
extern void APP_DateTimeSpeedup(WSEC_SYSTIME_T* pTime);
#endif

#define WSEC_IS_LEAP_YEAR(y) (((!((y) % 4)) && ((y) % 100)) || (!((y) % 400)))
#define WSEC_MONTH_DAYS(y, m) (((4==(m))||(6==(m))||(9==(m))||(11==(m))) ? 30 : ((2==(m)) ? (WSEC_IS_LEAP_YEAR(y) ? 29 : 28) : 31))
#define WSEC_WEEK_DAYS (7)

/* 本模块私有函数，不对外开放 */
WSEC_VOID WSEC_Tm2WsecSystemTime(const struct tm* pTm, WSEC_SYSTIME_T* pWsecTime);
WSEC_BOOL WSEC_LocalTimeMinusUtcSec(WSEC_INT32* pDiffSec);
WSEC_BOOL WSEC_OsLocalTime(struct tm * _Tm, const time_t * _Time);
WSEC_BOOL WSEC_OsGmTime(struct tm * _Tm, const time_t * _Time);

/*****************************************************************************
 函 数 名  : WSEC_DateTimeAdd
 功能描述  : 对指定的日期&时间(pstBase)加nAdd个时间单位(ePart)，
             计算结果存pstNew。
 输入参数  : pstBase: 参与加法的日期及时间
             nAdd:    增加量, 负数则表示做减法
             ePart:   对日期&时间的哪个部分做加法
 输出参数  : pstNew:  计算结果
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2014年7月11日
    作    者   : z118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_DateTimeAdd(const WSEC_SYSTIME_T* pstBase, WSEC_INT32 nAdd, WSEC_DATETIME_PART_ENUM ePart, WSEC_SYSTIME_T* pstNew)
{
    WSEC_INT32 i, nOffset, y, m, d, nDayOffset;
    WSEC_INT32 aItem[]            = {dtpSecond, dtpMinutes, dtpHour, dtpDay, dtpMonth, dtpYear}; /* 临时初始化，规避"non-constant aggregate initializer"告警 */
    const WSEC_INT32 aTop[]       = {59,            59,        23,     -1,      12}; /* 上限 */
    const WSEC_INT32 aRound[]     = {60,            60,        24,     0,       12}; /* 轮回值 */
    WSEC_UINT8 bHandleDay = 0;

    WSEC_ASSERT(pstBase && WSEC_IsDateTime(pstBase) && pstNew);
    if (0 == nAdd)
    {
        WSEC_DateTimeCopy(pstNew, pstBase);
        return;
    }

    aItem[0] = pstBase->ucSecond;
    aItem[1] = pstBase->ucMinute;
    aItem[2] = pstBase->ucHour;
    aItem[3] = pstBase->ucDate;
    aItem[4] = pstBase->ucMonth;
    aItem[5] = pstBase->uwYear;

    nDayOffset = 0;
    aItem[ePart] += nAdd;

    for (i = ePart; i < dtpYear; i++)
    {
        if ((aItem[i] > aTop[i]) || (aItem[i] <= 0)) /* 进位 or 借位 */
        {
            if (aRound[i]) /* 非‘日’处理可以使用统一公式计算 */
            {
                nOffset = aItem[i] / aRound[i];
                if_oper((aItem[i] % aRound[i]) && (aItem[i] < 0), nOffset--);

                aItem[i + 1] += nOffset;
                aItem[i] -= aRound[i] * nOffset;

                if (dtpMonth == ePart) /* 月份不同于时分秒, 起始于1 */
                {
                    /* Misinformation: FORTIFY.Dead_Code */
                    if (0 == aItem[dtpMonth])
                    {
                        aItem[dtpMonth] = aRound[dtpMonth];
                        aItem[dtpYear]--;
                    }else{;}
                }else{;}
            }
            else /* ‘日’的处理 */
            {
                bHandleDay = 1;
                break;
            }
        }else{;}
    }

    if (bHandleDay) /* 对‘日’的处理 */
    {
        y = aItem[dtpYear];
        m = aItem[dtpMonth];
        d = aItem[dtpDay];

        nDayOffset = d - pstBase->ucDate; /* ‘日’的变化量 */

        while (!WSEC_IN_SCOPE(d, 1, WSEC_MONTH_DAYS(y, m)))
        {
            if (d > 0)
            {
                d -= WSEC_MONTH_DAYS(y, m);

                m++;
                if (m > aTop[dtpMonth]) /* 年底翻转 */
                {
                    y++;
                    m = 1;
                }else{;}
            }
            else
            {
                m--;
                if (0 == m) /* 年初逆转 */
                {
                    m = aRound[dtpMonth];
                    y--;
                }else{;}

                d += WSEC_MONTH_DAYS(y, m);
            }
        }

        aItem[dtpYear]  = y;
        aItem[dtpMonth] = m;
        aItem[dtpDay]   = d;
    }

    if ((dtpYear == ePart) || (dtpMonth == ePart)) /* 以月/年为单位的加法，需单独统计‘日’的变化量 */
    {
        /* Misinformation: FORTIFY.Dead_Code */
        y = (WSEC_INT32)pstBase->uwYear;
        m = (WSEC_INT32)pstBase->ucMonth;

        while ((y != aItem[dtpYear]) || (m != aItem[dtpMonth]))
        {
            if (nAdd > 0)
            {
                nDayOffset += WSEC_MONTH_DAYS(y, m);
                m++;
                if (m > aTop[dtpMonth]) /* 年底翻转 */
                {
                    y++;
                    m = 1;
                }else{;}
            }
            else
            {
                m--;
                if (0 == m) /* 年初逆转 */
                {
                    m = aRound[dtpMonth];
                    y--;
                }else{;}
                nDayOffset += WSEC_MONTH_DAYS(y, m);
            }
        }

        if_oper(nAdd < 0, nDayOffset *= (-1));
    }

    if ((2 == aItem[dtpMonth]) && (29 == aItem[dtpDay]) && (!WSEC_IS_LEAP_YEAR(aItem[dtpYear]))) /* 只有闰年才有2月29，否则需要调整 */
    {
        if (nAdd > 0) /* 加法，则正向调整 */
        {
            aItem[dtpMonth] = 3;
            aItem[dtpDay]   = 1;
        }
        else /* 减法，则反向调整 */
        {
            aItem[dtpMonth] = 2;
            aItem[dtpDay]   = 28;
        }
    }else{;}

    pstNew->ucSecond = (WSEC_UINT8)aItem[dtpSecond];
    pstNew->ucMinute = (WSEC_UINT8)aItem[dtpMinutes];
    pstNew->ucHour   = (WSEC_UINT8)aItem[dtpHour];
    pstNew->ucDate   = (WSEC_UINT8)aItem[dtpDay];
    pstNew->ucMonth  = (WSEC_UINT8)aItem[dtpMonth];
    pstNew->uwYear   = (WSEC_UINT16)aItem[dtpYear];

    nDayOffset %= WSEC_WEEK_DAYS;
    if_oper(nDayOffset < 0, nDayOffset += WSEC_WEEK_DAYS);
    pstNew->ucWeek = (((pstBase->ucWeek - 1) + nDayOffset) % WSEC_WEEK_DAYS) + 1;

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_DateTimeCopy
 功能描述  : 复制DateTime结构
 纯 入 参  : pstSrc: 源数据
 纯 出 参  : pstDst: 目的数据
 入参出参  : 无
 返 回 值  : 成功或失败.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月20日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_DateTimeCopy(WSEC_SYSTIME_T* pstDst, const WSEC_SYSTIME_T *pstSrc)
{
    WSEC_ASSERT(pstDst && pstSrc && WSEC_IsDateTime(pstSrc));
    return (WSEC_MEMCPY(pstDst, sizeof(WSEC_SYSTIME_T), pstSrc, sizeof(WSEC_SYSTIME_T)) == EOK);
}

/*****************************************************************************
 函 数 名  : WSEC_IsDateTime
 功能描述  : 判断指定的参数是否是合法的日期&时间
             只判断年、月、日、时、分、秒等的取值否合法;
             判断week是否在合法范围，但无法判断week的值和年月日匹配.
 输入参数  : pstVal: 被判断值
 输出参数  : 无
 返 回 值  : 真=合法的日期&时间; 假=非法

 修改历史      :
  1.日    期   : 2014年7月11日
    作    者   : z118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_IsDateTime(const WSEC_SYSTIME_T* pstVal)
{
    WSEC_BOOL bOk = (0 < pstVal->uwYear) 
           && WSEC_IN_SCOPE(pstVal->ucMonth, 1, 12) /* 月: 1~12 */
           && (pstVal->ucHour < 24)    /* 时: 0~23 */
           && (pstVal->ucMinute < 60)  /* 分: 0~59 */
           && (pstVal->ucSecond <= 60); /* 秒: 0~60(闰秒) */

    if (bOk)
    {
        bOk = WSEC_IN_SCOPE(pstVal->ucDate, 1, WSEC_MONTH_DAYS(pstVal->uwYear, pstVal->ucMonth));
    }

    return bOk;
}

/*****************************************************************************
 函 数 名  : WSEC_DateTimeCompare
 功能描述  : 比较pstBig和pstSmall这两个日期&时间的大小
 纯 入 参  : pstBig: 参与比较的数据;
             pstSmall: 参与比较的数据;
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : WSEC_CMP_RST_EQUAL:      A == B
             WSEC_CMP_RST_BIG_THAN:   A  > B
             WSEC_CMP_RST_SMALL_THAN: A  < B
 特别注意  : 建议这样使用函数:
             ......
             WSEC_CMP_RST_ENUM eCmp;

             eCmp = WSEC_DateTimeCompare(&stA, &stB);
             if (eCmp > 0)
             {
                <stA大于stB>
             }
             else if(eCmp < 0)
             {
                <stA小于stB>
             }
             else
             {
                <stA等于stB>
             }
             ......

 修改历史
  1.日    期   : 2014年7月11日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_CMP_RST_ENUM WSEC_DateTimeCompare(const WSEC_SYSTIME_T* pstBig, const WSEC_SYSTIME_T* pstSmall)
{
    WSEC_ASSERT(pstBig && WSEC_IsDateTime(pstBig) && pstSmall && WSEC_IsDateTime(pstSmall));
    
    /* 年 */
    if (pstBig->uwYear > pstSmall->uwYear) {return WSEC_CMP_RST_BIG_THAN;}
    if (pstBig->uwYear < pstSmall->uwYear) {return WSEC_CMP_RST_SMALL_THAN;}

    /* 月 */
    if (pstBig->ucMonth > pstSmall->ucMonth) {return WSEC_CMP_RST_BIG_THAN;}
    if (pstBig->ucMonth < pstSmall->ucMonth) {return WSEC_CMP_RST_SMALL_THAN;}

    /* 日 */
    if (pstBig->ucDate > pstSmall->ucDate) {return WSEC_CMP_RST_BIG_THAN;}
    if (pstBig->ucDate < pstSmall->ucDate) {return WSEC_CMP_RST_SMALL_THAN;}

    /* 时 */
    if (pstBig->ucHour > pstSmall->ucHour) {return WSEC_CMP_RST_BIG_THAN;}
    if (pstBig->ucHour < pstSmall->ucHour) {return WSEC_CMP_RST_SMALL_THAN;}

    /* 分 */
    if (pstBig->ucMinute > pstSmall->ucMinute) {return WSEC_CMP_RST_BIG_THAN;}
    if (pstBig->ucMinute < pstSmall->ucMinute) {return WSEC_CMP_RST_SMALL_THAN;}

    /* 秒 */
    if (pstBig->ucSecond > pstSmall->ucSecond) {return WSEC_CMP_RST_BIG_THAN;}
    if (pstBig->ucSecond < pstSmall->ucSecond) {return WSEC_CMP_RST_SMALL_THAN;}

    return WSEC_CMP_RST_EQUAL;
}

/*****************************************************************************
 函 数 名  : WSEC_DateTimeDiff
 功能描述  : 计算stTo相对于stFrom多出多少个时间单位eUnit，
 输入参数  : eUnit:   计算差值得时间单位：年/月/日/时/分/秒
             pstFrom: 被减数
             pstTo:   减数
 输出参数  : 无
 返 回 值  : 时间差值

 修改历史      :
  1.日    期   : 2014年7月18日
    作    者   : z118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_INT32 WSEC_DateTimeDiff(WSEC_DATETIME_PART_ENUM eUnit, const WSEC_SYSTIME_T* pstFrom, const WSEC_SYSTIME_T* pstTo)
{
    const WSEC_INT32 aRound[] = {60, 60, 24, 0, 12}; /* 轮回值 */
    WSEC_INT32 nDays = 0;
    WSEC_INT32 nDiff = 0;
    WSEC_UINT16 y, y1;
    WSEC_UINT8 m, m1;
    WSEC_INT32 nCmp;

    WSEC_ASSERT(pstFrom && WSEC_IsDateTime(pstFrom) && pstTo && WSEC_IsDateTime(pstTo));

    /* 以年/月为单位的差值，可以直接计算 */
    if (dtpYear == eUnit)
    {
        return (WSEC_INT32)(pstTo->uwYear - pstFrom->uwYear);
    }
    else if (dtpMonth == eUnit)
    {
        return ((WSEC_INT32)(pstTo->uwYear - pstFrom->uwYear)) * aRound[dtpMonth]
              + (WSEC_INT32)(pstTo->ucMonth - pstFrom->ucMonth);
    }else{;}
    
    /* 计算相差多少天 */
    nCmp = WSEC_DateTimeCompare(pstTo, pstFrom);
    if (nCmp > 0) /* stTo > stFrom */
    {
        y = pstFrom->uwYear;
        m = pstFrom->ucMonth;

        y1 = pstTo->uwYear;
        m1 = pstTo->ucMonth;

        nDays =(WSEC_INT32)(pstTo->ucDate - pstFrom->ucDate);
    }
    else /* stTo <= stFrom */
    {
        y = pstTo->uwYear;
        m = pstTo->ucMonth;

        y1 = pstFrom->uwYear;
        m1 = pstFrom->ucMonth;

        nDays =(WSEC_INT32)(pstFrom->ucDate - pstTo->ucDate);
    }

    /* 以‘月’为步长，统计相差多少天 */
    while ((y != y1) || (m != m1))
    {
        nDays += WSEC_MONTH_DAYS(y, m);
        m++;

        if (m > aRound[dtpMonth]) /* 年份翻转 */
        {
            y++;
            m = 1;
        }else{;}
    }

    if_oper(nCmp < 0, nDays *= (-1));

    if (dtpDay == eUnit)
    {
        return nDays;
    }

    nDiff = nDays * aRound[dtpHour] + (WSEC_INT32)pstTo->ucHour - (WSEC_INT32)pstFrom->ucHour; /* 相差多少小时 */

    if (dtpMinutes == eUnit)
    {
        /* Misinformation: FORTIFY.Dead_Code */
        nDiff = nDiff * aRound[dtpMinutes] + (pstTo->ucMinute - pstFrom->ucMinute);
    }
    else if (dtpSecond == eUnit)
    {
        nDiff = nDiff * aRound[dtpMinutes] + (pstTo->ucMinute - pstFrom->ucMinute);
        nDiff = nDiff * aRound[dtpSecond] + (pstTo->ucSecond - pstFrom->ucSecond);
    }else{;}

    return nDiff;
}

/*****************************************************************************
 函 数 名  : WSEC_GetLocalDateTime
 功能描述  : 获取当前本地日期&时间
 输入参数  : 无
 输出参数  : pstNow 回传当前本地日期&时间
 返 回 值  : 是否获取成功，当系统时间在1970前返回失败.

 修改历史      :
  1.日    期   : 2014年7月22日
    作    者   : z118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_GetLocalDateTime(WSEC_SYSTIME_T* pstNow)
{
    time_t tNow;
    struct tm tTime = {0};

#ifdef CBB_INNER_TEST
    WSEC_SYSTIME_T tUtc;
    WSEC_GetUtcDateTime(&tUtc);
    WSEC_UtcTime2Local(&tUtc, pstNow);
    return WSEC_TRUE;
#endif    

    WSEC_ASSERT(pstNow);

    time(&tNow);
    return_oper_if(!WSEC_OsLocalTime(&tTime, &tNow), oper_null, WSEC_FALSE);

    pstNow->uwYear   = (WSEC_UINT16)(tTime.tm_year + 1900);
    pstNow->ucMonth  = (WSEC_UINT8)(tTime.tm_mon + 1);
    pstNow->ucDate   = (WSEC_UINT8)tTime.tm_mday;
    pstNow->ucHour   = (WSEC_UINT8)tTime.tm_hour;
    pstNow->ucMinute = (WSEC_UINT8)tTime.tm_min;
    pstNow->ucSecond = (WSEC_UINT8)tTime.tm_sec;
    pstNow->ucWeek   = (WSEC_UINT8)tTime.tm_wday;

    if (0 == pstNow->ucWeek)
    {
        pstNow->ucWeek = 7;
    }

    return WSEC_TRUE;
}

/*****************************************************************************
 函 数 名  : WSEC_GetUtcDateTime
 功能描述  : 获取当前UTC(Universal Time Coordinated)日期&时间
 输入参数  : 无
 输出参数  : pstNow 回传当前UTC日期&时间
 返 回 值  : 是否获取成功.

 修改历史      :
  1.日    期   : 2014年7月22日
    作    者   : z118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_GetUtcDateTime(WSEC_SYSTIME_T* pstNow)
{
    time_t tNow;
    struct tm tTime;

    WSEC_ASSERT(pstNow);
    
    time(&tNow);

    return_oper_if(!WSEC_OsGmTime(&tTime, &tNow), oper_null, WSEC_FALSE);

    WSEC_Tm2WsecSystemTime(&tTime, pstNow);

#ifdef CBB_INNER_TEST
    APP_DateTimeSpeedup(pstNow);
#endif
    return WSEC_TRUE;
}

/*****************************************************************************
 函 数 名  : WSEC_Tm2WsecSystemTime
 功能描述  : 将C语言中的tm数据结构(pTm)转换为WSEC_SYSTIME_T
 输入参数  : 略
 输出参数  : 略
 返 回 值  : 无
 注    意  : 本函数是该模块内部队私有函数.

 修改历史      :
  1.日    期   : 2014年7月25日
    作    者   : z118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_Tm2WsecSystemTime(const struct tm* pTm, WSEC_SYSTIME_T* pWsecTime)
{
    WSEC_ASSERT(pTm && pWsecTime);
    
    pWsecTime->uwYear   = (WSEC_UINT16)(pTm->tm_year + 1900);
    pWsecTime->ucMonth  = (WSEC_UINT8)(pTm->tm_mon + 1);
    pWsecTime->ucDate   = (WSEC_UINT8)pTm->tm_mday;
    pWsecTime->ucHour   = (WSEC_UINT8)pTm->tm_hour;
    pWsecTime->ucMinute = (WSEC_UINT8)pTm->tm_min;
    pWsecTime->ucSecond = (WSEC_UINT8)pTm->tm_sec;
    pWsecTime->ucWeek   = (WSEC_UINT8)pTm->tm_wday;

    if (0 == pWsecTime->ucWeek)
    {
        pWsecTime->ucWeek = 7;
    }

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_LocalTimeMinusUtcSec
 功能描述  : 计算本地时间超出UTC时间多少秒
 输入参数  : 无
 输出参数  : pDiffSec: 回传差值
 返 回 值  : 成功与否，当系统时间在1970前返回失败.
 注    意  : 本函数是该模块内部的私有函数.

 修改历史      :
  1.日    期   : 2014年7月25日
    作    者   : z118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_LocalTimeMinusUtcSec(WSEC_INT32* pDiffSec)
{
    time_t tSysNow, tLocal;
    struct tm tmLocal;
    struct tm tmUtc;
    static WSEC_INT32 nDiffSec = -1;

    if (nDiffSec != -1) /* 直接使用上次计算结果 */
    {
        *pDiffSec = nDiffSec;
        return WSEC_TRUE;
    }

    time(&tSysNow);
    return_oper_if(!WSEC_OsLocalTime(&tmLocal, &tSysNow), oper_null, WSEC_FALSE);
    tLocal = mktime(&tmLocal);

    return_oper_if(!WSEC_OsGmTime(&tmUtc, &tSysNow), oper_null, WSEC_FALSE);

    nDiffSec  = (WSEC_INT32)(tLocal - mktime(&tmUtc));
    *pDiffSec = nDiffSec;
    
    return WSEC_TRUE;
}

/*****************************************************************************
 函 数 名  : WSEC_UtcTime2Local
 功能描述  : 将UTC时间(pUtc)转换为本地时间(pLocal)
 输入参数  : 略
 输出参数  : 略
 返 回 值  : 成功与否，当系统时间在1970前返回失败.

 修改历史      :
  1.日    期   : 2014年7月25日
    作    者   : z118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_UtcTime2Local(const WSEC_SYSTIME_T* pUtc, WSEC_SYSTIME_T* pLocal)
{
    WSEC_INT32 nLocalMinusUtcSec;

    return_oper_if(!WSEC_LocalTimeMinusUtcSec(&nLocalMinusUtcSec), oper_null, WSEC_FALSE);
    WSEC_DateTimeAdd(pUtc, nLocalMinusUtcSec, dtpSecond, pLocal);
    return WSEC_TRUE;
}

/*****************************************************************************
 函 数 名  : WSEC_LocalTime2Utc
 功能描述  : 将本地时间(pLocal)转换为UTC时间(pUtc)
 输入参数  : 略
 输出参数  : 略
 返 回 值  : 成功与否，当系统时间在1970前返回失败.

 修改历史      :
  1.日    期   : 2014年7月25日
    作    者   : z118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_LocalTime2Utc(const WSEC_SYSTIME_T* pLocal, WSEC_SYSTIME_T* pUtc)
{
    WSEC_INT32 nLocalMinusUtcSec;

    WSEC_ASSERT(pLocal && WSEC_IsDateTime(pLocal) && pUtc);

    return_oper_if(!WSEC_LocalTimeMinusUtcSec(&nLocalMinusUtcSec), oper_null, WSEC_FALSE);
    WSEC_DateTimeAdd(pLocal, (-1)*nLocalMinusUtcSec, dtpSecond, pUtc);
    return WSEC_TRUE;
}

/*****************************************************************************
 函 数 名  : WSEC_DateTime2String
 功能描述  : 将时间数据输出成字符串
 纯 入 参  : pTime: 日期&时间
             nBuffSize: 输出缓冲区长度
 纯 出 参  : pszBuff: 输出缓冲区;
 入参出参  : 无
 返 回 值  : 指向输出缓冲区的指针(方便编程用)
 特别注意  : 可这样使用该函数
             ......
             WSEC_SYSTIME_T stNow;
             WSEC_CHAR szTime[30];
             
             WSEC_GetLocalDateTime(&stNow);
             printf("Now: %s", WSEC_DateTime2String(&stNow, szTime, sizeof(szTime)));
             ......

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_CHAR* WSEC_DateTime2String(const WSEC_SYSTIME_T* pTime, WSEC_VOID* pBuff, WSEC_SIZE_T nBuffSize)
{
    /*lint -e506 -e668 */
    WSEC_UNCARE(sprintf_s((char*)pBuff, nBuffSize, "%04d-%02d-%02d %02d:%02d:%02d(Week-%d)", pTime->uwYear, pTime->ucMonth, pTime->ucDate, pTime->ucHour, pTime->ucMinute, pTime->ucSecond, pTime->ucWeek));
    return (WSEC_CHAR*)pBuff;
}

WSEC_BOOL WSEC_OsLocalTime(struct tm * _Tm, const time_t * _Time)
{
//    WSEC_DISABLE_WARNING_BEGIN(4996)
    struct tm* outTm = localtime(_Time);
    return_oper_if(!outTm, oper_null, WSEC_FALSE);

    *_Tm = *outTm;
    return WSEC_MEMCPY(_Tm, sizeof(struct tm), outTm, sizeof(struct tm)) == EOK ? WSEC_TRUE : WSEC_FALSE;

//    WSEC_DISABLE_WARNING_END
}

WSEC_BOOL WSEC_OsGmTime(struct tm * _Tm, const time_t * _Time)
{
//    WSEC_DISABLE_WARNING_BEGIN(4996)

    struct tm* outTm = gmtime(_Time);

    return_oper_if(!outTm, oper_null, WSEC_FALSE);
    
    *_Tm = *outTm;
    return WSEC_MEMCPY(_Tm, sizeof(struct tm), outTm, sizeof(struct tm)) == EOK ? WSEC_TRUE : WSEC_FALSE;

//    WSEC_DISABLE_WARNING_END
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */

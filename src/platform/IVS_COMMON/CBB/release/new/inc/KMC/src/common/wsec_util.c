/* 如下pc lint告警可忽略 */
/*lint -e506 -e522 -e526 -e533 -e534 -e545 -e550 -e573 -e574 -e602 -e603 -e632 -e633 -e634 -e638 -e639 -e655 -e665 -e668 -e701 -e702 -e750 -e785 -e794 -e830 -e960 */

/*******************************************************************************
* Copyright @ Huawei Technologies Co., Ltd. 1998-2014. All rights reserved.  
* File name: WSEC_Util.c
* Decription:
  CBB公用函数实现
*********************************************************************************/
#include "wsec_type.h"
#include "wsec_itf.h"
#include "wsec_pri.h"
#include "wsec_share.h"
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* 是否支持缺省的Lock操作 */
#ifdef WSEC_WIN32
    #define WSEC_SUPPORT_LOCK_FUN
#endif
#ifdef WSEC_LINUX
    #define WSEC_SUPPORT_LOCK_FUN
#endif

WSEC_HANDLE g_hLock[WSEC_LOCK_NUM] = {0};

#ifdef WSEC_DEBUG
    WSEC_BOOL g_bCpuEndianModeChked = WSEC_FALSE;
#endif
WSEC_BOOL g_bIsBigEndianMode = WSEC_FALSE;

/* 注册函数 */
WSEC_CALLBACK_FUN_STRU g_RegFun = {
    /* WSEC_MEMORY_CALLBACK_STRU */
    {malloc, free, memcmp}, 

    /* WSEC_FILE_CALLBACK_STRU */
    /*{WSEC_NULL_PTR},*/
    {fopen, fclose, fread, fwrite, fflush, remove, fgetc, fgets, ftell, fseek, feof, ferror}, 

    /* WSEC_LOCK_CALLBACK_STRU */
#ifdef WSEC_SUPPORT_LOCK_FUN
    {WSEC_DeftCreateLock, WSEC_DeftDestroyLock, WSEC_DeftLock, WSEC_DeftUnlock}, 
#else
    {WSEC_NULL_PTR},
#endif

    /* WSEC_BASE_RELY_APP_CALLBACK_STRU */
    {WSEC_NULL_PTR}, 
};

/* 周期性调用的函数 */
/*lint -e651*/
WSEC_PERIODIC_CALL_STRU g_PeriodCall[] = {
    {KMC_RefreshMkMask, {0}, {0}, 3600}, /* 每1小时刷新掩码 */
    {KMC_ChkMkStatus,   {0}, {0}, 3600}, /* 每1小时尝试检测一次MK密钥状态是否过期 */
    {KMC_ChkRkStatus,   {0}, {0}, 3600}, /* 每1小时尝试检测一次RK密钥状态是否过期 */
};
WSEC_INT32 g_nChkPeriodCallIndex = 0;
WSEC_SYS_STRU g_CbbSys = {WSEC_WAIT_INIT};

WSEC_SUBCBB_STRU g_SubCbbFunc[] = {
    {"KMC", KMC_Finalize,  KMC_Reset},
};

WSEC_VOID WSEC_ChkCpuEndianMode();
WSEC_ERR_T WSEC_RegFunc(const WSEC_FP_CALLBACK_STRU* pstCallBack);
WSEC_VOID WSEC_LogEnvironment();

#ifdef WSEC_DEBUG
extern WSEC_VOID WSEC_ShowStructSize(WSEC_FP_ShowStructSize pfShow);
extern WSEC_VOID KMC_ShowStructSize(WSEC_FP_ShowStructSize pfShow);
extern WSEC_VOID SDP_ShowStructSize(WSEC_FP_ShowStructSize pfShow);
#endif

/*****************************************************************************
 函 数 名  : WSEC_ChkCpuEndianMode
 功能描述  : 检测CPU字节序对齐模式
             将检测结果保存到全局变量 g_bIsBigEndianMode
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2014-8-29
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_ChkCpuEndianMode()
{
    WSEC_BYTE bytes[] = {0x12, 0x34};
    WSEC_UINT16* pShort = (WSEC_UINT16*)bytes;

    g_bIsBigEndianMode = (0x1234 == *pShort) ? WSEC_TRUE : WSEC_FALSE;
#ifdef WSEC_DEBUG
    g_bCpuEndianModeChked = WSEC_TRUE;
#endif

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_IsBigEndianMode
 功能描述  : 判断CPU字节序对齐是否为大端对齐
 输入参数  : 无
 输出参数  : 无
 返 回 值  : T=大端对齐/F=小端对齐

 修改历史      :
  1.日    期   : 2014-8-29
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_IsBigEndianMode()
{
#ifdef WSEC_DEBUG
    WSEC_ASSERT(g_bCpuEndianModeChked);
#endif
    return g_bIsBigEndianMode;
}

/*****************************************************************************
 函 数 名  : WSEC_RegFunc
 功能描述  : 登记APP的注册的回调函数
 输入参数  : pstCallBack: 各类回调函数
 输出参数  : 无
 返 回 值  : WSEC_ERR_T

 修改历史      :
  1.日    期   : 2014-8-26
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T WSEC_RegFunc(const WSEC_FP_CALLBACK_STRU* pstCallBack)
{
    const WSEC_LOCK_CALLBACK_STRU* pstLock = WSEC_NULL_PTR;
    const WSEC_BASE_RELY_APP_CALLBACK_STRU* pstRelyApp = WSEC_NULL_PTR;
    WSEC_SIZE_T nNullFunNum;

    if (!pstCallBack) {return WSEC_ERR_INVALID_ARG;}

    pstRelyApp = &pstCallBack->stRelyApp;
    pstLock    = &pstCallBack->stLock;

    /* 1. 内存类(可选) */
    nNullFunNum = WSEC_GetZeroItemCount(&pstCallBack->stMemory, sizeof(pstCallBack->stMemory), sizeof(pstCallBack->stMemory.pfMemAlloc));
    if (0 == nNullFunNum) /* 提供了所有回调函数都 */
    {
        return_oper_if(WSEC_MEMCPY(&g_RegFun.stMemory, sizeof(g_RegFun.stMemory), &pstCallBack->stMemory, sizeof(pstCallBack->stMemory)) != EOK,
                       WSEC_LOG_E4MEMCPY, WSEC_ERR_MEMCPY_FAIL);
    }
    else if (nNullFunNum != (sizeof(pstCallBack->stMemory) / sizeof(pstCallBack->stMemory.pfMemAlloc)))
    {
        WSEC_LOG_E("Memory-oper callback function must be all provided");
        return WSEC_ERR_INVALID_ARG;
    }else{;}

    /* 2. 文件操作类(可选) */
    nNullFunNum = WSEC_GetZeroItemCount(&pstCallBack->stFile, sizeof(pstCallBack->stFile), sizeof(pstCallBack->stFile.pfFclose));
    if (0 == nNullFunNum) /* 提供了所有回调函数都 */
    {
        return_oper_if(WSEC_MEMCPY(&g_RegFun.stFile, sizeof(g_RegFun.stFile), &pstCallBack->stFile, sizeof(pstCallBack->stFile)) != EOK,
                       WSEC_LOG_E4MEMCPY, WSEC_ERR_MEMCPY_FAIL);
    }
    else if (nNullFunNum != (sizeof(pstCallBack->stFile) / sizeof(pstCallBack->stFile.pfFclose)))
    {
        WSEC_LOG_E("File-oper callback function must be all provided");
        return WSEC_ERR_INVALID_ARG;
    }else{;}

    /* 3. 强依赖APP且必须提供的函数 */
    return_oper_if(!pstRelyApp->pfWriLog, WSEC_LOG_E("'pfWriLog' must be provided"), WSEC_ERR_INVALID_ARG);
    g_RegFun.stRelyApp.pfWriLog = pstRelyApp->pfWriLog;
    
    return_oper_if(!pstRelyApp->pfNotify, WSEC_LOG_E("'pfNotify' must be provided"), WSEC_ERR_INVALID_ARG);
    g_RegFun.stRelyApp.pfNotify = pstRelyApp->pfNotify;

    return_oper_if(!pstRelyApp->pfDoEvents, WSEC_LOG_E("'pfDoEvents' must be provided"), WSEC_ERR_INVALID_ARG);
    g_RegFun.stRelyApp.pfDoEvents = pstRelyApp->pfDoEvents;

    /* 4. 锁操作类(半强制类) */
    nNullFunNum = WSEC_GetZeroItemCount(pstLock, sizeof(WSEC_LOCK_CALLBACK_STRU), sizeof(pstLock->pfCreateLock));
    if (0 == nNullFunNum) /* 完整地给出了锁操作函数 */
    {
        return_oper_if(WSEC_MEMCPY(&g_RegFun.stLock, sizeof(g_RegFun.stLock), pstLock, sizeof(WSEC_LOCK_CALLBACK_STRU)) != EOK, 
                       WSEC_LOG_E4MEMCPY, WSEC_ERR_MEMCPY_FAIL);
    }
    else if (nNullFunNum != (sizeof(WSEC_LOCK_CALLBACK_STRU) / sizeof(pstLock->pfCreateLock))) /* 锁操作函数不完整 */
    {
        WSEC_LOG_E("All callback functions of LOCK must provid.");
        return WSEC_ERR_INVALID_ARG;
    }
    else /* 锁操作类函数全未给出, 则检查OS编译开关是否可以用缺省函数 */
    {
#ifndef WSEC_SUPPORT_LOCK_FUN
        WSEC_LOG_E("All callback functions of LOCK must provid.");
        return WSEC_ERR_INVALID_ARG;
#endif
    }

    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : WSEC_LogEnvironment
 功能描述  : 将运行环境信息记录到日志.
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年3月7日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_LogEnvironment()
{
    WSEC_BOOL bEndialBig = WSEC_FALSE;
    const WSEC_CHAR* pszTemp = WSEC_NULL_PTR;
    const WSEC_CHAR *pszKmc = "", *pszSdp = "";

    WSEC_LOG_I("\r\n\r\n============= Environment begin =============");
#ifdef WSEC_DEBUG
    WSEC_ShowStructSize(WSEC_LogStructSize);

    KMC_ShowStructSize(WSEC_LogStructSize);
#ifdef WSEC_COMPILE_SDP
    SDP_ShowStructSize(WSEC_LogStructSize);
#endif
#endif

    /* 密码学算法库 */
#ifdef WSEC_COMPILE_CAC_OPENSSL
    WSEC_LOG_I("CBB based on OpenSSL");
#else
#ifdef WSEC_COMPILE_CAC_IPSI
    WSEC_LOG_I("CBB based on iPSI");
#else
    WSEC_LOG_I("CBB based on nothing");
#endif
#endif

    /* OS平台 */
#ifdef WSEC_WIN32
    WSEC_LOG_I("OS is Windows.");
#else
#ifdef WSEC_LINUX
    WSEC_LOG_I("OS is LINUX.");
#else
    WSEC_LOG_I("OS is neither Windows nor LINUX."); 
#endif
#endif

    /* 启用了哪些CBB子模块 */
    pszKmc = "KMC";
#ifdef WSEC_COMPILE_SDP
    pszSdp = "SDP";
#endif

    WSEC_LOG_I2("Use subcbb: %s %s", pszKmc, pszSdp);

    /* CPU字节对齐模式 */
#if WSEC_CPU_ENDIAN_MODE == WSEC_CPU_ENDIAL_BIG /* 指定为大端对齐模式 */
    bEndialBig = WSEC_TRUE;
#elif WSEC_CPU_ENDIAN_MODE == WSEC_CPU_ENDIAL_LITTLE /* 指定为小端对齐模式 */
    bEndialBig = WSEC_FALSE;
#else /* 程序自动检测 */
    bEndialBig = g_bIsBigEndianMode;
#endif
    pszTemp = bEndialBig ? "Big" : "Small";
    WSEC_LOG_I1("CPU ENDIAL is %s", pszTemp);

    WSEC_LOG_I("\r\n============= Environment end =============\r\n\r\n");

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_Initialize
 功能描述  : CBB初始化.
             CBB所有函数均跨平台，与平台相关的函数则由APP注册, CBB回调方式解决.
             因此, APP在使用CBB的其它功能函数前(如SDP, KMC的函数), 需要先调用
             此函数.
 纯 入 参  : pstFileName: 含Keystore文件名和KMC配置文件.
                          若使用SDP或KMC则必须提供，否则可NULL.
                          若提供文件名, 则必须提供2个Keystore或KMC配置文件名. 需要2个文件名的
                          目的是出于可靠性考虑, CBB自动实现2个文件的数据同步, 如果某文件数据损
                          坏, 则自动切换使用另一个文件的数据.
             pstCallbackFun: APP提供给CBB的回调函数.
             pstRptProgress: 初始化过程中如果耗时较长则上报进度[可空]
             pvReserved:       预留参数, 后续扩展用
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月20日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T WSEC_Initialize(const KMC_FILE_NAME_STRU* pstFileName,
                           const WSEC_FP_CALLBACK_STRU* pstCallbackFun,
                           const WSEC_PROGRESS_RPT_STRU* pstRptProgress,
                           const WSEC_VOID* pvReserved)
{
    WSEC_ERR_T nRet = WSEC_SUCCESS;

    return_oper_if(WSEC_RUNNING == g_CbbSys.eState, WSEC_LOG_E("WSEC CBB is running, cannot be Initialize before finalize."), WSEC_ERR_INVALID_CALL_SEQ);
    g_CbbSys.eState = WSEC_INIT_FAIL;

    /* 1. 回调函数注册 */
    nRet = WSEC_RegFunc(pstCallbackFun);
    if (nRet != WSEC_SUCCESS) {return nRet;}

    /* 2. 确定CPU字节序 */
    WSEC_ChkCpuEndianMode();

    WSEC_LogEnvironment();

    /* 3. 对锁初始化 */
    nRet = WSEC_InitializeLock();
    return_oper_if((nRet != WSEC_SUCCESS), WSEC_LOG_E1("WSEC_InitializeLock() = %u.", nRet), nRet);

    /* 4. 如果启用了SDP/KMC则对KMC初始化 */
    nRet = KMC_Initialize(pstFileName, &pstCallbackFun->stKmcCallbackFun, pstRptProgress);
    if (WSEC_SUCCESS == nRet)
    {
#ifdef WSEC_COMPILE_SDP
        nRet = SDP_Initialize();
#endif
    }

    if (WSEC_SUCCESS == nRet)
    {
        g_CbbSys.eState = WSEC_RUNNING;
        WSEC_LOG_E("WSEC CBB Initialized successful.");
    }
    else
    {
        WSEC_Finalize();
        WSEC_LOG_E("WSEC CBB Initialized fail.");
    }

    return nRet;
}

/*****************************************************************************
 函 数 名  : WSEC_OnTimer
 功能描述  : APP对CBB周期性驱动
             CBB需要例行执行一些功能, 如检测密钥过期, 掩码刷新等, 但CBB不是'任务',
             是静态的函数集合, 所以需要APP周期性驱动.
 纯 入 参  : pstRptProgress: 进度上报指示相关的回调函数信息
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 驱动周期: 建议5分钟.

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_OnTimer(const WSEC_PROGRESS_RPT_STRU* pstRptProgress)
{
    WSEC_PERIODIC_CALL_STRU* pCall;
    WSEC_SYSTIME_T stLocalNow = {0}, stUtcNow = {0};
    WSEC_INT32 nSec;
    WSEC_BOOL bCallNow = WSEC_FALSE;
#ifdef WSEC_DEBUG
    const WSEC_CHAR* pszFunName = WSEC_NULL_PTR;
#endif

    if_oper((!WSEC_GetLocalDateTime(&stLocalNow)) || (!WSEC_GetUtcDateTime(&stUtcNow)), return);
    if_oper(!WSEC_IS_NORMAL_YEAR(stLocalNow.uwYear), return); /* 时钟尚未就绪, 无法处理 */

    KMC_Correct4Clock();

    if_oper(g_nChkPeriodCallIndex >= WSEC_NUM_OF(g_PeriodCall), g_nChkPeriodCallIndex = 0);
    pCall = g_PeriodCall + g_nChkPeriodCallIndex;
    for (; g_nChkPeriodCallIndex < WSEC_NUM_OF(g_PeriodCall); g_nChkPeriodCallIndex++, pCall++)
    {
        continue_if(!pCall->pfPeriodicCall);

        if (!WSEC_IsDateTime(&pCall->stPreCallTimeUtc)) {WSEC_DateTimeCopy(&pCall->stPreCallTimeUtc, &stUtcNow);}
        nSec = WSEC_DateTimeDiff(dtpSecond, &pCall->stPreCallTimeUtc, &stUtcNow);
        bCallNow = ((WSEC_UINT32)nSec >= pCall->ulPeriodSec);
        
        continue_if(!bCallNow);
        if (nSec <= 0) /* 系统时间回调, 则无法判断本次是否例行检查, 记录当前时间, 以便后续判断  */
        {
            WSEC_DateTimeCopy(&pCall->stPreCallTimeUtc, &stUtcNow);
            continue;
        }
        
#ifdef WSEC_DEBUG
        if (pCall->pfPeriodicCall == KMC_RefreshMkMask) pszFunName = "KMC_RefreshMkMask";
        if (pCall->pfPeriodicCall == KMC_ChkMkStatus) pszFunName = "KMC_ChkMkStatus";
        if (pCall->pfPeriodicCall == KMC_ChkRkStatus) pszFunName = "KMC_ChkRkStatus";
        WSEC_LOG_I2("OnTimer() call '%s', PeriodSec = %d", pszFunName, pCall->ulPeriodSec);
#endif
        pCall->pfPeriodicCall(&stLocalNow, &stUtcNow, pstRptProgress, &pCall->stExecInfo);
        WSEC_DateTimeCopy(&pCall->stPreCallTimeUtc, &stUtcNow);

        g_nChkPeriodCallIndex++;
        break; /* 每次只执行一个函数, 避免过度占用CPU */
    }

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_OnAppReady
 功能描述  : APP就绪通知
             当APP的某些资源就绪后，通知CBB.
 输入参数  : eType: 标识哪种资源就绪
             pData: 通知所带的数据(可NULL)
             nDataSize: 通知数据的长度(可0)
 输出参数  : 无
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2015年5月25日
    作    者   : z118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_OnAppReady(WSEC_APP_READY_ENUM eType, const WSEC_VOID* pData, WSEC_SIZE_T nDataSize)
{
    switch (eType)
    {
    case WSEC_APP_CLOCK_READY:
        KMC_Correct4Clock();
        break;

    default:
        WSEC_ASSERT_FALSE;
    }
}

/*****************************************************************************
 函 数 名  : WSEC_Finalize
 功能描述  : CBB优雅结束
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)

 修改历史      :
  1.日    期   : 2014年8月29日
    作    者   : z118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T WSEC_Finalize()
{
    WSEC_ERR_T nRet = WSEC_SUCCESS, nTemp = WSEC_SUCCESS;
    WSEC_SUBCBB_STRU* pstSubCbb;
    WSEC_SIZE_T i;

    for (i = 0, pstSubCbb = g_SubCbbFunc; i < WSEC_NUM_OF(g_SubCbbFunc); i++, pstSubCbb++)
    {
        continue_if(!pstSubCbb->pfFinalize);
        
        nTemp = pstSubCbb->pfFinalize();
        if (nTemp != WSEC_SUCCESS)
        {
            WSEC_LOG_E2("'%s' Finalize fail, return %u.", pstSubCbb->szSubCbbName, nTemp);
            nRet = nTemp;
        }
    }

    WSEC_FinalizeLock();
    
    g_CbbSys.eState = WSEC_WAIT_INIT;
    WSEC_LOG_W("WSEC Finalized.");
    
    return nRet;
}

/*****************************************************************************
 函 数 名  : WSEC_Reset
 功能描述  : CBB复位
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)

 修改历史      :
  1.日    期   : 2015年1月22日
    作    者   : z118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T WSEC_Reset()
{
    WSEC_ERR_T nErrCode = WSEC_SUCCESS, nTemp;
    WSEC_SUBCBB_STRU* pstSubCbb;
    WSEC_SIZE_T i;

    return_oper_if(g_CbbSys.eState != WSEC_RUNNING, WSEC_LOG_E("CBB is not running."), WSEC_ERR_INVALID_CALL_SEQ);

    for (i = 0, pstSubCbb = g_SubCbbFunc; i < WSEC_NUM_OF(g_SubCbbFunc); i++, pstSubCbb++)
    {
        continue_if(!pstSubCbb->pfReset);
        
        nTemp = pstSubCbb->pfReset();
        if (nTemp != WSEC_SUCCESS)
        {
            WSEC_LOG_E2("Reset '%s' fail, return %u.", pstSubCbb->szSubCbbName, nTemp);
            nErrCode = nTemp;
        }
    }

    g_CbbSys.eState = WSEC_SUCCESS == nErrCode ? WSEC_RUNNING : WSEC_INIT_FAIL;
    WSEC_LOG_W("WSEC Reset.");

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : WSEC_GetVersion
 功能描述  : 查询CBB版本号
 输入参数  : 无
 输出参数  : 无
 返 回 值  : CBB版本号

序号 版本号       发布时间    主要功能
--------------------------------------------------------------------------
1    V100R001C00  2015-05-30  初始版本.
2    ?
3    ?
*****************************************************************************/
const WSEC_CHAR* WSEC_GetVersion()
{
    const WSEC_CHAR* pszVer = "KMC V100R001C00SPC001";
    return pszVer;
}

/*****************************************************************************
 函 数 名  : WSEC_CreateLock
 功能描述  : 创建一个锁
 输入参数  : OUT WSEC_HANDLE *phMutex
 输出参数  : 无
 返 回 值  : WSEC_ERR_T

 修改历史      :
  1.日    期   : 2013 年6 - 13
    作    者   : l00171031
    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T WSEC_CreateLock(WSEC_HANDLE *phMutex)
{
    WSEC_ASSERT(phMutex);
    WSEC_ASSERT(g_RegFun.stLock.pfCreateLock);

    return g_RegFun.stLock.pfCreateLock(phMutex) ? WSEC_SUCCESS : WSEC_FAILURE;
}

/*****************************************************************************
 函 数 名  : WSEC_DestroyLock
 功能描述  : 销毁一个锁
 输入参数  : INOUT WSEC_HANDLE *phMutex
 输出参数  : 无
 返 回 值  : WSEC_VOID

 修改历史      :
  1.日    期   : 2013年6 - 13
    作    者   : l00171031
    修改内容   : 新生成函数

*****************************************************************************/
WSEC_VOID WSEC_DestroyLock(WSEC_HANDLE *phMutex)
{
    if (phMutex && (*phMutex))
    {
        g_RegFun.stLock.pfDestroyLock(*phMutex);
        *phMutex = WSEC_NULL_PTR;
    }

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_Lock
 功能描述  : 加锁
 输入参数  : eLockId: 资源标识
 输出参数  : 无
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2014-12-27
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_Lock(WSEC_LOCK_FOR_ENUM eLockId)
{
    WSEC_HANDLE hMutex = g_hLock[eLockId];

    /* Misinformation: FORTIFY.Unreleased_Resource--Synchronization */
    if (hMutex) {g_RegFun.stLock.pfLock(hMutex);}

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_Unlock
 功能描述  : 解锁
 输入参数  : eLockId: 资源标识
 输出参数  : 无
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2014-12-27
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_Unlock(WSEC_LOCK_FOR_ENUM eLockId)
{
    WSEC_HANDLE hMutex = g_hLock[eLockId];

    if (hMutex)
    {
        WSEC_ASSERT(g_RegFun.stLock.pfUnlock);
        g_RegFun.stLock.pfUnlock(hMutex);
    }else{;}

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_StringClone
 功能描述  : 克隆字符串
 纯 入 参  : pszCloneFrom: 被克隆的字符串
             pszCallerFile, nCallerLine: 调用此函数的语句所在文件及行号[DEBUG版用]
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 克隆出的字符串, WSEC_NULL_PTR则意味克隆失败.
 特别注意  : Caller需要管理返回指针并负责释放.

 修改历史
  1.日    期   : 2014年10月22日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_CHAR* WSEC_StringClone(const WSEC_CHAR* pszCloneFrom, const WSEC_CHAR* pszCallerFile, WSEC_INT32 nCallerLine)
{
    WSEC_CHAR* pszNew = WSEC_NULL_PTR;
    WSEC_SIZE_T nLen = 0;

    return_oper_if(!pszCloneFrom, oper_null, pszNew);

    nLen = WSEC_STRLEN(pszCloneFrom);
    return_oper_if(nLen < 1, oper_null, pszNew);

    nLen += 2; /* 存储字符串需考虑'\0'及unicode编码 */

    pszNew = (WSEC_CHAR*)WSEC_MALLOC(nLen);
    if (pszNew)
    {
        if (strcpy_s(pszNew, nLen, pszCloneFrom) != EOK) /* 复制失败则释放内存 */
        {
            WSEC_FREE(pszNew);
        }
    }
#ifdef WSEC_TRACE_MEMORY
    if (pszNew) {WSEC_LOG_E3("WSEC-StringClone() = 0x%p, at %s, Line-%d", pszNew, pszCallerFile, nCallerLine);}
#endif

    return pszNew;
}

/*****************************************************************************
 函 数 名  : WSEC_BuffClone
 功能描述  : 缓冲区克隆:
             1. 申请一块等大的缓冲区
             2. 将源数据复制
 纯 入 参  : pCloneFrom: 克隆源对象;
             nSize:      克隆源长度.
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 克隆成功后的新地址
 特别注意  : Caller需要管理返回指针并负责释放.

 修改历史
  1.日    期   : 2014年10月31日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID* WSEC_BuffClone(const WSEC_VOID* pCloneFrom, WSEC_SIZE_T nSize, const WSEC_CHAR* pszCallerFile, WSEC_INT32 nCallerLine)
{
    WSEC_VOID* pNew = WSEC_NULL_PTR;

    WSEC_ASSERT(pCloneFrom && (nSize > 0));
    pNew = WSEC_MALLOC(nSize);

    return_oper_if(!pNew, oper_null, pNew);

    if (WSEC_MEMCPY(pNew, nSize, pCloneFrom, nSize) != EOK) /* 复制失败则释放内存 */
    {
        WSEC_FREE(pNew);
    }

#ifdef WSEC_TRACE_MEMORY
    if (pNew) {WSEC_LOG_E3("WSEC-BuffClone() = 0x%p, at %s, Line-%d", pNew, pszCallerFile, nCallerLine);}
#endif

    return pNew;
}

/*****************************************************************************
 函 数 名  : WSEC_Xor
 功能描述  : 将两块内存的数据逐字节异或
 纯 入 参  : pOperand1, nOperand1Len: 参与异或的数据流1及其长度
             pOperand2, nOperand2Len: 参与异或的数据流2及其长度
             nResultLen: 保存异或结果的缓冲区长度
 纯 出 参  : pResult: 保存异或结果.
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月24日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_Xor(const WSEC_BYTE* pOperand1, WSEC_SIZE_T nOperand1Len,
                   const WSEC_BYTE* pOperand2, WSEC_SIZE_T nOperand2Len,
                   WSEC_BYTE* pResult, WSEC_SIZE_T nResultLen)
{
    WSEC_SIZE_T nXorLen = (nOperand1Len < nOperand2Len) ? nOperand1Len : nOperand2Len;
    WSEC_SIZE_T i;

    if (nXorLen > nResultLen) {nXorLen = nResultLen;} /* 取3方最短缓冲区的数据异或 */
    for (i = 0; i < nXorLen; i++, pOperand1++, pOperand2++, pResult++)
    {
        *pResult = (*pOperand1) ^ (*pOperand2);
    }

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_GetZeroItemCount
 功能描述  : 统计缓冲区中是0的元素个数
 纯 入 参  : pvData: 缓冲区首地址
             nSize:  缓冲区总长度
             nItemSize: 缓冲区中各元素的长度
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 0元素个数
 特别注意  : 无

 修改历史
  1.日    期   : 2014年3月3日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_SIZE_T WSEC_GetZeroItemCount(const WSEC_VOID* pvData, WSEC_SIZE_T nSize, WSEC_SIZE_T nItemSize)
{
    WSEC_SIZE_T nCount = 0, i;
    WSEC_BYTE aZero[16] = {0};
    WSEC_BYTE* pbChk;
    
    WSEC_ASSERT((nSize % nItemSize) == 0);
    WSEC_ASSERT(nItemSize <= sizeof(aZero));

    for (pbChk = (WSEC_BYTE*)pvData, i = 0; i < nSize; i += nItemSize, pbChk += nItemSize)
    {
        if (WSEC_MEMCMP(pbChk, aZero, nItemSize) == 0)
        {
            nCount++;
        }
    }

    return nCount;
}

/*****************************************************************************
 函 数 名  : WSEC_CopyFile
 功能描述  : 文件复制
 纯 入 参  : pszFrom: 源文件;
             pszTo:   目标文件.
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 成功或失败
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月03日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_CopyFile(const WSEC_CHAR* pszFrom, const WSEC_CHAR* pszTo)
{
    WSEC_FILE fRead = WSEC_NULL_PTR;
    WSEC_FILE fWri  = WSEC_NULL_PTR;
    WSEC_BYTE* pRead = WSEC_NULL_PTR;
    const WSEC_SIZE_T nBuffLen = WSEC_FILE_IO_SIZE_MAX;
    WSEC_SIZE_T nRead, nWrite;
    WSEC_BOOL bReturn = WSEC_TRUE;

    return_oper_if((!pszFrom) || (!pszTo), oper_null, WSEC_FALSE);

    fRead = WSEC_FOPEN(pszFrom, "rb");
    fWri  = WSEC_FOPEN(pszTo, "wb");
    pRead = (WSEC_BYTE*)WSEC_MALLOC(nBuffLen);

    do
    {
        break_oper_if(!fRead, WSEC_LOG_E1("Open file(%s) fail.", pszFrom), bReturn = WSEC_FALSE);
        break_oper_if(!fWri, WSEC_LOG_E1("Open file(%s) fail.", pszTo), bReturn = WSEC_FALSE);
        break_oper_if(!pRead, WSEC_LOG_E4MALLOC(nBuffLen), bReturn = WSEC_FALSE);

        while (!WSEC_FEOF(fRead))
        {
            nRead = WSEC_FREAD(pRead, 1, nBuffLen, fRead);
            WSEC_UNCARE(nRead);
            break_oper_if(WSEC_FERROR(fRead), WSEC_LOG_E1("Read file(%s) fail.", pszFrom), bReturn = WSEC_FALSE);

            nWrite = WSEC_FWRITE(pRead, 1, nRead, fWri);
            break_oper_if(nWrite != nRead, WSEC_LOG_E1("Write file(%s) fail.", pszTo), bReturn = WSEC_FALSE);
        }

        if (bReturn) {WSEC_FFLUSH(fWri);}
    } do_end;
    
    if (fRead) {WSEC_FCLOSE(fRead);}
    if (fWri) {WSEC_FCLOSE(fWri);}
    if (pRead) {WSEC_FREE(pRead);}

    return bReturn;
}

/*****************************************************************************
 函 数 名  : WSEC_GetFileLen
 功能描述  : 获取文件占用长度
 纯 入 参  : pszFileName: 文件名;
 纯 出 参  : pulLen: 文件长度
 入参出参  : 无
 返 回 值  : 成功或失败
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_GetFileLen(const WSEC_CHAR* pszFileName, WSEC_FILE_LEN* pulLen)
{
    WSEC_FILE fRead;
    long nLen = -1;

    return_oper_if((!pszFileName) || (!pulLen), oper_null, WSEC_FALSE);
    fRead = WSEC_FOPEN(pszFileName, "rb");
    if (!fRead) {return WSEC_FALSE;}

    do
    {
        if (WSEC_FSEEK(fRead, 0, SEEK_END) != 0) {break;}
        nLen = WSEC_FTELL(fRead);
    }do_end;
    WSEC_FCLOSE((fRead));

    if (nLen >= 0) {*pulLen = (WSEC_FILE_LEN)nLen;}

    return (nLen >= 0);
}

/*****************************************************************************
 函 数 名  : WSEC_DeleteFileS
 功能描述  : 安全地删除文件
 纯 入 参  : pszFileName: 文件名;
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 成功或失败
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_DeleteFileS(const WSEC_CHAR* pszFileName)
{
    WSEC_FILE fWri = WSEC_NULL_PTR;
    WSEC_FILE_LEN ulFileLen = 0;
    WSEC_UINT32 ulWriLen = 0;
    WSEC_BUFF stWri = {0};
    WSEC_BOOL bReturn = WSEC_TRUE;

    WSEC_ASSERT(pszFileName);
    
    return_oper_if(!WSEC_GetFileLen(pszFileName, &ulFileLen), WSEC_LOG_E1("WSEC_GetFileLen(%s) fail.", pszFileName), WSEC_FALSE);
    
    WSEC_BUFF_ALLOC(stWri, 2048); /* 最多以2K为单位覆盖待删除文件 */
    return_oper_if(!stWri.pBuff, WSEC_LOG_E4MALLOC(stWri.nLen), WSEC_FALSE);

    do
    {
        fWri = WSEC_FOPEN(pszFileName, "wb");
        break_oper_if(!fWri, WSEC_LOG_E1("Open file(%s) fail.", pszFileName), bReturn = WSEC_FALSE);

        while (ulFileLen > 0)
        {
            ulWriLen = (ulFileLen > stWri.nLen) ? stWri.nLen : ulFileLen;
            WSEC_UNCARE(WSEC_FWRITE_MUST(stWri.pBuff, ulWriLen, fWri));
            ulFileLen -= ulWriLen;
        }
    } do_end;
    WSEC_BUFF_FREE(stWri);
    if (fWri) {WSEC_FCLOSE(fWri);}

    /* Misinformation: FORTIFY.Race_Condition--File_System_Access */
    if (bReturn) {bReturn = (WSEC_FREMOVE(pszFileName) == 0);}

    return bReturn;
}

/*****************************************************************************
 函 数 名  : WSEC_WriteFileS
 功能描述  : 安全地向一组文件保存数据
 纯 入 参  : pvData: 待写数据;
             ppszFile: 一组文件名
             nFileNum: 文件个数
             pfWriFile: 写文件的执行函数
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T WSEC_WriteFileS(const WSEC_VOID* pvData, WSEC_CHAR** ppszFile, WSEC_SIZE_T nFileNum, WSEC_WriteFile pfWriFile, const WSEC_VOID* pvReserved)
{
    WSEC_SIZE_T i;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    WSEC_CHAR** ppszFileFrom = WSEC_NULL_PTR;
    WSEC_CHAR* pszCopyFromFile = WSEC_NULL_PTR;

    for (i = 0, ppszFileFrom = ppszFile; i < nFileNum; i++, ppszFileFrom++)
    {
        nErrCode = pfWriFile(pvData, *ppszFileFrom, pvReserved);
        break_oper_if(WSEC_SUCCESS == nErrCode, pszCopyFromFile = *ppszFileFrom, oper_null); /* 任意文件写成功, 即将此文件内容拷贝到其余文件 */
    }
    return_oper_if(!pszCopyFromFile, oper_null, nErrCode); /* 向所有文件写数据均失败 */

    /* 将写成功的文件复制给其余文件 */
    for (i = 0, ppszFileFrom = ppszFile; i < nFileNum; i++, ppszFileFrom++)
    {
        continue_if(pszCopyFromFile == *ppszFileFrom);
        if (!WSEC_CopyFile(pszCopyFromFile, *ppszFileFrom)) /* 复制失败, 仅记日志即可 */
        {
            WSEC_LOG_E2("copy file from '%s' to '%s' fail.", pszCopyFromFile, *ppszFileFrom);
        }
    }

    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : WSEC_MemAlloc
 功能描述  : 分配内存, 并且初始化为全0
 输入参数  : ulSize: 待分配内存长度
             pszFile, nLine: 调用此函数的语句所在文件及行号[DEBUG版用]
 输出参数  : 无
 返 回 值  : 分配内存的起始地址.

 修改历史      :
  1.日    期   : 2014年10月20日
    作    者   : z118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID* WSEC_MemAlloc(WSEC_UINT32 ulSize, const WSEC_CHAR* pszFile, WSEC_INT32 nLine)
{
    WSEC_VOID * ptr = WSEC_NULL_PTR;

    if (0 == ulSize)
    {
        WSEC_ASSERT(WSEC_FALSE);
        return ptr;
    }

    if (!g_RegFun.stMemory.pfMemAlloc) {g_RegFun.stMemory.pfMemAlloc = malloc;}

    /* Misinformation: FORTIFY.Integer_Overflow */
    ptr = g_RegFun.stMemory.pfMemAlloc(ulSize);
    if(ptr != WSEC_NULL_PTR)
    {
        WSEC_UNCARE(WSEC_MEMSET(ptr, ulSize, 0, ulSize));
    }

    /* DEBUG版本下记录内存申请 */
#ifdef WSEC_TRACE_MEMORY
    WSEC_LOG_E4("WSEC-MemAlloc(%d) = 0x%p, at: %s, Line-%d", ulSize, ptr, pszFile, nLine);
#endif

    return ptr;
}

/*****************************************************************************
 函 数 名  : WSEC_MemFree
 功能描述  : 释放内存.
 输入参数  : ptr: 待释放内存
             pszFile, nLine: 调用此函数的语句所在文件及行号[DEBUG版用]
 输出参数  : 无
 返 回 值  : 空指针, 以方便Caller释放内存的同时将指针置为无效, 如: 
             pBuff = WSEC_MemFree(pBuff, __FILE__, __LINE__);

 修改历史      :
  1.日    期   : 2014年10月20日
    作    者   : z118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID* WSEC_MemFree(WSEC_VOID* ptr, const WSEC_CHAR* pszFile, WSEC_INT32 nLine)
{
    if (!ptr) {return ptr;} /* 避免重复释放 */

    /* DEBUG版本下记录内存释放 */
#ifdef WSEC_TRACE_MEMORY
    WSEC_LOG_E3("WSEC-MemFree(0x%p) at: %s, Line-%d", ptr, pszFile, nLine);
#endif

    if (!g_RegFun.stMemory.pfMemFree) {g_RegFun.stMemory.pfMemFree = free;}
    g_RegFun.stMemory.pfMemFree(ptr);
    ptr = WSEC_NULL_PTR;

    return ptr;
}

/*****************************************************************************
 函 数 名  : WSEC_WriLog
 功能描述  : CBB内部写日志操作，该函数将日志信息通过回调APP的日志注册函数抛出.
 输入参数  : pszFileName: 调用者所在文件名
             nLineNo:     调用者所在行号
             eLevel:      信息重要程度
             format:      格式化字符串
             ...          个数及类型可变的参数表
 输出参数  : 无
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2014年7月11日
    作    者   : z118096
    修改内容   : 新生成函数
*****************************************************************************/
/*lint -e960*/
WSEC_VOID WSEC_WriLog(const WSEC_CHAR* pszFileName, WSEC_INT32 nLineNo, WSEC_LOG_LEVEL_ENUM eLevel, const WSEC_CHAR* format, ...)
{
    va_list marker;
    WSEC_CHAR szLog[WSEC_LOG_BUFF_SIZE] = {0};
    const WSEC_CHAR* pPureFileName;
    WSEC_INT32 nPos = 0;

    if (!g_RegFun.stRelyApp.pfWriLog) {return;}

    /*1. 通过格式化解析出日志串 */
    va_start(marker, format);
    WSEC_UNCARE(vsprintf_s(szLog, WSEC_LOG_BUFF_SIZE, format, marker)); /* Misinformation: FORTIFY.Format_String */
    va_end(marker);

    /*2. 根据产品输出日志的特点，自动补齐或去除日志中的换行符 */
    /*2.1 删除日志串尾部的换行符 */
    nPos = WSEC_STRLEN(szLog) - 1;
    if (nPos >= 0)
    {
        if ('\n' == szLog[nPos])
        {
            szLog[nPos] = '\0';
            nPos--;
        }
    }
    if (nPos >= 0)
    {
        if ('\r' == szLog[nPos])
        {
            szLog[nPos] = '\0';
            nPos--;
        }
    }

    /* 2.2 根据需要, 自动添加换行符 */
#ifndef WSEC_WRI_LOG_AUTO_END_WITH_CRLF /* 产品写日志时未自动换行，则追加以保证日志的可读性 */
    nPos++;
    if ((0 <= nPos) && ((nPos + 2) < WSEC_LOG_BUFF_SIZE))
    {
        szLog[nPos] = '\r';
        nPos++;
        szLog[nPos] = '\n';
        nPos++;
        szLog[nPos] = '\0';
    }
#endif

    /* 3. 获取纯文件名(不含路径) */
    pPureFileName = strrchr(pszFileName, '\\');
    if_oper(!pPureFileName, pPureFileName = strrchr(pszFileName, '/'));
    if (pPureFileName)
    {
        pPureFileName++;
    }
    else
    {
        pPureFileName = pszFileName;
    }

    /* 4. 写日志 */
    g_RegFun.stRelyApp.pfWriLog((int)eLevel, "WSEC_CBB", pPureFileName, nLineNo, szLog);

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_CallPeriodicFunc
 功能描述  : 立即调用周期性调度程序
 纯 入 参  : pFunc: 被执行函数
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_CallPeriodicFunc(WSEC_FP_PeriodicCall pFunc)
{
    WSEC_SIZE_T i;
    WSEC_PERIODIC_CALL_STRU* pCall;
    WSEC_SYSTIME_T stLocalNow = {0}, stUtcNow = {0};

    if (!WSEC_GetLocalDateTime(&stLocalNow)) {return;}
    if (!WSEC_GetUtcDateTime(&stUtcNow)) {return;}

    pCall = g_PeriodCall;
    for (i = 0; i < WSEC_NUM_OF(g_PeriodCall); i++, pCall++)
    {
        continue_if(pCall->pfPeriodicCall != pFunc);

        pCall->stExecInfo.bUnconditionalExec = WSEC_TRUE;
        pFunc(&stLocalNow, &stUtcNow, WSEC_NULL_PTR, &pCall->stExecInfo);
        WSEC_DateTimeCopy(&pCall->stPreCallTimeUtc, &stUtcNow);
        pCall->stExecInfo.bUnconditionalExec = WSEC_FALSE;

        break;
    }

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_InitializeLock
 功能描述  : 对锁进行初始化
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月20日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T WSEC_InitializeLock()
{
    WSEC_INT32 i = 0;
    WSEC_INT32 j = 0;

    for(i = 0;  i < WSEC_NUM_OF(g_hLock);  i++)
    {
        WSEC_DestroyLock(&(g_hLock[i])); /* 确保锁未被创建 */

        if (WSEC_CreateLock(&g_hLock[i]) != WSEC_SUCCESS) /* 失败, 则销毁已经创建的锁 */
        {
            for(j = 0;  j < i;  j++)
            {
                WSEC_DestroyLock(&(g_hLock[j]));
            }

            return WSEC_FAILURE;
        }
    }

    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : WSEC_FinalizeLock
 功能描述  : 锁销毁
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月20日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_FinalizeLock()
{
    WSEC_INT32 i = 0;

    for(i = 0;  i < WSEC_NUM_OF(g_hLock);  i++)
    {
        WSEC_DestroyLock(&(g_hLock[i]));
    }

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_ReadTlv
 功能描述  : 从文件流中按照TLV格式读取一个TLV单元
 纯 入 参  : stream:    文件流
             nBuffSize: 待写缓冲区长度
 纯 出 参  : pTlv:      读取到的TLV数据
 入参出参  : pBuff: [in]待写缓冲区
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月19日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_ReadTlv(WSEC_FILE stream, WSEC_VOID* pBuff, WSEC_SIZE_T nBuffSize, WSEC_TLV_STRU* pTlv, WSEC_ERR_T* pnErrCode)
{
    WSEC_SIZE_T nReadLen;
    WSEC_ERR_T nErrCode;
    WSEC_ERR_T* pErr;
    
    WSEC_ASSERT(stream && pBuff && pTlv && (nBuffSize > 0));

    pErr = pnErrCode ? pnErrCode : &nErrCode;
    *pErr = WSEC_SUCCESS;

    if (WSEC_FEOF(stream)) {return WSEC_FALSE;}

    nReadLen = WSEC_FREAD(pTlv, 1, sizeof(WSEC_TLV_STRU) - sizeof(pTlv->pVal), stream);
    if (0 == nReadLen) {return WSEC_FALSE;} /* 文件读结束了 */

    return_oper_if(nReadLen != sizeof(WSEC_TLV_STRU) - sizeof(pTlv->pVal), *pErr = WSEC_ERR_FILE_FORMAT, WSEC_FALSE);
    
    WSEC_CvtByteOrder4Tlv(pTlv, wbcNetwork2Host);

    if (nBuffSize < pTlv->ulLen)
    {
        WSEC_LOG_E2("Cannot write %u bytes to buffer(%u bytes).", pTlv->ulLen, nBuffSize);
        *pErr = WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH;
        return WSEC_FALSE;
    }

    return_oper_if(!WSEC_FREAD_MUST(pBuff, pTlv->ulLen, stream), *pErr = WSEC_ERR_READ_FILE_FAIL, WSEC_FALSE);

    pTlv->pVal = pBuff;
#ifdef WSEC_TRACE_TLV
    printf("ReadTlv: T=%u, L=%u\r\n", pTlv->ulTag, pTlv->ulLen);
#endif
    return WSEC_TRUE;
}

/*****************************************************************************
 函 数 名  : WSEC_WriteTlv
 功能描述  : 向文件流中写入一个TLV单元
 纯 入 参  : stream: 文件流
             ulTag, ulLen, pVal: 对应TLV
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月19日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T WSEC_WriteTlv(WSEC_FILE stream, WSEC_UINT32 ulTag, WSEC_SIZE_T ulLen, const WSEC_VOID* pVal)
{
    WSEC_TLV_STRU stTlvWri;

#ifdef WSEC_TRACE_TLV
    printf("WriteTlv: T=%u, L=%u\r\n", ulTag, ulLen);
#endif
    WSEC_ASSERT(stream && pVal);
    WSEC_TLV_ASSIGN(stTlvWri, ulTag, ulLen, (WSEC_VOID*)pVal);
    WSEC_CvtByteOrder4Tlv(&stTlvWri, wbcHost2Network);
    
    if (!WSEC_FWRITE_MUST(&stTlvWri, sizeof(stTlvWri) - sizeof(stTlvWri.pVal), stream)) {return WSEC_ERR_READ_FILE_FAIL;}
    if (!WSEC_FWRITE_MUST(pVal, ulLen, stream)) {return WSEC_ERR_READ_FILE_FAIL;}

    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : WSEC_CvtByteOrder4Tlv
 功能描述  : 将TLV结构类型数据作字节序转换.
 纯 入 参  : eOper: 字节序转换方法
 纯 出 参  : 无
 入参出参  : pTlv: [in]字节序待转换数据; [out]转换后的数据
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月19日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_CvtByteOrder4Tlv(WSEC_TLV_STRU* pTlv, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    if (wbcHost2Network == eOper)
    {
        pTlv->ulTag = WSEC_H2N_L(pTlv->ulTag);
        pTlv->ulLen = WSEC_H2N_L(pTlv->ulLen);
    }
    else if (wbcNetwork2Host == eOper)
    {
        pTlv->ulTag = WSEC_N2H_L(pTlv->ulTag);
        pTlv->ulLen = WSEC_N2H_L(pTlv->ulLen);
    }
    else
    {
        WSEC_ASSERT_FALSE;
    }

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_IsTimerout
 功能描述  : 判断计时是否超时
 纯 入 参  : ulWaitTick: 计时最长时间, 单位近似于毫秒. 0则以WSEC_ENABLE_BLOCK_MILSEC取代
 纯 出 参  : 无
 入参出参  : pTimer: [in]计时管理数据结构; [out]若出现计时翻转或超时, 则改写之.
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_IsTimerout(WSEC_SPEND_TIME_STRU* pTimer, WSEC_UINT32 ulWaitMilSec)
{
    WSEC_CLOCK_T tNow = 0;
    WSEC_UINT32 nWaitClocks, nDiff;
    WSEC_BOOL bTimerout;

    tNow = clock();
    if (0 == ulWaitMilSec) {ulWaitMilSec = WSEC_ENABLE_BLOCK_MILSEC;}
    nWaitClocks = ulWaitMilSec * CLOCKS_PER_SEC / 1000;

    nDiff = (WSEC_UINT32)(tNow - pTimer->tPre);

    bTimerout = (nDiff > nWaitClocks);
    if (bTimerout) {pTimer->tPre = clock();}

    return bTimerout;
}

/*****************************************************************************
 函 数 名  : WSEC_RptProgress
 功能描述  : 上报进度指示
 纯 入 参  : pstRptProgressFun: 进度上报回调函数相关信息;
             pstTimer:          记录上次上报的时间信息;
             ulScale:           事务规模;
             ulCurrent:         当前进展;
 纯 出 参  : pbCancel:          App是否取消后续进度.
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年1月4日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_RptProgress(const WSEC_PROGRESS_RPT_STRU* pstRptProgressFun, WSEC_SPEND_TIME_STRU* pstTimer, WSEC_UINT32 ulScale, WSEC_UINT32 ulCurrent, WSEC_BOOL* pbCancel)
{
    if (pstTimer)
    {
        if (!WSEC_IsTimerout(pstTimer, WSEC_ENABLE_BLOCK_MILSEC)) {return;}
    }
    WSEC_DO_EVENTS;

    if (!pstRptProgressFun) {return;}
    if (!pstRptProgressFun->pfRptProgress) {return;}
    pstRptProgressFun->pfRptProgress(pstRptProgressFun->ulTag, ulScale, ulCurrent, pbCancel);

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_LogStructSize
 功能描述  : 将结构体长度信息记录到日志
 纯 入 参  : pszStructName: 结构体名称
             nSize: 结构体长度
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年3月6日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_LogStructSize(const WSEC_CHAR* pszStructName, WSEC_SIZE_T nSize)
{
    WSEC_LOG_I2("sizeof(%s) = %d", pszStructName, nSize);

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_CvtByteOrder4DateTime
 功能描述  : 将日期&时间类型数据作字节序转换.
 纯 入 参  : eOper: 字节序转换方法
 纯 出 参  : 无
 入参出参  : pstDateTime: [in]字节序待转换数据; [out]转换后的数据
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_CvtByteOrder4DateTime(WSEC_SYSTIME_T* pstDateTime, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    WSEC_ASSERT(WSEC_IS2(eOper, wbcHost2Network, wbcNetwork2Host));

    pstDateTime->uwYear = WSEC_BYTE_ORDER_CVT_S(eOper, pstDateTime->uwYear);

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_ShowStructSize
 功能描述  : 显示结构长度, 用于不同环境下的调测
 输入参数  : pfShow: 调用者实现的回显函数
 输出参数  : 无
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2015年3月6日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
#ifdef WSEC_DEBUG
WSEC_VOID WSEC_ShowStructSize(WSEC_FP_ShowStructSize pfShow)
{
    pfShow("WSEC_UINT8", sizeof(WSEC_UINT8));
    pfShow("WSEC_BYTE", sizeof(WSEC_BYTE));
    pfShow("WSEC_UINT16", sizeof(WSEC_UINT16));
    pfShow("WSEC_UINT32", sizeof(WSEC_UINT32));
    pfShow("WSEC_INT32", sizeof(WSEC_INT32));
    pfShow("WSEC_BOOL", sizeof(WSEC_BOOL));
    pfShow("WSEC_CHAR", sizeof(WSEC_CHAR));
    pfShow("WSEC_HANDLE", sizeof(WSEC_HANDLE));
    pfShow("WSEC_FILE", sizeof(WSEC_FILE));
    pfShow("WSEC_POSITION", sizeof(WSEC_POSITION));
    pfShow("WSEC_SIZE_T", sizeof(WSEC_SIZE_T));
    pfShow("WSEC_CLOCK_T", sizeof(WSEC_CLOCK_T));
    pfShow("WSEC_ERR_T", sizeof(WSEC_ERR_T));
    pfShow("WSEC_VOID*", sizeof(WSEC_VOID*));

    pfShow("WSEC_SYSTIME_T", sizeof(WSEC_SYSTIME_T));
    pfShow("WSEC_SCHEDULE_TIME_STRU", sizeof(WSEC_SCHEDULE_TIME_STRU));
    pfShow("WSEC_TLV_STRU", sizeof(WSEC_TLV_STRU));

    return;
}
#endif

/*****************************************************************************
 函 数 名  : WSEC_CreateHashCode
 功能描述  : 对指定的数据块的生成Hash值
 输入参数  : ulHashAlg: Hash算法
             pBuff:     待Hash的数据块
             ulBuffNum: 被Hash的数据块个数
             pHashCode: 用于存储Hash值的缓冲区(含容量)
 输出参数  : pHashCode: 输出Hash值及其大小
 返 回 值  : 成功/失败

 修改历史      :
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_CreateHashCode(WSEC_UINT32 ulHashAlg, const WSEC_BUFF* pBuff, WSEC_UINT32 ulBuffNum, INOUT WSEC_BUFF* pHashCode)
{
    WSEC_UINT32 i;
    WSEC_CRYPT_CTX ctx = WSEC_NULL_PTR;
    const WSEC_BUFF* pReadBuff = WSEC_NULL_PTR;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    
    if (1 == ulBuffNum)
    {
        return (CAC_Digest(ulHashAlg, pBuff->pBuff, pBuff->nLen, pHashCode->pBuff, (WSEC_UINT32 *)&(pHashCode->nLen)) == WSEC_SUCCESS);
    }
    else if (ulBuffNum > 1)
    {
        nErrCode = CAC_DigestInit(&ctx, ulHashAlg);
        return_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("CAC_DigestInit() = %u", nErrCode), WSEC_FALSE);

        for (i = 0, pReadBuff = pBuff; i < ulBuffNum; i++, pReadBuff++)
        {
            if(WSEC_SUCCESS != CAC_DigestUpdate(ctx, pReadBuff->pBuff, pReadBuff->nLen))
            {
                CAC_DigestFree(&ctx);
                return WSEC_FALSE;
            }
        }

        return (CAC_DigestFinal(&ctx, pHashCode->pBuff, &(pHashCode->nLen)) == WSEC_SUCCESS);
    }
    else
    {
        return WSEC_FALSE;
    }
}

/*****************************************************************************
 函 数 名  : WSEC_ChkIntegrity
 功能描述  : 验证指定缓冲区的数据完整性是否和已知的Hash值匹配
 纯 入 参  : ulHashAlg: Hash算法
             pChkBuff:  待验证的数据块
             ulBuffNum: 待验证的数据块个数
             pCmpHashCode: 已知的Hash值
             ulHashCodeLen: 已知的Hash值长度
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : WSEC_SUCCESS=校验成功，其它为错误码
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T WSEC_ChkIntegrity(WSEC_UINT32 ulHashAlg,
                              const WSEC_BUFF* pChkBuff, WSEC_UINT32 ulBuffNum,
                              const WSEC_VOID* pCmpHashCode, WSEC_UINT32 ulHashCodeLen)
{
    WSEC_BUFF stHashNew = {0};
    WSEC_ERR_T ulReturnCode = WSEC_SUCCESS;
    
    WSEC_ASSERT(pChkBuff && (ulBuffNum > 0) && pCmpHashCode && (ulHashCodeLen > 0));

    WSEC_BUFF_ALLOC(stHashNew, ulHashCodeLen);

    return_oper_if(!stHashNew.pBuff, WSEC_LOG_E1("Allocate memory(len=%u) fail.", stHashNew.nLen), WSEC_ERR_MALLOC_FAIL);

    do
    {
        break_oper_if(!WSEC_CreateHashCode(ulHashAlg, pChkBuff, ulBuffNum, &stHashNew), WSEC_LOG_E("Generate hash fail."), ulReturnCode = WSEC_ERR_GEN_HASH_CODE_FAIL);

        ulReturnCode = WSEC_ERR_HASH_NOT_MATCH;
        if (stHashNew.nLen == ulHashCodeLen)
        {
            if (WSEC_MEMCMP(stHashNew.pBuff, pCmpHashCode, ulHashCodeLen) == 0){ulReturnCode = WSEC_SUCCESS;}
        }
    } do_end;

    WSEC_BUFF_FREE(stHashNew);
    return ulReturnCode;
}

/*****************************************************************************
 函 数 名  : WSEC_CreateHmacCode
 功能描述  : 计算HMAC
 纯 入 参  : eHmacAlg:         HMAC算法
             pBuff, ulBuffNum: 参与HMAC计算的数据段
             pKey:             HMAC密钥
 纯 出 参  : 无
 入参出参  : pHmacCode: [in]存放HMAC的缓冲区, [out]回传HMAC结果
 返 回 值  : WSEC_SUCCESS=成功，其它为错误码
 特别注意  : 无

 修改历史
  1.日    期   : 2015年3月3日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T WSEC_CreateHmacCode(WSEC_ALGID_E eHmacAlg, const WSEC_BUFF* pBuff, WSEC_UINT32 ulBuffNum, const WSEC_BUFF* pKey, INOUT WSEC_BUFF* pHmacCode)
{
    WSEC_CRYPT_CTX stCtx = {0};
    WSEC_UINT32 i;
    const WSEC_BUFF* pDataBuff = WSEC_NULL_PTR;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS, nTemp;

    return_err_if_para_invalid("WSEC_CreateHmacCode", pBuff && ulBuffNum && pKey && pHmacCode);

    /* 1. Initialize */
    nErrCode = CAC_HmacInit(&stCtx, (WSEC_UINT32)eHmacAlg, pKey->pBuff, pKey->nLen);
    return_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("The function return %u", nErrCode), nErrCode);

    /* 2. Update */
    for (i = 0, pDataBuff = pBuff; i < ulBuffNum; i++, pDataBuff++)
    {
        nErrCode = CAC_HmacUpdate(stCtx, pDataBuff->pBuff, pDataBuff->nLen);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("The function return %u", nErrCode), oper_null);
    }

    /* 3. Finalize */
    nTemp = CAC_HmacFinal(&stCtx, pHmacCode->pBuff, &pHmacCode->nLen);
    if (nTemp != WSEC_SUCCESS) {nErrCode = nTemp;}

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : WSEC_ChkHmacCode
 功能描述  : 验证HMAC
 纯 入 参  : eHmacAlg:         HMAC算法
             pBuff, ulBuffNum: 参与HMAC计算的数据段
             pKey:             HMAC密钥
             pHmacCode:        旧HMAC结果
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : WSEC_SUCCESS=成功，其它为错误码
 特别注意  : 无

 修改历史
  1.日    期   : 2015年3月3日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T WSEC_ChkHmacCode(WSEC_ALGID_E eHmacAlg, const WSEC_BUFF* pBuff, WSEC_UINT32 ulBuffNum, const WSEC_BUFF* pKey, const WSEC_BUFF* pHmacCode)
{
    WSEC_BUFF stHmacNew = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    return_err_if_para_invalid("WSEC_ChkHmacCode", pBuff && ulBuffNum && pKey && pHmacCode);
    return_err_if_para_invalid("WSEC_ChkHmacCode", pHmacCode->pBuff && (pHmacCode->nLen > 0));

    WSEC_BUFF_ALLOC(stHmacNew, pHmacCode->nLen);

    do
    {
        break_oper_if(!stHmacNew.pBuff, WSEC_LOG_E4MALLOC(stHmacNew.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);
        
        nErrCode = WSEC_CreateHmacCode(eHmacAlg, pBuff, ulBuffNum, pKey, &stHmacNew);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("Above function return %u", nErrCode), oper_null);

        break_oper_if(WSEC_MEMCMP(pHmacCode->pBuff, stHmacNew.pBuff, stHmacNew.nLen) != 0, WSEC_LOG_E("Authenticate HMAC fail."), nErrCode = WSEC_ERR_HMAC_AUTH_FAIL);
    }do_end;

    WSEC_BUFF_FREE(stHmacNew);
    return nErrCode;
}

/*****************************************************************************
 函 数 名  : WSEC_HashFile
 功能描述  : 计算文件的Hash值
 纯 入 参  : ulHashAlg:     Hash算法
             pFile:         被打开的文件句柄
             ulDataSize:    从当前位置开始, 参与Hash计算的文件数据长度.
                            如果为0, 则从当前位置开始到文件结束的所有数据均参与Hash.
 纯 出 参  : pHash: 存放Hash结果.
 入参出参  : 无
 返 回 值  : WSEC_SUCCESS=校验成功，其它为错误码
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月31日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T WSEC_HashFile(WSEC_UINT32 ulHashAlg, WSEC_FILE pFile, WSEC_UINT32 ulDataSize, WSEC_BUFF* pHash)
{
    const WSEC_SIZE_T nMaxReadLen = WSEC_FILE_IO_SIZE_MAX;
    WSEC_CRYPT_CTX ctx = WSEC_NULL_PTR;
    WSEC_BUFF stReadBuff = {0};
    WSEC_SIZE_T nReadSize = 0;
    WSEC_SIZE_T nRemainSize = 0;
    WSEC_BOOL bReadToEnd;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    WSEC_ASSERT(pFile && pHash && pHash->pBuff && (pHash->nLen > 0));
    WSEC_BUFF_ALLOC(stReadBuff, nMaxReadLen);
    return_oper_if(!stReadBuff.pBuff, WSEC_LOG_E4MALLOC(stReadBuff.nLen), WSEC_ERR_MALLOC_FAIL);

    do
    {
        break_oper_if(CAC_DigestInit(&ctx, ulHashAlg) != WSEC_SUCCESS,
                      WSEC_LOG_E("CAC_DigestInit() fail."), nErrCode = WSEC_ERR_GEN_HASH_CODE_FAIL);

        bReadToEnd = (0 == ulDataSize);
        nRemainSize = ulDataSize;
        while (!WSEC_FEOF(pFile))
        {
            /* 确定本次读取数据的长度 */
            stReadBuff.nLen = bReadToEnd ? nMaxReadLen : nRemainSize;
            if (stReadBuff.nLen > nMaxReadLen) {stReadBuff.nLen = nMaxReadLen;}

            nReadSize = WSEC_FREAD(stReadBuff.pBuff, 1, stReadBuff.nLen, pFile);
            continue_if(0 == nReadSize);

            if (!bReadToEnd) {nRemainSize -= nReadSize;}
            break_oper_if(CAC_DigestUpdate(ctx, stReadBuff.pBuff, nReadSize) != WSEC_SUCCESS,
                          WSEC_LOG_E("CAC_DigestUpdate() fail."), nErrCode = WSEC_ERR_GEN_HASH_CODE_FAIL);
            if ((!bReadToEnd) && (0 == nRemainSize)) {break;}
        }
        break_oper_if(nErrCode != WSEC_SUCCESS, CAC_DigestFree(&ctx), oper_null);

        /* 提取Hash值 */
        break_oper_if(CAC_DigestFinal(&ctx, pHash->pBuff, &pHash->nLen) != WSEC_SUCCESS,
                      WSEC_LOG_E("CAC_DigestFinal() fail."), nErrCode = WSEC_ERR_GEN_HASH_CODE_FAIL)
    }do_end;

    WSEC_BUFF_FREE(stReadBuff);
    return nErrCode;
}

/*****************************************************************************
 函 数 名  : WSEC_ChkFileIntegrity
 功能描述  : 检查文件的完整性.
 纯 入 参  : ulHashAlg:  Hash算法
             pFile:      被打开的文件句柄
             ulDataSize: 从当前位置开始, 参与Hash计算的文件数据长度.
                         如果为0, 则从当前位置开始到文件结束的所有数据均参与Hash.
             pHash:      该文件的原Hash值.
 纯 出 参  : 
 入参出参  : 无
 返 回 值  : WSEC_SUCCESS=校验成功，其它为错误码
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月31日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T WSEC_ChkFileIntegrity(WSEC_UINT32 ulHashAlg, WSEC_FILE pFile, WSEC_UINT32 ulDataSize, const WSEC_BUFF* pHash)
{
    WSEC_BUFF stHashNew = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    WSEC_ASSERT(pFile && pHash && pHash->pBuff && (pHash->nLen > 0));
    WSEC_BUFF_ALLOC(stHashNew, pHash->nLen);
    return_oper_if(!stHashNew.pBuff, WSEC_LOG_E4MALLOC(stHashNew.nLen), WSEC_ERR_MALLOC_FAIL);

    do
    {
        nErrCode = WSEC_HashFile(ulHashAlg, pFile, ulDataSize, &stHashNew);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("WSEC_HashFile()=%u", nErrCode), oper_null);

        break_oper_if(pHash->nLen != stHashNew.nLen, oper_null, nErrCode = WSEC_ERR_HASH_NOT_MATCH);
        nErrCode = (WSEC_MEMCMP(pHash->pBuff, stHashNew.pBuff, stHashNew.nLen) == 0) ? WSEC_SUCCESS : WSEC_ERR_HASH_NOT_MATCH;
    }do_end;

    WSEC_BUFF_FREE(stHashNew);
    return nErrCode;
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */



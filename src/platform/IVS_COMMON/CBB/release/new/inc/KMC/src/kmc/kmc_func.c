/* 如下pc lint告警可忽略 */
/*lint -e506 -e522 -e533 -e534 -e539 -e545 -e550 -e573 -e574 -e602 -e603 -e632 -e633 -e634 -e636 -e638 -e639 -e641 -e655 -e665 -e701 -e702 -e750 -e785 -e794 -e830 -e960 */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

                        KMC - Key Manangement Component(密钥管理组件)
% KMC_PRI_ 前缀的, 是KMC的私有函数, 其余的是公开函数
% 全局变量 g_pKeystore, g_pKmcCfg 必须做防冲突访问保护, 保护机制是: 公开函数加解锁, 私有函数不主动加解锁。

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include "wsec_config.h"
#include "wsec_type.h"
#include "kmc_itf.h"
#include "cac_pri.h"
#include "kmc_pri.h"
#include "wsec_pri.h"
#include "wsec_share.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#define return_err_if_kmc_not_work do{if (g_KmcSys.eState != WSEC_RUNNING){WSEC_LOG_E("KMC not running.");return WSEC_ERR_KMC_CBB_NOT_INIT;}} do_end
#define return_err_if_domain_privacy(ulDomainId) return_oper_if(KMC_IS_PRI_DOMAIN(ulDomainId), KMC_LOG_DOMAIN_PRIVACY(ulDomainId), WSEC_ERR_KMC_CANNOT_ACCESS_PRI_DOMAIN)

#define KMC_LOG_DOMAIN_PRIVACY(ulDomainId) WSEC_LOG_E3("DomainId(%u) is privacy(%d ~ %d)", (ulDomainId), KMC_PRI_DOMAIN_ID_MIN, KMC_PRI_DOMAIN_ID_MAX)

/* MK加/去掩 */
#define KMC_MASK_MK(pMk)   WSEC_Xor(pMk->stMkRear.abKey, pMk->stMkRear.ulPlainLen, g_KmcSys.abMkMaskCode, sizeof(g_KmcSys.abMkMaskCode), pMk->stMkRear.abKey, pMk->stMkRear.ulPlainLen);
#define KMC_UNMASK_MK(pMk) KMC_MASK_MK(pMk) /* 加掩后再加掩即为脱掩, 可读性考虑 */

/* 将MK去掩到... */
#define KMC_UNMASK_MK_TO(pMk, Dst, DstLen) WSEC_Xor(pMk->stMkRear.abKey, pMk->stMkRear.ulPlainLen, g_KmcSys.abMkMaskCode, sizeof(g_KmcSys.abMkMaskCode), Dst, DstLen);

#define KMC_IS_VALID_KEY_TYPE(usKeyType) (WSEC_IS3(usKeyType, KMC_KEY_TYPE_ENCRPT, KMC_KEY_TYPE_INTEGRITY, KMC_KEY_TYPE_ENCRPT_INTEGRITY))
#define KMC_IS_VALID_KEY_TYPE_LEN(ulLen) WSEC_IN_SCOPE(ulLen, 1, g_KmcSys.ulMkPlainLenMax)
#define KMC_IS_VALID_RK_FROM(ucKeyFrom) WSEC_IS2(ucKeyFrom, KMC_RK_GEN_BY_INNER, KMC_RK_GEN_BY_IMPORT)
#define KMC_IS_VALID_MK_FROM(ucKeyFrom) WSEC_IS2(ucKeyFrom, KMC_MK_GEN_BY_INNER, KMC_MK_GEN_BY_IMPORT)
#define KMC_IS_VALID_KEY_STATUS(ucStatus) WSEC_IS2(ucStatus, KMC_KEY_STATUS_INACTIVE, KMC_KEY_STATUS_ACTIVE)
#define KMC_IS_INNER_CREATE_RK (KMC_RK_GEN_BY_INNER == g_KmcSys.eRootKeyFrom)
#define KMC_IS_PRI_DOMAIN(ulDomainId) WSEC_IN_SCOPE(ulDomainId, KMC_PRI_DOMAIN_ID_MIN, KMC_PRI_DOMAIN_ID_MAX)

#define KMC_CFG_IS_ROOT_KEY_VALID(pstCfgRootKey) (((pstCfgRootKey)->ulRootKeyLifeDays > 0) && ((pstCfgRootKey)->ulRootMasterKeyIterations > 0))
#define KMC_CFG_IS_KEY_MAN_VALID(pstCfgKeyMan)  ((pstCfgKeyMan->ulWarningBeforeKeyExpiredDays > 0) && (pstCfgKeyMan->ulGraceDaysForUseExpiredKey) && \
                                                ((pstCfgKeyMan)->stAutoUpdateKeyTime.ucHour < 24) && ((pstCfgKeyMan)->stAutoUpdateKeyTime.ucMinute < 60))

#define KMC_ENCRYPT_MK_ALGID  WSEC_ALGID_AES256_CBC  /* MK加密算法ID */
#define KMC_HMAC_MK_ALGID     WSEC_ALGID_HMAC_SHA256 /* HMAC保护MK数据的算法ID */
#define KMC_IS_MAN_CFG_FILE ((g_KmcSys.apszKmcCfgFile[0] != WSEC_NULL_PTR) && (g_KmcSys.apszKmcCfgFile[1] != WSEC_NULL_PTR))

#define KMC_KSF_NUM WSEC_NUM_OF(g_KmcSys.apszKeystoreFile)

const WSEC_BYTE g_KsfFlag[32] = {0x5F, 0x64, 0x97, 0x8D, 0x19, 0x4F, 0x89, 0xCF, 0xA8, 0x3F, 0x8E, 0xE1, 0xDB, 0x01, 0x3C, 0x0C,
                                 0x88, 0x42, 0x4A, 0x1C, 0xB7, 0xFC, 0xAD, 0x70, 0x4E, 0x45, 0x13, 0xA5, 0x14, 0x46, 0x71, 0x6C};
const WSEC_BYTE g_MkfFlag[32] = {0xF5, 0x06, 0xEF, 0xD8, 0x56, 0x3F, 0xD3, 0x07, 0x3F, 0xDE, 0x29, 0xD7, 0x89, 0xFE, 0xD3, 0xEC,
                                 0x8D, 0x48, 0xA4, 0x17, 0xC7, 0xDD, 0x3D, 0xD0, 0xF3, 0xED, 0x92, 0x7D, 0x46, 0x3A, 0xAA, 0x0D};
const WSEC_BYTE g_KcfFlag[32] = {0xB7, 0x0D, 0xB7, 0xC5, 0xBD, 0xF7, 0xE8, 0x4B, 0x47, 0xBB, 0xE4, 0x75, 0xAE, 0x52, 0x85, 0x4B,
                                 0x79, 0xF0, 0x27, 0x6E, 0x5F, 0xEB, 0xE9, 0x6C, 0x3F, 0x3D, 0x21, 0xD9, 0x99, 0xD0, 0xC7, 0xB2};

KMC_KSF_MEM_STRU* g_pKeystore = WSEC_NULL_PTR;
/*lint -e64*/
KMC_CFG_STRU*     g_pKmcCfg   = WSEC_NULL_PTR;
/*line -restore*/
KMC_SYS_STRU      g_KmcSys    = {0};
KMC_INI_CTX_STRU* g_pKmcIniCtx = WSEC_NULL_PTR; /* KMC初始化使用的上下文, 初始化结束后无效 */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                          一、 内部私有函数
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*****************************************************************************
 函 数 名  : KMC_PRI_FreeKsfSnapshot
 功能描述  : 释放KMC_KSF_SNAPSHOT_STRU类型的动态内存
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : p
 返 回 值  : WSEC_NULL_PTR, 方便Caller使用
 特别注意  : 

 修改历史
  1.日    期   : 2014年10月23日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
KMC_KSF_MEM_STRU* KMC_PRI_FreeKsfSnapshot(KMC_KSF_MEM_STRU* pData)
{
    if (pData)
    {
        pData->arrMk = WSEC_ARR_Finalize(pData->arrMk);
        WSEC_FREE(pData);
    }

    return pData;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_FreeKmcCfg
 功能描述  : 释放KMC_KEY_CFG_STRU类型的动态内存
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : pKmcCfg
 返 回 值  : WSEC_NULL_PTR, 方便Caller使用
 特别注意  : 

 修改历史
  1.日    期   : 2014年10月23日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
KMC_CFG_STRU* KMC_PRI_FreeKmcCfg(KMC_CFG_STRU* pKmcCfg)
{
    if (pKmcCfg)
    {
        pKmcCfg->arrDomainCfg = WSEC_ARR_Finalize(pKmcCfg->arrDomainCfg);
        WSEC_FREE(pKmcCfg);
    }

    return pKmcCfg;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_FreeGlobalMem
 功能描述  : 释放全局资源
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : ppKmcCfg: 指向配置数据指针的指针
             ppKeystore: 指向Keystore指针的指针
 返 回 值  : 无
 特别注意  : 全局资源将被释放

 修改历史
  1.日    期   : 2014年10月23日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_FreeGlobalMem(KMC_CFG_STRU** ppKmcCfg, KMC_KSF_MEM_STRU** ppKeystore)
{
    WSEC_SIZE_T i;

    *ppKeystore = KMC_PRI_FreeKsfSnapshot(*ppKeystore);
    *ppKmcCfg = KMC_PRI_FreeKmcCfg(*ppKmcCfg);

    for (i = 0; i < KMC_KSF_NUM; i++)
    {
        WSEC_FREE(g_KmcSys.apszKeystoreFile[i]);
        WSEC_FREE(g_KmcSys.apszKmcCfgFile[i]);
    }

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_Finalize
 功能描述  : KMC去初始化
 纯 入 参  : eLock: 是否加锁
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : WSEC_SUCCESS: 成功
             其它:         具体错误原因.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月03日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_Finalize(KMC_LOCK_OPER_ENUM eLock)
{
    WSEC_LOG_E("KMC_PRI_Finalize()");

    if (KMC_NEED_LOCK == eLock) {KMC_PRI_Lock(KMC_LOCK_BOTH);}
    KMC_PRI_FreeGlobalMem(&g_pKmcCfg, &g_pKeystore);
    g_KmcSys.eState = WSEC_WAIT_INIT;
    if (KMC_NEED_LOCK == eLock) {KMC_PRI_Unlock(KMC_LOCK_BOTH);}

    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_UpdateRootKeyPri
 功能描述  : 更新根密钥
             原理: 重新生成根密钥, 但MK保持不变.
 纯 入 参  : pbKeyEntropy: 密钥熵码流;[可空];
             ulSize:       密钥熵码流长.
             eLock:        是否需要对临界资源加锁
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月07日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_UpdateRootKeyPri(const WSEC_BYTE* pbKeyEntropy, WSEC_SIZE_T ulSize, KMC_LOCK_OPER_ENUM eLock)
{
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    WSEC_BUFF stEntropy = {0};
    WSEC_BUFF* pEntropy = WSEC_NULL_PTR;
    KMC_KSF_RK_STRU* pRk = WSEC_NULL_PTR;

    return_err_if_kmc_not_work;
    return_oper_if(!WSEC_GetUtcDateTime(&g_pKeystore->stRkInfo.stRkExpiredTimeUtc), WSEC_LOG_E("Get current UTC fail."), WSEC_ERR_GET_CURRENT_TIME_FAIL);

    pRk = (KMC_KSF_RK_STRU*)WSEC_MALLOC(sizeof(KMC_KSF_RK_STRU));
    return_oper_if(!pRk, WSEC_LOG_E4MALLOC(sizeof(KMC_KSF_RK_STRU)), WSEC_ERR_MALLOC_FAIL);
    
    KMC_PRI_NtfRkExpire(&g_pKeystore->stRkInfo, 0); /* 更新根密钥物料前触发告警 */

    if (pbKeyEntropy)
    {
        pEntropy = &stEntropy;
        pEntropy->pBuff = (WSEC_BYTE*)pbKeyEntropy;
        pEntropy->nLen  = ulSize;
    }

    if (KMC_NEED_LOCK == eLock) {KMC_PRI_Lock(KMC_LOCK_KEYSTORE);}
    /* 更新根密钥 */
    do
    {
        nErrCode = KMC_PRI_CreateRootKey(pEntropy, pRk);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E("KMC_PRI_CreateRootKey() fail."), oper_null); /* 失败了也没有关系 */

        nErrCode = KMC_PRI_WriteKsfSafety(g_pKeystore, pRk);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E("KMC_PRI_WriteKsfSafety() fail."), oper_null);
    } do_end;
    if (KMC_NEED_LOCK == eLock) {KMC_PRI_Unlock(KMC_LOCK_KEYSTORE);}

    WSEC_FREE(pRk);

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_GetMaxMkId
 功能描述  : 获取指定Domain下当前最大MK ID
 纯 入 参  : eLock: 是否需要加锁
             ulDomainId:  功能域标识
 纯 出 参  : pulMaxKeyId: 最大Master Key ID
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月23日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_GetMaxMkId(KMC_LOCK_OPER_ENUM eLock, WSEC_UINT32 ulDomainId, WSEC_UINT32* pulMaxKeyId)
{
    WSEC_INT32 i;
    WSEC_UINT32 ulMaxId = 0;
    WSEC_BOOL bDomainFound = WSEC_FALSE;
    const KMC_MEM_MK_STRU* pItem;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    return_err_if_para_invalid("KMC_PRI_GetMaxMkId", pulMaxKeyId);

    if_oper(KMC_NEED_LOCK == eLock, KMC_PRI_Lock(KMC_LOCK_KEYSTORE));
    for (i = 0; i < WSEC_ARR_GetCount(g_pKeystore->arrMk); i++)
    {
        pItem = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(g_pKeystore->arrMk, i);
        break_oper_if(!pItem, WSEC_LOG_E("memory access fail."), nErrCode = WSEC_ERR_OPER_ARRAY_FAIL);

        if (pItem->stMkInfo.ulDomainId == ulDomainId)
        {
            bDomainFound = WSEC_TRUE;
            if (ulMaxId < pItem->stMkInfo.ulKeyId) {ulMaxId = pItem->stMkInfo.ulKeyId;}
        }
        else if (bDomainFound) /* MK按照Domain排列的, 扫描到Domain进入另外的Domain, 则不需要再循环了 */
        {
            break;
        }else{;}
    }
    if_oper(KMC_NEED_LOCK == eLock, KMC_PRI_Unlock(KMC_LOCK_KEYSTORE));

    if (bDomainFound)
    {
        *pulMaxKeyId = ulMaxId;
    }
    else if (WSEC_SUCCESS == nErrCode)
    {
        nErrCode = WSEC_ERR_KMC_DOMAIN_MISS;
    }else{;}

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_MakeDefaultCfg4RootKey
 功能描述  : 构造RootKey缺省配置
 纯 入 参  : 无
 纯 出 参  : pstRkCfg: 输出缺省配置
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_MakeDefaultCfg4RootKey(KMC_CFG_ROOT_KEY_STRU* pstRkCfg)
{
    WSEC_ASSERT(pstRkCfg);
    
    pstRkCfg->ulRootKeyLifeDays         = 3650;
    pstRkCfg->ulRootMasterKeyIterations = 5000;
    
    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_MakeDefaultCfg4KeyMan
 功能描述  : 构造密钥生命周期管理缺省配置
 纯 入 参  : 无
 纯 出 参  : pstRkCfg: 输出缺省配置
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_MakeDefaultCfg4KeyMan(KMC_CFG_KEY_MAN_STRU* pstKmCfg)
{
    WSEC_ASSERT(pstKmCfg);

    pstKmCfg->ulWarningBeforeKeyExpiredDays = 30;
    pstKmCfg->ulGraceDaysForUseExpiredKey   = 60;
    pstKmCfg->bKeyAutoUpdate                = WSEC_TRUE;

    pstKmCfg->stAutoUpdateKeyTime.ucHour   = 1;
    pstKmCfg->stAutoUpdateKeyTime.ucMinute = 10;
    pstKmCfg->stAutoUpdateKeyTime.ucWeek   = 7;
    
    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_MakeDefaultCfg4DataProtect
 功能描述  : 构造数据保护缺省配置
 纯 入 参  : eType: 数据保护类型
 纯 出 参  : pstCfg: 输出缺省配置
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_MakeDefaultCfg4DataProtect(KMC_SDP_ALG_TYPE_ENUM eType, KMC_CFG_DATA_PROTECT_STRU *pstCfg)
{
    WSEC_ASSERT(pstCfg);

    if (SDP_ALG_ENCRPT == eType)
    {
        pstCfg->ulAlgId         = WSEC_ALGID_AES128_CBC;
        pstCfg->usKeyType       = KMC_KEY_TYPE_ENCRPT_INTEGRITY;
        pstCfg->bAppendMac      = WSEC_TRUE;
        pstCfg->ulKeyIterations = 0;
    }
    else if (SDP_ALG_INTEGRITY == eType)
    {
        pstCfg->ulAlgId         = WSEC_ALGID_HMAC_SHA256;
        pstCfg->usKeyType       = KMC_KEY_TYPE_ENCRPT_INTEGRITY;
        pstCfg->bAppendMac      = WSEC_FALSE;
        pstCfg->ulKeyIterations = 0;
    }
    else if (SDP_ALG_PWD_PROTECT == eType)
    {
        pstCfg->ulAlgId         = WSEC_ALGID_PBKDF2_HMAC_SHA256;
        pstCfg->usKeyType       = 0;
        pstCfg->bAppendMac      = 0;
        pstCfg->ulKeyIterations = 2000;
    }
    else
    {
        WSEC_ASSERT_FALSE;
    }

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_MakeDefaultCfg4Domain
 功能描述  : 给出缺省的Domain以及其KeyType配置
 纯 入 参  : 无
 纯 出 参  : pKmcCfg: 输出缺省的Domain及其KeyType配置
 入参出参  : 无
 返 回 值  : 成功或失败
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月24日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL KMC_PRI_MakeDefaultCfg4Domain(KMC_CFG_STRU* pKmcCfg)
{
    KMC_CFG_DOMAIN_INFO_STRU astDomainCfg[] = {
            {0, KMC_MK_GEN_BY_INNER,  "Generate MK by KMC"},
            {1, KMC_MK_GEN_BY_IMPORT, "Register MK by APP"}};
    KMC_CFG_KEY_TYPE_STRU stKeyType = {KMC_KEY_TYPE_ENCRPT_INTEGRITY, 32, 180};
    WSEC_SIZE_T i;
    WSEC_ERR_T nRet = WSEC_SUCCESS;

    /* 1. 缺省Domain配置信息 */
    for (i = 0; i < WSEC_NUM_OF(astDomainCfg); i++)
    {
        nRet = KMC_PRI_AddDomain2Array(pKmcCfg, &astDomainCfg[i]);
        if (nRet != WSEC_SUCCESS)
        {
            WSEC_LOG_E1("KMC_PRI_AddDomain2Array() = %u", nRet);
            WSEC_ARR_RemoveAll(pKmcCfg->arrDomainCfg);
            return WSEC_FALSE;
        }
    }

    /* 2. Domain下缺省KeyType配置 */
    return_oper_if(KMC_PRI_AddDomainKeyType2Array(pKmcCfg, astDomainCfg[0].ulId, &stKeyType) != WSEC_SUCCESS, oper_null, WSEC_FALSE);
    return_oper_if(KMC_PRI_AddDomainKeyType2Array(pKmcCfg, astDomainCfg[1].ulId, &stKeyType) != WSEC_SUCCESS, oper_null, WSEC_FALSE);

    return WSEC_TRUE;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_ReadRootKeyCfg
 功能描述  : 读取RK相关配置信息
 纯 入 参  : 无
 纯 出 参  : pKmcCfg: 输出RK相关配置信息
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年1月4日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL KMC_PRI_ReadRootKeyCfg(KMC_CFG_ROOT_KEY_STRU* pstRkCfg)
{
    WSEC_TLV_STRU stTlv = {0};
    KMC_CFG_ROOT_KEY_STRU* pstReadRkCfg = WSEC_NULL_PTR;
    KMC_READ_KCF_CTX_STRU* pReadKcfCtx = WSEC_NULL_PTR;

    if (!g_pKmcIniCtx) {return WSEC_FALSE;}
    if (!g_pKmcIniCtx->bCbbReadCfg) {return WSEC_FALSE;}
    pReadKcfCtx = &g_pKmcIniCtx->stReadKcfCtx;
    if (!pstRkCfg) {return WSEC_FALSE;}

    return_oper_if(WSEC_FSEEK(pReadKcfCtx->pFile, 0, SEEK_SET), WSEC_LOG_E("Cannot move file-ptr to start-pos."), WSEC_ERR_READ_FILE_FAIL);

    while (WSEC_ReadTlv(pReadKcfCtx->pFile, pReadKcfCtx->stReadBuff.pBuff, pReadKcfCtx->stReadBuff.nLen, &stTlv, WSEC_NULL_PTR))
    {
        continue_if(stTlv.ulTag != KMC_CFT_RK_CFG);
        return_oper_if(stTlv.ulLen != sizeof(KMC_CFG_ROOT_KEY_STRU), WSEC_LOG_E1("The RK-Cfg in '%s' is incorrect.", pReadKcfCtx->pszFile), WSEC_FALSE);

        pstReadRkCfg = (KMC_CFG_ROOT_KEY_STRU*)stTlv.pVal;
        KMC_PRI_CvtByteOrder4RkCfg(pstReadRkCfg, wbcNetwork2Host);

        return WSEC_MEMCPY(pstRkCfg, sizeof(KMC_CFG_ROOT_KEY_STRU), pstReadRkCfg, sizeof(KMC_CFG_ROOT_KEY_STRU)) == EOK ? WSEC_TRUE : WSEC_FALSE;
    }
    
    return WSEC_FALSE;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_ReadKeyManCfg
 功能描述  : 读取Key管理相关配置信息
 纯 入 参  : 无
 纯 出 参  : pstKmCfg: 输出Key管理相关配置信息
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年1月4日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL KMC_PRI_ReadKeyManCfg(KMC_CFG_KEY_MAN_STRU* pstKmCfg)
{
    WSEC_TLV_STRU stTlv = {0};
    KMC_CFG_KEY_MAN_STRU* pstReadCfg = WSEC_NULL_PTR;
    KMC_READ_KCF_CTX_STRU* pReadKcfCtx = WSEC_NULL_PTR;

    if (!g_pKmcIniCtx) {return WSEC_FALSE;}
    if (!g_pKmcIniCtx->bCbbReadCfg) {return WSEC_FALSE;}
    pReadKcfCtx = &g_pKmcIniCtx->stReadKcfCtx;
    if (!pstKmCfg) {return WSEC_FALSE;}

    return_oper_if(WSEC_FSEEK(pReadKcfCtx->pFile, 0, SEEK_SET), WSEC_LOG_E("Cannot move file-ptr to start-pos."), WSEC_ERR_READ_FILE_FAIL);

    while (WSEC_ReadTlv(pReadKcfCtx->pFile, pReadKcfCtx->stReadBuff.pBuff, pReadKcfCtx->stReadBuff.nLen, &stTlv, WSEC_NULL_PTR))
    {
        continue_if(stTlv.ulTag != KMC_CFT_KEY_MAN);
        return_oper_if(stTlv.ulLen != sizeof(KMC_CFG_KEY_MAN_STRU), WSEC_LOG_E1("The RK-Cfg in '%s' is incorrect.", pReadKcfCtx->pszFile), WSEC_FALSE);

        pstReadCfg = (KMC_CFG_KEY_MAN_STRU*)stTlv.pVal;
        KMC_PRI_CvtByteOrder4KeyManCfg(pstReadCfg, wbcNetwork2Host);

        return WSEC_MEMCPY(pstKmCfg, sizeof(KMC_CFG_KEY_MAN_STRU), pstReadCfg, sizeof(KMC_CFG_KEY_MAN_STRU)) == EOK ? WSEC_TRUE : WSEC_FALSE;
    }
    
    return WSEC_FALSE;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_ReadCfgOfDomainCount
 功能描述  : 读取Domain配置个数
 纯 入 参  : 无
 纯 出 参  : pulDomainCount: 输出Domain配置个数
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年1月4日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL KMC_PRI_ReadCfgOfDomainCount(WSEC_UINT32* pulDomainCount)
{
    WSEC_TLV_STRU stTlv = {0};
    WSEC_UINT32 ulDomainCount = 0;
    KMC_READ_KCF_CTX_STRU* pReadKcfCtx = WSEC_NULL_PTR;
    
    if (!g_pKmcIniCtx) {return WSEC_FALSE;}
    if (!g_pKmcIniCtx->bCbbReadCfg) {return WSEC_FALSE;}
    pReadKcfCtx = &g_pKmcIniCtx->stReadKcfCtx;
    if (!pulDomainCount) {return WSEC_FALSE;}

    return_oper_if(WSEC_FSEEK(pReadKcfCtx->pFile, 0, SEEK_SET), WSEC_LOG_E("Cannot move file-ptr to start-pos."), WSEC_ERR_READ_FILE_FAIL);

    while (WSEC_ReadTlv(pReadKcfCtx->pFile, pReadKcfCtx->stReadBuff.pBuff, pReadKcfCtx->stReadBuff.nLen, &stTlv, WSEC_NULL_PTR))
    {
        continue_if(stTlv.ulTag != KMC_CFT_DOMAIN_CFG);
        ulDomainCount++;
    }

    *pulDomainCount = ulDomainCount;
    
    return WSEC_TRUE;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_ReadCfgOfDomainInfo
 功能描述  : 读取ulDomainCount项Domain配置
 纯 入 参  : ulDomainCount: Domain个数
 纯 出 参  : pstAllDomainInfo: 输出ulDomainCount项Domain配置
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年1月4日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL KMC_PRI_ReadCfgOfDomainInfo(KMC_CFG_DOMAIN_INFO_STRU* pstAllDomainInfo, WSEC_UINT32 ulDomainCount)
{
    KMC_READ_KCF_CTX_STRU* pReadKcfCtx = WSEC_NULL_PTR;
    WSEC_TLV_STRU stTlv = {0};
    KMC_CFG_DOMAIN_INFO_STRU *pRead, *pWri;
    
    if (!g_pKmcIniCtx) {return WSEC_FALSE;}
    if (!g_pKmcIniCtx->bCbbReadCfg) {return WSEC_FALSE;}
    pReadKcfCtx = &g_pKmcIniCtx->stReadKcfCtx;
    if (!pstAllDomainInfo) {return WSEC_FALSE;}
    if (ulDomainCount < 1) {return WSEC_FALSE;}

    return_oper_if(WSEC_FSEEK(pReadKcfCtx->pFile, 0, SEEK_SET), WSEC_LOG_E("Cannot move file-ptr to start-pos."), WSEC_ERR_READ_FILE_FAIL);

    pWri = pstAllDomainInfo;
    while (WSEC_ReadTlv(pReadKcfCtx->pFile, pReadKcfCtx->stReadBuff.pBuff, pReadKcfCtx->stReadBuff.nLen, &stTlv, WSEC_NULL_PTR))
    {
        continue_if(stTlv.ulTag != KMC_CFT_DOMAIN_CFG);
        return_oper_if(stTlv.ulLen != sizeof(KMC_CFG_DOMAIN_INFO_STRU), WSEC_LOG_E1("The Domain-Cfg in '%s' is incorrect.", pReadKcfCtx->pszFile), WSEC_FALSE);
        if (ulDomainCount < 1) {break;}

        pRead = (KMC_CFG_DOMAIN_INFO_STRU*)stTlv.pVal;
        KMC_PRI_CvtByteOrder4DomainInfo(pRead, wbcNetwork2Host);

        return_oper_if(WSEC_MEMCPY(pWri, sizeof(KMC_CFG_DOMAIN_INFO_STRU), pRead, sizeof(KMC_CFG_DOMAIN_INFO_STRU)) != EOK, 
                       WSEC_LOG_E("Memory copy fail."), WSEC_ERR_MEMCPY_FAIL);
        pWri++;
        ulDomainCount--;
    }

    return WSEC_TRUE;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_ReadCfgOfDomainKeyTypeCount
 功能描述  : 读取指定Domain下KeyType配置个数
 纯 入 参  : ulDomainId: 功能域标识
 纯 出 参  : pulKeyTypeCount: 输出指定Domain下KeyType配置个数
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年1月4日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL KMC_PRI_ReadCfgOfDomainKeyTypeCount(WSEC_UINT32 ulDomainId, WSEC_UINT32* pulKeyTypeCount)
{
    KMC_READ_KCF_CTX_STRU* pReadKcfCtx = WSEC_NULL_PTR;
    WSEC_TLV_STRU stTlv = {0};
    KMC_CFG_DOMAIN_INFO_STRU* pDomain = WSEC_NULL_PTR;
    WSEC_BOOL bDomainFound = WSEC_FALSE;
    WSEC_UINT32 ulKeyTypeNum = 0;
    
    if (!g_pKmcIniCtx) {return WSEC_FALSE;}
    if (!g_pKmcIniCtx->bCbbReadCfg) {return WSEC_FALSE;}
    pReadKcfCtx = &g_pKmcIniCtx->stReadKcfCtx;
    if (!pulKeyTypeCount) {return WSEC_FALSE;}

    return_oper_if(WSEC_FSEEK(pReadKcfCtx->pFile, 0, SEEK_SET), WSEC_LOG_E("Cannot move file-ptr to start-pos."), WSEC_ERR_READ_FILE_FAIL);

    /* 1. 找到Domain配置 */
    while (WSEC_ReadTlv(pReadKcfCtx->pFile, pReadKcfCtx->stReadBuff.pBuff, pReadKcfCtx->stReadBuff.nLen, &stTlv, WSEC_NULL_PTR))
    {
        continue_if(stTlv.ulTag != KMC_CFT_DOMAIN_CFG);
        return_oper_if(stTlv.ulLen != sizeof(KMC_CFG_DOMAIN_INFO_STRU), WSEC_LOG_E1("The Domain-Cfg in '%s' is incorrect.", pReadKcfCtx->pszFile), WSEC_FALSE);

        pDomain = (KMC_CFG_DOMAIN_INFO_STRU*)stTlv.pVal;
        KMC_PRI_CvtByteOrder4DomainInfo(pDomain, wbcNetwork2Host);
        continue_if(pDomain->ulId != ulDomainId);

        bDomainFound = WSEC_TRUE;
        break;
    }
    if (!bDomainFound) {return WSEC_FALSE;}

    /* 2. 读取该Domain配置下的KeyType配置项数 */
    while (WSEC_ReadTlv(pReadKcfCtx->pFile, pReadKcfCtx->stReadBuff.pBuff, pReadKcfCtx->stReadBuff.nLen, &stTlv, WSEC_NULL_PTR))
    {
        if (stTlv.ulTag != KMC_CFT_DOMAIN_KEY_TYPE) {break;}
        ulKeyTypeNum++;
    }

    *pulKeyTypeCount = ulKeyTypeNum;

    return WSEC_TRUE;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_ReadCfgOfDomainKeyType
 功能描述  : 读取指定Domain下所有KeyType配置
 纯 入 参  : ulDomainId: 功能域标识
             ulKeyTypeCount: 指定Domain下的KeyType配置个数
 纯 出 参  : pstDomainAllKeyType: 输出指定Domain下所有KeyType配置
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年1月4日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL KMC_PRI_ReadCfgOfDomainKeyType(WSEC_UINT32 ulDomainId, KMC_CFG_KEY_TYPE_STRU* pstDomainAllKeyType, WSEC_UINT32 ulKeyTypeCount)
{
    KMC_READ_KCF_CTX_STRU* pReadKcfCtx = WSEC_NULL_PTR;
    WSEC_TLV_STRU stTlv = {0};
    KMC_CFG_DOMAIN_INFO_STRU* pDomain = WSEC_NULL_PTR;
    WSEC_BOOL bDomainFound = WSEC_FALSE;
    KMC_CFG_KEY_TYPE_STRU *pRead, *pWri;
    
    if (!g_pKmcIniCtx) {return WSEC_FALSE;}
    if (!g_pKmcIniCtx->bCbbReadCfg) {return WSEC_FALSE;}
    pReadKcfCtx = &g_pKmcIniCtx->stReadKcfCtx;
    if (!pstDomainAllKeyType) {return WSEC_FALSE;}
    if (ulKeyTypeCount < 1) {return WSEC_FALSE;}

    return_oper_if(WSEC_FSEEK(pReadKcfCtx->pFile, 0, SEEK_SET), WSEC_LOG_E("Cannot move file-ptr to start-pos."), WSEC_ERR_READ_FILE_FAIL);

    /* 1. 找到Domain配置 */
    while (WSEC_ReadTlv(pReadKcfCtx->pFile, pReadKcfCtx->stReadBuff.pBuff, pReadKcfCtx->stReadBuff.nLen, &stTlv, WSEC_NULL_PTR))
    {
        continue_if(stTlv.ulTag != KMC_CFT_DOMAIN_CFG);
        return_oper_if(stTlv.ulLen != sizeof(KMC_CFG_DOMAIN_INFO_STRU), WSEC_LOG_E1("The RK-Cfg in '%s' is incorrect.", pReadKcfCtx->pszFile), WSEC_FALSE);
        
        pDomain = (KMC_CFG_DOMAIN_INFO_STRU*)stTlv.pVal;
        KMC_PRI_CvtByteOrder4DomainInfo(pDomain, wbcNetwork2Host);
        continue_if(pDomain->ulId != ulDomainId);

        bDomainFound = WSEC_TRUE;
        break;
    }
    if (!bDomainFound) {return WSEC_FALSE;}

    /* 2. 读取该Domain配置下的所有KeyType配置 */
    pWri = pstDomainAllKeyType;
    while (WSEC_ReadTlv(pReadKcfCtx->pFile, pReadKcfCtx->stReadBuff.pBuff, pReadKcfCtx->stReadBuff.nLen, &stTlv, WSEC_NULL_PTR))
    {
        if (stTlv.ulTag != KMC_CFT_DOMAIN_KEY_TYPE) {break;}
        return_oper_if(stTlv.ulLen != sizeof(KMC_CFG_KEY_TYPE_STRU), WSEC_LOG_E1("The KeyType-Cfg in '%s' is incorrect.", pReadKcfCtx->pszFile), WSEC_FALSE);
        if (ulKeyTypeCount < 1) {break;}

        pRead = (KMC_CFG_KEY_TYPE_STRU*)stTlv.pVal;
        KMC_PRI_CvtByteOrder4KeyType(pRead, wbcNetwork2Host);
        return_oper_if(WSEC_MEMCPY(pWri, sizeof(KMC_CFG_KEY_TYPE_STRU), pRead, sizeof(KMC_CFG_KEY_TYPE_STRU)) != EOK, WSEC_LOG_E4MEMCPY, WSEC_ERR_MEMCPY_FAIL);

        ulKeyTypeCount--;
        pWri++;
    }

    return WSEC_TRUE;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_ReadCfgOfDataProtection
 功能描述  : 读取指定索引的数据保护算法配置
 纯 入 参  : eType: 数据保护算法的索引
 纯 出 参  : pstPara: 输出指定索引的数据保护算法配置
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年1月4日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL KMC_PRI_ReadCfgOfDataProtection(KMC_SDP_ALG_TYPE_ENUM eType, KMC_CFG_DATA_PROTECT_STRU *pstPara)
{
    KMC_READ_KCF_CTX_STRU* pReadKcfCtx = WSEC_NULL_PTR;
    WSEC_TLV_STRU stTlv = {0};
    WSEC_INT32 nIndex = 0;
    KMC_CFG_DATA_PROTECT_STRU* pRead = WSEC_NULL_PTR;
    
    if (!g_pKmcIniCtx) {return WSEC_FALSE;}
    if (!g_pKmcIniCtx->bCbbReadCfg) {return WSEC_FALSE;}
    pReadKcfCtx = &g_pKmcIniCtx->stReadKcfCtx;
    if (!pstPara) {return WSEC_FALSE;}

    return_oper_if(WSEC_FSEEK(pReadKcfCtx->pFile, 0, SEEK_SET), WSEC_LOG_E("Cannot move file-ptr to start-pos."), WSEC_ERR_READ_FILE_FAIL);

    while (WSEC_ReadTlv(pReadKcfCtx->pFile, pReadKcfCtx->stReadBuff.pBuff, pReadKcfCtx->stReadBuff.nLen, &stTlv, WSEC_NULL_PTR))
    {
        continue_if(stTlv.ulTag != KMC_CFT_DP_CFG);
        return_oper_if(stTlv.ulLen != sizeof(KMC_CFG_DATA_PROTECT_STRU), WSEC_LOG_E1("The DataProtect-Cfg in '%s' is incorrect.", pReadKcfCtx->pszFile), WSEC_FALSE);

        if (nIndex == eType)
        {
            pRead = (KMC_CFG_DATA_PROTECT_STRU*)stTlv.pVal;
            KMC_PRI_CvtByteOrder4DataProtectCfg(pRead, wbcNetwork2Host);
            return WSEC_MEMCPY(pstPara, sizeof(KMC_CFG_DATA_PROTECT_STRU), pRead, sizeof(KMC_CFG_DATA_PROTECT_STRU)) == EOK ? WSEC_TRUE : WSEC_FALSE;
        }
        nIndex++;
    }

    return WSEC_FALSE;
}


/*****************************************************************************
 函 数 名  : KMC_PRI_OpenIniCtx
 功能描述  : 创建KMC初始化阶段使用的上下文.
 纯 入 参  : bManCfgFile: 是否由KMC管理配置文件
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_OpenIniCtx(WSEC_BOOL bManCfgFile)
{
    WSEC_SIZE_T i;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    g_pKmcIniCtx = (KMC_INI_CTX_STRU*)WSEC_MALLOC(sizeof(KMC_INI_CTX_STRU));
    return_oper_if(!g_pKmcIniCtx, WSEC_LOG_E4MALLOC(sizeof(KMC_INI_CTX_STRU)), WSEC_ERR_MALLOC_FAIL);
    g_pKmcIniCtx->bCbbReadCfg = bManCfgFile;
    if (!bManCfgFile) {return WSEC_SUCCESS;}

    do
    {
        WSEC_BUFF_ALLOC(g_pKmcIniCtx->stReadKcfCtx.stReadBuff, WSEC_FILE_IO_SIZE_MAX);
        break_oper_if(!g_pKmcIniCtx->stReadKcfCtx.stReadBuff.pBuff, WSEC_LOG_E4MALLOC(g_pKmcIniCtx->stReadKcfCtx.stReadBuff.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);

        for (i = 0; i < WSEC_NUM_OF(g_KmcSys.apszKmcCfgFile); i++)
        {
            g_pKmcIniCtx->stReadKcfCtx.pszFile = g_KmcSys.apszKmcCfgFile[i];
            nErrCode = KMC_PRI_ChkCfgFile(&g_pKmcIniCtx->stReadKcfCtx);
            if (WSEC_SUCCESS == nErrCode)
            {
                WSEC_LOG_I1("'%s' chose as master-cfg-file.", g_pKmcIniCtx->stReadKcfCtx.pszFile);
                break; /* 已找到正确的配置文件 */
            }
            else
            {
                continue; /* 继续确认其它配置文件是否可用 */
            }
        }
    }do_end;

    if (nErrCode != WSEC_SUCCESS) {KMC_PRI_CloseIniCtx();}

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CloseIniCtx
 功能描述  : 关闭KMC初始化过程的上下文, 释放该过程中的资源.
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_CloseIniCtx()
{
    KMC_READ_KCF_CTX_STRU* pReadKcfCtx = WSEC_NULL_PTR;
    
    if (!g_pKmcIniCtx) {return;}

    pReadKcfCtx = &g_pKmcIniCtx->stReadKcfCtx;

    if (pReadKcfCtx->pFile) {WSEC_FCLOSE(pReadKcfCtx->pFile);}
    WSEC_BUFF_FREE(pReadKcfCtx->stReadBuff);
    WSEC_FREE(g_pKmcIniCtx);

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_ChkProtectCfg
 功能描述  : 检查数据保护算法配置合法性
 纯 入 参  : eType: 数据保护类型
             pstPara: 配置参数
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2015年03月19日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_ChkProtectCfg(KMC_SDP_ALG_TYPE_ENUM eType, const KMC_CFG_DATA_PROTECT_STRU *pstPara)
{
    WSEC_BOOL bAlgIdOk = WSEC_FALSE, bKeyTypeOk = WSEC_FALSE, bIterationOk = WSEC_TRUE;

    return_err_if_para_invalid("KMC_PRI_ChkProtectCfg", pstPara && WSEC_IS3(eType, SDP_ALG_ENCRPT, SDP_ALG_INTEGRITY, SDP_ALG_PWD_PROTECT));

    /* 检查算法ID, KeyType */
    if (SDP_ALG_ENCRPT == eType)
    {
        bAlgIdOk = WSEC_IS_ENCRYPT_ALGID(pstPara->ulAlgId);
        bKeyTypeOk = (pstPara->usKeyType & KMC_KEY_TYPE_ENCRPT);
    }
    else if (SDP_ALG_INTEGRITY == eType)
    {
        bAlgIdOk = WSEC_IS_HMAC_ALGID(pstPara->ulAlgId);
        bKeyTypeOk = (pstPara->usKeyType & KMC_KEY_TYPE_INTEGRITY);
    }
    else if (SDP_ALG_PWD_PROTECT == eType)
    {
        bAlgIdOk = WSEC_IS_PBKDF_ALGID(pstPara->ulAlgId);
        bKeyTypeOk = WSEC_TRUE;
        bIterationOk = KMC_IS_KEYITERATIONS_VALID(pstPara->ulKeyIterations);
    }
    else
    {
        return WSEC_ERR_INVALID_ARG;
    }
    return_oper_if(!bAlgIdOk, WSEC_LOG_E1("Invalid alg-id(%d)", pstPara->ulAlgId), WSEC_ERR_INVALID_ARG);
    return_oper_if(!bKeyTypeOk, WSEC_LOG_E1("Invalid KeyType(%d)", pstPara->usKeyType), WSEC_ERR_INVALID_ARG);
    return_oper_if(!bIterationOk, WSEC_LOG_E1("Invalid KeyIterationS(%d)", pstPara->ulKeyIterations), WSEC_ERR_INVALID_ARG);

    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_WriteKcfSafety
 功能描述  : 可靠地将数据写入KMC配置文件
 纯 入 参  : pKmcCfg: 用于写入配置文件的数据
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月31日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_WriteKcfSafety(const KMC_CFG_STRU* pKmcCfg)
{
    WSEC_ASSERT(KMC_IS_MAN_CFG_FILE);
    return WSEC_WriteFileS(pKmcCfg, g_KmcSys.apszKmcCfgFile, WSEC_NUM_OF(g_KmcSys.apszKmcCfgFile), (WSEC_WriteFile)KMC_PRI_WriteKcf, WSEC_NULL_PTR);
}

/*****************************************************************************
 函 数 名  : KMC_PRI_WriteKcf
 功能描述  : 保存KMC配置数据
 纯 入 参  : pKmcCfg: KMC配置数据
             pszKmcCfgFile: KMC配置文件名
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月24日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_WriteKcf(const KMC_CFG_STRU* pKmcCfg, const WSEC_CHAR* pszKmcCfgFile, const WSEC_VOID* pvReserved)
{
    WSEC_FILE pFile = WSEC_NULL_PTR;
    KMC_KCF_HDR_STRU* pstHdr = WSEC_NULL_PTR;
    const KMC_CFG_DATA_PROTECT_STRU* pstDataProtectCfg = WSEC_NULL_PTR;
    const KMC_DOMAIN_CFG_STRU* pstDomainCfg = WSEC_NULL_PTR;
    const KMC_CFG_KEY_TYPE_STRU* pstKeyType = WSEC_NULL_PTR;
    WSEC_ERR_T ulErrCode = WSEC_SUCCESS;
    WSEC_BUFF stBuff = {0};
    WSEC_BUFF stHash = {0};
    WSEC_INT32 i, j, nPos;

    WSEC_ASSERT(pKmcCfg && pszKmcCfgFile);
    WSEC_UNREFER(pvReserved);

    /*lint -e668*/
    pFile = WSEC_FOPEN(pszKmcCfgFile, "w+b");
    return_oper_if(!pFile, WSEC_LOG_E1("Cannot write '%s'", pszKmcCfgFile), WSEC_ERR_WRI_FILE_FAIL);

    WSEC_BUFF_ALLOC(stBuff, WSEC_FILE_IO_SIZE_MAX); /* 按照最大长度分配TLV缓冲区 */
    pstHdr = (KMC_KCF_HDR_STRU*)WSEC_MALLOC(sizeof(KMC_KCF_HDR_STRU));
    
    do
    {
        break_oper_if(!stBuff.pBuff, WSEC_LOG_E4MALLOC(stBuff.nLen), ulErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!pstHdr, WSEC_LOG_E4MALLOC(sizeof(KMC_KCF_HDR_STRU)), ulErrCode = WSEC_ERR_MALLOC_FAIL);

        WSEC_ASSERT(sizeof(g_KcfFlag) == sizeof(pstHdr->abFormatFlag));

        /* 1. 临时写文件头占位(后续还要回写) */
        ulErrCode = WSEC_WriteTlv(pFile, KMC_CFT_HDR, sizeof(KMC_KCF_HDR_STRU), pstHdr);
        break_oper_if(ulErrCode != WSEC_SUCCESS, WSEC_LOG_E1("Cannot write '%s'", pszKmcCfgFile), oper_null);
        nPos = WSEC_FTELL(pFile); /* 记住此刻的文件位置, 后续从此开始计算文件的Hash */
        break_oper_if(nPos < 0, WSEC_LOG_E("ftell() fail."), ulErrCode = WSEC_ERR_WRI_FILE_FAIL);

        /* 2. 写RK配置信息 */
        WSEC_ASSERT(stBuff.nLen >= sizeof(pKmcCfg->stRkCfg));
        break_oper_if(WSEC_MEMCPY(stBuff.pBuff, stBuff.nLen, &pKmcCfg->stRkCfg, sizeof(pKmcCfg->stRkCfg)) != EOK,
                      WSEC_LOG_E4MEMCPY, ulErrCode = WSEC_ERR_MEMCPY_FAIL);
        KMC_PRI_CvtByteOrder4RkCfg((KMC_CFG_ROOT_KEY_STRU*)stBuff.pBuff, wbcHost2Network);
        ulErrCode = WSEC_WriteTlv(pFile, KMC_CFT_RK_CFG, sizeof(KMC_CFG_ROOT_KEY_STRU), stBuff.pBuff);
        break_oper_if(ulErrCode != WSEC_SUCCESS, WSEC_LOG_E1("Cannot write '%s'", pszKmcCfgFile), oper_null);

        /* 3. 写Key管理配置 */
        WSEC_ASSERT(stBuff.nLen >= sizeof(pKmcCfg->stKmCfg));
        break_oper_if(WSEC_MEMCPY(stBuff.pBuff, stBuff.nLen, &pKmcCfg->stKmCfg, sizeof(pKmcCfg->stKmCfg)) != EOK,
                      WSEC_LOG_E4MEMCPY, ulErrCode = WSEC_ERR_MEMCPY_FAIL);
        KMC_PRI_CvtByteOrder4KeyManCfg((KMC_CFG_KEY_MAN_STRU*)stBuff.pBuff, wbcHost2Network);
        ulErrCode = WSEC_WriteTlv(pFile, KMC_CFT_KEY_MAN, sizeof(KMC_CFG_KEY_MAN_STRU), stBuff.pBuff);
        break_oper_if(ulErrCode != WSEC_SUCCESS, WSEC_LOG_E1("Cannot write '%s'", pszKmcCfgFile), oper_null);

        /* 4. 写数据保护的配置数据 */
        WSEC_ASSERT(stBuff.nLen >= sizeof(pKmcCfg->astDataProtectCfg[0]));
        for (i = 0, pstDataProtectCfg = pKmcCfg->astDataProtectCfg; i < (WSEC_INT32)WSEC_NUM_OF(pKmcCfg->astDataProtectCfg); i++, pstDataProtectCfg++)
        {
            break_oper_if(WSEC_MEMCPY(stBuff.pBuff, stBuff.nLen, pstDataProtectCfg, sizeof(KMC_CFG_DATA_PROTECT_STRU)) != EOK,
                          WSEC_LOG_E4MEMCPY, ulErrCode = WSEC_ERR_MEMCPY_FAIL);
                          
            KMC_PRI_CvtByteOrder4DataProtectCfg((KMC_CFG_DATA_PROTECT_STRU*)stBuff.pBuff, wbcHost2Network);
            ulErrCode = WSEC_WriteTlv(pFile, KMC_CFT_DP_CFG, sizeof(KMC_CFG_DATA_PROTECT_STRU), stBuff.pBuff);
            break_oper_if(ulErrCode != WSEC_SUCCESS, WSEC_LOG_E1("Cannot write '%s'", pszKmcCfgFile), oper_null);
            WSEC_FFLUSH(pFile);
        }
        if (ulErrCode != WSEC_SUCCESS) {break;}

        /* 5. 写Domain及其KeyType配置 */
        WSEC_ASSERT(stBuff.nLen >= sizeof(KMC_CFG_DOMAIN_INFO_STRU));
        WSEC_ASSERT(stBuff.nLen >= sizeof(KMC_CFG_KEY_TYPE_STRU));
        for (i = 0; i < WSEC_ARR_GetCount(pKmcCfg->arrDomainCfg); i++)
        {
            pstDomainCfg = (KMC_DOMAIN_CFG_STRU*)WSEC_ARR_GetAt(pKmcCfg->arrDomainCfg, i);
            break_oper_if(!pstDomainCfg, WSEC_LOG_E("memory access fail."), ulErrCode = WSEC_ERR_OPER_ARRAY_FAIL);
            break_oper_if(WSEC_MEMCPY(stBuff.pBuff, stBuff.nLen, &pstDomainCfg->stDomainInfo, sizeof(pstDomainCfg->stDomainInfo)) != EOK,
                          WSEC_LOG_E4MEMCPY, ulErrCode = WSEC_ERR_MEMCPY_FAIL);
            KMC_PRI_CvtByteOrder4DomainInfo((KMC_CFG_DOMAIN_INFO_STRU*)stBuff.pBuff, wbcHost2Network);
            ulErrCode = WSEC_WriteTlv(pFile, KMC_CFT_DOMAIN_CFG, sizeof(KMC_CFG_DOMAIN_INFO_STRU), stBuff.pBuff);
            break_oper_if(ulErrCode != WSEC_SUCCESS, WSEC_LOG_E1("Cannot write '%s'", pszKmcCfgFile), oper_null);
            
            for (j = 0; j < WSEC_ARR_GetCount(pstDomainCfg->arrKeyTypeCfg); j++)
            {
                pstKeyType = (KMC_CFG_KEY_TYPE_STRU*)WSEC_ARR_GetAt(pstDomainCfg->arrKeyTypeCfg, j);
                break_oper_if(!pstKeyType, WSEC_LOG_E("memory access fail."), ulErrCode = WSEC_ERR_OPER_ARRAY_FAIL);

                break_oper_if(WSEC_MEMCPY(stBuff.pBuff, stBuff.nLen, pstKeyType, sizeof(KMC_CFG_KEY_TYPE_STRU)) != EOK,
                              WSEC_LOG_E4MEMCPY, ulErrCode = WSEC_ERR_MEMCPY_FAIL);
                KMC_PRI_CvtByteOrder4KeyType((KMC_CFG_KEY_TYPE_STRU*)stBuff.pBuff, wbcHost2Network);
                ulErrCode = WSEC_WriteTlv(pFile, KMC_CFT_DOMAIN_KEY_TYPE, sizeof(KMC_CFG_KEY_TYPE_STRU), stBuff.pBuff);
                break_oper_if(ulErrCode != WSEC_SUCCESS, WSEC_LOG_E1("Cannot write '%s'", pszKmcCfgFile), oper_null);

                WSEC_FFLUSH(pFile);
            }
            if (ulErrCode != WSEC_SUCCESS) {break;}
        }
        if (ulErrCode != WSEC_SUCCESS) {break;}
        WSEC_FFLUSH(pFile);

        /* 6. 计算文件的Hash值, 以防篡改 */
        pstHdr->ulHashAlgId = WSEC_ALGID_SHA256;
        break_oper_if(WSEC_FSEEK(pFile, nPos, SEEK_SET) != 0, WSEC_LOG_E("fseek fail."), ulErrCode = WSEC_ERR_WRI_FILE_FAIL);
        WSEC_BUFF_ASSIGN(stHash, pstHdr->abHash, sizeof(pstHdr->abHash));
        ulErrCode = WSEC_HashFile(pstHdr->ulHashAlgId, pFile, 0, &stHash);
        break_oper_if(ulErrCode != WSEC_SUCCESS, WSEC_LOG_E1("WSEC_HashFile()=%u", ulErrCode), oper_null);

        /* 7. 回写文件头 */
        break_oper_if(WSEC_MEMCPY(pstHdr->abFormatFlag, sizeof(pstHdr->abFormatFlag), g_KcfFlag, sizeof(g_KcfFlag)) != EOK,
                      WSEC_LOG_E4MEMCPY, ulErrCode = WSEC_ERR_MEMCPY_FAIL);
        pstHdr->ulVer = KMC_KCF_VER;
        KMC_PRI_CvtByteOrder4KcfHdr(pstHdr, wbcHost2Network);

        break_oper_if(WSEC_FSEEK(pFile, 0, SEEK_SET) != 0, WSEC_LOG_E("fseek fail."), ulErrCode = WSEC_ERR_WRI_FILE_FAIL);
        ulErrCode = WSEC_WriteTlv(pFile, KMC_CFT_HDR, sizeof(KMC_KCF_HDR_STRU), pstHdr);
        break_oper_if(ulErrCode != WSEC_SUCCESS, WSEC_LOG_E1("WSEC_WriteTlv()=%u", ulErrCode), oper_null);

        WSEC_FFLUSH(pFile);
    }do_end;
    
    WSEC_FCLOSE(pFile);
    WSEC_FREE(pstHdr);
    WSEC_BUFF_FREE(stBuff);

    /* Misinformation: FORTIFY.Race_Condition--File_System_Access */
    if (ulErrCode != WSEC_SUCCESS) {WSEC_UNCARE(WSEC_FREMOVE(pszKmcCfgFile));}
    return ulErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_ChkCfgFile
 功能描述  : 检查KMC配置文件合法性
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : pstCtx: 读取KMC配置文件的上下文信息.
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月31日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_ChkCfgFile(KMC_READ_KCF_CTX_STRU* pstCtx)
{
    WSEC_CHAR* pszCfgFile = pstCtx->pszFile;
    WSEC_FILE pFile = WSEC_NULL_PTR;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    WSEC_BUFF stRead = {0};
    WSEC_BUFF stHash = {0};
    WSEC_TLV_STRU stTlv = {0};
    KMC_KCF_HDR_STRU* pstHdr = WSEC_NULL_PTR;

    pFile = WSEC_FOPEN(pszCfgFile, "rb");
    return_oper_if(!pFile, WSEC_LOG_E1("Cannot open '%s'", pszCfgFile), WSEC_ERR_OPEN_FILE_FAIL);

    WSEC_BUFF_ASSIGN(stRead, pstCtx->stReadBuff.pBuff, pstCtx->stReadBuff.nLen);

    do
    {
        /* 读文件头 */
        if (!WSEC_ReadTlv(pFile, stRead.pBuff, stRead.nLen, &stTlv, &nErrCode)) {break;}
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E2("Read TLV from '%s' = %u", pszCfgFile, nErrCode), nErrCode = WSEC_ERR_FILE_FORMAT);
        break_oper_if(stTlv.ulTag != KMC_CFT_HDR, WSEC_LOG_E1("'%s' is not the KMC-CFG file.", pszCfgFile), nErrCode = WSEC_ERR_FILE_FORMAT);
        break_oper_if(stTlv.ulLen != sizeof(KMC_KCF_HDR_STRU), WSEC_LOG_E1("'%s' is not the KMC-CFG file.", pszCfgFile), nErrCode = WSEC_ERR_FILE_FORMAT);
        
        pstHdr = (KMC_KCF_HDR_STRU*)stTlv.pVal;
        KMC_PRI_CvtByteOrder4KcfHdr(pstHdr, wbcNetwork2Host);

        /* 检查格式标记 */
        break_oper_if(WSEC_MEMCMP(pstHdr->abFormatFlag, g_KcfFlag, sizeof(g_KcfFlag)) != 0, WSEC_LOG_E1("'%s' is not the KMC-CFG file.", pszCfgFile), nErrCode = WSEC_ERR_FILE_FORMAT);

        /* 检查版本号 */
        if (pstHdr->ulVer != KMC_KCF_VER)
        {
            WSEC_LOG_E1("The KMC-CFG version of '%s' is not correct.", pszCfgFile); /* 后续开发者解决版本兼容性问题. 本段代码重写 */
            nErrCode = WSEC_FAILURE;
            break;
        }

        /* 检查完整性 */
        WSEC_BUFF_ASSIGN(stHash, pstHdr->abHash, sizeof(pstHdr->abHash));
        nErrCode = WSEC_ChkFileIntegrity(pstHdr->ulHashAlgId, pFile, 0, &stHash);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E2("WSEC_ChkFileIntegrity('%s') = %u", pszCfgFile, nErrCode), oper_null);
    }do_end;

    if (WSEC_SUCCESS == nErrCode)
    {
        pstCtx->pFile = pFile;
    }
    else
    {
        WSEC_FCLOSE(pFile);
    }
    
    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_ReadKsfSafety
 功能描述  : 安全地读取Keystore数据.
             系统支持双Keystore文件, 如果读取数据过程出现错误, 则读取另外文件.
             1. 优选能正确读出文件的数据;
             2. 如果双文件均读取异常, 则取读取到MK条数最多的数据.
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : ppKeystore: 指向Keystore数据结构指针的指针.
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 本函数将成功读取到的Keystore数据写入全局变量g_pReystore

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_ReadKsfSafety(KMC_KSF_MEM_STRU** ppKeystore)
{
    KMC_KSF_MEM_STRU* pRead[KMC_KSF_NUM]  = {WSEC_NULL_PTR};
    WSEC_INT32        aMkNum[KMC_KSF_NUM] = {-1};
    WSEC_SIZE_T i, nAimAt = 0;
    WSEC_BOOL bReadKsfOk = WSEC_FALSE, bReWriKeystore = WSEC_FALSE;
    KMC_KSF_RK_STRU *pRk = WSEC_NULL_PTR;
    WSEC_ERR_T nRet = WSEC_SUCCESS;

    WSEC_ASSERT(ppKeystore);

    /* 1. 申请资源 */
    for (i = 0; i < KMC_KSF_NUM; i++)
    {
        pRead[i] = (KMC_KSF_MEM_STRU*)WSEC_MALLOC(sizeof(KMC_KSF_MEM_STRU));
        break_oper_if(!pRead[i], WSEC_LOG_E4MALLOC(sizeof(KMC_KSF_MEM_STRU)), nRet = WSEC_ERR_MALLOC_FAIL);
    }
    if (nRet != WSEC_SUCCESS) /* 资源没全申请成功, 则释放 */
    {
        for (i = 0; i < KMC_KSF_NUM; i++)
        {
            WSEC_FREE(pRead[i]);
        }

        return nRet;
    }
    
    /* 2. 读取A, B文件 */
    for (i = 0; i < KMC_KSF_NUM; i++)
    {
        pRead[i]->pszFromFile = g_KmcSys.apszKeystoreFile[i];
        nRet = KMC_PRI_ReadKsf(g_KmcSys.apszKeystoreFile[i], pRead[i]);
        if (WSEC_SUCCESS == nRet)
        {
            aMkNum[i] = WSEC_MK_NUM_MAX; /* 读取成功, 无条件以此文件数据为准 */
            bReadKsfOk = WSEC_TRUE;
            break;
        }

        if (WSEC_ERR_CANCEL_BY_APP == nRet) {goto ExitPort;}

        if (WSEC_ERR_KMC_READ_MK_FAIL == nRet)
        {
            aMkNum[i] = WSEC_ARR_GetCount(pRead[i]->arrMk); /* 读取MK失败, 已读取部分可用. */
        }
    }

    /* 3. 确定使用哪份数据: MK条数多者胜出 */
    nAimAt = 0;
    for (i = 1; i < KMC_KSF_NUM; i++)
    {
        if (aMkNum[nAimAt] < aMkNum[i])
        {
            nAimAt = i;
        }
    }

    /* 4. 处理读取的数据 */
    do
    {
        if (aMkNum[nAimAt] > 0) /* 目标数据选定, 以此为准. */
        {
            nRet = WSEC_SUCCESS;
            WSEC_LOG_E1("Used %s", g_KmcSys.apszKeystoreFile[nAimAt]);
            bReWriKeystore = !bReadKsfOk; /* 说明Keystore文件受损, 需要重写Keystore */
        }
        else /* 需要创建Keystore */
        {
            pRk = (KMC_KSF_RK_STRU*)WSEC_MALLOC(sizeof(KMC_KSF_RK_STRU));
            break_oper_if(!pRk, WSEC_LOG_E4MALLOC(sizeof(KMC_KSF_RK_STRU)), nRet = WSEC_ERR_MALLOC_FAIL);
            
            nAimAt = 0; /* 利用此资源创建Keystore */
            WSEC_ASSERT(WSEC_ARR_IsEmpty(pRead[nAimAt]->arrMk)); /* 验证上述处理无BUG */
            nRet = KMC_PRI_CreateKsf(pRead[nAimAt], pRk);
            WSEC_LOG_E1("KMC_PRI_CreateKsf() = %u", nRet);
        }

        if (WSEC_SUCCESS == nRet)
        {
            KMC_PRI_FreeKsfSnapshot(*ppKeystore);
            *ppKeystore = pRead[nAimAt];
        }
        else
        {
            nAimAt = KMC_KSF_NUM + 1; /* 下面释放资源时将不遗余力 */
        }
        
        if (!bReadKsfOk) {WSEC_NOTIFY(WSEC_KMC_NTF_KEY_STORE_CORRUPT, WSEC_NULL_PTR, 0);}
        if (bReWriKeystore) {KMC_PRI_WriteKsfSafety(*ppKeystore, pRk);}
    }do_end;
    
ExitPort:

    /* 5. 资源释放 */
    for (i = 0; i < KMC_KSF_NUM; i++)
    {
        if (i != nAimAt)
        {
            pRead[i] = KMC_PRI_FreeKsfSnapshot(pRead[i]);
        }
    }
   
    if (pRk) { WSEC_FREE(pRk); }

    return nRet;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_ReadKsf
 功能描述  : 读取指定Keystore文件(简称KSF)中的数据
 纯 入 参  : pszKeystoreFile: Keystore全文件名
 纯 出 参  : 输出Keystore中的数据.
 入参出参  : 无
 返 回 值  : WSEC_SUCCESS:          完全成功
             WSEC_ERR_KMC_READ_MK_FAIL: 读取MK过程部分出错, pKeystoreData输出正常数据,
                                    已经读取的MK仍然正确地输出到pKeystoreData->pMkList, 
             其它:                  读KSF失败的具体错误原因.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_ReadKsf(const WSEC_CHAR* pszKeystoreFile, KMC_KSF_MEM_STRU* pKeystoreData)
{
    WSEC_FILE fKeystore = WSEC_NULL_PTR;
    KMC_KSF_RK_STRU* pRk = WSEC_NULL_PTR;
    KMC_KSF_MK_STRU stMkRead = {0};
    KMC_MEM_MK_STRU* pstMkRec = WSEC_NULL_PTR;
    WSEC_UINT32 ulRet = WSEC_SUCCESS;
    WSEC_UINT32 ulReadMkNum = 0;
    WSEC_BUFF aBuffs[4] = {{0},{0},{0},{0}}, stHmacRstOld = {0}, stHmacKey = {0};
    WSEC_BUFF stRmk = {0};
    WSEC_BUFF stCipherBuff = {0};
    WSEC_UINT32 i = 0;
    WSEC_PROGRESS_RPT_STRU* pstRptProgress = WSEC_NULL_PTR;
    WSEC_SPEND_TIME_STRU stTimer = {0};
    WSEC_BOOL bCancel = WSEC_FALSE;

    if (g_pKmcIniCtx) {pstRptProgress = &g_pKmcIniCtx->stProgressRpt;}

    WSEC_ASSERT(pszKeystoreFile && pKeystoreData);
    WSEC_ASSERT(WSEC_ARR_IsEmpty(pKeystoreData->arrMk));

    pRk = (KMC_KSF_RK_STRU*)WSEC_MALLOC(sizeof(KMC_KSF_RK_STRU));
    return_oper_if(!pRk, WSEC_LOG_E4MALLOC(sizeof(KMC_KSF_RK_STRU)), WSEC_ERR_MALLOC_FAIL);

    /* 1 读Rootkey信息*/
    do
    {
        ulRet = KMC_PRI_ReadRootKey(pszKeystoreFile, pRk);
        break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E1("KMC_PRI_ReadRootKey()=%u", ulRet), oper_null);

        if (pRk->stRkAttr.usVer != KMC_KSF_VER) /* 版本号不匹配, 不能按照本函数逻辑来读取 */
        {
            ulRet = KMC_PRI_ReadKsfBasedVer(pszKeystoreFile, pRk->stRkAttr.usVer, pKeystoreData);
        }
    }do_end;

    if (ulRet != WSEC_SUCCESS)
    {
        WSEC_FREE(pRk);
        return ulRet;
    }

    WSEC_BUFF_ALLOC(stRmk, KMC_RMK_LEN * 2); /* 前半部分用于EK, 后半部分用于IK */
    WSEC_BUFF_ALLOC(stCipherBuff, sizeof(stMkRead.stMkRear.abKey));

    do
    {
        break_oper_if(!stRmk.pBuff, WSEC_LOG_E4MALLOC(stRmk.nLen), ulRet = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!stCipherBuff.pBuff, WSEC_LOG_E4MALLOC(stCipherBuff.nLen), ulRet = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(WSEC_MEMCPY(&(pKeystoreData->stRkInfo), sizeof(pKeystoreData->stRkInfo), &pRk->stRkAttr, sizeof(pRk->stRkAttr)) != EOK,
                      WSEC_LOG_E4MEMCPY, ulRet = WSEC_ERR_MEMCPY_FAIL);

        /* 计算RMK */
        break_oper_if(!KMC_PRI_MakeRmk(pRk, &stRmk), WSEC_LOG_E("KMC_PRI_MakeRmk() fail."), ulRet = WSEC_ERR_PBKDF2_FAIL);

        /*lint -e668*/
        fKeystore = WSEC_FOPEN(pszKeystoreFile, "rb");
        break_oper_if(!fKeystore, WSEC_LOG_E1("Open '%s' fail.", pszKeystoreFile), ulRet = WSEC_ERR_OPEN_FILE_FAIL);

        break_oper_if(WSEC_FSEEK(fKeystore, sizeof(g_KsfFlag) + sizeof(KMC_KSF_RK_STRU), SEEK_SET) != 0, WSEC_LOG_E("fseek fail."), ulRet = WSEC_ERR_WRI_FILE_FAIL);
    }do_end;

    if (ulRet != WSEC_SUCCESS)
    {
        WSEC_FCLOSE(fKeystore);
		WSEC_BUFF_FREE_S(stRmk);
        WSEC_BUFF_FREE(stCipherBuff);
        WSEC_FREE(pRk);
        return ulRet;
    }
    
    /*---------------------------------------------------------------------------------------------------
    2 读MK
    说明: 读取MK过程中, 出于可用性考虑, 单条MK记录读取失败, 仍然继续读取后续的MK.
    ---------------------------------------------------------------------------------------------------*/
    for (i = 0; i < pRk->ulMkNum; i++)
    {
        /* 2.1 申请拟加入MK链表的节点 */
        if (!pstMkRec) {pstMkRec = (KMC_MEM_MK_STRU*)WSEC_MALLOC(sizeof(KMC_MEM_MK_STRU));}
        if (!pstMkRec)
        {
            WSEC_LOG_E4MALLOC(sizeof(KMC_MEM_MK_STRU));
            continue;
        }

        /* 2.2 完整地读出1条MK记录 */
        /*lint -e802*/
        if (!WSEC_FREAD_MUST(&stMkRead, sizeof(stMkRead), fKeystore))
        {
            WSEC_LOG_E("WSEC_FREAD_MUST fali.");
            continue;
        }

        /* 2.3 字节序转换 */
        KMC_PRI_CvtByteOrder4KsfMk(&stMkRead, wbcNetwork2Host);

        /* 2.4 MK完整性校验 */
        /* 2.4.1 解密MK密钥 */
        if (WSEC_MEMCPY(stCipherBuff.pBuff, stCipherBuff.nLen, stMkRead.stMkRear.abKey, stMkRead.ulCipherLen) != EOK)
        {
            WSEC_LOG_E4MEMCPY;
            continue;
        }
		pstMkRec->stMkRear.ulPlainLen = WSEC_MK_LEN_MAX;
        if(CAC_Decrypt(KMC_ENCRYPT_MK_ALGID,
                       stRmk.pBuff, KMC_RMK_LEN,
                       stMkRead.abIv, sizeof(stMkRead.abIv),
                       stCipherBuff.pBuff, stMkRead.ulCipherLen,
                       pstMkRec->stMkRear.abKey, &pstMkRec->stMkRear.ulPlainLen) != WSEC_SUCCESS)
        {
            WSEC_LOG_E("CAC_Decrypt() fali.");
            continue;
        }

        /* 2.4.2 MK完整性校验 */
        WSEC_BUFF_ASSIGN(aBuffs[0], &stMkRead.stMkInfo, sizeof(stMkRead.stMkInfo));
        WSEC_BUFF_ASSIGN(aBuffs[1], stMkRead.abReserved, sizeof(stMkRead.abReserved));
        WSEC_BUFF_ASSIGN(aBuffs[2], &pstMkRec->stMkRear.ulPlainLen, sizeof(pstMkRec->stMkRear.ulPlainLen));
        WSEC_BUFF_ASSIGN(aBuffs[3], pstMkRec->stMkRear.abKey, pstMkRec->stMkRear.ulPlainLen);
        WSEC_BUFF_ASSIGN(stHmacKey, (WSEC_BYTE*)stRmk.pBuff + KMC_RMK_LEN, KMC_RMK_LEN); /* IK */
        WSEC_BUFF_ASSIGN(stHmacRstOld, stMkRead.abMkHash, sizeof(stMkRead.abMkHash)); /* HMAC code */
        if(WSEC_ChkHmacCode(KMC_HMAC_MK_ALGID, aBuffs, 4, &stHmacKey, &stHmacRstOld) != WSEC_SUCCESS)
        {
            WSEC_LOG_E("Above function return fail.");
            continue;
        }

        if(WSEC_MEMCPY(&(pstMkRec->stMkInfo), sizeof(pstMkRec->stMkInfo), &(stMkRead.stMkInfo), sizeof(stMkRead.stMkInfo)) != EOK)
        {
            WSEC_LOG_E4MEMCPY;
            continue;
        }

        /* 2.5 将MK加入数组 */
        if(KMC_PRI_AddMk2Array(pKeystoreData, pstMkRec) == WSEC_SUCCESS)
        {
            pstMkRec = WSEC_NULL_PTR; /* Misinformation: FORTIFY.Memory_Leak(This branch means that 'pstMkRec' added ARRAY.) */
            ulReadMkNum++;
        }
        else
        {
            WSEC_LOG_E("KMC_PRI_AddMk2Array() fail.");
            continue;
        }

        WSEC_RptProgress(pstRptProgress, &stTimer, pRk->ulMkNum, ulReadMkNum, &bCancel);
        break_oper_if(bCancel, WSEC_LOG_E("App Canceled."), ulRet = WSEC_ERR_CANCEL_BY_APP);
    }
    if (ulReadMkNum > 0) {WSEC_ASSERT((WSEC_INT32)ulReadMkNum == WSEC_ARR_GetCount(pKeystoreData->arrMk));}
    if (!bCancel) {WSEC_RptProgress(pstRptProgress, WSEC_NULL_PTR, pRk->ulMkNum, pRk->ulMkNum, WSEC_NULL_PTR);} /* 确保进度100%上报 */
    
    WSEC_FCLOSE(fKeystore);
    WSEC_FREE(pstMkRec);
    WSEC_BUFF_FREE(stCipherBuff);
    WSEC_BUFF_FREE_S(stRmk);

    if (pRk->ulMkNum != ulReadMkNum)
    {
        WSEC_LOG_E3("Read MK from key-store-file(%s) fail, The header define MKs = %d, but success read MKs = %d.", 
                    pszKeystoreFile, pRk->ulMkNum, ulReadMkNum);
        ulRet = WSEC_ERR_KMC_READ_MK_FAIL;
    }

    WSEC_FREE(pRk);

    return ulRet;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_ReadRootKey
 功能描述  : 读取Keystore文件中的RootKey信息
 纯 入 参  : pszFile: Keystore全文件名
 纯 出 参  : pRk: 输出Keystore中的RootKey
 入参出参  : 无
 返 回 值  : WSEC_SUCCESS: 成功
             其它:         失败的具体错误原因.
 特别注意  : 无

 修改历史
  1.日    期   : 2015年05月19日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_ReadRootKey(const WSEC_CHAR* pszFile, KMC_KSF_RK_STRU* pRk)
{
    WSEC_BYTE   abKsfFmtFlg[sizeof(g_KsfFlag)] = {0};
    WSEC_FILE fKeystore = WSEC_NULL_PTR;
    WSEC_BUFF aBuffs[1] = {{0}};
    WSEC_UINT32 ulRet = WSEC_SUCCESS;

    WSEC_ASSERT(pszFile && pRk);

    /*lint -e668*/
    fKeystore = WSEC_FOPEN(pszFile, "rb");
    
    do
    {
        /* 1) 检查资源是否就绪 */
        break_oper_if(!fKeystore, WSEC_LOG_E1("Open '%s' fail.", pszFile), ulRet = WSEC_ERR_OPEN_FILE_FAIL);

        /* 2) 读取并检查格式码 */
        break_oper_if(!WSEC_FREAD_MUST(abKsfFmtFlg, sizeof(abKsfFmtFlg), fKeystore), 
                      WSEC_LOG_E1("Read File(%s) fail.", pszFile),
                      ulRet = WSEC_ERR_KMC_NOT_KSF_FORMAT);
        break_oper_if(WSEC_MEMCMP(abKsfFmtFlg, g_KsfFlag, sizeof(g_KsfFlag)) != 0,
                      WSEC_LOG_E1("%s is not KSF format.", pszFile),
                      ulRet = WSEC_ERR_KMC_NOT_KSF_FORMAT);

        /* 3) 读取RootKey信息 */
        break_oper_if(!WSEC_FREAD_MUST(pRk, sizeof(KMC_KSF_RK_STRU), fKeystore), 
                      WSEC_LOG_E1("Read File(%s) fail.", pszFile),
                      ulRet = WSEC_ERR_KMC_NOT_KSF_FORMAT);

        /* 4) 字节序转换 */
        KMC_PRI_CvtByteOrder4KsfRk(pRk, wbcNetwork2Host);

        /* 5) 文件版本号检查 */
        if (pRk->stRkAttr.usVer != KMC_KSF_VER) {break;}

        /* 6) 检查MK数量是否超限 */
        break_oper_if(pRk->ulMkNum > WSEC_MK_NUM_MAX, \
                      WSEC_LOG_E2("ulMkNum(%u) invalid.(MAX = %u)", pRk->ulMkNum, WSEC_MK_NUM_MAX), \
                      ulRet = WSEC_ERR_KMC_KSF_DATA_INVALID);

        /* 7) 检查Hash值 */
        WSEC_BUFF_ASSIGN(aBuffs[0], pRk, sizeof(KMC_KSF_RK_STRU) - sizeof(pRk->abAboveHash));

        ulRet = WSEC_ChkIntegrity(WSEC_ALGID_SHA256, aBuffs, 1, pRk->abAboveHash, sizeof(pRk->abAboveHash));
        break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("The KSF's header integrity fail."), oper_null);
    }do_end;        
    WSEC_FCLOSE(fKeystore);
    
    return ulRet;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_ReadKsfBasedVer
 功能描述  : 按照指定版本号读取指定Keystore文件(简称KSF)中的数据
 纯 入 参  : pszKeystoreFile: Keystore全文件名
             usVer:           该文件的版本号
 纯 出 参  : 输出Keystore中的数据.
 入参出参  : 无
 返 回 值  : WSEC_SUCCESS:          完全成功
             WSEC_ERR_KMC_READ_MK_FAIL: 读取MK过程部分出错, pKeystoreData输出正常数据,
                                    已经读取的MK仍然正确地输出到pKeystoreData->pMkList, 
             其它:                  读KSF失败的具体错误原因.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_ReadKsfBasedVer(const WSEC_CHAR* pszKeystoreFile, WSEC_UINT16 usVer, KMC_KSF_MEM_STRU* pKeystoreData)
{
    WSEC_UNREFER(pszKeystoreFile);
    WSEC_UNREFER(usVer);
    WSEC_UNREFER(pKeystoreData);

    /* 当Keystore文件版本号变更时, 需要程序员实现该函数 */
    return WSEC_ERR_KMC_READ_DIFF_VER_KSF_FAIL;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_WriteKsfSafety
 功能描述  : 可靠地将数据写入Keystore文件
 纯 入 参  : pKeystoreData: 用于写入Keystore文件的数据
             pRk: 根密钥相关信息
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月02日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_WriteKsfSafety(const KMC_KSF_MEM_STRU* pKeystoreData, const KMC_KSF_RK_STRU* pRk)
{
    WSEC_ERR_T nErrCode;
    KMC_WRI_KSF_FAIL_NTF_STRU stRpt = {0};
    
    nErrCode = WSEC_WriteFileS(pKeystoreData, g_KmcSys.apszKeystoreFile, WSEC_NUM_OF(g_KmcSys.apszKeystoreFile), (WSEC_WriteFile)KMC_PRI_WriteKsf, pRk);
    if (nErrCode != WSEC_SUCCESS)
    {
        stRpt.ulCause = nErrCode;
        WSEC_NOTIFY(WSEC_KMC_NTF_WRI_KEY_STORE_FAIL, &stRpt, sizeof(stRpt));
    }
    
    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_WriteKsf
 功能描述  : 写Keystore文件
 纯 入 参  : pKeystoreData: 用于写入Keystore文件的数据
             pszFile:       Keystore文件名
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月01日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_WriteKsf(const KMC_KSF_MEM_STRU* pKeystoreData, const WSEC_CHAR* pszFile, const KMC_KSF_RK_STRU* pRk)
{
    WSEC_FILE fKeystore = WSEC_NULL_PTR;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    KMC_MEM_MK_STRU* pMemMk = WSEC_NULL_PTR;
    KMC_KSF_RK_STRU* pstRkWri = WSEC_NULL_PTR;
    KMC_KSF_MK_STRU* pstMkWri = WSEC_NULL_PTR;
    WSEC_BUFF stRmk = {0};
    WSEC_BUFF aBuffs[4] = {{0},{0},{0},{0}}, stHmacKey = {0}, stHmacRst = {0};
    WSEC_BUFF stPlainBuff = {0};
    WSEC_PROGRESS_RPT_STRU* pstRptProgress = WSEC_NULL_PTR;
    WSEC_SPEND_TIME_STRU stTimer = {0};
    WSEC_INT32 nMkNum, i;

    if (g_pKmcIniCtx) {pstRptProgress = &g_pKmcIniCtx->stProgressRpt;}

    /* 1. 确保RK信息存在 */
    do
    {
        pstRkWri = (KMC_KSF_RK_STRU*)WSEC_MALLOC(sizeof(KMC_KSF_RK_STRU));
        break_oper_if(!pstRkWri, WSEC_LOG_E4MALLOC(sizeof(KMC_KSF_RK_STRU)), nErrCode = WSEC_ERR_MALLOC_FAIL);

        if (pRk)
        {
            break_oper_if(WSEC_MEMCPY(pstRkWri, sizeof(KMC_KSF_RK_STRU), pRk, sizeof(KMC_KSF_RK_STRU)) != EOK, WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
        }
        else
        {
            nErrCode = KMC_PRI_ReadRootKey(pKeystoreData->pszFromFile, pstRkWri);
            break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC_PRI_ReadRootKey() = %u", nErrCode), oper_null);
            break_oper_if(WSEC_MEMCPY(&pstRkWri->stRkAttr, sizeof(pstRkWri->stRkAttr), &pKeystoreData->stRkInfo, sizeof(pKeystoreData->stRkInfo)) != EOK, 
                          WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
        }
    }do_end;
    if (nErrCode != WSEC_SUCCESS)
    {
        WSEC_FREE(pstRkWri);
        return nErrCode;
    }

    /* 2. 写RootKey及MK */
    do
    {
        /*lint -e668*/
        fKeystore = WSEC_FOPEN(pszFile, "wb");
        break_oper_if(!fKeystore, WSEC_LOG_E1("Open file(%s) fail", pszFile), nErrCode = WSEC_ERR_OPEN_FILE_FAIL);

        WSEC_BUFF_ALLOC(stRmk, KMC_RMK_LEN * 2); /* 前半部分用于EK, 后半部分用于IK */
        WSEC_BUFF_ALLOC(stPlainBuff, sizeof(pstMkWri->stMkRear.abKey));
        pstMkWri = (KMC_KSF_MK_STRU*)WSEC_MALLOC(sizeof(KMC_KSF_MK_STRU));

        break_oper_if(!stRmk.pBuff, WSEC_LOG_E4MALLOC(stRmk.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!stPlainBuff.pBuff, WSEC_LOG_E4MALLOC(stPlainBuff.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!pstMkWri, WSEC_LOG_E4MALLOC(sizeof(KMC_KSF_MK_STRU)), nErrCode = WSEC_ERR_MALLOC_FAIL);

        pstRkWri->ulMkNum = (WSEC_UINT32)WSEC_ARR_GetCount(pKeystoreData->arrMk);

        /* RootKey信息的Hash */
        WSEC_BUFF_ASSIGN(aBuffs[0], pstRkWri, sizeof(KMC_KSF_RK_STRU) - sizeof(pstRkWri->abAboveHash));
        WSEC_BUFF_ASSIGN(aBuffs[1], pstRkWri->abAboveHash, sizeof(pstRkWri->abAboveHash));
        break_oper_if(!WSEC_CreateHashCode(WSEC_ALGID_SHA256, aBuffs, 1, &(aBuffs[1])),
                      WSEC_LOG_E("WSEC_CreateHashCode() fail"),
                      nErrCode = WSEC_ERR_GEN_HASH_CODE_FAIL);

        KMC_PRI_CvtByteOrder4KsfRk(pstRkWri, wbcHost2Network);
        
        /* 将RK写入文件 */
        break_oper_if(!WSEC_FWRITE_MUST(g_KsfFlag, sizeof(g_KsfFlag), fKeystore),
                      WSEC_LOG_E1("Write file(%s) fail", pszFile),
                      nErrCode = WSEC_ERR_WRI_FILE_FAIL);
        break_oper_if(!WSEC_FWRITE_MUST(pstRkWri, sizeof(KMC_KSF_RK_STRU), fKeystore),
                      WSEC_LOG_E1("Write file(%s) fail", pszFile),
                      nErrCode = WSEC_ERR_WRI_FILE_FAIL);

        KMC_PRI_CvtByteOrder4KsfRk(pstRkWri, wbcNetwork2Host); /* 字节序还原, 后续还需要使用 */

        /* 构造RMK */
        break_oper_if(!KMC_PRI_MakeRmk(pstRkWri, &stRmk), WSEC_LOG_E("KMC_PRI_MakeRmk() fail."), nErrCode = WSEC_ERR_PBKDF2_FAIL);

        /* 将各MK写入文件 */
        nMkNum = WSEC_ARR_GetCount(pKeystoreData->arrMk);
        for (i = 0; i < nMkNum; i++)
        {
            pMemMk = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(pKeystoreData->arrMk, i);
            break_oper_if(!pMemMk, WSEC_LOG_E("memory access fail."), nErrCode = WSEC_ERR_OPER_ARRAY_FAIL);
            break_oper_if(WSEC_MEMCPY(&pstMkWri->stMkInfo, sizeof(pstMkWri->stMkInfo), &pMemMk->stMkInfo, sizeof(pMemMk->stMkInfo)) != EOK,
                          WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
            break_oper_if(WSEC_MEMCPY(&pstMkWri->stMkRear, sizeof(pstMkWri->stMkRear), &pMemMk->stMkRear, sizeof(pMemMk->stMkRear)) != EOK,
                          WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
            WSEC_UNCARE(CAC_Random(pstMkWri->abIv, sizeof(pstMkWri->abIv))); /* 尽量使用随机数做加密用IV */

            /* 存储在数组中的密钥明文不是真正的明文, 是加掩处理的, 所以需要脱掩 */
            KMC_UNMASK_MK_TO(pMemMk, pstMkWri->stMkRear.abKey, pstMkWri->stMkRear.ulPlainLen);

            /* 计算MK的Hash */
            WSEC_BUFF_ASSIGN(aBuffs[0], &pstMkWri->stMkInfo, sizeof(pstMkWri->stMkInfo));
            WSEC_BUFF_ASSIGN(aBuffs[1], &pstMkWri->abReserved, sizeof(pstMkWri->abReserved));
            WSEC_BUFF_ASSIGN(aBuffs[2], &pstMkWri->stMkRear.ulPlainLen, sizeof(pstMkWri->stMkRear.ulPlainLen));
            WSEC_BUFF_ASSIGN(aBuffs[3], pstMkWri->stMkRear.abKey, pstMkWri->stMkRear.ulPlainLen);
            WSEC_BUFF_ASSIGN(stHmacRst, pstMkWri->abMkHash, sizeof(pstMkWri->abMkHash));
            WSEC_BUFF_ASSIGN(stHmacKey, (WSEC_BYTE*)stRmk.pBuff + KMC_RMK_LEN, KMC_RMK_LEN);
			
            /* 计算HMAC */
            nErrCode = WSEC_CreateHmacCode(KMC_HMAC_MK_ALGID, aBuffs, 4, &stHmacKey, &stHmacRst);
            break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E("Create HMAC fail."), oper_null);

            /* 将MK明文用RMK加密 */
			pstMkWri->ulCipherLen = WSEC_MK_LEN_MAX;
            break_oper_if(WSEC_MEMCPY(stPlainBuff.pBuff, stPlainBuff.nLen, pstMkWri->stMkRear.abKey, pstMkWri->stMkRear.ulPlainLen) != EOK, 
                          WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
            break_oper_if(CAC_Encrypt(KMC_ENCRYPT_MK_ALGID,
                                       stRmk.pBuff, KMC_RMK_LEN,
                                       pstMkWri->abIv, sizeof(pstMkWri->abIv),
                                       stPlainBuff.pBuff, pstMkWri->stMkRear.ulPlainLen,
                                       pstMkWri->stMkRear.abKey, &pstMkWri->ulCipherLen) != WSEC_SUCCESS,
                           WSEC_LOG_E("CAC_Encrypt() fail."), nErrCode = WSEC_ERR_ENCRPT_FAIL);

            KMC_PRI_CvtByteOrder4KsfMk(pstMkWri, wbcHost2Network); /* 写文件前字节序转网络序 */

            break_oper_if(!WSEC_FWRITE_MUST(pstMkWri, sizeof(KMC_KSF_MK_STRU), fKeystore),
                          WSEC_LOG_E1("Write file(%s) fail", pszFile),
                          nErrCode = WSEC_ERR_WRI_FILE_FAIL);
            WSEC_RptProgress(pstRptProgress, &stTimer, (WSEC_UINT32)nMkNum, (WSEC_UINT32)(i + 1), WSEC_NULL_PTR);
        }
    } do_end;

    WSEC_FFLUSH(fKeystore);
    WSEC_FCLOSE(fKeystore);
    WSEC_BUFF_FREE_S(stRmk);
    WSEC_BUFF_FREE_S(stPlainBuff);
    WSEC_FREE(pstRkWri);
    WSEC_FREE(pstMkWri);
    
    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CreateRootKey
 功能描述  : 创建Root Key文件
 纯 入 参  : pstEntropy: 根密钥熵
 纯 出 参  : pRkInfo: 输出新建Root Key过程中产生的数据
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月24日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_CreateRootKey(const WSEC_BUFF* pstEntropy, KMC_KSF_RK_STRU* pRk)
{
    const WSEC_INT32 nRndNum = 2;
    KMC_RK_ATTR_STRU* pKeyAttr = WSEC_NULL_PTR;
    KMC_RK_PARA_STRU* pKeyPara = WSEC_NULL_PTR;
    WSEC_SIZE_T nOneMeteriaLen = 0;
    WSEC_BYTE* pbAllMeterial = WSEC_NULL_PTR;
    WSEC_BYTE abBase[2] = {0x01, 0x02};
    WSEC_BUFF aBuff[4] = {{0},{0},{0},{0}};
    WSEC_BUFF aWriHash[2] = {{0},{0}};
    WSEC_UINT32 ulBuffNum = 0;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    WSEC_INT32 i;
    
    WSEC_ASSERT(pRk);
    
    pKeyAttr = &pRk->stRkAttr;
    pKeyPara = &pRk->stRkPara;
    nOneMeteriaLen = sizeof(pKeyPara->abRkMeterial1);
    WSEC_ASSERT(sizeof(pKeyPara->abRkMeterial2) == nOneMeteriaLen);

    /* 1. 基本信息 */
    pKeyAttr->usVer            = KMC_KSF_VER;
    pKeyAttr->usRkMeterialFrom = KMC_RK_GEN_BY_INNER;
    
    nErrCode = KMC_PRI_SetLifeTime(g_pKmcCfg->stRkCfg.ulRootKeyLifeDays, &pKeyAttr->stRkCreateTimeUtc, &pKeyAttr->stRkExpiredTimeUtc);
    return_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC_PRI_SetLifeTime()=%u", nErrCode), nErrCode);
    return_oper_if(CAC_Random(pKeyPara->abRmkSalt, sizeof(pKeyPara->abRmkSalt)) != WSEC_SUCCESS, WSEC_LOG_E("CAC_Random() fail."), WSEC_ERR_GET_RAND_FAIL);
    pKeyAttr->ulRmkIterations = g_pKmcCfg->stRkCfg.ulRootMasterKeyIterations;

    /* 2. Keystore存放2个根密钥物料
        设2个随机数R1, R2及外部熵E, 则2个根密钥物料M1~M2按如下算法产生:
        M1 = SHA256(0x1 + R1 + R2 + E)
        M2 = SHA256(0x2 + R1 + R2 + E)         */
    pbAllMeterial = (WSEC_BYTE*)WSEC_MALLOC(nRndNum * nOneMeteriaLen); /* 存放随机数 */
    return_oper_if(!pbAllMeterial, WSEC_LOG_E4MALLOC(nRndNum * nOneMeteriaLen), WSEC_ERR_MALLOC_FAIL);

    if (CAC_Random(pbAllMeterial, nRndNum * nOneMeteriaLen) != WSEC_SUCCESS)
    {
        WSEC_FREE(pbAllMeterial);
        WSEC_LOG_E("CAC_Random() fail.");
        return WSEC_ERR_GET_RAND_FAIL;
    }

    WSEC_BUFF_ASSIGN(aBuff[1], pbAllMeterial, nOneMeteriaLen);
    WSEC_BUFF_ASSIGN(aBuff[2], pbAllMeterial + nOneMeteriaLen, nOneMeteriaLen);
    if (pstEntropy) {WSEC_BUFF_ASSIGN(aBuff[3], pstEntropy->pBuff, pstEntropy->nLen);}
    ulBuffNum = 1 + nRndNum + (pstEntropy ? 1 : 0);

    WSEC_BUFF_ASSIGN(aWriHash[0], pKeyPara->abRkMeterial1, nOneMeteriaLen);
    WSEC_BUFF_ASSIGN(aWriHash[1], pKeyPara->abRkMeterial2, nOneMeteriaLen);

    for (i = 0; i < nRndNum; i++) /* 产生用于根密钥的 nRndNum 个物料 */
    {
        WSEC_BUFF_ASSIGN(aBuff[0], &abBase[i], sizeof(abBase[i]));
        if (!WSEC_CreateHashCode(WSEC_ALGID_SHA256, aBuff, ulBuffNum, &aWriHash[i]))
        {
            WSEC_LOG_E("WSEC_CreateHashCode() fail.");
            nErrCode = WSEC_ERR_GEN_HASH_CODE_FAIL;
            break;
        }
    }

    WSEC_FREE(pbAllMeterial);

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CreateMkArr
 功能描述  : 创建Master Key集合
 纯 入 参  : 无
 纯 出 参  : pKeystore: 存储MK数组的Keystore数据结构
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月24日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_CreateMkArr(KMC_KSF_MEM_STRU* pKeystore)
{
    WSEC_ERR_T nErrCode = WSEC_SUCCESS, nTemp;
    KMC_DOMAIN_CFG_STRU* pDomainCfg = WSEC_NULL_PTR;
    KMC_CFG_KEY_TYPE_STRU*   pKeyType   = WSEC_NULL_PTR;
    WSEC_UINT32 ulKeyId = 0;
    WSEC_INT32 i, j, nDomainNum;
    WSEC_PROGRESS_RPT_STRU* pstRptProgress = WSEC_NULL_PTR;
    WSEC_SPEND_TIME_STRU stTimer = {0};
    WSEC_BOOL bCancel = WSEC_FALSE;

    WSEC_ASSERT(pKeystore);

    if (g_pKmcIniCtx) {pstRptProgress = &g_pKmcIniCtx->stProgressRpt;}

    /* 根据KMC配置, 创建MK */
    nDomainNum = WSEC_ARR_GetCount(g_pKmcCfg->arrDomainCfg);
    for (i = 0; i < nDomainNum; i++)
    {
        pDomainCfg = (KMC_DOMAIN_CFG_STRU*)WSEC_ARR_GetAt(g_pKmcCfg->arrDomainCfg, i);
        break_oper_if(!pDomainCfg, WSEC_LOG_E("memory access fail."), nErrCode = WSEC_ERR_OPER_ARRAY_FAIL);
        continue_if(pDomainCfg->stDomainInfo.ucKeyFrom != KMC_MK_GEN_BY_INNER);

        ulKeyId = 0; /* KeyId在每个Domain内唯一分配 */
        for (j = 0; j < WSEC_ARR_GetCount(pDomainCfg->arrKeyTypeCfg); j++)
        {
            ulKeyId++;
            pKeyType = (KMC_CFG_KEY_TYPE_STRU*)WSEC_ARR_GetAt(pDomainCfg->arrKeyTypeCfg, j);
            break_oper_if(!pKeyType, WSEC_LOG_E("memory access fail."), nErrCode = WSEC_ERR_OPER_ARRAY_FAIL);
            nTemp = KMC_PRI_CreateMkItem(pKeystore, &pDomainCfg->stDomainInfo, pKeyType, WSEC_NULL_PTR, ulKeyId);
            continue_if(WSEC_SUCCESS == nTemp);

            nErrCode = nTemp;
            WSEC_LOG_E1("KMC_PRI_CreateMkItem()=%u", nTemp);
        }

        WSEC_RptProgress(pstRptProgress, &stTimer, (WSEC_UINT32)nDomainNum, i + 1, &bCancel);
        break_oper_if(bCancel, WSEC_LOG_E("App Canceled."), nErrCode = WSEC_ERR_CANCEL_BY_APP);
    }
    if (!bCancel) {WSEC_RptProgress(pstRptProgress, WSEC_NULL_PTR, (WSEC_UINT32)nDomainNum, (WSEC_UINT32)nDomainNum, WSEC_NULL_PTR);} /* 确保进度100%上报 */

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CreateMkItem
 功能描述  : 向Keystore创建MK
 纯 入 参  : pKeystore:  存储MK的Keystore;
             pstDomain:  Domain配置信息;
             pstKeyType: KeyType配置
             pKeyPlain:  密钥明文(NULL则需要自动生成)
             ulKeyId:    Key ID
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_CreateMkItem(KMC_KSF_MEM_STRU* pKeystore, const KMC_CFG_DOMAIN_INFO_STRU* pstDomain, const KMC_CFG_KEY_TYPE_STRU* pstKeyType, const WSEC_BUFF* pKeyPlain, WSEC_UINT32 ulKeyId)
{
    KMC_MEM_MK_STRU* pMk = WSEC_NULL_PTR;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    pMk = (KMC_MEM_MK_STRU*)WSEC_MALLOC(sizeof(KMC_MEM_MK_STRU));
    return_oper_if(!pMk, WSEC_LOG_E4MALLOC(sizeof(KMC_MEM_MK_STRU)), WSEC_ERR_MALLOC_FAIL);

    do
    {
        nErrCode = KMC_PRI_MakeMk(pstDomain, pstKeyType, pKeyPlain, ulKeyId, pMk);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC_PRI_MakeMk() = %u", nErrCode), oper_null);

        nErrCode = KMC_PRI_AddMk2Array(pKeystore, pMk);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC_PRI_AddMk2Array() = %u", nErrCode), oper_null);
    }do_end;

    if (WSEC_SUCCESS == nErrCode)
    {
        KMC_PRI_NtfMkChanged(&pMk->stMkInfo, KMC_KEY_ACTIVATED);
    }
    else
    {
        WSEC_FREE(pMk); /* 如果操作成功, 则内存资源归数组管理, 否则需要释放 */
    }

    /* Misinformation: FORTIFY.Memory_Leak ('pMk' added to 'pKeystore' when success) */

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_ReCreateMkItem
 功能描述  : 重新创建MK
             1. 重新生成MK相同类型的MK;
             2. 将老MK的状态置为'非在用'状态.
 纯 入 参  : pKeystore: Keystore数据结构;
 纯 出 参  : 无
 入参出参  :
             pMk:       过期的MK.
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月17日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_ReCreateMkItem(KMC_KSF_MEM_STRU* pKeystore, KMC_MK_INFO_STRU* pMk)
{
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    KMC_DOMAIN_CFG_STRU* pDomain = WSEC_NULL_PTR;
    KMC_CFG_KEY_TYPE_STRU* pKeyType = WSEC_NULL_PTR;
    WSEC_UINT32 ulMaxKeyId = 0;

    WSEC_ASSERT(pKeystore && pMk);

    /* 只能对内部MK重新创建 */
    return_oper_if(pMk->ucGenStyle != KMC_MK_GEN_BY_INNER, WSEC_LOG_E2("Cannot Recreate external MK(DomainId=%d, KeyId=%d)", pMk->ulDomainId, pMk->ulKeyId), WSEC_ERR_KMC_MK_GENTYPE_REJECT_THE_OPER);

    /* 更改老MK的相关信息 */
    return_oper_if(!WSEC_GetUtcDateTime(&pMk->stMkExpiredTimeUtc), WSEC_LOG_E("WSEC_GetUtcDateTime() fail."), WSEC_ERR_GET_CURRENT_TIME_FAIL);
    pMk->ucStatus = KMC_KEY_STATUS_INACTIVE;
    WSEC_LOG_W3("The MK(DomainId=%u, KeyId=%u, KeyType=%u) expired, it's status become in-active.", pMk->ulDomainId, pMk->ulKeyId, pMk->usType);

    /* 老密钥状态变更, 通知APP, 记录日志 */
    KMC_PRI_NtfMkChanged(pMk, KMC_KEY_INACTIVATED);

    return_oper_if(!KMC_PRI_SearchDomainKeyTypeCfg(pMk->ulDomainId, pMk->usType, &pDomain, &pKeyType), 
                   WSEC_LOG_E2("No KeyType found for the MK(DomainId=%u, KeyType=%u)", pMk->ulDomainId, pMk->usType),
                   WSEC_ERR_KMC_DOMAIN_KEYTYPE_MISS);

    /* 获取最大KeyId */
    nErrCode = KMC_PRI_GetMaxMkId(KMC_NOT_LOCK, pMk->ulDomainId, &ulMaxKeyId);
    return_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC_PRI_GetMaxMkId()=%u", nErrCode), nErrCode);

    /* 重新创建MK */
    return KMC_PRI_CreateMkItem(pKeystore, &pDomain->stDomainInfo, pKeyType, WSEC_NULL_PTR, ulMaxKeyId + 1);
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CreateKsf
 功能描述  : 创建Keystore文件
 纯 入 参  : 无
 纯 出 参  : pKeystoreData, pRk: 输出新建Keystore过程中产生的数据
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月24日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_CreateKsf(KMC_KSF_MEM_STRU* pKeystoreData, KMC_KSF_RK_STRU* pRk)
{
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    WSEC_ASSERT(pKeystoreData && pRk);

    /* 创建Root Key及MK数组 */
    do
    {
        nErrCode = KMC_PRI_CreateRootKey((WSEC_BUFF*)WSEC_NULL_PTR, pRk);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC_PRI_CreateRootKey()=%u", nErrCode), oper_null);
        break_oper_if(WSEC_MEMCPY(&pKeystoreData->stRkInfo, sizeof(pKeystoreData->stRkInfo), &pRk->stRkAttr, sizeof(pRk->stRkAttr)) != EOK, 
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);

        nErrCode = KMC_PRI_CreateMkArr(pKeystoreData);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC_CreateMasterKey()=%u", nErrCode), oper_null);
    } do_end;

    if (nErrCode != WSEC_SUCCESS) {return nErrCode;}

    /* 将数据写入文件 */
    return KMC_PRI_WriteKsfSafety(pKeystoreData, pRk);
}

/*****************************************************************************
 函 数 名  : KMC_PRI_MakeRmk
 功能描述  : 构造根密钥导出密钥, 用于对MK进行保护.
 纯 入 参  : pstRkInfo:  根密钥信息
 纯 入 参  : pstRk:  根密钥信息
 纯 出 参  : 无
 入参出参  : pstBuff: 输入时指定存放RMK的内存区及其长度(入), 输出时带出RMK及其真实长度
 返 回 值  : 成功或失败
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL KMC_PRI_MakeRmk(const KMC_KSF_RK_STRU* pstRk, WSEC_BUFF* pstBuff)
{
    WSEC_BYTE abRkMeterial[sizeof(pstRk->stRkPara.abRkMeterial1)];
    WSEC_BYTE abMeterial3[sizeof(pstRk->stRkPara.abRkMeterial1)] = {0};
    const WSEC_BYTE *pMeterial1 = WSEC_NULL_PTR;
    const WSEC_BYTE *pMeterial2 = WSEC_NULL_PTR;
    const WSEC_BYTE *pMeterial3 = WSEC_NULL_PTR;
    WSEC_UINT32 i;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    
    WSEC_ASSERT(pstRk && pstBuff);
    WSEC_ASSERT((pstBuff->pBuff) && (pstBuff->nLen > 0));

    pMeterial1 = pstRk->stRkPara.abRkMeterial1;
    pMeterial2 = pstRk->stRkPara.abRkMeterial2;
    pMeterial3 = abMeterial3;
    KMC_PRI_GetRkMeterial(abMeterial3, sizeof(abMeterial3));
    
    for (i = 0; i < sizeof(abRkMeterial); i++, pMeterial1++, pMeterial2++, pMeterial3++)
    {
        abRkMeterial[i] = (WSEC_BYTE)((*pMeterial1) ^ (*pMeterial2) ^ (*pMeterial3));
    }

    nErrCode = CAC_Pbkdf2(WSEC_ALGID_PBKDF2_HMAC_SHA256,
                          abRkMeterial, sizeof(abRkMeterial),
                          pstRk->stRkPara.abRmkSalt, sizeof(pstRk->stRkPara.abRmkSalt),
                          pstRk->stRkAttr.ulRmkIterations,
                          pstBuff->nLen, pstBuff->pBuff);
    return_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("CAC_Pbkdf2() = %u", nErrCode), WSEC_FALSE);
    
    return WSEC_TRUE;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_MakeMk
 功能描述  : 按照KMC配置的Domain及KeyType信息生成MK
 纯 入 参  : pstDomain:  Domain配置信息;
             pstKeyType: KeyType配置
             pKeyPlain:  密钥明文(NULL则需要自动生成)
             ulKeyId:    Key ID
 纯 出 参  : pMk:        构造的MK数据
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_MakeMk(const KMC_CFG_DOMAIN_INFO_STRU* pstDomain,
                      const KMC_CFG_KEY_TYPE_STRU* pstKeyType, 
                      const WSEC_BUFF* pKeyPlain, 
                      WSEC_UINT32 ulKeyId, 
                      KMC_MEM_MK_STRU* pMk)
{
    KMC_MK_INFO_STRU* pMkMainInfo = WSEC_NULL_PTR;
    KMC_MK_REAR_STRU* pMkRear = WSEC_NULL_PTR;
    WSEC_ERR_T ulErrCode = WSEC_SUCCESS;

    WSEC_ASSERT(pstDomain && pstKeyType && pMk);

    pMkMainInfo = &pMk->stMkInfo;
    pMkRear = &pMk->stMkRear;
    
    /* 1) 设置密钥创建&过期时间 */
    ulErrCode = KMC_PRI_SetLifeTime(pstKeyType->ulKeyLifeDays, &pMkMainInfo->stMkCreateTimeUtc, &pMkMainInfo->stMkExpiredTimeUtc);
    return_oper_if(ulErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC_PRI_SetLifeTime()=%u", ulErrCode), ulErrCode);
    
    /* 2) 密钥 */
    if (pKeyPlain)
    {
        return_oper_if(WSEC_MEMCPY(pMk->stMkRear.abKey, sizeof(pMk->stMkRear.abKey), pKeyPlain->pBuff, pKeyPlain->nLen) != EOK,
                       WSEC_LOG_E4MEMCPY, WSEC_ERR_MEMCPY_FAIL);
        pMkRear->ulPlainLen = pKeyPlain->nLen;
    }
    else
    {
        return_oper_if(CAC_Random(pMk->stMkRear.abKey, pstKeyType->ulKeyLen) != WSEC_SUCCESS, /* 生成随机数作为密钥 */
                       WSEC_LOG_E("CAC_Random() fail."), WSEC_ERR_GET_RAND_FAIL);
        pMkRear->ulPlainLen = pstKeyType->ulKeyLen;
    }

    /* 3) 其它信息 */
    pMkMainInfo->ulDomainId   = pstDomain->ulId;
    pMkMainInfo->ulKeyId      = ulKeyId;
    pMkMainInfo->usType       = pstKeyType->usKeyType;
    pMkMainInfo->ucStatus     = KMC_KEY_STATUS_ACTIVE;
    pMkMainInfo->ucGenStyle   = pstDomain->ucKeyFrom;

    WSEC_LOG_W3("Create master Key(DomainId=%u, KeyId= %u, KeyType=%u) OK.", pstDomain->ulId, ulKeyId, pstKeyType->usKeyType);

    return ulErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_GetRkMeterial
 功能描述  : 获取硬编码根密钥物料
             为了保证根密钥物料的安全性, 总计3个物料之中的2个存储在Keystore,
             另1个物料硬编码获得.
 纯 入 参  : nSize: pMeterial指向缓冲区长度
 纯 出 参  : pMeterial: 输出根密钥物料.
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 该函数实现不能改动, 否则将导致已生成的Keystore不可用!

 修改历史
  1.日    期   : 2015年02月03日
    作    者   : z00118096
    修改内容   : 新生成函数

  2.日    期   : 2015年07月27日
    作    者   : z00118096
    修改内容   : 计算z会出现溢出, 在ARM出错, 修改方式为确保z值计算不会溢出.
                 且兼容老版本. 增加参数判断，防止x,y溢出--by l00171031
*****************************************************************************/
WSEC_VOID KMC_PRI_GetRkMeterial(WSEC_BYTE* pMeterial, WSEC_SIZE_T nSize)
{
    WSEC_SIZE_T i = 0;
    WSEC_INT32 x = 0, y = 0, z = 0, nType = 0;
    WSEC_BYTE aOffset[32] = {59, 5, 91, 0, 68, 5, 52, 64, 75, 73, 11, 0, 20, 69, 68, 32, 91, 69, 91, 73, 32, 88, 51, 37, 64, 24, 51, 69, 68, 32, 32, 32};
    WSEC_BYTE aType[32] = {0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};

    if( !pMeterial || nSize > WSEC_NUM_OF(g_KsfFlag) )
    {
    	WSEC_LOG_E1("The Rk meterial input is wrong, input length is %d .",  nSize);
		return;
    }
		
    for (i = 0; i < nSize; i++)
    {
        x = (i + 11) * (i + 29);
        y = (x + 17) * (x + 31);
        z = ((y % 29) + 23) * ((y % 29) + 37) + aOffset[i];

        nType = z % 3;
        if (aType[i] > 0) { nType = nType - 3; }

        x %= WSEC_NUM_OF(g_KsfFlag);
        y %= WSEC_NUM_OF(g_MkfFlag);
        z %= WSEC_NUM_OF(g_KcfFlag);

        if (0 == nType)
        {
            *(pMeterial + i)= (WSEC_BYTE)(g_KsfFlag[x] & (g_MkfFlag[y] + g_KcfFlag[z]));
        }
        else if (1 == nType)
        {
            *(pMeterial + i)= (WSEC_BYTE)(g_KsfFlag[x] * g_MkfFlag[y] - g_KcfFlag[z]);
        }
        else
        {
            *(pMeterial + i)= (WSEC_BYTE)(g_KsfFlag[x] + g_MkfFlag[y] * g_KcfFlag[z]);
        }
    }

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CalcMkPlainLenMax
 功能描述  : 计算MK明文的最大长度
             由于MK明文被加密成最大WSEC_MK_LEN_MAX长度的密文存储于Keystore,
             为了防止密文超长, 需要对明文长度限制.
 纯 入 参  : 无
 纯 出 参  : pulLenMax: MK明文最大长度(字节数)
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月25日
    作    者   : z00118096
    修改内容   : 新生成函数
  2.日    期   : 2015年8月7日
    作    者   : z00118096
    修改内容   : 将最长明文长度固话, 在DEBUG版本测试固话值是否超大.
*****************************************************************************/
WSEC_ERR_T KMC_PRI_CalcMkPlainLenMax(WSEC_UINT32* pulLenMax)
{
    WSEC_UINT32 ulPlainLenMax = WSEC_MK_LEN_MAX - 1;
    *pulLenMax = ulPlainLenMax;
    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_GetMkDetail
 功能描述  : 根据DomainId, KeyId获取MK
 纯 入 参  : eLock: 是否加锁
             ulDomainId, ulKeyId: MK唯一身份ID
 纯 出 参  : pstMkInfo: MK基本属性[可空]
             pbKeyPlainText: MK密文
 入参出参  : pKeyLen: [in]分配给pbKeyPlainText的长度;
                      [out]输出pbKeyPlainText的实际长度.
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 该函数只开放给CBB内部调用

 修改历史
  1.日    期   : 2014年11月08日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_GetMkDetail(KMC_LOCK_OPER_ENUM eLock, WSEC_UINT32 ulDomainId, WSEC_UINT32 ulKeyId, KMC_MK_INFO_STRU* pstMkInfo, WSEC_BYTE* pbKeyPlainText, WSEC_UINT32* pKeyLen)
{
    WSEC_INT32 nIndex = -1;
    WSEC_INT32 nExpiredDays = 0;
    KMC_MEM_MK_STRU* pMk = WSEC_NULL_PTR;
    KMC_USE_EXPIRED_MK_NTF_STRU* pstMkNtf = WSEC_NULL_PTR;
    WSEC_SYSTIME_T stNowUtc = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    return_err_if_para_invalid("KMC_PRI_GetMkDetail", pbKeyPlainText && pKeyLen);

    if (KMC_NEED_LOCK == eLock) {KMC_PRI_Lock(KMC_LOCK_BOTH);}
    do
    {
        nIndex = KMC_PRI_SearchMkByKeyId(g_pKeystore, ulDomainId, ulKeyId);
        break_oper_if(nIndex < 0, oper_null, nErrCode = WSEC_ERR_KMC_MK_MISS);

        pMk = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(g_pKeystore->arrMk, nIndex);
        break_oper_if(!pMk, WSEC_LOG_E("memory access fail."), nErrCode = WSEC_ERR_OPER_ARRAY_FAIL);

        if (pstMkInfo)
        {
            break_oper_if(WSEC_MEMCPY(pstMkInfo, sizeof(KMC_MK_INFO_STRU), &pMk->stMkInfo, sizeof(pMk->stMkInfo)) != EOK, 
                           WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
        }
        break_oper_if(*pKeyLen < pMk->stMkRear.ulPlainLen,
                       WSEC_LOG_E2("*pKeyLen must at least given %d, but %d, so input-buff insufficient.", pMk->stMkRear.ulPlainLen, *pKeyLen),
                       nErrCode = WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH);

        KMC_UNMASK_MK_TO(pMk, pbKeyPlainText, pMk->stMkRear.ulPlainLen);
        *pKeyLen = pMk->stMkRear.ulPlainLen;

        /* 如果密钥过期已超宽限期, 则通告(尽力而为, 出错不影响函数正确返回) */
        break_oper_if(!WSEC_GetUtcDateTime(&stNowUtc), WSEC_LOG_E("Get UTC fail."), oper_null);
        nExpiredDays = WSEC_DateTimeDiff(dtpDay, &pMk->stMkInfo.stMkExpiredTimeUtc, &stNowUtc);
        break_oper_if(nExpiredDays < (WSEC_INT32)g_pKmcCfg->stKmCfg.ulGraceDaysForUseExpiredKey, oper_null, oper_null);
        pstMkNtf = WSEC_MALLOC(sizeof(KMC_USE_EXPIRED_MK_NTF_STRU));
        break_oper_if(!pstMkNtf, WSEC_LOG_E4MALLOC(sizeof(KMC_USE_EXPIRED_MK_NTF_STRU)), oper_null);
        pstMkNtf->nExpiredDays = nExpiredDays;
        if (WSEC_MEMCPY(&pstMkNtf->stExpiredMkInfo, sizeof(pstMkNtf->stExpiredMkInfo), &pMk->stMkInfo, sizeof(pMk->stMkInfo)) != EOK)
        {
            WSEC_FREE(pstMkNtf);
        }
    }do_end;
    if (KMC_NEED_LOCK == eLock) {KMC_PRI_Unlock(KMC_LOCK_BOTH);}

    if (pstMkNtf)
    {
        WSEC_NOTIFY(WSEC_KMC_NTF_USING_EXPIRED_MK, pstMkNtf, sizeof(KMC_USE_EXPIRED_MK_NTF_STRU));
        WSEC_FREE(pstMkNtf);
    }

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_IsTime4ChkKey
 功能描述  : 判断当前时间是否可对密钥有效期检查
 纯 入 参  : pstCfg:    例行检查需要的配置信息;
             pExecInfo: 例行检查所需要的时间信息
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 真: 可检查, 假: 检查时间未到或检测开关关闭.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月14日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL KMC_PRI_IsTime4ChkKey(const WSEC_SYSTIME_T* pstLocalNow, const KMC_CFG_KEY_MAN_STRU* pstCfg, WSEC_EXEC_INFO_STRU* pExecInfo)
{
    const WSEC_SCHEDULE_TIME_STRU* pstScheduleTime;
    WSEC_BOOL bChk = WSEC_FALSE;
    
    if (g_KmcSys.eState != WSEC_RUNNING) {return WSEC_FALSE;}
    if (pExecInfo->bUnconditionalExec) {return WSEC_TRUE;} /* 本次无条件检查 */

    if (!pstCfg->bKeyAutoUpdate) {return WSEC_FALSE;}

    if (!WSEC_IsDateTime(&pExecInfo->stPreExecTime)) /* 还没有上次检测的时间 */
    {
        WSEC_DateTimeCopy(&pExecInfo->stPreExecTime, pstLocalNow); /* 系统启动当天不检查 */
    }

    pstScheduleTime = &pstCfg->stAutoUpdateKeyTime;
    if (WSEC_IN_SCOPE(pstScheduleTime->ucWeek, 1, 7)) /* 每周x的h:m可检查 */
    {
        if (pstScheduleTime->ucWeek != pstLocalNow->ucWeek) {return WSEC_FALSE;} /* 未到周x, 不检查 */
    }

    /* 日期满足检测条件, 需检查时,分 */
    if (pstLocalNow->ucHour < pstScheduleTime->ucHour) {return WSEC_FALSE;} /* '时'未到, 不检查 */
    if ((pstLocalNow->ucHour == pstScheduleTime->ucHour) && (pstLocalNow->ucMinute < pstScheduleTime->ucMinute)) {return WSEC_FALSE;} /* '分'未到, 不检查 */

    bChk = (WSEC_DateTimeDiff(dtpSecond, &pExecInfo->stPreExecTime, pstLocalNow) >= 24*3600); /* 距上次检测时刻相差至少1整天, 才检查 */
    if (bChk) /* 记录发生检测的时间 */
    {
        WSEC_DateTimeCopy(&pExecInfo->stPreExecTime, pstLocalNow);
    }

    pExecInfo->stPreExecTime.ucHour   = pstScheduleTime->ucHour;
    pExecInfo->stPreExecTime.ucMinute = pstScheduleTime->ucMinute; /* 校正'时', '分'的目的是防止调度精度不足, 引发后续真正'执行'的时刻越来越晚 */
    
    return bChk;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_SetLifeTime
 功能描述  : 设置生命周期相关的日期&时间信息
 纯 入 参  : ulLifeDays: 生命周期(天)
 纯 出 参  : pstCreateUtc: 创建时间
             pstExpireUtc: 过期时间
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2015年5月25日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_SetLifeTime(WSEC_UINT32 ulLifeDays, WSEC_SYSTIME_T* pstCreateUtc, WSEC_SYSTIME_T* pstExpireUtc)
{
    return_oper_if((((WSEC_INT32)ulLifeDays) < 1), WSEC_LOG_E1("ulLifeDays(%u) too big.", ulLifeDays), WSEC_ERR_INVALID_ARG);

    if (!WSEC_GetUtcDateTime(pstCreateUtc)) {return WSEC_ERR_GET_CURRENT_TIME_FAIL;}
    WSEC_DateTimeAdd(pstCreateUtc, (WSEC_INT32)ulLifeDays, dtpDay, pstExpireUtc);
    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CvtByteOrder4KsfRk
 功能描述  : KSF中RK信息字节序转换.
             由于KSF可能会用于异系统, 因此存储在介质上的数据均为网络序, 读入到
             内存，均应转换为主机序
 纯 入 参  : eOper: 字节序转换方法
 纯 出 参  : 无
 入参出参  : pstRk: [in]字节序待转换数据; [out]字节序转换后的数据
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_CvtByteOrder4KsfRk(KMC_KSF_RK_STRU* pstRk, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    KMC_RK_ATTR_STRU* pData = WSEC_NULL_PTR;
    
    WSEC_ASSERT(pstRk);
    WSEC_ASSERT(WSEC_IS2(eOper, wbcHost2Network, wbcNetwork2Host));
    
    pData = &(pstRk->stRkAttr);

    pData->usVer            = WSEC_BYTE_ORDER_CVT_S(eOper, pData->usVer);
    pData->usRkMeterialFrom = WSEC_BYTE_ORDER_CVT_S(eOper, pData->usRkMeterialFrom);
    pData->ulRmkIterations  = WSEC_BYTE_ORDER_CVT_L(eOper, pData->ulRmkIterations);
    pstRk->ulMkNum          = WSEC_BYTE_ORDER_CVT_L(eOper, pstRk->ulMkNum);

    WSEC_CvtByteOrder4DateTime(&(pData->stRkCreateTimeUtc), eOper);
    WSEC_CvtByteOrder4DateTime(&(pData->stRkExpiredTimeUtc), eOper);

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CvtByteOrder4KsfMk
 功能描述  : KSF中MK信息字节序转换.
             由于KSF可能会用于异系统, 因此存储在介质上的数据均为网络序, 读入到
             内存，均应转换为主机序
 纯 入 参  : eOper: 字节序转换方法
 纯 出 参  : 无
 入参出参  : pstMk: [in]字节序待转换数据; [out]字节序转换后的数据.
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_CvtByteOrder4KsfMk(KMC_KSF_MK_STRU* pstMk, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    WSEC_ASSERT(pstMk);
    WSEC_ASSERT(WSEC_IS2(eOper, wbcHost2Network, wbcNetwork2Host));

    pstMk->stMkRear.ulPlainLen  = WSEC_BYTE_ORDER_CVT_L(eOper, pstMk->stMkRear.ulPlainLen);
    KMC_PRI_CvtByteOrder4MkInfo(&pstMk->stMkInfo, eOper);

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CvtByteOrder4MkInfo
 功能描述  : MK信息字作节序转换.
 纯 入 参  : eOper: 字节序转换方法
 纯 出 参  : 无
 入参出参  : pstMkInfo: [in]字节序待转换数据; [out]字节序转换后的数据.
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月25日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_CvtByteOrder4MkInfo(KMC_MK_INFO_STRU* pstMkInfo, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    WSEC_ASSERT(WSEC_IS2(eOper, wbcHost2Network, wbcNetwork2Host));

    pstMkInfo->ulDomainId = WSEC_BYTE_ORDER_CVT_L(eOper, pstMkInfo->ulDomainId);
    pstMkInfo->ulKeyId    = WSEC_BYTE_ORDER_CVT_L(eOper, pstMkInfo->ulKeyId);
    pstMkInfo->usType     = WSEC_BYTE_ORDER_CVT_S(eOper, pstMkInfo->usType);

    WSEC_CvtByteOrder4DateTime(&(pstMkInfo->stMkCreateTimeUtc), eOper);
    WSEC_CvtByteOrder4DateTime(&(pstMkInfo->stMkExpiredTimeUtc), eOper);

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CvtByteOrder4MkfHdr
 功能描述  : MKF中头信息字节序转换.
             由于MKF(MK导出/入文件)可能会用于异系统, 因此存储在介质上的数据均为网络序, 读入到
             内存，均应转换为主机序
 纯 入 参  : eOper: 字节序转换方法
 纯 出 参  : 无
 入参出参  : pstMkfHdr: [in]字节序待转换数据; [out]字节序转换后的数据.
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月25日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_CvtByteOrder4MkfHdr(KMC_MKF_HDR_STRU* pstMkfHdr, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    WSEC_ASSERT(WSEC_IS2(eOper, wbcHost2Network, wbcNetwork2Host));

    pstMkfHdr->usVer                 = WSEC_BYTE_ORDER_CVT_S(eOper, pstMkfHdr->usVer);
    pstMkfHdr->usKsfVer              = WSEC_BYTE_ORDER_CVT_S(eOper, pstMkfHdr->usKsfVer);
    pstMkfHdr->ulEncryptAlgId        = WSEC_BYTE_ORDER_CVT_L(eOper, pstMkfHdr->ulEncryptAlgId);
    pstMkfHdr->ulIteration4EncrypKey = WSEC_BYTE_ORDER_CVT_L(eOper, pstMkfHdr->ulIteration4EncrypKey);
    pstMkfHdr->ulHmacAlgId           = WSEC_BYTE_ORDER_CVT_L(eOper, pstMkfHdr->ulHmacAlgId);
    pstMkfHdr->ulIteration4HmacKey   = WSEC_BYTE_ORDER_CVT_L(eOper, pstMkfHdr->ulIteration4HmacKey);
    pstMkfHdr->ulCipherLenPerMk      = WSEC_BYTE_ORDER_CVT_L(eOper, pstMkfHdr->ulCipherLenPerMk);
    pstMkfHdr->ulMkNum               = WSEC_BYTE_ORDER_CVT_L(eOper, pstMkfHdr->ulMkNum);

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CvtByteOrder4KcfHdr
 功能描述  : KMC配置文件头字节序转换.
 纯 入 参  : eOper: 字节序转换方法
 纯 出 参  : 无
 入参出参  : pstKcfHdr: [in]字节序待转换数据; [out]字节序转换后的数据.
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月30日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_CvtByteOrder4KcfHdr(KMC_KCF_HDR_STRU* pstKcfHdr, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    WSEC_ASSERT(WSEC_IS2(eOper, wbcHost2Network, wbcNetwork2Host));

    pstKcfHdr->ulVer       = WSEC_BYTE_ORDER_CVT_L(eOper, pstKcfHdr->ulVer);
    pstKcfHdr->ulHashAlgId = WSEC_BYTE_ORDER_CVT_L(eOper, pstKcfHdr->ulHashAlgId);

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CvtByteOrder4RkCfg
 功能描述  : RootKey配置信息字节序转换.
 纯 入 参  : eOper: 字节序转换方法
 纯 出 参  : 无
 入参出参  : pstRkCfg: [in]字节序待转换数据; [out]字节序转换后的数据.
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月30日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_CvtByteOrder4RkCfg(KMC_CFG_ROOT_KEY_STRU* pstRkCfg, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    WSEC_ASSERT(WSEC_IS2(eOper, wbcHost2Network, wbcNetwork2Host));

    pstRkCfg->ulRootKeyLifeDays         = WSEC_BYTE_ORDER_CVT_L(eOper, pstRkCfg->ulRootKeyLifeDays);
    pstRkCfg->ulRootMasterKeyIterations = WSEC_BYTE_ORDER_CVT_L(eOper, pstRkCfg->ulRootMasterKeyIterations);

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CvtByteOrder4KeyManCfg
 功能描述  : KeyMan配置信息字节序转换.
 纯 入 参  : eOper: 字节序转换方法
 纯 出 参  : 无
 入参出参  : pstKeyManCfg: [in]字节序待转换数据; [out]字节序转换后的数据.
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月30日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_CvtByteOrder4KeyManCfg(KMC_CFG_KEY_MAN_STRU* pstKeyManCfg, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    WSEC_ASSERT(WSEC_IS2(eOper, wbcHost2Network, wbcNetwork2Host));

    pstKeyManCfg->ulWarningBeforeKeyExpiredDays = WSEC_BYTE_ORDER_CVT_L(eOper, pstKeyManCfg->ulWarningBeforeKeyExpiredDays);
    pstKeyManCfg->ulGraceDaysForUseExpiredKey   = WSEC_BYTE_ORDER_CVT_L(eOper, pstKeyManCfg->ulGraceDaysForUseExpiredKey);
    pstKeyManCfg->bKeyAutoUpdate                = WSEC_BYTE_ORDER_CVT_L(eOper, (WSEC_UINT32)pstKeyManCfg->bKeyAutoUpdate);

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CvtByteOrder4DataProtectCfg
 功能描述  : 数据保护配置信息字节序转换.
 纯 入 参  : eOper: 字节序转换方法
 纯 出 参  : 无
 入参出参  : pstCfg: [in]字节序待转换数据; [out]字节序转换后的数据.
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月30日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_CvtByteOrder4DataProtectCfg(KMC_CFG_DATA_PROTECT_STRU* pstCfg, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    WSEC_ASSERT(WSEC_IS2(eOper, wbcHost2Network, wbcNetwork2Host));

    pstCfg->ulAlgId         = WSEC_BYTE_ORDER_CVT_L(eOper, pstCfg->ulAlgId);
    pstCfg->usKeyType       = WSEC_BYTE_ORDER_CVT_S(eOper, pstCfg->usKeyType);
    pstCfg->bAppendMac      = WSEC_BYTE_ORDER_CVT_L(eOper, (WSEC_UINT32)pstCfg->bAppendMac);
    pstCfg->ulKeyIterations = WSEC_BYTE_ORDER_CVT_L(eOper, pstCfg->ulKeyIterations);

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CvtByteOrder4DomainInfo
 功能描述  : Domain配置信息字节序转换.
 纯 入 参  : eOper: 字节序转换方法
 纯 出 参  : 无
 入参出参  : pstCfg: [in]字节序待转换数据; [out]字节序转换后的数据.
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月30日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_CvtByteOrder4DomainInfo(KMC_CFG_DOMAIN_INFO_STRU* pstCfg, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    WSEC_ASSERT(WSEC_IS2(eOper, wbcHost2Network, wbcNetwork2Host));

    pstCfg->ulId = WSEC_BYTE_ORDER_CVT_L(eOper, pstCfg->ulId);

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CvtByteOrder4KeyType
 功能描述  : KeyType配置信息字节序转换.
 纯 入 参  : eOper: 字节序转换方法
 纯 出 参  : 无
 入参出参  : pstCfg: [in]字节序待转换数据; [out]字节序转换后的数据.
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月30日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_CvtByteOrder4KeyType(KMC_CFG_KEY_TYPE_STRU* pstCfg, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    WSEC_ASSERT(WSEC_IS2(eOper, wbcHost2Network, wbcNetwork2Host));

    pstCfg->usKeyType     = WSEC_BYTE_ORDER_CVT_S(eOper, pstCfg->usKeyType);
    pstCfg->ulKeyLen      = WSEC_BYTE_ORDER_CVT_L(eOper, pstCfg->ulKeyLen);
    pstCfg->ulKeyLifeDays = WSEC_BYTE_ORDER_CVT_L(eOper, pstCfg->ulKeyLifeDays);

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_NtfWriCfgFileFail
 功能描述  : 通告APP: 写配置文件失败
 纯 入 参  : ulCause: 失败原因
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年3月25日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_NtfWriCfgFileFail(WSEC_ERR_T ulCause)
{
    KMC_WRI_KCF_FAIL_NTF_STRU stRpt = {0};
    
    stRpt.ulCause = ulCause;
    WSEC_NOTIFY(WSEC_KMC_NTF_WRI_CFG_FILE_FAIL, &stRpt, sizeof(stRpt));

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_NtfMkChanged
 功能描述  : 通告APP: MK发生了变化.
 纯 入 参  : pMk:   变化了的MK信息
             eType: 具体变化类型
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月5日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_NtfMkChanged(const KMC_MK_INFO_STRU* pMk, KMC_KEY_CHANGE_TYPE_ENUM eType)
{
    KMC_MK_CHANGE_NTF_STRU stNtfData = {0};

    if (WSEC_MEMCPY(&stNtfData.stMkInfo, sizeof(stNtfData.stMkInfo), pMk, sizeof(KMC_MK_INFO_STRU)) != EOK)
    {
        WSEC_LOG_E4MEMCPY;
        return;
    }
    
    stNtfData.eType = eType;
    WSEC_NOTIFY(WSEC_KMC_NTF_MK_CHANGED, &stNtfData, sizeof(stNtfData));

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_NtfRkExpire
 功能描述  : 通告APP: RK即将过期.
 纯 入 参  : pRk:             RK基本信息
             nRemainLifeDays: 还剩多少天过期
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月5日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_NtfRkExpire(const KMC_RK_ATTR_STRU* pRk, WSEC_INT32 nRemainLifeDays)
{
    KMC_RK_EXPIRE_NTF_STRU stNtfData = {0};

    if (WSEC_MEMCPY(&stNtfData.stRkInfo, sizeof(stNtfData.stRkInfo), pRk, sizeof(KMC_RK_ATTR_STRU)) != EOK)
    {
        WSEC_LOG_E4MEMCPY;
        return;
    }

    stNtfData.nRemainDays = nRemainLifeDays;
    WSEC_NOTIFY(WSEC_KMC_NTF_RK_EXPIRE, &stNtfData, sizeof(stNtfData));

    return;
}

/*****************************************************************************
 函 数 名  : 加载KMC所需数据
 功能描述  : 分配资源并读取KMC配置、Keystore数据.
 纯 入 参  : 无
 纯 出 参  : ppKmcCfg: 驻留在内存中的KMC配置
             ppKeystore: 驻留在内存中的Keystore
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2015年07月31日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_LoadData(KMC_CFG_STRU** ppKmcCfg, KMC_KSF_MEM_STRU** ppKeystore, const WSEC_PROGRESS_RPT_STRU* pstRptProgress, const KMC_FP_CFG_CALLBACK_STRU* pstCfgFun, WSEC_BOOL bCbbManCfg)
{
    WSEC_ERR_T nRet = WSEC_SUCCESS, nTemp;

    /* 1 获取KMC配置 */
    *ppKmcCfg = (KMC_CFG_STRU*)WSEC_MALLOC(sizeof(KMC_CFG_STRU));
    do
    {
        break_oper_if(!g_KmcSys.apszKeystoreFile[0], WSEC_LOG_E("Allocate memory fail."), nRet = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!g_KmcSys.apszKeystoreFile[1], WSEC_LOG_E("Allocate memory fail."), nRet = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!(*ppKmcCfg), WSEC_LOG_E("Allocate memory fail."), nRet = WSEC_ERR_MALLOC_FAIL);

        nRet = CAC_Random(g_KmcSys.abMkMaskCode, sizeof(g_KmcSys.abMkMaskCode));
        break_oper_if(nRet != WSEC_SUCCESS, WSEC_LOG_E1("CAC_Random()=%u", nRet), nRet = WSEC_ERR_GET_RAND_FAIL);

        if (bCbbManCfg)
        {
            break_oper_if(!g_KmcSys.apszKmcCfgFile[0], WSEC_LOG_E("Allocate memory fail."), nRet = WSEC_ERR_MALLOC_FAIL);
            break_oper_if(!g_KmcSys.apszKmcCfgFile[1], WSEC_LOG_E("Allocate memory fail."), nRet = WSEC_ERR_MALLOC_FAIL);
        }
        
        nRet = KMC_PRI_OpenIniCtx(bCbbManCfg);
        if (nRet != WSEC_SUCCESS) /* 上下文信息创建失败, 还可以继续运行. */
        {
            WSEC_LOG_E1("KMC_PRI_OpenIniCtx() = %u", nRet);
            WSEC_NOTIFY(WSEC_KMC_NTF_CFG_FILE_CORRUPT, WSEC_NULL_PTR, 0);
        }

        if (g_pKmcIniCtx && pstRptProgress)
        {
            g_pKmcIniCtx->stProgressRpt.pfRptProgress = pstRptProgress->pfRptProgress;
            g_pKmcIniCtx->stProgressRpt.ulTag         = pstRptProgress->ulTag;
        }

        /* 初始化KMC配置 */
        nTemp = KMC_PRI_CfgDataInit(pstCfgFun, *ppKmcCfg);
        nRet = ((nRet != WSEC_SUCCESS) && (nTemp == WSEC_SUCCESS)) ? KMC_PRI_WriteKcfSafety(*ppKmcCfg) : nTemp; /* 若读取失败而获取缺省配置, 则回写文件 */
    } do_end;
    KMC_PRI_CloseIniCtx();

    /* 2 读Keystore */
    do
    {
        if_oper(nRet != WSEC_SUCCESS, break);

        /* 读Keystore */
        nRet = KMC_PRI_ReadKsfSafety(ppKeystore);

        break_oper_if(nRet != WSEC_SUCCESS, WSEC_LOG_E1("[KMC] KMC_PRI_ReadKsfSafety() = %u.", nRet), oper_null);
    } do_end;

    return nRet;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CfgDataInit
 功能描述  : 对KMC配置数据初始化: 通过回调函数向APP索取KMC配置, 如果某项配置获
             取失败, 则使用缺省配置.
 纯 入 参  : pCallbackFun: 各种回调函数
 纯 出 参  : pKmcCfg:      获取到的配置
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_CfgDataInit(const KMC_FP_CFG_CALLBACK_STRU* pCallbackFun, KMC_CFG_STRU* pKmcCfg)
{
    WSEC_BUFF stBuff = {0};
    KMC_CFG_DOMAIN_INFO_STRU* pstDomainInfo = WSEC_NULL_PTR;
    KMC_CFG_KEY_TYPE_STRU* pstKeyType4Read = WSEC_NULL_PTR;
    KMC_DOMAIN_CFG_STRU* pstDomainCfg = WSEC_NULL_PTR;
    KMC_CFG_ROOT_KEY_STRU* pstRkCfg = WSEC_NULL_PTR;
    KMC_CFG_KEY_MAN_STRU* pstKmCfg = WSEC_NULL_PTR;
    KMC_CFG_DATA_PROTECT_STRU* pDataProtectCfg = WSEC_NULL_PTR;
    KMC_SDP_ALG_TYPE_ENUM eAlgType[] = {SDP_ALG_ENCRPT, SDP_ALG_INTEGRITY, SDP_ALG_PWD_PROTECT};
    WSEC_SPEND_TIME_STRU stTimer = {0}, *pstTimer = WSEC_NULL_PTR;
    WSEC_UINT32 ulNum = 0, ulDomainNum = 0;
    WSEC_UINT32 ulDomainId;
    WSEC_UINT32 i, j;
    WSEC_PROGRESS_RPT_STRU* pstRptProgress = WSEC_NULL_PTR;
    WSEC_ERR_T nRet = WSEC_SUCCESS;
    WSEC_BOOL bReadOk = WSEC_FALSE;
    WSEC_BOOL bCancel = WSEC_FALSE;

    WSEC_TRACE("KMC_PRI_CfgDataInit() begin:");
    WSEC_ASSERT(pCallbackFun && pKmcCfg);
    WSEC_ASSERT(WSEC_ARR_IsEmpty(pKmcCfg->arrDomainCfg)); /* 确保Domain数组为空 */
    WSEC_ASSERT(WSEC_NUM_OF(eAlgType) == WSEC_NUM_OF(pKmcCfg->astDataProtectCfg));

    if (g_pKmcIniCtx) {pstRptProgress = &g_pKmcIniCtx->stProgressRpt;}

    /* 1. RootKey配置 */
    if (KMC_IS_INNER_CREATE_RK)
    {
        pstRkCfg = &pKmcCfg->stRkCfg;
        bReadOk = pCallbackFun->pfReadRootKeyCfg(pstRkCfg);
        if (bReadOk) {bReadOk = KMC_CFG_IS_ROOT_KEY_VALID(pstRkCfg);} /* 需检查数据合法性 */
        if (!bReadOk)
        {
            WSEC_LOG_E("Callback to get RootKey CFG fail. so default CFG used.");
            KMC_PRI_MakeDefaultCfg4RootKey(pstRkCfg);
        }
    }

    /* 2. 密钥生命周期管理参数 */
    pstKmCfg = &pKmcCfg->stKmCfg;
    bReadOk = pCallbackFun->pfReadKeyManCfg(pstKmCfg);
    if (bReadOk) {bReadOk = KMC_CFG_IS_KEY_MAN_VALID(pstKmCfg);}/* 需检查数据合法性 */
    if (!bReadOk)
    {
        WSEC_LOG_E("Callback to get Key-Management CFG fail. so default CFG used.");
        KMC_PRI_MakeDefaultCfg4KeyMan(pstKmCfg);
    }
    
    /* 3. DataProtection配置 */
    pDataProtectCfg = pKmcCfg->astDataProtectCfg;
    for (i = 0; i < WSEC_NUM_OF(eAlgType); i++, pDataProtectCfg++)
    {
        bReadOk = pCallbackFun->pfReadCfgOfDataProtection(eAlgType[i], pDataProtectCfg);
        if (bReadOk) /* 检查数据合法性 */
        {
            bReadOk = (KMC_PRI_ChkProtectCfg(eAlgType[i], pDataProtectCfg) == WSEC_SUCCESS);
        }
        if_oper(!bReadOk, KMC_PRI_MakeDefaultCfg4DataProtect(eAlgType[i], pDataProtectCfg));
    }

    /* 4. Domain配置 */
    if (!pCallbackFun->pfReadCfgOfDomainCount(&ulNum)) {ulNum = 0;}
    if (0 == ulNum) /* 使用缺省Domain及其KeyType配置 */
    {
        WSEC_LOG_E("Callback to get DomainNum fail. so default config used.");
        return_oper_if(!KMC_PRI_MakeDefaultCfg4Domain(pKmcCfg), WSEC_LOG_E("Make DefaultCfg4Domain fail."), WSEC_ERR_KMC_CALLBACK_KMCCFG_FAIL);
        return WSEC_SUCCESS;
    }

    return_oper_if(ulNum > WSEC_DOMAIN_NUM_MAX, 
                   WSEC_LOG_E2("Callbacked invalid DomainNum(%u), it cannot over %d.", ulNum, WSEC_DOMAIN_NUM_MAX), 
                   WSEC_ERR_KMC_DOMAIN_NUM_OVERFLOW);

    WSEC_BUFF_ALLOC(stBuff, (WSEC_SIZE_T)(sizeof(KMC_CFG_DOMAIN_INFO_STRU) * ulNum));
    return_oper_if(!stBuff.pBuff, WSEC_LOG_E4MALLOC(stBuff.nLen), WSEC_ERR_MALLOC_FAIL);

    do
    {
        pstDomainInfo = (KMC_CFG_DOMAIN_INFO_STRU*)stBuff.pBuff;
        break_oper_if(!pCallbackFun->pfReadCfgOfDomainInfo(pstDomainInfo, ulNum), WSEC_LOG_E("Callback to get all DomainInfo fail."), nRet = WSEC_ERR_KMC_CALLBACK_KMCCFG_FAIL);

        /* 将Domain加入数组 */
        for (i = 0; i < ulNum; i++, pstDomainInfo++)
        {
            /* 检查配置数据合法性 */
            break_oper_if(!KMC_IS_VALID_MK_FROM(pstDomainInfo->ucKeyFrom),
                          WSEC_LOG_E1("Callback to got invalid domain-cfg.keyfrom(%d).", pstDomainInfo->ucKeyFrom),
                          nRet = WSEC_ERR_KMC_KMCCFG_INVALID);
            nRet = KMC_PRI_AddDomain2Array(pKmcCfg, pstDomainInfo);
            break_oper_if(nRet != WSEC_SUCCESS, WSEC_LOG_E1("KMC_PRI_AddDomain2Array()=%u", nRet), oper_null);

            pstTimer = (i != (ulNum - 1)) ? &stTimer : WSEC_NULL_PTR;
            WSEC_RptProgress(pstRptProgress, pstTimer, ulNum, i + 1, &bCancel);
            break_oper_if(bCancel, WSEC_LOG_E("App Canceled."), nRet = WSEC_ERR_CANCEL_BY_APP);
        }
        break_oper_if(nRet != WSEC_SUCCESS, oper_null, oper_null);

        WSEC_ASSERT(WSEC_ARR_GetCount(pKmcCfg->arrDomainCfg) == (WSEC_INT32)ulNum); /* 验证数组操作无BUG */
    }do_end;
    WSEC_BUFF_FREE(stBuff); /* 释放用于获取Domain配置的缓冲区 */

    /* 如果上述获取Domain配置失败, 则释放数组资源 */
    if (nRet != WSEC_SUCCESS)
    {
        WSEC_ARR_RemoveAll(pKmcCfg->arrDomainCfg);
        return nRet;
    }

    /* 5. Domain KeyType */
    ulDomainNum = WSEC_ARR_GetCount(pKmcCfg->arrDomainCfg);
    for (i = 0; (WSEC_INT32)i < ulDomainNum; i++)
    {
        pstDomainCfg = (KMC_DOMAIN_CFG_STRU*)WSEC_ARR_GetAt(pKmcCfg->arrDomainCfg, i);
        break_oper_if(!pstDomainCfg, WSEC_LOG_E("memory access fail."), nRet = WSEC_ERR_OPER_ARRAY_FAIL);
        ulDomainId = pstDomainCfg->stDomainInfo.ulId;
        
        /* 1) 获取该Domain下KeyType个数 */
        break_oper_if(!pCallbackFun->pfReadCfgOfDomainKeyTypeCount(ulDomainId, &ulNum), \
                       WSEC_LOG_E1("Callback to get DomainKeyTypeCount(DomainId = %u) fail.", ulDomainId), \
                       nRet = WSEC_ERR_KMC_CALLBACK_KMCCFG_FAIL);
        break_oper_if(ulNum > WSEC_DOMAIN_KEY_TYPE_NUM_MAX, \
                      WSEC_LOG_E3("Callback Domain key-type-num(%u) invalid(max-num = %d) for DomainId = %u", ulNum, WSEC_DOMAIN_KEY_TYPE_NUM_MAX, ulDomainId), \
                      nRet = WSEC_ERR_KMC_KEYTYPE_NUM_OVERFLOW);
        continue_if(0 == ulNum);

        /* 2) 分配用于获取该Domain下所有KeyType的缓冲区 */
        WSEC_BUFF_ALLOC(stBuff, sizeof(KMC_CFG_KEY_TYPE_STRU) * ulNum);
        break_oper_if(!stBuff.pBuff, WSEC_LOG_E4MALLOC(stBuff.nLen), nRet = WSEC_ERR_MALLOC_FAIL);

        /* 3) 读取该Domain下所有KeyType */
        pstKeyType4Read = (KMC_CFG_KEY_TYPE_STRU*)stBuff.pBuff;
        break_oper_if(!pCallbackFun->pfReadCfgOfDomainKeyType(ulDomainId, pstKeyType4Read, ulNum), \
                      WSEC_LOG_E1("Callback to read DomainKeyType(for domain-id = %u) fail.", ulDomainId), \
                      nRet = WSEC_ERR_KMC_CALLBACK_KMCCFG_FAIL);

        /* 4) 将KeyType加入数组 */
        for (j = 0; j < ulNum; j++, pstKeyType4Read++)
        {
            break_oper_if(!KMC_IS_VALID_KEY_TYPE(pstKeyType4Read->usKeyType),
                          WSEC_LOG_E1("Callback to read invalid KeyType(usKeyType=%u)", pstKeyType4Read->usKeyType), 
                          nRet = WSEC_ERR_KMC_KMCCFG_INVALID);
            break_oper_if(!KMC_IS_VALID_KEY_TYPE_LEN(pstKeyType4Read->ulKeyLen), 
                          WSEC_LOG_E2("Callback to read invalid KeyType Len(%u), it must between[1, %u]", pstKeyType4Read->ulKeyLen, g_KmcSys.ulMkPlainLenMax),
                          nRet = WSEC_ERR_KMC_KMCCFG_INVALID);
            break_oper_if(pstKeyType4Read->ulKeyLifeDays < 1, 
                          WSEC_LOG_E("Callback to read invalid KeyType(ulKeyLifeDays must bigger than 0)"),
                          nRet = WSEC_ERR_KMC_KMCCFG_INVALID);

            nRet = KMC_PRI_AddDomainKeyType2Array(pKmcCfg, ulDomainId, pstKeyType4Read);
            break_oper_if(nRet != WSEC_SUCCESS, WSEC_LOG_E1("KMC_PRI_AddDomainKeyType2Array()=%u", nRet), oper_null);
        }
        break_oper_if(nRet != WSEC_SUCCESS, oper_null, oper_null);
        WSEC_BUFF_FREE(stBuff);

        WSEC_ASSERT((WSEC_INT32)ulNum == WSEC_ARR_GetCount(pstDomainCfg->arrKeyTypeCfg)); /* 验证数组操作无BUG */

        pstTimer = (i != (ulDomainNum - 1)) ? &stTimer : WSEC_NULL_PTR;
        WSEC_RptProgress(pstRptProgress, pstTimer, ulDomainNum, i + 1, &bCancel);
        break_oper_if(bCancel, WSEC_LOG_E("APP Canceled."), nRet = WSEC_ERR_CANCEL_BY_APP);
    }
    WSEC_BUFF_FREE(stBuff);

    if (WSEC_SUCCESS == nRet) 
    {
        if (WSEC_ARR_IsEmpty(pKmcCfg->arrDomainCfg)) /* 没有Domain配置, 则给出缺省值 */
        {
            nRet = KMC_PRI_MakeDefaultCfg4Domain(pKmcCfg) ? WSEC_SUCCESS : WSEC_ERR_KMC_CALLBACK_KMCCFG_FAIL;
        }
    }
    else /* 上述操作失败, 则需要释放所有链表资源 */
    {
        WSEC_ARR_RemoveAll(pKmcCfg->arrDomainCfg);
    }

    WSEC_TRACE("KMC_PRI_CfgDataInit() End.");
    return nRet;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_AddDomain2Array
 功能描述  : 向KMC配置信息中添加Domain信息
 纯 入 参  : pDomainInfo: Domain配置信息
 纯 出 参  : 无
 入参出参  : pKmcCfg: KMC配置数据
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 增加Domain元素时, 确保按DomainId升序排列

 修改历史
  1.日    期   : 2014年10月24日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_AddDomain2Array(KMC_CFG_STRU* pKmcCfg, const KMC_CFG_DOMAIN_INFO_STRU* pDomainInfo)
{
    KMC_DOMAIN_CFG_STRU* pElement = WSEC_NULL_PTR;
    WSEC_INT32 nNum;
    WSEC_ERR_T nRetCode = WSEC_SUCCESS;

    WSEC_ASSERT(pKmcCfg && pDomainInfo);

    if (pKmcCfg->arrDomainCfg)
    {
        nNum = WSEC_ARR_GetCount(pKmcCfg->arrDomainCfg);
        return_oper_if(WSEC_DOMAIN_NUM_MAX == nNum,
                       WSEC_LOG_E2("DomainNum(%d) cannot over %u", nNum, WSEC_ERR_KMC_DOMAIN_NUM_OVERFLOW), 
                       WSEC_ERR_KMC_DOMAIN_NUM_OVERFLOW);
    }
    else
    {
        pKmcCfg->arrDomainCfg = WSEC_ARR_Initialize(0, 0, KMC_PRI_CompareDomain4Arr, KMC_PRI_OnRemoveDomainArr);
        return_oper_if(!pKmcCfg->arrDomainCfg, WSEC_LOG_E("WSEC_ARR_Initialize() fail."), WSEC_ERR_OPER_ARRAY_FAIL);
    }

    /* 构造待加入数组的元素 */
    pElement = (KMC_DOMAIN_CFG_STRU*)WSEC_MALLOC(sizeof(KMC_DOMAIN_CFG_STRU));
    return_oper_if(!pElement, WSEC_LOG_E4MALLOC(sizeof(KMC_DOMAIN_CFG_STRU)), WSEC_ERR_MALLOC_FAIL);

    do
    {
        pElement->arrKeyTypeCfg = WSEC_ARR_Initialize(0, 0, KMC_PRI_CompareDomainKeyType4Arr, WSEC_ARR_StdRemoveElement);
        break_oper_if(!pElement->arrKeyTypeCfg, WSEC_LOG_E("WSEC_ARR_Initialize() fail."), nRetCode = WSEC_ERR_OPER_ARRAY_FAIL);
        break_oper_if(WSEC_MEMCPY(&pElement->stDomainInfo, sizeof(pElement->stDomainInfo), pDomainInfo, sizeof(KMC_CFG_DOMAIN_INFO_STRU)) != EOK, 
                      WSEC_LOG_E4MEMCPY, nRetCode = WSEC_ERR_MEMCPY_FAIL);

        /* 检查唯一性 */
        break_oper_if(WSEC_ARR_BinarySearch(pKmcCfg->arrDomainCfg, pElement), 
                      WSEC_LOG_E1("The Domain(ID=%u) already existed.", pDomainInfo->ulId), nRetCode = WSEC_ERR_KMC_ADD_REPEAT_DOMAIN);

        /* 放入数组 */
        break_oper_if(WSEC_ARR_AddOrderly(pKmcCfg->arrDomainCfg, pElement) < 0, 
                      WSEC_LOG_E("WSEC_ARR_AddOrderly() fail."), nRetCode = WSEC_ERR_OPER_ARRAY_FAIL);
    }do_end;

    if (nRetCode != WSEC_SUCCESS)
    {
        WSEC_ARR_Finalize(pElement->arrKeyTypeCfg);
        WSEC_FREE(pElement);
    }

    /* Misinformation: FORTIFY.Memory_Leak ('pElement' added to 'pKmcCfg->arrDomainCfg' when success) */

    return nRetCode;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_AddDomainKeyType2Array
 功能描述  : 向Domain配置中添加KeyType配置
 纯 入 参  : ulDomainId: KeyType隶属的Domain;
             pKeyType:   KeyType配置
 纯 出 参  : 无
 入参出参  : pKmcCfg: KMC配置数据
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月24日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_AddDomainKeyType2Array(KMC_CFG_STRU* pKmcCfg, WSEC_UINT32 ulDomainId, const KMC_CFG_KEY_TYPE_STRU* pKeyType)
{
    KMC_DOMAIN_CFG_STRU* pDomainCfg = WSEC_NULL_PTR;
    WSEC_ARRAY arrKeyType = WSEC_NULL_PTR;
    KMC_CFG_KEY_TYPE_STRU* pElement = WSEC_NULL_PTR;
    WSEC_INT32 iNum;

    /* 1. 查找Domain是否存在 */
    pDomainCfg = KMC_PRI_SearchDomain(pKmcCfg, ulDomainId);
    return_oper_if(!pDomainCfg, WSEC_LOG_E1("The Domain(ID=%u) not exist", ulDomainId), WSEC_ERR_KMC_DOMAIN_MISS);

    arrKeyType = pDomainCfg->arrKeyTypeCfg;
    WSEC_ASSERT(arrKeyType);

    /* 2. 每个Domain的KeyType数量不能超限 */
    iNum = WSEC_ARR_GetCount(arrKeyType);
    return_oper_if(WSEC_DOMAIN_KEY_TYPE_NUM_MAX == iNum,
                   WSEC_LOG_E2("Each Domain's KeyType num(%d) cannot over %u", iNum, WSEC_DOMAIN_KEY_TYPE_NUM_MAX), 
                   WSEC_ERR_KMC_KEYTYPE_NUM_OVERFLOW);

    /* 3. 确保该Domain下KeyType唯一性 */
    return_oper_if(WSEC_ARR_BinarySearch(arrKeyType, pKeyType), 
                   WSEC_LOG_E2("The KeyType(DomainId=%u, KeyType=%u) already existed.", ulDomainId, pKeyType->usKeyType), 
                   WSEC_ERR_KMC_ADD_REPEAT_KEY_TYPE);
    
    /* 4. 放入到数组 */
    pElement = (KMC_CFG_KEY_TYPE_STRU*)WSEC_CLONE_BUFF(pKeyType, sizeof(KMC_CFG_KEY_TYPE_STRU));
    return_oper_if(!pElement, WSEC_LOG_E("WSEC_CLONE_BUFF() fail."), WSEC_ERR_MEMCLONE_FAIL);

    if (WSEC_ARR_AddOrderly(arrKeyType, pElement) < 0) /* 插入失败, 需释放克隆出的数据 */
    {
        WSEC_FREE(pElement);
        return WSEC_ERR_OPER_ARRAY_FAIL;
    }

    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_AddMk2Array
 功能描述  : 增加MK到数组
 纯 入 参  : pKeystore: 存储MK数组的Keystore数据结构
             pMk:       MK数据;
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : WSEC_SUCCESS: 成功
             其它:         具体错误原因.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月31日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_PRI_AddMk2Array(KMC_KSF_MEM_STRU* pKeystore, KMC_MEM_MK_STRU* pMk)
{
    WSEC_INT32 iNum;
    WSEC_ASSERT(pKeystore && pMk);

    if (pKeystore->arrMk)
    {
        iNum = WSEC_ARR_GetCount(pKeystore->arrMk);
        return_oper_if(WSEC_MK_NUM_MAX == iNum,
                       WSEC_LOG_E2("MkNum(%d) cannot over %u", iNum, WSEC_MK_NUM_MAX), 
                       WSEC_ERR_KMC_MK_NUM_OVERFLOW);
    }
    else
    {
        pKeystore->arrMk = WSEC_ARR_Initialize(0, 0, KMC_PRI_CompareMk4Arr, KMC_PRI_OnRemoveMkArr);
        return_oper_if(!pKeystore->arrMk, WSEC_LOG_E("WSEC_ARR_Initialize() fail"), WSEC_ERR_OPER_ARRAY_FAIL);
    }

    return_oper_if(KMC_PRI_SearchMkByKeyId(pKeystore, pMk->stMkInfo.ulDomainId, pMk->stMkInfo.ulKeyId) >= 0, 
                   WSEC_LOG_E2("The MK(Domain=%u, KeyId=%u) exist.", pMk->stMkInfo.ulDomainId, pMk->stMkInfo.ulKeyId), 
                   WSEC_ERR_KMC_ADD_REPEAT_MK);

    /* 放数组前, 将明文加掩 */
    KMC_MASK_MK(pMk);
    return_oper_if(WSEC_ARR_AddOrderly(pKeystore->arrMk, pMk) < 0,
                   WSEC_LOG_E("WSEC_ARR_AddOrderly() fail."), WSEC_ERR_OPER_ARRAY_FAIL);

    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CompareDomain4Arr
 功能描述  : 用于数组快速排序/查找时比较两个元素大小的回调函数.
 纯 入 参  : p1: 存放Domain1数据地址的指针;
             p2: 存放Domain2数据地址的指针
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 1:  参数1 大于 参数2
             0:  参数1 等于 参数2
             -1: 参数1 小于 参数2
 特别注意  : 

 修改历史
  1.日    期   : 2014年10月31日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_INT32 KMC_PRI_CompareDomain4Arr(const WSEC_VOID* p1, const WSEC_VOID* p2)
{
    const KMC_DOMAIN_CFG_STRU *pDomain1, *pDomain2;
    
    WSEC_ASSERT(p1 && p2);

    pDomain1 = (const KMC_DOMAIN_CFG_STRU*)(*(const WSEC_VOID**)p1);
    pDomain2 = (const KMC_DOMAIN_CFG_STRU*)(*(const WSEC_VOID**)p2);

    if (pDomain1->stDomainInfo.ulId > pDomain2->stDomainInfo.ulId)
    {
        return WSEC_CMP_RST_BIG_THAN;
    }

    if (pDomain1->stDomainInfo.ulId < pDomain2->stDomainInfo.ulId)
    {
        return WSEC_CMP_RST_SMALL_THAN;
    }

    return WSEC_CMP_RST_EQUAL;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CompareDomainKeyType4Arr
 功能描述  : 用于数组快速排序/查找时比较两个元素大小的回调函数.
 纯 入 参  : p1: 存放KeyType1数据地址的指针;
             p2: 存放KeyType2数据地址的指针
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 1:  参数1 大于 参数2
             0:  参数1 等于 参数2
             -1: 参数1 小于 参数2
 特别注意  : 

 修改历史
  1.日    期   : 2014年11月01日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_INT32 KMC_PRI_CompareDomainKeyType4Arr(const WSEC_VOID* p1, const WSEC_VOID* p2)
{
    const KMC_CFG_KEY_TYPE_STRU *pKeyType1, *pKeyType2;

    WSEC_ASSERT(p1 && p2);

    pKeyType1 = (const KMC_CFG_KEY_TYPE_STRU*)(*(const WSEC_VOID**)p1);
    pKeyType2 = (const KMC_CFG_KEY_TYPE_STRU*)(*(const WSEC_VOID**)p2);

    if (pKeyType1->usKeyType > pKeyType2->usKeyType)
    {
        return WSEC_CMP_RST_BIG_THAN;
    }
    
    if (pKeyType1->usKeyType < pKeyType2->usKeyType)
    {
        return WSEC_CMP_RST_SMALL_THAN;
    }

    return WSEC_CMP_RST_EQUAL;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_CompareMk4Arr
 功能描述  : 用于数组快速排序/查找时比较两个元素大小的回调函数.
 纯 入 参  : p1: 存放MK1数据地址的指针;
             p2: 存放MK1数据地址的指针
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 1:  参数1 大于 参数2
             0:  参数1 等于 参数2
             -1: 参数1 小于 参数2
 特别注意  : 

 修改历史
  1.日    期   : 2014年11月01日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_INT32 KMC_PRI_CompareMk4Arr(const WSEC_VOID* p1, const WSEC_VOID* p2)
{
    const KMC_MEM_MK_STRU* pElement1;
    const KMC_MEM_MK_STRU* pElement2;
    const KMC_MK_INFO_STRU* pMk1;
    const KMC_MK_INFO_STRU* pMk2;
    
    WSEC_ASSERT(p1 && p2);
    pElement1 = (const KMC_MEM_MK_STRU*)(*(const WSEC_VOID**)p1);
    pElement2 = (const KMC_MEM_MK_STRU*)(*(const WSEC_VOID**)p2);

    pMk1 = &pElement1->stMkInfo;
    pMk2 = &pElement2->stMkInfo;
    
    if (pMk1->ulDomainId > pMk2->ulDomainId) {return WSEC_CMP_RST_BIG_THAN;}
    if (pMk1->ulDomainId < pMk2->ulDomainId) {return WSEC_CMP_RST_SMALL_THAN;}

    /* ulDomainId相同, 检查usType */
    if (pMk1->usType > pMk2->usType) {return WSEC_CMP_RST_BIG_THAN;}
    if (pMk1->usType < pMk2->usType) {return WSEC_CMP_RST_SMALL_THAN;}

    /* ulDomainId + usType均相同, 需要再检查ucStatus */
    if (pMk1->ucStatus > pMk2->ucStatus) {return WSEC_CMP_RST_BIG_THAN;}
    if (pMk1->ucStatus < pMk2->ucStatus) {return WSEC_CMP_RST_SMALL_THAN;}

    /* ulDomainId + usType + ucStatus均相同 */
    return WSEC_CMP_RST_EQUAL;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_SearchDomainKeyTypeCfg
 功能描述  : 按照DomainId,[usKeyType]搜索Domain, [KeyType]配置
 纯 入 参  : ulDomainId: 必填参数
             usKeyType:  选填参数
 纯 出 参  : ppDomain:   被查找到的Domain[可NULL];
             ppKeyType:  被查找到的KeyType[可NULL];
 入参出参  : 无
 返 回 值  : WSEC_TRUE:  按要求查找成功.
             WSEC_FALSE: 查找失败.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月04日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL KMC_PRI_SearchDomainKeyTypeCfg(WSEC_UINT32 ulDomainId, WSEC_UINT16 usKeyType, KMC_DOMAIN_CFG_STRU** ppDomain, KMC_CFG_KEY_TYPE_STRU** ppKeyType)
{
    KMC_DOMAIN_CFG_STRU stFindDomain = {0};
    KMC_CFG_KEY_TYPE_STRU   stFindKeyType = {0};
    KMC_DOMAIN_CFG_STRU* pstFoundDomain = WSEC_NULL_PTR;

    stFindDomain.stDomainInfo.ulId = ulDomainId;
    pstFoundDomain = (KMC_DOMAIN_CFG_STRU*)WSEC_ARR_BinarySearch(g_pKmcCfg->arrDomainCfg, &stFindDomain);

    if (!pstFoundDomain) {return WSEC_FALSE;}
    if (ppDomain) {*ppDomain = pstFoundDomain;}
    if (!ppKeyType) {return WSEC_TRUE;}

    /* 还需要查该Domain下的KeyType */
    stFindKeyType.usKeyType = usKeyType;
    *ppKeyType = (KMC_CFG_KEY_TYPE_STRU*)WSEC_ARR_BinarySearch(pstFoundDomain->arrKeyTypeCfg, &stFindKeyType);

    return (*ppKeyType != WSEC_NULL_PTR);
}

/*****************************************************************************
 函 数 名  : KMC_PRI_SearchDomain
 功能描述  : 从KMC配置信息中搜索Domain
 纯 入 参  : pKmcCfg:    KMC配置数据
             ulDomainId: 被搜索的DomainId
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 指向Domain配置的指针
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月31日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
KMC_DOMAIN_CFG_STRU* KMC_PRI_SearchDomain(const KMC_CFG_STRU* pKmcCfg, WSEC_UINT32 ulDomainId)
{
    KMC_DOMAIN_CFG_STRU stDomain;

    stDomain.stDomainInfo.ulId = ulDomainId;
    return (KMC_DOMAIN_CFG_STRU*)WSEC_ARR_BinarySearch(pKmcCfg->arrDomainCfg, &stDomain);
}

/*****************************************************************************
 函 数 名  : KMC_PRI_SearchMkByKeyId
 功能描述  : 查找MK
 纯 入 参  : pKeystore: 存储Keystore的数据结构
             ulDomainId, ulKeyId: 唯一标识MK
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 非负数: 被查找到的MK在数组中的下标
             -1:     查找失败.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月04日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_INT32 KMC_PRI_SearchMkByKeyId(const KMC_KSF_MEM_STRU* pKeystore, WSEC_UINT32 ulDomainId, WSEC_UINT32 ulKeyId)
{
    WSEC_INT32 i;
    KMC_MEM_MK_STRU* pMk = WSEC_NULL_PTR;
    
    for (i = 0; i < WSEC_ARR_GetCount(pKeystore->arrMk); i++)
    {
        pMk = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(pKeystore->arrMk, i);
        continue_if(!pMk);
        if ((pMk->stMkInfo.ulDomainId == ulDomainId) && (pMk->stMkInfo.ulKeyId == ulKeyId))
        {
            return i;
        }
    }

    return -1;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_OnRemoveDomainArr
 功能描述  : 删除Domain数组元素时, 处理存储在该元素上的数据.
             数组元素存放Domain数据的地址, 需要将该Domain释放.
             释放Domain前, 需要释放其KeyType配置.
 纯 入 参  : pElement: 数组元素所在地址
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 
 特别注意  : 

 修改历史
  1.日    期   : 2014年11月01日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_OnRemoveDomainArr(WSEC_VOID *pElement)
{
    KMC_DOMAIN_CFG_STRU* pData;

    pData = (KMC_DOMAIN_CFG_STRU*)pElement;
    if (pData)
    {
        WSEC_ARR_Finalize(pData->arrKeyTypeCfg);
        WSEC_FREE(pData);
    }

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_OnRemoveMkArr
 功能描述  : 删除MK数组元素时, 处理存储在该元素上的数据.
             数组元素存放MK数据的地址, 需要将该MK释放.
 纯 入 参  : pElement: 数组元素所在地址
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 
 特别注意  : 

 修改历史
  1.日    期   : 2014年11月01日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_OnRemoveMkArr(WSEC_VOID *pElement)
{
    WSEC_FREE_S(pElement, sizeof(KMC_MEM_MK_STRU)); /* 有MK明文, 需安全地释放(释放前擦除内存) */

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_Lock
 功能描述  : 锁定KMC的临界资源
 纯 入 参  : eType: 临界资源类型
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 
 特别注意  : 

 修改历史
  1.日    期   : 2015年3月25日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_Lock(KMC_LOCK_TYPE_ENUM eType)
{
    if (eType & KMC_LOCK_KEYSTORE) {WSEC_Lock(LOCK4KEYSTORE);}
    if (eType & KMC_LOCK_CFG) {WSEC_Lock(LOCK4KMC_CFG);}

    return;
}

/*****************************************************************************
 函 数 名  : KMC_PRI_Unlock
 功能描述  : 解除锁定KMC的临界资源
 纯 入 参  : eType: 临界资源类型
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 
 特别注意  : 

 修改历史
  1.日    期   : 2015年3月25日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_PRI_Unlock(KMC_LOCK_TYPE_ENUM eType)
{
    if (eType & KMC_LOCK_KEYSTORE) {WSEC_Unlock(LOCK4KEYSTORE);}
    if (eType & KMC_LOCK_CFG) {WSEC_Unlock(LOCK4KMC_CFG);}

    return;
}


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                          二、 对外接口函数
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*****************************************************************************
 函 数 名  : KMC_Initialize
 功能描述  : KMC模块初始化
 纯 入 参  : pstFileName: KMC所需文件名.
             pKmcCallbackFun:  CBB回调函数, 如果不注册, 则系统使用缺省值.
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(0为成功, 其余为错误码)
 特别注意  : 1. APP调用KMC的任何函数前，首先需要调用此函数完成KMC初始化.
             2. App调用此函数前, 应确保文件系统可工作.

 修改历史
  1.日    期   : 2014年10月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_Initialize(const KMC_FILE_NAME_STRU* pstFileName, const KMC_FP_CALLBACK_STRU* pKmcCallbackFun, const WSEC_PROGRESS_RPT_STRU* pstRptProgress)
{
    WSEC_ERR_T nRet = WSEC_SUCCESS;
    KMC_FP_CFG_CALLBACK_STRU stCallbackFun = {0};
    const KMC_FP_CFG_CALLBACK_STRU* pstCfgFun = WSEC_NULL_PTR;
    const KMC_FP_CFG_CALLBACK_STRU* pReadCfgCallback = WSEC_NULL_PTR;
    WSEC_BOOL bCbbManCfg = WSEC_FALSE;

    return_oper_if(WSEC_RUNNING == g_KmcSys.eState, WSEC_LOG_E("KMC is running, cannot be Initialize before finalize."), WSEC_ERR_KMC_INI_MUL_CALL);
    return_err_if_para_invalid("KMC_Initialize", pstFileName);

    WSEC_LOG_E("KMC Initialize begin.");

    if (pKmcCallbackFun)
    {
        pReadCfgCallback = &pKmcCallbackFun->stReadCfg;
    }

    g_KmcSys.eState       = WSEC_INIT_FAIL; /* 初始化流程没有走完而返回, 均属于初始化失败 */
    g_KmcSys.eRootKeyFrom = KMC_RK_GEN_BY_INNER; /* 暂时仅支持CBB内部管理RK */

    WSEC_ASSERT(KMC_KSF_NUM >= 2); /* 至少支持2个互为主备的Keystore文件. */

    /* 1 入参检查 */
    return_oper_if((!pstFileName->pszKeyStoreFile[0]) || (!pstFileName->pszKeyStoreFile[1]),
                    WSEC_LOG_E1("The %d KSF name must be all-provided.", WSEC_NUM_OF(pstFileName->pszKeyStoreFile)), WSEC_ERR_INVALID_ARG);
    return_oper_if(WSEC_PTR_XOR(pstFileName->pszKmcCfgFile[0], pstFileName->pszKmcCfgFile[1]), /* 配置文件名要么全部提供, 要么全不提供 */
                   WSEC_LOG_E1("The %d KMC-CFG-File must be all-provided or all-missed.", WSEC_NUM_OF(pstFileName->pszKmcCfgFile)),
                   WSEC_ERR_INVALID_ARG);

    if (pstFileName->pszKmcCfgFile[0] && pstFileName->pszKmcCfgFile[1]) /* 由CBB管理配置数据, 则APP不应该注册读取配置的回调函数 */
    {
        if (pReadCfgCallback)
        {
            return_oper_if(WSEC_GetZeroItemCount(pReadCfgCallback, sizeof(KMC_FP_CFG_CALLBACK_STRU), sizeof(pReadCfgCallback->pfReadKeyManCfg))
                           != (sizeof(KMC_FP_CFG_CALLBACK_STRU) / sizeof(pReadCfgCallback->pfReadKeyManCfg)),
                           WSEC_LOG_E("App should not provid para of 'KMC_FP_CALLBACK_STRU' because of KMC-CFG-File provided."), WSEC_ERR_INVALID_ARG);
        }

        stCallbackFun.pfReadRootKeyCfg              = KMC_PRI_ReadRootKeyCfg;
        stCallbackFun.pfReadKeyManCfg               = KMC_PRI_ReadKeyManCfg;
        stCallbackFun.pfReadCfgOfDomainCount        = KMC_PRI_ReadCfgOfDomainCount;
        stCallbackFun.pfReadCfgOfDomainInfo         = KMC_PRI_ReadCfgOfDomainInfo;
        stCallbackFun.pfReadCfgOfDomainKeyTypeCount = KMC_PRI_ReadCfgOfDomainKeyTypeCount;
        stCallbackFun.pfReadCfgOfDomainKeyType      = KMC_PRI_ReadCfgOfDomainKeyType;
        stCallbackFun.pfReadCfgOfDataProtection     = KMC_PRI_ReadCfgOfDataProtection;
        pstCfgFun = &stCallbackFun;

        g_KmcSys.apszKmcCfgFile[0] = WSEC_CLONE_STR(pstFileName->pszKmcCfgFile[0]);
        g_KmcSys.apszKmcCfgFile[1] = WSEC_CLONE_STR(pstFileName->pszKmcCfgFile[1]);
        bCbbManCfg = WSEC_TRUE;
    }
    else /* 需要APP完整地提供回调函数读取配置数据 */
    {
        return_oper_if(!pReadCfgCallback, WSEC_LOG_E("App must provid para of 'KMC_FP_CALLBACK_STRU'."), WSEC_ERR_INVALID_ARG);
        return_oper_if(WSEC_GetZeroItemCount(pReadCfgCallback, sizeof(KMC_FP_CFG_CALLBACK_STRU), sizeof(pReadCfgCallback->pfReadKeyManCfg)) != 0, 
                       WSEC_LOG_E("App must provid all callback function of 'KMC_FP_CALLBACK_STRU'."), WSEC_ERR_INVALID_ARG);
        pstCfgFun = pReadCfgCallback;
    }

    g_KmcSys.apszKeystoreFile[0] = WSEC_CLONE_STR(pstFileName->pszKeyStoreFile[0]);
    g_KmcSys.apszKeystoreFile[1] = WSEC_CLONE_STR(pstFileName->pszKeyStoreFile[1]);
    
    WSEC_UNCARE(KMC_PRI_CalcMkPlainLenMax(&g_KmcSys.ulMkPlainLenMax));

    KMC_PRI_Lock(KMC_LOCK_BOTH);
    nRet = KMC_PRI_LoadData(&g_pKmcCfg, &g_pKeystore, pstRptProgress, pstCfgFun, bCbbManCfg);
    if (nRet != WSEC_SUCCESS) /* 上述操作不成功, 则释放资源 */
    {
        KMC_PRI_FreeGlobalMem(&g_pKmcCfg, &g_pKeystore);
        WSEC_LOG_E1("KMC_Initialize() fail. ErrCode = %u", nRet);
    }
    KMC_PRI_Unlock(KMC_LOCK_BOTH);

    if (nRet != WSEC_SUCCESS) {return nRet;}

    WSEC_ASSERT(g_pKeystore->pszFromFile);

    /* 将参数备份以备Reset用 */
    if (pReadCfgCallback)
    {
        if (WSEC_MEMCPY(&g_KmcSys.stBackupPara.stCallbackFun, sizeof(g_KmcSys.stBackupPara.stCallbackFun), pKmcCallbackFun, sizeof(KMC_FP_CALLBACK_STRU)) != EOK)
        {
            if (WSEC_MEMSET(&g_KmcSys.stBackupPara.stCallbackFun, sizeof(g_KmcSys.stBackupPara.stCallbackFun), 0, sizeof(KMC_FP_CALLBACK_STRU)) != EOK)
            {
                WSEC_LOG_E("memory oper fail.");
            }
        }
    }

    if (pstRptProgress)
    {
        if (WSEC_MEMCPY(&g_KmcSys.stBackupPara.stProgressRpt, sizeof(g_KmcSys.stBackupPara.stProgressRpt), pstRptProgress, sizeof(WSEC_PROGRESS_RPT_STRU)) != EOK)
        {
            if (WSEC_MEMSET(&g_KmcSys.stBackupPara.stProgressRpt, sizeof(g_KmcSys.stBackupPara.stProgressRpt), 0, sizeof(WSEC_PROGRESS_RPT_STRU)) != EOK)
            {
                WSEC_LOG_E("memory oper fail.");
            }
        }
    }

    g_KmcSys.eState = WSEC_RUNNING;
    WSEC_LOG_E("KMC Initialized successful.");
    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : KMC_Finalize
 功能描述  : KMC去初始化
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : WSEC_SUCCESS: 成功
             其它:         具体错误原因.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月03日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_Finalize()
{
    WSEC_ERR_T nErrCode;

    nErrCode = KMC_PRI_Finalize(KMC_NEED_LOCK);
    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_Reset
 功能描述  : KMC模块复位
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : WSEC_SUCCESS: 成功
             其它:         具体错误原因.
 特别注意  : 无

 修改历史
  1.日    期   : 2015年3月03日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_Reset()
{
    KMC_FILE_NAME_STRU stFileName = {0};
    KMC_FP_CALLBACK_STRU stKmcCallbackFun = {0};
    WSEC_PROGRESS_RPT_STRU stRptProgress = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    return_err_if_kmc_not_work;

    do
    {
        /* 1. 构造初始化参数 */
        stFileName.pszKeyStoreFile[0] = WSEC_CLONE_STR(g_KmcSys.apszKeystoreFile[0]);
        stFileName.pszKeyStoreFile[1] = WSEC_CLONE_STR(g_KmcSys.apszKeystoreFile[1]);
        stFileName.pszKmcCfgFile[0] = WSEC_CLONE_STR(g_KmcSys.apszKmcCfgFile[0]);
        stFileName.pszKmcCfgFile[1] = WSEC_CLONE_STR(g_KmcSys.apszKmcCfgFile[1]);
        break_oper_if(WSEC_MEMCPY(&stKmcCallbackFun, sizeof(stKmcCallbackFun), &g_KmcSys.stBackupPara.stCallbackFun, sizeof(g_KmcSys.stBackupPara.stCallbackFun)) != EOK, 
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
        break_oper_if(WSEC_MEMCPY(&stRptProgress, sizeof(stRptProgress), &g_KmcSys.stBackupPara.stProgressRpt, sizeof(g_KmcSys.stBackupPara.stProgressRpt)) != EOK, 
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);

        /* 2. 去初始化 */
        nErrCode = KMC_PRI_Finalize(KMC_NEED_LOCK);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC Finalize ret %u", nErrCode), oper_null);

        /* 3. 初始化 */
        nErrCode = KMC_Initialize(&stFileName, &stKmcCallbackFun, &stRptProgress);
    }do_end;

    WSEC_FREE(stFileName.pszKeyStoreFile[0]);
    WSEC_FREE(stFileName.pszKeyStoreFile[1]);
    WSEC_FREE(stFileName.pszKmcCfgFile[0]);
    WSEC_FREE(stFileName.pszKmcCfgFile[1]);

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_RmvMk
 功能描述  : 删除MasterKey
 纯 入 参  : ulDomainId, ulKeyId: MK的唯一身份.
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : WSEC_SUCCESS: 成功
             其它:         具体错误原因.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月04日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_RmvMk(WSEC_UINT32 ulDomainId, WSEC_UINT32 ulKeyId)
{
    WSEC_INT32 nAt = -1;
    KMC_DOMAIN_CFG_STRU* psDomain = WSEC_NULL_PTR;
    KMC_MEM_MK_STRU* pMk = WSEC_NULL_PTR;
    KMC_MK_INFO_STRU stRmvMk = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    return_err_if_kmc_not_work;
    return_err_if_domain_privacy(ulDomainId);

    KMC_PRI_Lock(KMC_LOCK_BOTH);
    do
    {
        /* 1. 搜索该MK */
        nAt = KMC_PRI_SearchMkByKeyId(g_pKeystore, ulDomainId, ulKeyId);
        break_oper_if(nAt < 0, WSEC_LOG_E2("MK(DomainId=%u, KeyId=%u) not found", ulDomainId, ulKeyId), nErrCode = WSEC_ERR_KMC_MK_MISS);
        pMk = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(g_pKeystore->arrMk, nAt);
        break_oper_if(!pMk, WSEC_LOG_E("memory access fail."), nErrCode = WSEC_ERR_OPER_ARRAY_FAIL);

        /* 2. 搜索对应的Domain */
        psDomain = KMC_PRI_SearchDomain(g_pKmcCfg, ulDomainId);
        if (psDomain) /* Domain配置尚存时禁止删除活动密钥 */
        {
            break_oper_if(KMC_KEY_STATUS_ACTIVE == pMk->stMkInfo.ucStatus, 
                           WSEC_LOG_E2("Cannot remove active MK(DomainId=%u, KeyId=%u)", ulDomainId, ulKeyId), nErrCode = WSEC_ERR_KMC_CANNOT_RMV_ACTIVE_MK);
        }

        break_oper_if(WSEC_MEMCPY(&stRmvMk, sizeof(stRmvMk), &pMk->stMkInfo, sizeof(pMk->stMkInfo)) != EOK,
                       WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);

        WSEC_ARR_RemoveAt(g_pKeystore->arrMk, nAt);
        nErrCode = KMC_PRI_WriteKsfSafety(g_pKeystore, WSEC_NULL_PTR);
        WSEC_LOG_W1("KMC_PRI_WriteKsfSafety()=%u", nErrCode);
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_BOTH);

    if (nErrCode != WSEC_SUCCESS) {return nErrCode;}

    /* 通知APP, 写日志 */
    KMC_PRI_NtfMkChanged(&stRmvMk, KMC_KEY_REMOVED);
    WSEC_LOG_W3("Del MasterKey(DomainId=%u, KeyId=%u, KeyType=%u)", stRmvMk.ulDomainId, stRmvMk.ulKeyId, stRmvMk.usType);

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_ExportMkFile
 功能描述  : 将Keystore文件导出到指定文件.
 纯 入 参  : pszToFile: 导出文件名
             pbPwd:     口令
             ulPwdLen:  口令长度
             ulKeyIterations: 密钥迭代次数, 如果为0, 则使用 KMC_KEY_DFT_ITERATIONS
             pstRptProgress:  上报进度指示
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : WSEC_SUCCESS: 成功
             其它:         具体错误原因.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月05日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_ExportMkFile(const WSEC_CHAR* pszToFile, const WSEC_BYTE* pbPwd, WSEC_UINT32 ulPwdLen, WSEC_UINT32 ulKeyIterations, const WSEC_PROGRESS_RPT_STRU* pstRptProgress)
{
    KMC_MKF_HDR_WITH_HMAC_STRU* pstHdrWithHmac = WSEC_NULL_PTR;
    KMC_MKF_HDR_STRU*    pstHdr = WSEC_NULL_PTR;
    KMC_MEM_MK_STRU*     pMemMk = WSEC_NULL_PTR;
    WSEC_BUFF            stEncrptKey     = {0}; /* MK加密密钥 */
    WSEC_BUFF            stHmacKey       = {0}; /* HMAC密钥 */
    WSEC_BUFF            stMkCiphertext  = {0}; /* 单条MK密文 */
    WSEC_BUFF            stHmac4MkCipher = {0}; /* MK密文的HMAC */
    KMC_MKF_MK_STRU*     pstMkWri = WSEC_NULL_PTR;
    WSEC_FILE            fWri = WSEC_NULL_PTR;
    WSEC_CRYPT_CTX       ctx  = WSEC_NULL_PTR;
    WSEC_SIZE_T nMkCipherLenMax = 0;
    WSEC_SIZE_T nHmacRstLen = 0;
    WSEC_INT32 i, nMkNum;
    WSEC_SPEND_TIME_STRU stTimer = {0};
    WSEC_BOOL bCancel = WSEC_FALSE;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    return_err_if_kmc_not_work;
    return_err_if_para_invalid("KMC_ExportMkFile", pszToFile && pbPwd && (ulPwdLen > 0));

    if (ulKeyIterations < 1) {ulKeyIterations = KMC_KEY_DFT_ITERATIONS;}
    
    pstHdrWithHmac = (KMC_MKF_HDR_WITH_HMAC_STRU*)WSEC_MALLOC(sizeof(KMC_MKF_HDR_WITH_HMAC_STRU)); /* 该数据比较大, 使用堆为宜 */
    return_oper_if(!pstHdrWithHmac, WSEC_LOG_E4MALLOC(sizeof(KMC_MKF_HDR_WITH_HMAC_STRU)), WSEC_ERR_MALLOC_FAIL);

    pstHdr = &pstHdrWithHmac->stHdr;
    /* 1. 加密、HMAC计算相关参数设置 */
    do
    {
        pstHdr->usVer = KMC_MKF_VER;
        pstHdr->usKsfVer = KMC_KSF_VER;
        pstHdr->ulEncryptAlgId = KMC_ENCRYPT_MK_ALGID;
        pstHdr->ulIteration4EncrypKey = ulKeyIterations;
        break_oper_if(CAC_Random(pstHdr->abSalt4EncrypKey, sizeof(pstHdr->abSalt4EncrypKey)) != WSEC_SUCCESS, WSEC_LOG_E("CAC_Random() fail."), nErrCode = WSEC_ERR_GET_RAND_FAIL);
        break_oper_if(CAC_Random(pstHdr->abIv4EncrypMk, sizeof(pstHdr->abIv4EncrypMk)) != WSEC_SUCCESS, WSEC_LOG_E("CAC_Random() fail."), nErrCode = WSEC_ERR_GET_RAND_FAIL);

        pstHdr->ulHmacAlgId = KMC_HMAC_MK_ALGID;
        pstHdr->ulIteration4HmacKey = ulKeyIterations;
        break_oper_if(CAC_Random(pstHdr->abSalt4HmacKey, sizeof(pstHdr->abSalt4HmacKey)) != WSEC_SUCCESS, WSEC_LOG_E("CAC_Random() fail."), nErrCode = WSEC_ERR_GET_RAND_FAIL);
    } do_end;
    if (nErrCode != WSEC_SUCCESS)
    {
        WSEC_FREE(pstHdrWithHmac);
        return nErrCode;
    }

    /* 2. 准备资源: 内存分配、打开文件 */
    WSEC_BUFF_ALLOC(stEncrptKey, KMC_EK4MKF_LEN);
    WSEC_BUFF_ALLOC(stHmacKey, KMC_KEY4HMAC_LEN);
    nMkCipherLenMax = sizeof(KMC_MKF_MK_STRU) * 2; /* 防止密文溢出, 不必抠门 */
    WSEC_BUFF_ALLOC(stMkCiphertext, nMkCipherLenMax);
    WSEC_BUFF_ALLOC(stHmac4MkCipher, KMC_HMAC_RST_LEN);
    pstMkWri = (KMC_MKF_MK_STRU*)WSEC_MALLOC(sizeof(KMC_MKF_MK_STRU));
    fWri = WSEC_FOPEN(pszToFile, "wb");

    KMC_PRI_Lock(KMC_LOCK_KEYSTORE);
    do /* 3. 构造数据, 并写文件 */
    {
        break_oper_if(!stEncrptKey.pBuff, WSEC_LOG_E4MALLOC(stEncrptKey.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!stHmacKey.pBuff, WSEC_LOG_E4MALLOC(stHmacKey.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!stMkCiphertext.pBuff, WSEC_LOG_E4MALLOC(stMkCiphertext.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!stHmac4MkCipher.pBuff, WSEC_LOG_E4MALLOC(stHmac4MkCipher.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!pstMkWri, WSEC_LOG_E4MALLOC(sizeof(KMC_MKF_MK_STRU)), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!fWri, WSEC_LOG_E1("Open file(%s) for write fail.", pszToFile), nErrCode = WSEC_ERR_OPEN_FILE_FAIL);
        
        /* 1) 构造MK加密密钥 */
        break_oper_if(CAC_Pbkdf2(WSEC_ALGID_PBKDF2_HMAC_SHA256, 
                                  pbPwd, ulPwdLen, 
                                  pstHdr->abSalt4EncrypKey, sizeof(pstHdr->abSalt4EncrypKey),
                                  pstHdr->ulIteration4EncrypKey,
                                  stEncrptKey.nLen, stEncrptKey.pBuff) != WSEC_SUCCESS,
                      WSEC_LOG_E("CAC_pbkdf2() fail."), nErrCode = WSEC_ERR_PBKDF2_FAIL);

        /* 2) 构造HMAC密钥 */
        break_oper_if(CAC_Pbkdf2(WSEC_ALGID_PBKDF2_HMAC_SHA256, 
                                  pbPwd, ulPwdLen, 
                                  pstHdr->abSalt4HmacKey, sizeof(pstHdr->abSalt4HmacKey),
                                  pstHdr->ulIteration4HmacKey,
                                  stHmacKey.nLen, stHmacKey.pBuff) != WSEC_SUCCESS,
                      WSEC_LOG_E("CAC_Pbkdf2() fail."), nErrCode = WSEC_ERR_PBKDF2_FAIL);

        /* 3) 构造MK数据, 加密并计算HMAC */
        break_oper_if(CAC_HmacInit(&ctx, pstHdr->ulHmacAlgId, stHmacKey.pBuff, stHmacKey.nLen) != WSEC_SUCCESS, WSEC_LOG_E("CAC_HmacInit() fail."), nErrCode = WSEC_ERR_HMAC_FAIL);
        nMkNum = WSEC_ARR_GetCount(g_pKeystore->arrMk);
        break_oper_if(nMkNum < 1, WSEC_LOG_E("No MK exist."), nErrCode = WSEC_ERR_KMC_MK_MISS);
        
        pstHdr->ulMkNum = nMkNum;
        for (i = 0; i < nMkNum; i++)
        {
            pMemMk = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(g_pKeystore->arrMk, i);
            break_oper_if(!pMemMk, WSEC_LOG_E("memory access fail."), nErrCode = WSEC_ERR_OPER_ARRAY_FAIL);
            break_oper_if(WSEC_MEMCPY(&pstMkWri->stMkInfo, sizeof(pstMkWri->stMkInfo), &pMemMk->stMkInfo, sizeof(pMemMk->stMkInfo)) != EOK,
                          WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
            pstMkWri->ulPlainLen = pMemMk->stMkRear.ulPlainLen;

            /* a. 将MK加掩的明文脱掩, 使之成为真明文 */
            KMC_UNMASK_MK_TO(pMemMk, pstMkWri->abPlainText, sizeof(pstMkWri->abPlainText));

            /* b. 将该MK加密 */
            stMkCiphertext.nLen = nMkCipherLenMax;
            WSEC_UNCARE(CAC_Random(pstMkWri->abIv, sizeof(pstMkWri->abIv)));   /* 尽量用随机数作IV */
            WSEC_UNCARE(CAC_Random(stMkCiphertext.pBuff, stMkCiphertext.nLen)); /* 密文富余空间尽量用随机数填充 */
            break_oper_if(CAC_Encrypt(pstHdr->ulEncryptAlgId,
                                      stEncrptKey.pBuff, stEncrptKey.nLen,
                                      pstHdr->abIv4EncrypMk, sizeof(pstHdr->abIv4EncrypMk),
                                      pstMkWri, sizeof(KMC_MKF_MK_STRU),
                                      stMkCiphertext.pBuff, &stMkCiphertext.nLen) != WSEC_SUCCESS,
                          WSEC_LOG_E("CAC_Encrypt() fail."), nErrCode = WSEC_ERR_ENCRPT_FAIL);

            if (0 == i) /* 对首个MK加密, 则获得了单条MK密文的长度, 可以写MKF文件头并对文件头做HMAC */
            {
                pstHdr->ulCipherLenPerMk = stMkCiphertext.nLen;

                break_oper_if(!WSEC_FWRITE_MUST(g_MkfFlag, sizeof(g_MkfFlag), fWri),
                              WSEC_LOG_E1("Write file(%s) fail.", pszToFile), nErrCode = WSEC_ERR_WRI_FILE_FAIL);
                nHmacRstLen = sizeof(pstHdrWithHmac->abHmac);
                break_oper_if(CAC_Hmac(pstHdr->ulHmacAlgId,
                                       stHmacKey.pBuff, stHmacKey.nLen, 
                                       &pstHdrWithHmac->stHdr, sizeof(pstHdrWithHmac->stHdr),
                                       pstHdrWithHmac->abHmac, &nHmacRstLen),
                              WSEC_LOG_E("CAC_Hmac() fail."), nErrCode = WSEC_ERR_HMAC_FAIL);
                WSEC_ASSERT(nHmacRstLen <= sizeof(pstHdrWithHmac->abHmac)); /* 若出错, 则需要调整KMC_MKF_HDR_STRU.abHmac[x]之x */
                KMC_PRI_CvtByteOrder4MkfHdr(&pstHdrWithHmac->stHdr, wbcHost2Network);
                break_oper_if(!WSEC_FWRITE_MUST(pstHdrWithHmac, sizeof(KMC_MKF_HDR_WITH_HMAC_STRU), fWri),
                              WSEC_LOG_E1("Write file(%s) fail.", pszToFile), nErrCode = WSEC_ERR_WRI_FILE_FAIL);
                KMC_PRI_CvtByteOrder4MkfHdr(&pstHdrWithHmac->stHdr, wbcNetwork2Host); /* 由于加密MK时需要使用头信息中的数据，所以需还原 */
            }
            else
            {
                WSEC_ASSERT(stMkCiphertext.nLen == pstHdr->ulCipherLenPerMk); /* MK明文等长, 密文也应等长, 否则是BUG */
            }

            /* c. 写MK密文 */
            break_oper_if(!WSEC_FWRITE_MUST(stMkCiphertext.pBuff, stMkCiphertext.nLen, fWri),
                          WSEC_LOG_E1("Write file(%s) fail.", pszToFile), nErrCode = WSEC_ERR_WRI_FILE_FAIL);

            /* d. 计算HMAC */
            break_oper_if(CAC_HmacUpdate(ctx, stMkCiphertext.pBuff, stMkCiphertext.nLen) != WSEC_SUCCESS,
                          WSEC_LOG_E("CAC_HmacUpdate() fail."), nErrCode = WSEC_ERR_HMAC_FAIL);

            WSEC_RptProgress(pstRptProgress, &stTimer, nMkNum, i + 1, &bCancel);
            break_oper_if(bCancel, WSEC_LOG_E("App Canceled"), nErrCode = WSEC_ERR_CANCEL_BY_APP);
        }
        if (nErrCode != WSEC_SUCCESS) {break;}
        if (!bCancel) {WSEC_RptProgress(pstRptProgress, WSEC_NULL_PTR, nMkNum, nMkNum, WSEC_NULL_PTR);} /* 确保进度100%上报 */

        nHmacRstLen = stHmac4MkCipher.nLen;
        break_oper_if(CAC_HmacFinal(&ctx, stHmac4MkCipher.pBuff, &nHmacRstLen) != WSEC_SUCCESS, 
                      WSEC_LOG_E("CAC_HmacFinal() fail."), nErrCode = WSEC_ERR_HMAC_FAIL);
        WSEC_ASSERT(nHmacRstLen <= stHmac4MkCipher.nLen);

        /* 5) 写HMAC相关信息 */
        break_oper_if(!WSEC_FWRITE_MUST(stHmac4MkCipher.pBuff, nHmacRstLen, fWri), WSEC_LOG_E1("Write file(%s) fail.", pszToFile), nErrCode = WSEC_ERR_WRI_FILE_FAIL);
    } do_end;
    KMC_PRI_Unlock(KMC_LOCK_KEYSTORE);

    WSEC_BUFF_FREE(stEncrptKey);
    WSEC_BUFF_FREE(stHmacKey);
    WSEC_BUFF_FREE(stMkCiphertext);
    WSEC_BUFF_FREE(stHmac4MkCipher);
    WSEC_FREE(pstHdrWithHmac);
    WSEC_FREE(pstMkWri);
    if (fWri) {WSEC_FCLOSE(fWri);}

    WSEC_LOG_W2("Export MK to '%s' finished, return code = %u.", pszToFile, nErrCode);
    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_ImportMkFile
 功能描述  : 从指定Keystore文件导入MK数据.
 纯 入 参  : pszFromFile: Keystore文件；
             pbPwd:       口令
             ulPwdLen:    口令长度
             pstRptProgress: 上报进度指示
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : WSEC_SUCCESS: 成功
             其它:         具体错误原因.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月05日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_ImportMkFile(const WSEC_CHAR* pszFromFile, const WSEC_BYTE* pbPwd, WSEC_UINT32 ulPwdLen, const WSEC_PROGRESS_RPT_STRU* pstRptProgress)
{
    KMC_KSF_MEM_STRU* pKeystore = WSEC_NULL_PTR;
    WSEC_BYTE   abFormatFlag[32];      /* MK文件格式标识 */
    KMC_MKF_HDR_WITH_HMAC_STRU* pstHdrWithHmac = WSEC_NULL_PTR;
    KMC_MKF_HDR_STRU* pstHdr   = WSEC_NULL_PTR;
    KMC_MKF_MK_STRU*  pMkPlain = WSEC_NULL_PTR;
    WSEC_CRYPT_CTX    ctx      = WSEC_NULL_PTR;
    WSEC_FILE fRead            = WSEC_NULL_PTR;
    WSEC_BUFF stDecrptKey = {0};
    WSEC_BUFF stHmacKey   = {0};
    WSEC_BUFF stCipher    = {0};
    WSEC_BUFF stHmacRst   = {0};
    WSEC_BUFF stPlainKey = {0};
    WSEC_BUFF stHmac4MkInFile = {0};
    WSEC_SIZE_T nPlainLen = 0;
    KMC_KSF_RK_STRU* pstRk = {0};
    KMC_CFG_DOMAIN_INFO_STRU stDomain = {0};
    KMC_DOMAIN_CFG_STRU* pstDomainCfg = WSEC_NULL_PTR;
    KMC_CFG_KEY_TYPE_STRU        stKeyType = {0};
    KMC_MEM_MK_STRU*         pstMemMk = WSEC_NULL_PTR;
    KMC_MEM_MK_STRU*         pstMk4Add = WSEC_NULL_PTR;
    WSEC_SPEND_TIME_STRU tSpend = {0};
    WSEC_BOOL bCancel = WSEC_FALSE;
    WSEC_INT32 i;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    WSEC_ASSERT(sizeof(abFormatFlag) == sizeof(g_MkfFlag));

    return_err_if_kmc_not_work;
    return_err_if_para_invalid("KMC_ImportMk", pszFromFile && pbPwd && (ulPwdLen > 0));

    do
    {
        pstRk = (KMC_KSF_RK_STRU*)WSEC_MALLOC(sizeof(KMC_KSF_RK_STRU));
        break_oper_if(!pstRk, WSEC_LOG_E4MALLOC(sizeof(KMC_KSF_RK_STRU)), nErrCode = WSEC_ERR_MALLOC_FAIL);

        nErrCode = KMC_PRI_ReadRootKey(g_pKeystore->pszFromFile, pstRk);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC_PRI_ReadRootKey()=%u", nErrCode), oper_null);

        /* 打开文件 */
        fRead = WSEC_FOPEN(pszFromFile, "rb");
        break_oper_if(!fRead, WSEC_LOG_E1("Open file(%s) for read fail.", pszFromFile), nErrCode = WSEC_ERR_OPEN_FILE_FAIL);
    }do_end;

    if (nErrCode != WSEC_SUCCESS)
    {
        WSEC_FREE(pstRk);
        WSEC_FCLOSE(fRead);
        return nErrCode;
    }

    WSEC_BUFF_ALLOC(stDecrptKey, KMC_EK4MKF_LEN);
    WSEC_BUFF_ALLOC(stHmacKey, KMC_KEY4HMAC_LEN);
    WSEC_BUFF_ALLOC(stHmacRst, KMC_HMAC_RST_LEN);
    WSEC_BUFF_ALLOC(stHmac4MkInFile, KMC_HMAC_RST_LEN);
    pstHdrWithHmac = (KMC_MKF_HDR_WITH_HMAC_STRU*)WSEC_MALLOC(sizeof(KMC_MKF_HDR_WITH_HMAC_STRU));
    pstHdr = &pstHdrWithHmac->stHdr;
    pstMemMk = (KMC_MEM_MK_STRU*)WSEC_MALLOC(sizeof(KMC_MEM_MK_STRU));

    /* 读取文件头, 并判断文件格式 */
    KMC_PRI_Lock(KMC_LOCK_BOTH);
    do
    {
        break_oper_if(!stDecrptKey.pBuff, WSEC_LOG_E4MALLOC(stDecrptKey.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!stHmacKey.pBuff, WSEC_LOG_E4MALLOC(stHmacKey.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!stHmacRst.pBuff, WSEC_LOG_E4MALLOC(stHmacRst.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!stHmac4MkInFile.pBuff, WSEC_LOG_E4MALLOC(stHmac4MkInFile.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!pstHdrWithHmac, WSEC_LOG_E4MALLOC(sizeof(KMC_MKF_HDR_WITH_HMAC_STRU)), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!pstMemMk, WSEC_LOG_E4MALLOC(sizeof(KMC_MEM_MK_STRU)), nErrCode = WSEC_ERR_MALLOC_FAIL);

        break_oper_if(!WSEC_FREAD_MUST(abFormatFlag, sizeof(abFormatFlag), fRead),
                      WSEC_LOG_E1("Read file(%s) fail.", pszFromFile), nErrCode = WSEC_ERR_READ_FILE_FAIL);
        break_oper_if(WSEC_MEMCMP(abFormatFlag, g_MkfFlag, sizeof(g_MkfFlag)) != 0,
                      WSEC_LOG_E1("%s is not MK file.", pszFromFile), nErrCode = WSEC_ERR_FILE_FORMAT);
        break_oper_if(!WSEC_FREAD_MUST(pstHdrWithHmac, sizeof(KMC_MKF_HDR_WITH_HMAC_STRU), fRead),
                      WSEC_LOG_E1("%s is not MK file.", pszFromFile), nErrCode = WSEC_ERR_FILE_FORMAT);

        KMC_PRI_CvtByteOrder4MkfHdr(&pstHdrWithHmac->stHdr, wbcNetwork2Host);

        /* 构造用于HMAC的密钥 */
        break_oper_if(CAC_Pbkdf2(WSEC_ALGID_PBKDF2_HMAC_SHA256, 
                                  pbPwd, ulPwdLen, 
                                  pstHdr->abSalt4HmacKey, sizeof(pstHdr->abSalt4HmacKey),
                                  pstHdr->ulIteration4HmacKey,
                                  stHmacKey.nLen, stHmacKey.pBuff) != WSEC_SUCCESS,
                      WSEC_LOG_E("CAC_Pbkdf2() fail."), nErrCode = WSEC_ERR_PBKDF2_FAIL);

        /* 对文件头计算HMAC */
        break_oper_if(CAC_Hmac(pstHdr->ulHmacAlgId,
                               stHmacKey.pBuff, stHmacKey.nLen, 
                               pstHdr, sizeof(KMC_MKF_HDR_STRU),
                               stHmacRst.pBuff, &stHmacRst.nLen),
                      WSEC_LOG_E("CAC_Hmac() fail."), nErrCode = WSEC_ERR_HMAC_FAIL);

        /* 判断HMAC值来验证文件头是否被破坏 */
        break_oper_if(WSEC_MEMCMP(stHmacRst.pBuff, pstHdrWithHmac->abHmac, sizeof(pstHdrWithHmac->abHmac)) != 0,
                      WSEC_LOG_E1("The file(%s) tampered.", pszFromFile), nErrCode = WSEC_ERR_HMAC_FAIL);

        /* 构造用于解密MK密文的密钥 */
        break_oper_if(CAC_Pbkdf2(WSEC_ALGID_PBKDF2_HMAC_SHA256, 
                                  pbPwd, ulPwdLen, 
                                  pstHdr->abSalt4EncrypKey, sizeof(pstHdr->abSalt4EncrypKey),
                                  pstHdr->ulIteration4EncrypKey,
                                  stDecrptKey.nLen, stDecrptKey.pBuff) != WSEC_SUCCESS,
                      WSEC_LOG_E("CAC_Pbkdf2() fail."), nErrCode = WSEC_ERR_PBKDF2_FAIL);

        /* 分配内存, 为读取MK密文作准备 */
        WSEC_BUFF_ALLOC(stCipher, pstHdr->ulCipherLenPerMk);
        pKeystore = (KMC_KSF_MEM_STRU*)WSEC_MALLOC(sizeof(KMC_KSF_MEM_STRU)); /* 该数据较大, 使用堆内存为宜 */
        nPlainLen = pstHdr->ulCipherLenPerMk;
        pMkPlain  = (KMC_MKF_MK_STRU*)WSEC_MALLOC(nPlainLen); /* 明文长度不可能大于密文长度 */

        /* 检查资源是否到位 */
        break_oper_if(!stCipher.pBuff, WSEC_LOG_E4MALLOC(stCipher.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!pKeystore, WSEC_LOG_E4MALLOC(sizeof(KMC_KSF_MEM_STRU)), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!pMkPlain, WSEC_LOG_E4MALLOC(pstHdr->ulCipherLenPerMk), nErrCode = WSEC_ERR_MALLOC_FAIL);

        if (WSEC_MEMCPY(&pKeystore->stRkInfo, sizeof(pKeystore->stRkInfo), &g_pKeystore->stRkInfo, sizeof(g_pKeystore->stRkInfo)) != EOK) {nErrCode = WSEC_ERR_MEMCPY_FAIL;}
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E4MEMCPY, oper_null);

        break_oper_if(CAC_HmacInit(&ctx, pstHdr->ulHmacAlgId, stHmacKey.pBuff, stHmacKey.nLen) != WSEC_SUCCESS, WSEC_LOG_E("CAC_HmacInit() fail."), nErrCode = WSEC_ERR_HMAC_FAIL);
        
        /* 逐个读取MK密文 */
        for (i = 0; i < (WSEC_INT32)pstHdr->ulMkNum; i++)
        {
            break_oper_if(!WSEC_FREAD_MUST(stCipher.pBuff, stCipher.nLen, fRead),
                          WSEC_LOG_E1("Read data from(%s) fail.", pszFromFile), nErrCode = WSEC_ERR_READ_FILE_FAIL);

            /* 对本密文计算HMAC */
            break_oper_if(CAC_HmacUpdate(ctx, stCipher.pBuff, stCipher.nLen) != WSEC_SUCCESS,
                          WSEC_LOG_E("CAC_HmacUpdate() fail."), nErrCode = WSEC_ERR_HMAC_FAIL);

            /* 解密MK */
            nPlainLen = pstHdr->ulCipherLenPerMk;
            break_oper_if(CAC_Decrypt(pstHdr->ulEncryptAlgId,
                                      stDecrptKey.pBuff, stDecrptKey.nLen,
                                      pstHdr->abIv4EncrypMk, sizeof(pstHdr->abIv4EncrypMk),
                                      stCipher.pBuff, stCipher.nLen,
                                      pMkPlain, &nPlainLen) != WSEC_SUCCESS,
                          WSEC_LOG_E("CAC_Decrypt() fail."), nErrCode = WSEC_ERR_DECRPT_FAIL);
            break_oper_if(nPlainLen != sizeof(KMC_MKF_MK_STRU), WSEC_LOG_E1("Parse data in file(%s) fail.", pszFromFile), nErrCode = WSEC_ERR_FILE_FORMAT);

            /* 将解密出的MK转换为加入数组中的MK */
            pstDomainCfg = KMC_PRI_SearchDomain(g_pKmcCfg, pMkPlain->stMkInfo.ulDomainId);
            break_oper_if(pstDomainCfg && (pstDomainCfg->stDomainInfo.ucKeyFrom != pMkPlain->stMkInfo.ucGenStyle), 
                          WSEC_LOG_E4("Import MK(DomainId=%d, KeyId=%d) fail for KeyFrom(%d) conflict with Domain-CFG(%d).",
                                      pMkPlain->stMkInfo.ulDomainId, pMkPlain->stMkInfo.ulKeyId, pMkPlain->stMkInfo.ucGenStyle, pstDomainCfg->stDomainInfo.ucKeyFrom), 
                          nErrCode = WSEC_ERR_KMC_IMPORT_MK_CONFLICT_DOMAIN);

            stDomain.ulId      = pMkPlain->stMkInfo.ulDomainId;
            stDomain.ucKeyFrom = pMkPlain->stMkInfo.ucGenStyle;

            stKeyType.ulKeyLen  = pMkPlain->ulPlainLen;
            stKeyType.usKeyType = pMkPlain->stMkInfo.usType;
            stKeyType.ulKeyLifeDays = 180; /* 确保数据结构合法, 没有实际意义. */

            WSEC_BUFF_ASSIGN(stPlainKey, pMkPlain->abPlainText, pMkPlain->ulPlainLen);
            nErrCode = KMC_PRI_MakeMk(&stDomain, &stKeyType, &stPlainKey, pMkPlain->stMkInfo.ulKeyId, pstMemMk);
            if (nErrCode != WSEC_SUCCESS) {break;}

            /* 如下信息需以导入文件数据为准 */
            pstMemMk->stMkInfo.ucStatus = pMkPlain->stMkInfo.ucStatus;
            break_oper_if(!WSEC_DateTimeCopy(&pstMemMk->stMkInfo.stMkCreateTimeUtc, &pMkPlain->stMkInfo.stMkCreateTimeUtc),
                          WSEC_LOG_E("WSEC_DateTimeCopy() fail."), nErrCode = WSEC_ERR_MEMCPY_FAIL);
            break_oper_if(!WSEC_DateTimeCopy(&pstMemMk->stMkInfo.stMkExpiredTimeUtc, &pMkPlain->stMkInfo.stMkExpiredTimeUtc),
                          WSEC_LOG_E("WSEC_DateTimeCopy() fail."), nErrCode = WSEC_ERR_MEMCPY_FAIL);

            /* 加入数组 */
            pstMk4Add = (KMC_MEM_MK_STRU*)WSEC_CLONE_BUFF(pstMemMk, sizeof(KMC_MEM_MK_STRU));
            break_oper_if(!pstMk4Add, WSEC_LOG_E("WSEC_CLONE_BUFF() fail."), nErrCode = WSEC_ERR_MEMCLONE_FAIL);
            nErrCode = KMC_PRI_AddMk2Array(pKeystore, pstMk4Add);
            break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC_PRI_AddMk2Array() = %u", nErrCode), WSEC_FREE(pstMk4Add));

            WSEC_RptProgress(pstRptProgress, &tSpend, pstHdr->ulMkNum, i + 1, &bCancel);
            break_oper_if(bCancel, WSEC_LOG_E("App Canceled"), nErrCode = WSEC_ERR_CANCEL_BY_APP);
        }

        if (nErrCode != WSEC_SUCCESS) {break;}
        if (!bCancel) {WSEC_RptProgress(pstRptProgress, WSEC_NULL_PTR, pstHdr->ulMkNum, pstHdr->ulMkNum, WSEC_NULL_PTR);} /* 确保进度100%上报 */

        /* 获取上述MK密文的HMAC并验证完整性 */
        break_oper_if(CAC_HmacFinal(&ctx, stHmacRst.pBuff, &stHmacRst.nLen) != WSEC_SUCCESS, 
                      WSEC_LOG_E("CAC_HmacFinal() fail."), nErrCode = WSEC_ERR_HMAC_FAIL);

        /* 读取文件中对所有MK密文的HMAC */
        break_oper_if(!WSEC_FREAD_MUST(stHmac4MkInFile.pBuff, stHmac4MkInFile.nLen, fRead),
                      WSEC_LOG_E1("Read data from(%s) fail.", pszFromFile), nErrCode = WSEC_ERR_READ_FILE_FAIL);

        break_oper_if(WSEC_MEMCMP(stHmacRst.pBuff, stHmac4MkInFile.pBuff, stHmacRst.nLen) != 0,
                      WSEC_LOG_E1("The file(%s)'s HMAC authenticated fail.", pszFromFile), nErrCode = WSEC_ERR_HMAC_AUTH_FAIL);

        /* 将新Keystore写文件, 内存中的Keystore数据切换 */
        nErrCode = KMC_PRI_WriteKsfSafety(pKeystore, pstRk);
        if (nErrCode != WSEC_SUCCESS) {WSEC_LOG_E1("KMC_PRI_WriteKsfSafety() = %u.", nErrCode);}
    } do_end;
    
    if (WSEC_SUCCESS == nErrCode) /* 内存中的Keystore数据切换 */
    {
        pKeystore->pszFromFile = g_pKeystore->pszFromFile;
        KMC_PRI_FreeKsfSnapshot(g_pKeystore);
        g_pKeystore = pKeystore;
    }
    else
    {
        KMC_PRI_FreeKsfSnapshot(pKeystore);
    }
    
    KMC_PRI_Unlock(KMC_LOCK_BOTH);

    /* 资源释放 */
    WSEC_FCLOSE(fRead);
    WSEC_BUFF_FREE(stDecrptKey);
    WSEC_BUFF_FREE(stHmacKey);
    WSEC_BUFF_FREE(stHmacRst);
    WSEC_BUFF_FREE(stCipher);
    WSEC_BUFF_FREE(stHmac4MkInFile);
    WSEC_FREE(pMkPlain);
    WSEC_FREE(pstHdrWithHmac);
    WSEC_FREE(pstRk);
    WSEC_FREE(pstMemMk);

    WSEC_LOG_W2("Import MK from '%s' finished, return code = %u.", pszFromFile, nErrCode);

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_UpdateRootKey
 功能描述  : 更新根密钥
             原理: 重新生成根密钥, 但MK保持不变.
 纯 入 参  : pbKeyEntropy: 密钥熵码流;[可空];
             ulSize:       密钥熵码流长.
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月07日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_UpdateRootKey(const WSEC_BYTE* pbKeyEntropy, WSEC_SIZE_T ulSize)
{
    return_err_if_kmc_not_work;
    return KMC_PRI_UpdateRootKeyPri(pbKeyEntropy, ulSize, KMC_NEED_LOCK);
}

/*****************************************************************************
 函 数 名  : KMC_GetRootKeyInfo
 功能描述  : 获取Root Key非敏感信息
 纯 入 参  : 无
 纯 出 参  : pstRkInfo: Root Key非安全敏感信息
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月08日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_GetRootKeyInfo(KMC_RK_ATTR_STRU* pstRkInfo)
{
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    
    return_err_if_kmc_not_work;
    return_err_if_para_invalid("KMC_GetRootKeyInfo", pstRkInfo);

    KMC_PRI_Lock(KMC_LOCK_KEYSTORE);
    if (WSEC_MEMCPY(pstRkInfo, sizeof(KMC_RK_ATTR_STRU), &g_pKeystore->stRkInfo, sizeof(g_pKeystore->stRkInfo)) != EOK) {nErrCode = WSEC_ERR_MEMCPY_FAIL;}
    KMC_PRI_Unlock(KMC_LOCK_KEYSTORE);
    
    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_GetMkCount
 功能描述  : 获取MK个数
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : -1:     本CBB尚未被初始化;
             非负数: 当前MK个数.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月08日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_INT32 KMC_GetMkCount()
{
    WSEC_INT32 nCount = -1;

    KMC_PRI_Lock(KMC_LOCK_KEYSTORE);
    if (WSEC_RUNNING == g_KmcSys.eState)
    {
        nCount = WSEC_ARR_GetCount(g_pKeystore->arrMk);
    }
    KMC_PRI_Unlock(KMC_LOCK_KEYSTORE);

    return nCount;
}

/*****************************************************************************
 函 数 名  : KMC_GetMk
 功能描述  : 获取指定位置上的MK
 纯 入 参  : Index: 指定位置(0开始的索引)
 纯 出 参  : pstMk: 存放MK
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月08日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_GetMk(WSEC_INT32 Index, KMC_MK_INFO_STRU* pstMk)
{
    KMC_MK_INFO_STRU* pItem = WSEC_NULL_PTR;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    
    return_err_if_kmc_not_work;
    return_err_if_para_invalid("KMC_GetMk", (Index >= 0) && pstMk);

    KMC_PRI_Lock(KMC_LOCK_KEYSTORE);
    do
    {
        break_oper_if(Index >= WSEC_ARR_GetCount(g_pKeystore->arrMk), WSEC_LOG_E1("Index(%d) overflow the array.", Index), nErrCode = WSEC_ERR_INVALID_ARG);
        
        pItem = (KMC_MK_INFO_STRU*)WSEC_ARR_GetAt(g_pKeystore->arrMk, Index);
        break_oper_if(!pItem, WSEC_LOG_E("memory access fail."), nErrCode = WSEC_ERR_OPER_ARRAY_FAIL);

        break_oper_if(WSEC_MEMCPY(pstMk, sizeof(KMC_MK_INFO_STRU), pItem, sizeof(KMC_MK_INFO_STRU)) != EOK, WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_KEYSTORE);

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_GetMaxMkId
 功能描述  : 获取指定Domain下当前最大MK ID
 纯 入 参  : ulDomainId:  功能域标识
 纯 出 参  : pulMaxKeyId: 最大Master Key ID
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月23日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_GetMaxMkId(WSEC_UINT32 ulDomainId, WSEC_UINT32* pulMaxKeyId)
{
    return_err_if_kmc_not_work;
    return_err_if_domain_privacy(ulDomainId);
    return_err_if_para_invalid("KMC_GetMaxMkId", pulMaxKeyId);
    
    return KMC_PRI_GetMaxMkId(KMC_NEED_LOCK, ulDomainId, pulMaxKeyId);
}

/*****************************************************************************
 函 数 名  : KMC_SetMkExpireTime
 功能描述  : 设置指定MK的过期时间
 纯 入 参  : ulDomainId, ulKeyId: MK唯一身份ID
             psExpireTime: 过期时刻
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月08日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_SetMkExpireTime(WSEC_UINT32 ulDomainId, WSEC_UINT32 ulKeyId, const WSEC_SYSTIME_T* psExpireTimeUtc)
{
    WSEC_INT32 nIndex = -1, nRemainLifeDays = 0;
    KMC_MEM_MK_STRU* pMk = WSEC_NULL_PTR;
    KMC_MK_EXPIRE_NTF_STRU stWarn = {0};
    WSEC_SYSTIME_T stUtcNow = {0};
    WSEC_BOOL bWarn = WSEC_FALSE;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    
    return_err_if_kmc_not_work;
    return_err_if_domain_privacy(ulDomainId);
    return_err_if_para_invalid("KMC_SetMkExpireTime", psExpireTimeUtc && WSEC_IsDateTime(psExpireTimeUtc));

    return_oper_if(!WSEC_GetUtcDateTime(&stUtcNow), WSEC_LOG_E("WSEC_GetUtcDateTime() fail"), WSEC_ERR_GET_CURRENT_TIME_FAIL);

    KMC_PRI_Lock(KMC_LOCK_BOTH);
    do
    {
        nIndex = KMC_PRI_SearchMkByKeyId(g_pKeystore, ulDomainId, ulKeyId);
        break_oper_if(nIndex < 0, WSEC_LOG_E2("Cannot find MK(DomainId=%d, KeyId=%d)", ulDomainId, ulKeyId), nErrCode = WSEC_ERR_KMC_MK_MISS);

        pMk = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(g_pKeystore->arrMk, nIndex);
        break_oper_if(!pMk, WSEC_LOG_E("memory access fail."), nErrCode = WSEC_ERR_OPER_ARRAY_FAIL);

        break_oper_if(KMC_KEY_STATUS_INACTIVE == pMk->stMkInfo.ucStatus,
                       WSEC_LOG_E2("Cannot set expiretime for inactive MK(ulDomainId=%u, ulKeyId=%u)", ulDomainId, ulKeyId),
                       nErrCode = WSEC_ERR_KMC_CANNOT_SET_EXPIRETIME_FOR_INACTIVE_MK);
        break_oper_if(pMk->stMkInfo.ucGenStyle != KMC_MK_GEN_BY_IMPORT,
                       WSEC_LOG_E2("The MK(ulDomainId=%u, ulKeyId=%u) is not imported, cannot suppoert this oper.", ulDomainId, ulKeyId),
                       nErrCode = WSEC_ERR_KMC_MK_GENTYPE_REJECT_THE_OPER);

        break_oper_if(!WSEC_DateTimeCopy(&pMk->stMkInfo.stMkExpiredTimeUtc, psExpireTimeUtc), WSEC_LOG_E("Date time copy fail."), nErrCode = WSEC_ERR_MEMCPY_FAIL);

        /* 检查是否到期 */
        nRemainLifeDays = WSEC_DateTimeDiff(dtpDay, &stUtcNow, &pMk->stMkInfo.stMkExpiredTimeUtc);
        if (nRemainLifeDays <= 0) /* 过期了 */
        {
            pMk->stMkInfo.ucStatus = KMC_KEY_STATUS_INACTIVE;
            WSEC_DateTimeCopy(&pMk->stMkInfo.stMkExpiredTimeUtc, &stUtcNow);
        }
        else if (((WSEC_UINT32)nRemainLifeDays) < g_pKmcCfg->stKmCfg.ulWarningBeforeKeyExpiredDays) /* 预警 */
        {
            if (WSEC_MEMCPY(&stWarn.stMkInfo, sizeof(stWarn.stMkInfo), &pMk->stMkInfo, sizeof(pMk->stMkInfo)) == EOK)
            {
                stWarn.nRemainDays = nRemainLifeDays;
                bWarn = WSEC_TRUE;
            }else{;}
        }else{;}

        nErrCode = KMC_PRI_WriteKsfSafety(g_pKeystore, WSEC_NULL_PTR);
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_BOTH);

    if_oper(bWarn, WSEC_NOTIFY(WSEC_KMC_NTF_MK_EXPIRE, &stWarn, sizeof(stWarn)));

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_SetMkStatus
 功能描述  : 设置MK状态
 纯 入 参  : ulDomainId, ulKeyId: MK唯一身份ID
             ucStatus: 密钥状态(见 KMC_KEY_STATUS_ENUM)
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月08日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_SetMkStatus(WSEC_UINT32 ulDomainId, WSEC_UINT32 ulKeyId, WSEC_UINT8 ucStatus)
{
    WSEC_INT32 nIndex = -1;
    WSEC_BOOL bNtf = WSEC_FALSE;
    KMC_MEM_MK_STRU* pMk = WSEC_NULL_PTR;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    return_err_if_kmc_not_work;
    return_err_if_domain_privacy(ulDomainId);
    return_err_if_para_invalid("KMC_SetMkStatus", KMC_IS_VALID_KEY_STATUS(ucStatus));

    KMC_PRI_Lock(KMC_LOCK_KEYSTORE);
    do
    {
        nIndex = KMC_PRI_SearchMkByKeyId(g_pKeystore, ulDomainId, ulKeyId);
        break_oper_if(nIndex < 0, WSEC_LOG_E2("Cannot find MK(DomainId=%d, KeyId=%d)", ulDomainId, ulKeyId), nErrCode = WSEC_ERR_KMC_MK_MISS);

        pMk = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(g_pKeystore->arrMk, nIndex);
        break_oper_if(!pMk, WSEC_LOG_E("memory access fail."), nErrCode = WSEC_ERR_OPER_ARRAY_FAIL);

        break_oper_if(pMk->stMkInfo.ucGenStyle != KMC_MK_GEN_BY_IMPORT, WSEC_LOG_E("Only imported MK can support this oper."), nErrCode = WSEC_ERR_KMC_MK_GENTYPE_REJECT_THE_OPER);

        if (pMk->stMkInfo.ucStatus != ucStatus) /* 状态不同才需要修改 */
        {
            pMk->stMkInfo.ucStatus = ucStatus;
            if (KMC_KEY_STATUS_INACTIVE == ucStatus)
            {
                break_oper_if(!WSEC_GetUtcDateTime(&pMk->stMkInfo.stMkExpiredTimeUtc), WSEC_LOG_E("Get UTC fail."), nErrCode = WSEC_ERR_GET_CURRENT_TIME_FAIL);
            }

            WSEC_ARR_QuickSort(g_pKeystore->arrMk); /* 确保MK数组排序, 以便'对半查找' */
            nErrCode = KMC_PRI_WriteKsfSafety(g_pKeystore, WSEC_NULL_PTR);

            bNtf = WSEC_TRUE;
        }
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_KEYSTORE);

    if (bNtf && (pMk!=WSEC_NULL_PTR))
    {
        KMC_PRI_NtfMkChanged(&pMk->stMkInfo, KMC_KEY_STATUS_INACTIVE == ucStatus ? KMC_KEY_INACTIVATED : KMC_KEY_ACTIVATED);
        WSEC_LOG_W3("The MK(DomainId=%u, KeyId=%u)'s status change to %u", ulDomainId, ulKeyId, ucStatus);
    }

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_RegisterMk
 功能描述  : MK密钥注册
 纯 入 参  : ulDomainId: 所属应用域
             ulKeyId:    密钥ID
             usKeyType:  密钥用途
             pPlainTextKey: 密钥明文
             ulKeyLen:      密钥明文长度
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : WSEC_SUCCESS: 成功
             其它:         具体错误原因.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月04日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_RegisterMk(WSEC_UINT32 ulDomainId, WSEC_UINT32 ulKeyId, WSEC_UINT16 usKeyType, const WSEC_BYTE* pPlainTextKey, WSEC_UINT32 ulKeyLen)
{
    KMC_DOMAIN_CFG_STRU* pstFoundDomain = WSEC_NULL_PTR;
    KMC_CFG_KEY_TYPE_STRU*   pstFoundKeyType = WSEC_NULL_PTR;
    KMC_CFG_KEY_TYPE_STRU    stKeyType = {0};
    KMC_CFG_DOMAIN_INFO_STRU* pstDomainInfo = WSEC_NULL_PTR;
    WSEC_BUFF stPlainTextKey = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    return_err_if_kmc_not_work;
    return_err_if_domain_privacy(ulDomainId);
    return_err_if_para_invalid("KMC_RegisterMk", KMC_IS_VALID_KEY_TYPE(usKeyType) && (pPlainTextKey) && (ulKeyLen > 0));

    KMC_PRI_Lock(KMC_LOCK_BOTH);
    do
    {
        /* 1. 参数合规检查 */
        /* 1) Domain未配置或密钥来源为系统自动生成则非法 */
        break_oper_if(!KMC_PRI_SearchDomainKeyTypeCfg(ulDomainId, usKeyType, &pstFoundDomain, &pstFoundKeyType),
                       WSEC_LOG_E2("Domain KeyType(DomainId=%d, KeyType=%d) miss.", ulDomainId, usKeyType), nErrCode = WSEC_ERR_KMC_DOMAIN_KEYTYPE_MISS);
        WSEC_ASSERT(pstFoundDomain && pstFoundKeyType);

        pstDomainInfo = &pstFoundDomain->stDomainInfo;
        break_oper_if(pstDomainInfo->ucKeyFrom != KMC_MK_GEN_BY_IMPORT,
                       WSEC_LOG_E2("The keys of domain(%s, id=%d) defined as inner-generated, cannot register.", pstDomainInfo->szDesc, pstDomainInfo->ulId),
                       nErrCode = WSEC_ERR_KMC_CANNOT_REG_AUTO_KEY);
        
        /* 2) 若MK已存在, 则禁止注册 */
        break_oper_if(KMC_PRI_SearchMkByKeyId(g_pKeystore, ulDomainId, ulKeyId) >= 0,
                       WSEC_LOG_E2("The MasterKey(DomainId=%u, KeyId=%u) already exist.", ulDomainId, ulKeyId),
                       nErrCode = WSEC_ERR_KMC_REG_REPEAT_MK);

        /* 3) 密钥明文长度明显不靠谱则禁止注册 */
        break_oper_if(!KMC_IS_VALID_KEY_TYPE_LEN(ulKeyLen), WSEC_LOG_E2("MK len(%u) is too long, it must not over %u.", ulKeyLen, g_KmcSys.ulMkPlainLenMax), 
                       nErrCode = WSEC_ERR_KMC_MK_LEN_TOO_LONG);

        stKeyType.usKeyType     = usKeyType;
        stKeyType.ulKeyLen      = ulKeyLen;
        stKeyType.ulKeyLifeDays = pstFoundKeyType->ulKeyLifeDays;

        /* 2. 创建MK */
        WSEC_BUFF_ASSIGN(stPlainTextKey, (WSEC_BYTE*)pPlainTextKey, ulKeyLen);

        nErrCode = KMC_PRI_CreateMkItem(g_pKeystore, pstDomainInfo, &stKeyType, &stPlainTextKey, ulKeyId);
        if (WSEC_SUCCESS == nErrCode) {nErrCode = KMC_PRI_WriteKsfSafety(g_pKeystore, WSEC_NULL_PTR);} /* 密钥注册成功, 需写Keystore文件 */
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_BOTH);
    
    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_CreateMk
 功能描述  : 创建Master Key
             如果存在旧密钥, 则将其状态置为过期.
 纯 入 参  : ulDomainId: MK作用域
             usKeyType:  密钥类型
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月18日
    作    者   : z00118096
    修改内容   : 新生成函数
  2.日    期   : 2015年8月7日
    作    者   : z00118096
    修改内容   : 在重新创建MK前补充检查MK数量是否超限, 以消除老版本的不合理
                 逻辑: 先置密钥状态为过期, 再克隆MK失败.
*****************************************************************************/
WSEC_ERR_T KMC_CreateMk(WSEC_UINT32 ulDomainId, WSEC_UINT16 usKeyType)
{
    KMC_DOMAIN_CFG_STRU* pstDomain = WSEC_NULL_PTR;
    KMC_CFG_KEY_TYPE_STRU*   pstKeyType = WSEC_NULL_PTR;
    WSEC_INT32 i;
    KMC_MEM_MK_STRU* pMemMk = WSEC_NULL_PTR;
    KMC_MK_INFO_STRU* pMk = WSEC_NULL_PTR;
    WSEC_UINT32 ulMkIdMax = 0;
    WSEC_ARRAY arrSelMk = WSEC_NULL_PTR;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS, nTemp;

    return_err_if_kmc_not_work;
    return_err_if_domain_privacy(ulDomainId);

    /* 查询旧MK */
    arrSelMk = WSEC_ARR_Initialize(0, 0, WSEC_NULL_PTR, WSEC_NULL_PTR);
    return_oper_if(!arrSelMk, WSEC_LOG_E("Array Initialize fail."), WSEC_ERR_MALLOC_FAIL);

    KMC_PRI_Lock(KMC_LOCK_BOTH);
    do
    {
        break_oper_if(!KMC_PRI_SearchDomainKeyTypeCfg(ulDomainId, usKeyType, &pstDomain, &pstKeyType),
                       WSEC_LOG_E2("The DomainKey(DomainId=%u, KeyType=%u) not found.", ulDomainId, usKeyType), nErrCode = WSEC_ERR_KMC_DOMAIN_KEYTYPE_MISS);
        break_oper_if(pstDomain->stDomainInfo.ucKeyFrom != KMC_MK_GEN_BY_INNER,
                      WSEC_LOG_E("Only inner Key can support this oper."), nErrCode = WSEC_ERR_KMC_MK_GENTYPE_REJECT_THE_OPER);

        for (i = 0; i < WSEC_ARR_GetCount(g_pKeystore->arrMk); i++)
        {
            pMemMk = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(g_pKeystore->arrMk, i);
            break_oper_if(!pMemMk, WSEC_LOG_E("memory access fail."), nErrCode = WSEC_ERR_OPER_ARRAY_FAIL);
            continue_if((pMemMk->stMkInfo.ulDomainId != ulDomainId) || (pMemMk->stMkInfo.usType != usKeyType) || (pMemMk->stMkInfo.ucStatus != KMC_KEY_STATUS_ACTIVE));

            WSEC_ARR_Add(arrSelMk, &pMemMk->stMkInfo);
        }

        if (nErrCode != WSEC_SUCCESS) {break;}
        break_oper_if((WSEC_ARR_GetCount(arrSelMk) + WSEC_ARR_GetCount(g_pKeystore->arrMk)) > WSEC_MK_NUM_MAX, WSEC_LOG_E("Cannot CreateMK for the num of MK will overflow."), nErrCode = WSEC_ERR_KMC_MK_NUM_OVERFLOW);

        if (WSEC_ARR_GetCount(arrSelMk) > 0) /* 若存在旧密钥, 则在旧密钥基础上再创建新密钥 */
        {
            for (i = 0; i < WSEC_ARR_GetCount(arrSelMk); i++)
            {
                pMk = (KMC_MK_INFO_STRU*)WSEC_ARR_GetAt(arrSelMk, i);
                break_oper_if(!pMk, WSEC_LOG_E("memory access fail."), nErrCode = WSEC_ERR_OPER_ARRAY_FAIL);
                nTemp = KMC_PRI_ReCreateMkItem(g_pKeystore, pMk);
                if (nTemp != WSEC_SUCCESS) {nErrCode = nTemp;}
            }
        }
        else /* 不存在旧MK, 则新建之 */
        {
            KMC_PRI_GetMaxMkId(KMC_NOT_LOCK, ulDomainId, &ulMkIdMax);
            nErrCode = KMC_PRI_CreateMkItem(g_pKeystore, &pstDomain->stDomainInfo, pstKeyType, WSEC_NULL_PTR, ulMkIdMax + 1);
        }
        
        if (WSEC_SUCCESS == nErrCode) {nErrCode = KMC_PRI_WriteKsfSafety(g_pKeystore, WSEC_NULL_PTR);}
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_BOTH);

    WSEC_ARR_Finalize(arrSelMk);

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_GetMkDetailInner
 功能描述  : 根据DomainId, KeyId获取MK
 纯 入 参  : ulDomainId, ulKeyId: MK唯一身份ID
 纯 出 参  : pstMkInfo: MK基本属性[可空]
             pbKeyPlainText: MK密文
 入参出参  : pKeyLen: [in]分配给pbKeyPlainText的长度;
                      [out]输出pbKeyPlainText的实际长度.
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 该函数只开放给CBB内部调用

 修改历史
  1.日    期   : 2014年11月08日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_GetMkDetailInner(WSEC_UINT32 ulDomainId, WSEC_UINT32 ulKeyId, KMC_MK_INFO_STRU* pstMkInfo, WSEC_BYTE* pbKeyPlainText, WSEC_UINT32* pKeyLen)
{
    return_err_if_kmc_not_work;
    return_err_if_domain_privacy(ulDomainId);
    return KMC_PRI_GetMkDetail(KMC_NEED_LOCK, ulDomainId, ulKeyId, pstMkInfo, pbKeyPlainText, pKeyLen);
}

/*****************************************************************************
 函 数 名  : KMC_GetMkDetail
 功能描述  : 根据DomainId, KeyId获取MK
 纯 入 参  : ulDomainId, ulKeyId: MK唯一身份ID
 纯 出 参  : pstMkInfo: MK基本属性[可空]
             pbKeyPlainText: MK密文
 入参出参  : pKeyLen: [in]分配给pbKeyPlainText的长度;
                      [out]输出pbKeyPlainText的实际长度.
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 该函数作为APP调用的接口, 只能查询'外部导入'的密钥.

 修改历史
  1.日    期   : 2014年11月08日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_GetMkDetail(WSEC_UINT32 ulDomainId, WSEC_UINT32 ulKeyId, KMC_MK_INFO_STRU* pstMkInfo, WSEC_BYTE* pbKeyPlainText, WSEC_UINT32* pKeyLen)
{
    KMC_MK_INFO_STRU stMk = {0};
    WSEC_BYTE        abKeyPlain[WSEC_MK_LEN_MAX] = {0};
    WSEC_UINT32      ulKeyLen;
    WSEC_ERR_T       nErrCode = WSEC_SUCCESS;

    return_err_if_kmc_not_work;
    return_err_if_domain_privacy(ulDomainId);
    return_err_if_para_invalid("KMC_GetMkDetail", pbKeyPlainText && pKeyLen);

    do
    {
        ulKeyLen = sizeof(abKeyPlain);
        nErrCode = KMC_PRI_GetMkDetail(KMC_NEED_LOCK, ulDomainId, ulKeyId, &stMk, abKeyPlain, &ulKeyLen);
        if (nErrCode != WSEC_SUCCESS) {break;}
        
        /* 只有'外部导入密钥'才允许被查询 */
        break_oper_if(stMk.ucGenStyle != KMC_MK_GEN_BY_IMPORT,
                       WSEC_LOG_E2("The MK(DomainId=%u, KeyId=%u)'s KeyFrom cannot support this oper.", ulDomainId, ulKeyId), 
                       nErrCode = WSEC_ERR_KMC_MK_GENTYPE_REJECT_THE_OPER);

        /* 输出密钥明文 */
        break_oper_if(*pKeyLen < ulKeyLen, 
                       WSEC_LOG_E2("*pKeyLen must at least given %d, but %d, so input-buff insufficient.", ulKeyLen, *pKeyLen),
                       nErrCode = WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH);
        break_oper_if(WSEC_MEMCPY(pbKeyPlainText, *pKeyLen, abKeyPlain, ulKeyLen) != EOK, 
                       WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
        *pKeyLen = ulKeyLen;

        /* 输出MK基本信息 */
        if (pstMkInfo)
        {
            break_oper_if(WSEC_MEMCPY(pstMkInfo, sizeof(KMC_MK_INFO_STRU), &stMk, sizeof(stMk)) != EOK, WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
        }
    }do_end;

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_SecureEraseKeystore
 功能描述  : 彻底地销毁Keystore文件
             原理是: 删除文件前, 将文件内容全部以0重写.
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月08日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_SecureEraseKeystore()
{
    WSEC_CHAR* pszFile = WSEC_NULL_PTR;
    WSEC_SIZE_T i;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    return_err_if_kmc_not_work;

    KMC_PRI_Lock(KMC_LOCK_KEYSTORE);
    for (i = 0; i < KMC_KSF_NUM; i++)
    {
        pszFile = g_KmcSys.apszKeystoreFile[i];
        continue_if(!pszFile);

        if (!WSEC_DeleteFileS(pszFile))
        {
            nErrCode = WSEC_FAILURE;
            WSEC_LOG_E1("WSEC_DeleteFileS(%s) fail.", pszFile);
        }
    }
    KMC_PRI_Unlock(KMC_LOCK_KEYSTORE);

    if (WSEC_SUCCESS == nErrCode) {nErrCode = KMC_PRI_Finalize(KMC_NEED_LOCK);}

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_SetRootKeyCfg
 功能描述  : 设置RootKey配置
 纯 入 参  : pstRkCfg: RK配置数据[若CBB内部生成RK]
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月10日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_SetRootKeyCfg(const KMC_CFG_ROOT_KEY_STRU* pstRkCfg)
{
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    WSEC_INT32 nTemp = 0;
    WSEC_BOOL bNtf = WSEC_FALSE;
    
    return_err_if_kmc_not_work;
    if (!KMC_IS_INNER_CREATE_RK) {return WSEC_ERR_KMC_RK_GENTYPE_REJECT_THE_OPER;}
    return_err_if_para_invalid("KMC_SetRootKeyCfg", pstRkCfg);

    /* 检查参数内容合法性 */
    return_oper_if(!KMC_CFG_IS_ROOT_KEY_VALID(pstRkCfg), WSEC_LOG_E("'ulRootKeyLifeDays' or 'ulRootMasterKeyIterations' invalid"), WSEC_ERR_INVALID_ARG);

    nTemp = (WSEC_INT32)pstRkCfg->ulRootKeyLifeDays;
    return_oper_if(nTemp < 1, WSEC_LOG_E1("'ulRootKeyLifeDays'(%d) must great than 0.", nTemp), WSEC_ERR_INVALID_ARG);

    KMC_PRI_Lock(KMC_LOCK_CFG);
    do
    {
        nErrCode = (WSEC_MEMCPY(&g_pKmcCfg->stRkCfg, sizeof(KMC_CFG_ROOT_KEY_STRU), pstRkCfg, sizeof(KMC_CFG_ROOT_KEY_STRU)) == EOK) ? WSEC_SUCCESS : WSEC_ERR_MEMCPY_FAIL;
        if ((WSEC_SUCCESS == nErrCode) && (KMC_IS_MAN_CFG_FILE))
        {
            nErrCode = KMC_PRI_WriteKcfSafety(g_pKmcCfg);
            bNtf = (nErrCode != WSEC_SUCCESS);
        }
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_CFG);

    if_oper(bNtf, KMC_PRI_NtfWriCfgFileFail(nErrCode));

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_SetKeyManCfg
 功能描述  : 设置Key管理相关配置
 纯 入 参  : pstKmCfg: Key生命周期配置数据
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月10日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_SetKeyManCfg(const KMC_CFG_KEY_MAN_STRU* pstKmCfg)
{
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    WSEC_BOOL bNtf = WSEC_FALSE;

    return_err_if_kmc_not_work;
    return_err_if_para_invalid("KMC_SetKeyManCfg", pstKmCfg);
    return_err_if_para_invalid("KMC_SetKeyManCfg", KMC_CFG_IS_KEY_MAN_VALID(pstKmCfg));

    KMC_PRI_Lock(KMC_LOCK_CFG);
    do
    {
        nErrCode = (WSEC_MEMCPY(&g_pKmcCfg->stKmCfg, sizeof(KMC_CFG_KEY_MAN_STRU), pstKmCfg, sizeof(KMC_CFG_KEY_MAN_STRU)) == EOK) ? WSEC_SUCCESS : WSEC_ERR_MEMCPY_FAIL;
        if ((WSEC_SUCCESS == nErrCode) && (KMC_IS_MAN_CFG_FILE))
        {
            nErrCode = KMC_PRI_WriteKcfSafety(g_pKmcCfg);
            bNtf = (nErrCode != WSEC_SUCCESS);
        }
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_CFG);

    if_oper(bNtf, KMC_PRI_NtfWriCfgFileFail(nErrCode));

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_GetRootKeyCfg
 功能描述  : 获取RootKey配置信息
 纯 入 参  : 无
 纯 出 参  : pstRkCfg: RK配置数据[若CBB内部生成RK]
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月10日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_GetRootKeyCfg(KMC_CFG_ROOT_KEY_STRU* pstRkCfg)
{
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    
    return_err_if_kmc_not_work;
    if (!KMC_IS_INNER_CREATE_RK) {return WSEC_ERR_KMC_RK_GENTYPE_REJECT_THE_OPER;}
    return_err_if_para_invalid("KMC_GetRootKeyCfg", pstRkCfg);

    KMC_PRI_Lock(KMC_LOCK_CFG);
    do
    {
        break_oper_if(WSEC_MEMCPY(pstRkCfg, sizeof(KMC_CFG_ROOT_KEY_STRU), &g_pKmcCfg->stRkCfg, sizeof(KMC_CFG_ROOT_KEY_STRU)) != EOK, 
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_CFG);
    
    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_GetKeyManCfg
 功能描述  : 获取密钥生命周期管理配置信息
 纯 入 参  : 无
 纯 出 参  : pstRkCfg: RK配置数据[若CBB内部生成RK]
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月10日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_GetKeyManCfg(KMC_CFG_KEY_MAN_STRU* pstKmCfg)
{
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    return_err_if_kmc_not_work;
    return_err_if_para_invalid("KMC_GetKeyManCfg", pstKmCfg);

    KMC_PRI_Lock(KMC_LOCK_CFG);
    do
    {
        break_oper_if(WSEC_MEMCPY(pstKmCfg, sizeof(KMC_CFG_KEY_MAN_STRU), &g_pKmcCfg->stKmCfg, sizeof(KMC_CFG_KEY_MAN_STRU)) != EOK, 
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_CFG);

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_AddDomain
 功能描述  : 增加Domain配置
 纯 入 参  : pstDomain: Domain配置数据
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月10日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_AddDomain(const KMC_CFG_DOMAIN_INFO_STRU* pstDomain)
{
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    WSEC_BOOL bNtf = WSEC_FALSE;
    WSEC_INT32 i;
    KMC_MEM_MK_STRU* pMemMk = WSEC_NULL_PTR;
    
    return_err_if_kmc_not_work;
    return_err_if_para_invalid("KMC_AddDomain", pstDomain);
    return_err_if_domain_privacy(pstDomain->ulId);
    return_oper_if(!KMC_IS_VALID_MK_FROM(pstDomain->ucKeyFrom), WSEC_LOG_E("'ucKeyFrom' invalid."), WSEC_ERR_INVALID_ARG);

    KMC_PRI_Lock(KMC_LOCK_BOTH);
    do
    {
        /* 检查该Domain下是否残留MK */
        for (i = 0; i < WSEC_ARR_GetCount(g_pKeystore->arrMk); i++)
        {
            pMemMk = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(g_pKeystore->arrMk, i);
            break_oper_if(!pMemMk, WSEC_LOG_E("MK array corrupted"), nErrCode = WSEC_ERR_OPER_ARRAY_FAIL);
            break_oper_if(pMemMk->stMkInfo.ulDomainId > pstDomain->ulId, oper_null, oper_null); /* 因为MK数组按DomainId升序排列 */
            continue_if(pMemMk->stMkInfo.ulDomainId != pstDomain->ulId);
            break_oper_if(pMemMk->stMkInfo.ucGenStyle != pstDomain->ucKeyFrom,
                          WSEC_LOG_E2("MK(DomainId=%d, KeyId=%d) remained, but It's Keyfrom inconsistent with the Domain.", pMemMk->stMkInfo.ulDomainId, pMemMk->stMkInfo.ulKeyId),
                          nErrCode = WSEC_ERR_KMC_ADD_DOMAIN_DISCREPANCY_MK);
        }
        break_oper_if(nErrCode != WSEC_SUCCESS, oper_null, oper_null);

        nErrCode = KMC_PRI_AddDomain2Array(g_pKmcCfg, pstDomain);
        if ((WSEC_SUCCESS == nErrCode) && (KMC_IS_MAN_CFG_FILE))
        {
            nErrCode = KMC_PRI_WriteKcfSafety(g_pKmcCfg);
            bNtf = (nErrCode != WSEC_SUCCESS);
        }
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_BOTH);

    if_oper(bNtf, KMC_PRI_NtfWriCfgFileFail(nErrCode));

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_RmvDomain
 功能描述  : 删除Domain
 纯 入 参  : ulDomainId: Domain唯一标识
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月10日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_RmvDomain(WSEC_UINT32 ulDomainId)
{
    WSEC_INT32 nAt = -1;
    KMC_DOMAIN_CFG_STRU stDomain = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    WSEC_BOOL bNtf = WSEC_FALSE;

    return_err_if_kmc_not_work;
    return_err_if_domain_privacy(ulDomainId);

    KMC_PRI_Lock(KMC_LOCK_CFG);
    do
    {
        stDomain.stDomainInfo.ulId = ulDomainId;
        nAt = WSEC_ARR_BinarySearchAt(g_pKmcCfg->arrDomainCfg, &stDomain);
        break_oper_if(nAt < 0, WSEC_LOG_W1("The domain(Id=%u) not existed", ulDomainId), nErrCode = WSEC_ERR_KMC_DOMAIN_MISS);

        WSEC_ARR_RemoveAt(g_pKmcCfg->arrDomainCfg, nAt);
        WSEC_LOG_W1("Removed the domain(Id=%u).", ulDomainId);

        if (KMC_IS_MAN_CFG_FILE)
        {
            nErrCode = KMC_PRI_WriteKcfSafety(g_pKmcCfg);
            bNtf = (nErrCode != WSEC_SUCCESS);
        }
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_CFG);

    if_oper(bNtf, KMC_PRI_NtfWriCfgFileFail(nErrCode));

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_AddDomainKeyType
 功能描述  : 向指定Domain下增加KeyType
 纯 入 参  : ulDomainId: Domain表示
             pstKeyType: KeyType配置
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_AddDomainKeyType(WSEC_UINT32 ulDomainId, const KMC_CFG_KEY_TYPE_STRU* pstKeyType)
{
    KMC_DOMAIN_CFG_STRU* pDomain = WSEC_NULL_PTR;
    KMC_CFG_KEY_TYPE_STRU* pTempKeyType = WSEC_NULL_PTR;
    WSEC_INT32 nTemp = 0;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    WSEC_BOOL bNtf = WSEC_FALSE;
    
    return_err_if_kmc_not_work;
    return_err_if_domain_privacy(ulDomainId);
    return_err_if_para_invalid("KMC_AddDomainKeyType", pstKeyType);

    return_oper_if(!KMC_IS_VALID_KEY_TYPE(pstKeyType->usKeyType), WSEC_LOG_E1("Input pstKeyType->usKeyType(%d) invalid.", pstKeyType->usKeyType), WSEC_ERR_INVALID_ARG);
    return_oper_if(!KMC_IS_VALID_KEY_TYPE_LEN(pstKeyType->ulKeyLen), 
                   WSEC_LOG_E2("Input pstKeyType->ulKeyLen(%d) invalid. it must not over %u", pstKeyType->ulKeyLen, g_KmcSys.ulMkPlainLenMax), 
                   WSEC_ERR_INVALID_ARG);
    nTemp = (WSEC_INT32)pstKeyType->ulKeyLifeDays;
    return_oper_if(nTemp < 1, WSEC_LOG_E1("'KeyLifeDays'(%d) must great than 0", nTemp), WSEC_ERR_INVALID_ARG);

    return_oper_if(KMC_PRI_SearchDomainKeyTypeCfg(ulDomainId, pstKeyType->usKeyType, &pDomain, &pTempKeyType),
                   WSEC_LOG_E2("The KeyType(DomainId=%u, KeyType=%u) already exist.", ulDomainId, pstKeyType->usKeyType), WSEC_ERR_KMC_ADD_REPEAT_KEY_TYPE);

    KMC_PRI_Lock(KMC_LOCK_CFG);
    do
    {
        nErrCode = KMC_PRI_AddDomainKeyType2Array(g_pKmcCfg, ulDomainId, pstKeyType);
        if ((WSEC_SUCCESS == nErrCode) && (KMC_IS_MAN_CFG_FILE))
        {
            nErrCode = KMC_PRI_WriteKcfSafety(g_pKmcCfg);
            bNtf = (nErrCode != WSEC_SUCCESS);
        }
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_CFG);

    if_oper(bNtf, KMC_PRI_NtfWriCfgFileFail(nErrCode));

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_RmvDomainKeyType
 功能描述  : 删除指定Domain下的KeyType
 纯 入 参  : ulDomainId, usKeyType: KeyType的唯一标识
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_RmvDomainKeyType(WSEC_UINT32 ulDomainId, WSEC_UINT16 usKeyType)
{
    KMC_DOMAIN_CFG_STRU stFindDomain = {0};
    KMC_CFG_KEY_TYPE_STRU   stFindKeyType = {0};
    KMC_DOMAIN_CFG_STRU* pDomainCfg = WSEC_NULL_PTR;
    WSEC_INT32 nIndex = -1;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    WSEC_BOOL bNtf = WSEC_FALSE;
    
    return_err_if_kmc_not_work;
    return_err_if_domain_privacy(ulDomainId);

    KMC_PRI_Lock(KMC_LOCK_CFG);
    do
    {
        /* 查Domain */
        stFindDomain.stDomainInfo.ulId = ulDomainId;
        nIndex = WSEC_ARR_BinarySearchAt(g_pKmcCfg->arrDomainCfg, &stFindDomain);
        break_oper_if(nIndex < 0, WSEC_LOG_E1("The Domain(Id=%u) not exist.", ulDomainId), nErrCode = WSEC_ERR_KMC_DOMAIN_MISS);

        pDomainCfg = (KMC_DOMAIN_CFG_STRU*)WSEC_ARR_GetAt(g_pKmcCfg->arrDomainCfg, nIndex);
        break_oper_if(!pDomainCfg, WSEC_LOG_E("memory access fail."), nErrCode = WSEC_ERR_OPER_ARRAY_FAIL);

        /* 查Domain下的KeyType位置 */
        stFindKeyType.usKeyType = usKeyType;
        nIndex = WSEC_ARR_BinarySearchAt(pDomainCfg->arrKeyTypeCfg, &stFindKeyType);
        break_oper_if(nIndex < 0, WSEC_LOG_E2("The KeyType(DomainId=%u, KeyType=%u) not exist.", ulDomainId, usKeyType), nErrCode = WSEC_ERR_KMC_DOMAIN_KEYTYPE_MISS);

        /* 删除 */
        WSEC_ARR_RemoveAt(pDomainCfg->arrKeyTypeCfg, nIndex);

        WSEC_LOG_E2("Remove KeyType(DomainId=%u, KeyType=%u) succ.", ulDomainId, usKeyType);

        if (KMC_IS_MAN_CFG_FILE)
        {
            nErrCode = KMC_PRI_WriteKcfSafety(g_pKmcCfg);
            bNtf = (nErrCode != WSEC_SUCCESS);
        }
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_CFG);

    if_oper(bNtf, KMC_PRI_NtfWriCfgFileFail(nErrCode));
    
    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_GetDomainCount
 功能描述  : 获取配置的Domain个数
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 0或正数: 配置的Domain个数;
                负数: 出错
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_INT32 KMC_GetDomainCount()
{
    WSEC_INT32 nCount = 0;
    
    if (g_KmcSys.eState != WSEC_RUNNING) {return -1;}

    KMC_PRI_Lock(KMC_LOCK_CFG);
    nCount = WSEC_ARR_GetCount(g_pKmcCfg->arrDomainCfg);
    KMC_PRI_Unlock(KMC_LOCK_CFG);
    
    return nCount;
}

/*****************************************************************************
 函 数 名  : KMC_GetDomain
 功能描述  : 获取指定索引的Domain
 纯 入 参  : Index: Domain所在位置(0开始的索引)
 纯 出 参  : pstDomainInfo: 输出Domain配置信息
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 该函数一般和KMC_GetDomainCount配合使用, 如:
             ...
             KMC_CFG_DOMAIN_INFO_STRU stDomainInfo;
             int i;
             
             for (i = 0; i < KMC_GetDomainCount(); i++)
             {
                if (KMC_GetDomain(i, &stDomainInfo) == WSEC_SUCCESS)
                {
                    // 使用stDomainInfo
                }
             }
             ...
 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_GetDomain(WSEC_INT32 Index, KMC_CFG_DOMAIN_INFO_STRU* pstDomainInfo)
{
    KMC_DOMAIN_CFG_STRU* pDomainCfg = WSEC_NULL_PTR;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
 
    return_err_if_kmc_not_work;
    return_err_if_para_invalid("KMC_GetDomain", (Index >= 0) && pstDomainInfo);

    KMC_PRI_Lock(KMC_LOCK_CFG);
    do
    {
        break_oper_if(Index >= WSEC_ARR_GetCount(g_pKmcCfg->arrDomainCfg), oper_null, nErrCode = WSEC_ERR_INVALID_ARG);
        
        pDomainCfg = (KMC_DOMAIN_CFG_STRU*)WSEC_ARR_GetAt(g_pKmcCfg->arrDomainCfg, Index);
        break_oper_if(!pDomainCfg, WSEC_LOG_E("memory access fail."), nErrCode = WSEC_ERR_OPER_ARRAY_FAIL);

        break_oper_if(WSEC_MEMCPY(pstDomainInfo, sizeof(KMC_CFG_DOMAIN_INFO_STRU), &pDomainCfg->stDomainInfo, sizeof(KMC_CFG_DOMAIN_INFO_STRU)) != EOK, 
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL); 
     }do_end;
    KMC_PRI_Unlock(KMC_LOCK_CFG);

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_GetDomainKeyTypeCount
 功能描述  : 获取Domain下KeyType个数
 纯 入 参  : ulDomainId: Domain唯一标识
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 0或正数: 配置的Domain KeyType个数;
                负数: 出错
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_INT32 KMC_GetDomainKeyTypeCount(WSEC_UINT32 ulDomainId)
{
    KMC_DOMAIN_CFG_STRU stFindDomain = {0};
    KMC_DOMAIN_CFG_STRU* pDomain = WSEC_NULL_PTR;
    WSEC_INT32 nReturn = -1;
    
    if (g_KmcSys.eState != WSEC_RUNNING) {return nReturn;}

    stFindDomain.stDomainInfo.ulId = ulDomainId;

    KMC_PRI_Lock(KMC_LOCK_CFG);
    pDomain = (KMC_DOMAIN_CFG_STRU*)WSEC_ARR_BinarySearch(g_pKmcCfg->arrDomainCfg, &stFindDomain);
    if (pDomain) {nReturn = WSEC_ARR_GetCount(pDomain->arrKeyTypeCfg);}
    KMC_PRI_Unlock(KMC_LOCK_CFG);

    return nReturn;
}

/*****************************************************************************
 函 数 名  : KMC_GetDomainKeyType
 功能描述  : 获取指定Domain指定索引的KeyType
 纯 入 参  : ulDomainId: Domain唯一标识
             Index: 待查询KeyType所在位置(0开始的索引)
 纯 出 参  : pstDomainKeyType: 输出KeyType
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_GetDomainKeyType(WSEC_UINT32 ulDomainId, WSEC_INT32 Index, KMC_CFG_KEY_TYPE_STRU* pstKeyType)
{
    KMC_DOMAIN_CFG_STRU stFindDomain = {0};
    KMC_DOMAIN_CFG_STRU* pDomain = WSEC_NULL_PTR;
    KMC_CFG_KEY_TYPE_STRU* pstExistKeyType = WSEC_NULL_PTR;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    
    return_err_if_kmc_not_work;
    return_err_if_para_invalid("KMC_GetDomainKeyType", pstKeyType);

    KMC_PRI_Lock(KMC_LOCK_CFG);
    do
    {
        stFindDomain.stDomainInfo.ulId = ulDomainId;
        pDomain = (KMC_DOMAIN_CFG_STRU*)WSEC_ARR_BinarySearch(g_pKmcCfg->arrDomainCfg, &stFindDomain);
        break_oper_if(!pDomain, WSEC_LOG_E1("Domain(Id=%u) not exist.", ulDomainId), nErrCode = WSEC_ERR_KMC_DOMAIN_MISS);

        pstExistKeyType = (KMC_CFG_KEY_TYPE_STRU*)WSEC_ARR_GetAt(pDomain->arrKeyTypeCfg, Index);
        break_oper_if(!pstExistKeyType, WSEC_LOG_E1("Get KeyType at %d fail.", Index), nErrCode = WSEC_ERR_KMC_DOMAIN_KEYTYPE_MISS);

        break_oper_if(WSEC_MEMCPY(pstKeyType, sizeof(KMC_CFG_KEY_TYPE_STRU), pstExistKeyType, sizeof(KMC_CFG_KEY_TYPE_STRU)) != EOK,
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_CFG);

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_GetExpiredMkStartPos
 功能描述  : 获取过期MK的首位置
 纯 入 参  : 无
 纯 出 参  : pPos: 输出MK首个过期密钥的位置
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_GetExpiredMkStartPos(WSEC_POSITION* pPos)
{
    WSEC_INT32 i;
    KMC_MEM_MK_STRU* pMk = WSEC_NULL_PTR;

    *pPos = -1;
    if (g_KmcSys.eState != WSEC_RUNNING) {return;}
    WSEC_CallPeriodicFunc(KMC_ChkMkStatus); /* 获取过期密钥之前, 需要检查密钥状态 */

    KMC_PRI_Lock(KMC_LOCK_KEYSTORE);
    for (i = 0; i < WSEC_ARR_GetCount(g_pKeystore->arrMk); i++)
    {
        pMk = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(g_pKeystore->arrMk, i);
        continue_if(!pMk);
        if (KMC_KEY_STATUS_INACTIVE == pMk->stMkInfo.ucStatus)
        {
            *pPos = i;
            break;
        }
    }
    KMC_PRI_Unlock(KMC_LOCK_KEYSTORE);

    return;
}

/*****************************************************************************
 函 数 名  : KMC_GetExpiredMkByPos
 功能描述  : 获取指定位置上的过期MK并输出下一个过期MK的位置
 纯 入 参  : 无
 纯 出 参  : pstExpiredMk: 输出过期MK
 入参出参  : pPosNow: [in]当前位置的过期密钥
                      [out]下一个过期密钥的位置
 返 回 值  : 获取当前位置上的密钥是否成功
 特别注意  : 该函数和KMC_GetExpiredMkStartPos结合使用, 例如:
              ......
              WSEC_POSITION pos;
              KMC_MK_INFO_STRU stExpiredMk;

              KMC_GetExpiredMkStartPos(&pos);
              while (KMC_GetExpiredMkByPos(&pos, &stExpiredMk))
              {
                  <使用stExpiredMk的数据>
              }
              ......

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL KMC_GetExpiredMkByPos(WSEC_POSITION* pPosNow, KMC_MK_INFO_STRU* pstExpiredMk)
{
    WSEC_INT32 i = 0, nCount = 0;
    KMC_MEM_MK_STRU* pMk = WSEC_NULL_PTR;
    WSEC_BOOL bOk = WSEC_TRUE;
    
    if (g_KmcSys.eState != WSEC_RUNNING) {return WSEC_FALSE;}
    if ((!pPosNow) || (!pstExpiredMk)) {return WSEC_FALSE;}

    KMC_PRI_Lock(KMC_LOCK_KEYSTORE);
    do
    {
        nCount = WSEC_ARR_GetCount(g_pKeystore->arrMk);

        i = *pPosNow;
        break_oper_if((i < 0) || (i >= nCount), oper_null, bOk = WSEC_FALSE);

        pMk = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(g_pKeystore->arrMk, i);
        break_oper_if(!pMk, WSEC_LOG_E("memory access fail."), bOk = WSEC_FALSE);

        break_oper_if(pMk->stMkInfo.ucStatus != KMC_KEY_STATUS_INACTIVE, oper_null, bOk = WSEC_FALSE); /* 说明调用者对pPosNow内容进行了改写, 属于编程错误 */

        break_oper_if(WSEC_MEMCPY(pstExpiredMk, sizeof(KMC_MK_INFO_STRU), &pMk->stMkInfo, sizeof(pMk->stMkInfo)) != EOK, WSEC_LOG_E4MEMCPY, bOk = WSEC_FALSE);

        /* 寻找下一数据的位置 */
        *pPosNow = -1; /* 表示没有'下一个'了 */
        for (i++; i < nCount; i++)
        {
            pMk = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(g_pKeystore->arrMk, i);
            break_oper_if(!pMk, WSEC_LOG_E("memory access fail."), bOk = WSEC_FALSE);
            if (KMC_KEY_STATUS_INACTIVE == pMk->stMkInfo.ucStatus)
            {
                *pPosNow = i;
                break;
            }
        }
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_KEYSTORE);

    return bOk;
}

/*****************************************************************************
 函 数 名  : KMC_GetMkByType
 功能描述  : ulDomainId: MK隶属Domain;
             usKeyType:  KeyType
 纯 入 参  : 无
 纯 出 参  : pKeyBuf: 输出MK密钥明文;
             pKeyId:  输出MK ID;
 入参出参  : pKeyLen: [in]缓冲区pKeyBuf的长度;
                      [out]密钥明文实际长度
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_GetMkByType(WSEC_UINT32 ulDomainId, WSEC_UINT16 usKeyType, WSEC_BYTE* pKeyBuf, WSEC_UINT32 *pKeyLen, WSEC_UINT32 *pKeyId)
{
    KMC_MEM_MK_STRU stMkFind = {0};
    KMC_MEM_MK_STRU* pstMkFound = WSEC_NULL_PTR;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    
    return_err_if_kmc_not_work;
    return_err_if_domain_privacy(ulDomainId);
    return_err_if_para_invalid("KMC_GetMkByType", pKeyBuf && pKeyLen && pKeyId);

    stMkFind.stMkInfo.ulDomainId = ulDomainId;
    stMkFind.stMkInfo.usType     = usKeyType;
    stMkFind.stMkInfo.ucStatus   = KMC_KEY_STATUS_ACTIVE; /* 查找那些可用的MK */

    KMC_PRI_Lock(KMC_LOCK_KEYSTORE);
    do
    {
        pstMkFound = WSEC_ARR_BinarySearch(g_pKeystore->arrMk, &stMkFind);
        break_oper_if(!pstMkFound, WSEC_LOG_E2("Cannot find Active-MK(DomainId=%u, KeyType=%u)", ulDomainId, usKeyType), nErrCode = WSEC_ERR_KMC_MK_MISS);
        break_oper_if(*pKeyLen < pstMkFound->stMkRear.ulPlainLen, 
                       WSEC_LOG_E2("The buffer-len of pKeyBuf is too small(%d < %d).", *pKeyLen, pstMkFound->stMkRear.ulPlainLen), 
                       nErrCode = WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH);

        *pKeyId = pstMkFound->stMkInfo.ulKeyId;
        *pKeyLen = pstMkFound->stMkRear.ulPlainLen;
        KMC_UNMASK_MK_TO(pstMkFound, pKeyBuf, *pKeyLen); /* MK明文是以掩码方式存在于内存，需去掩 */
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_KEYSTORE);
    
    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_SetDataProtectCfg
 功能描述  : 设置敏感数据保护的算法配置
 纯 入 参  : eType: 算法类型;
             pstPara: 对应的敏感数据保护算法配置
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_SetDataProtectCfg(KMC_SDP_ALG_TYPE_ENUM eType, const KMC_CFG_DATA_PROTECT_STRU* pstPara)
{
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    WSEC_BOOL bNtf = WSEC_FALSE;
    
    return_err_if_kmc_not_work;

    nErrCode = KMC_PRI_ChkProtectCfg(eType, pstPara);
    return_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E("KMC_PRI_ChkProtectCfg() return fail."), nErrCode);

    KMC_PRI_Lock(KMC_LOCK_CFG);
    do
    {
        break_oper_if(WSEC_MEMCPY(&g_pKmcCfg->astDataProtectCfg[eType], sizeof(KMC_CFG_DATA_PROTECT_STRU), pstPara, sizeof(KMC_CFG_DATA_PROTECT_STRU)) != EOK,
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
        if (KMC_IS_MAN_CFG_FILE)
        {
            nErrCode = KMC_PRI_WriteKcfSafety(g_pKmcCfg);
            bNtf = (nErrCode != WSEC_SUCCESS);
        }
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_CFG);

    if_oper(bNtf, KMC_PRI_NtfWriCfgFileFail(nErrCode));

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : KMC_GetDataProtectCfg
 功能描述  : 获取指定算法类型的配置
 纯 入 参  : eType: 算法类型;
 纯 出 参  : pstPara: 输出算法配置
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T KMC_GetDataProtectCfg(KMC_SDP_ALG_TYPE_ENUM eType, KMC_CFG_DATA_PROTECT_STRU *pstPara)
{
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    return_err_if_kmc_not_work;
    return_err_if_para_invalid("KMC_GetDataProtectCfg", pstPara && WSEC_IS3(eType, SDP_ALG_ENCRPT, SDP_ALG_INTEGRITY, SDP_ALG_PWD_PROTECT));

    KMC_PRI_Lock(KMC_LOCK_CFG);
    do
    {
        break_oper_if(WSEC_MEMCPY(pstPara, sizeof(KMC_CFG_DATA_PROTECT_STRU), &g_pKmcCfg->astDataProtectCfg[eType], sizeof(KMC_CFG_DATA_PROTECT_STRU)) != EOK, 
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_CFG);
    
    return nErrCode;
}

/*****************************************************************************
> 函 数 名  : KMC_GetAlgList
> 功能描述  : 查询SDP组件支持的算法全集，以回调函数方式实现。
> 输入参数  : pfProcAlg 表示处理单个算法相关信息的回调函数，该回调函数将在本函数遍历每个算法时调用
>             pReserved 表示保留参数，用来传递回调函数的输入参数信息
> 输出参数  : pReserved 表示保留参数，用来传递回调函数的输出参数信息
> 返 回 值  : 错误码，WSEC_SUCCESS表示函数正常返回。

> 修改历史      :
>  1.日    期   : 2014年7月1日
>    作    者   : j00265291
>    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T KMC_GetAlgList(
    KMC_FP_ProcAlg pfProcAlg,
    INOUT WSEC_VOID* pReserved
    )
{
    WSEC_UINT32 ulRet = WSEC_SUCCESS;

    /* check input */
    return_err_if_para_invalid("KMC_GetAlgList", pfProcAlg);

    ulRet = CAC_GetAlgList((CAC_FP_ProcAlg)pfProcAlg, pReserved);
    return_oper_if((WSEC_SUCCESS != ulRet), WSEC_LOG_E("CAC_GetAlgList() fail."), ulRet);

    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : KMC_RefreshMkMask
 功能描述  : 生成用于对MK明文加掩的随机数, 并对MK密钥重新加掩
 纯 入 参  : 3个参数均未使用(仅因注册函数类型需要)
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月24日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_RefreshMkMask(const WSEC_SYSTIME_T* pstLocalNow, const WSEC_SYSTIME_T* pstUtcNow, const WSEC_PROGRESS_RPT_STRU* pstRptProgress, WSEC_EXEC_INFO_STRU* pExecInfo)
{
    KMC_MEM_MK_STRU* pstMk = WSEC_NULL_PTR;
    WSEC_INT32 i;

    WSEC_UNREFER(pstLocalNow);
    WSEC_UNREFER(pstUtcNow);
    WSEC_UNREFER(pstRptProgress);
    WSEC_UNREFER(pExecInfo);

    if (g_KmcSys.eState != WSEC_RUNNING) {return;}

    KMC_PRI_Lock(KMC_LOCK_KEYSTORE);
    do
    {
        /* 1. 用旧掩码还原 */
        for (i = 0; i < WSEC_ARR_GetCount(g_pKeystore->arrMk); i++)
        {
            pstMk = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(g_pKeystore->arrMk, i);
            continue_if(!pstMk);
            KMC_UNMASK_MK(pstMk);
        }

        /* 2. 刷新掩码 */
        WSEC_UNCARE(CAC_Random(g_KmcSys.abMkMaskCode, sizeof(g_KmcSys.abMkMaskCode))); /* 不需关注随机数生成成功与否 */

        /* 3. 用新掩码加掩 */
        for (i = 0; i < WSEC_ARR_GetCount(g_pKeystore->arrMk); i++)
        {
            pstMk = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(g_pKeystore->arrMk, i);
            continue_if(!pstMk);
            KMC_MASK_MK(pstMk);
        }
    } do_end;
    KMC_PRI_Unlock(KMC_LOCK_KEYSTORE);

    return;
}

/*****************************************************************************
 函 数 名  : KMC_ChkRkStatus
 功能描述  : 检查Root Key是否过期
 纯 入 参  : pstLocalNow: 当前本地时间;
             pstUtcNow:   当前UTC时间;
 纯 出 参  : 无
 入参出参  : pExecInfo:   记录执行信息
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_ChkRkStatus(const WSEC_SYSTIME_T* pstLocalNow, const WSEC_SYSTIME_T* pstUtcNow, const WSEC_PROGRESS_RPT_STRU* pstRptProgress, WSEC_EXEC_INFO_STRU* pExecInfo)
{
    const KMC_RK_ATTR_STRU* pstRkInfo = WSEC_NULL_PTR;
    const KMC_CFG_KEY_MAN_STRU* pstCfg = WSEC_NULL_PTR;
    KMC_RK_ATTR_STRU* pstRkNtf = WSEC_NULL_PTR;
    WSEC_INT32 nRemainLifeDays = 0; /* 本时刻距密钥过期的天数 */
    WSEC_CHAR szTime[30] = {0};

	WSEC_UNREFER(pstRptProgress);

    if_oper(g_KmcSys.eState != WSEC_RUNNING, return);
    WSEC_ASSERT(pstLocalNow && pstUtcNow && pExecInfo);

    KMC_PRI_Lock(KMC_LOCK_BOTH);
    do
    {
        if (!KMC_PRI_IsTime4ChkKey(pstLocalNow, &g_pKmcCfg->stKmCfg, pExecInfo)) {break;} /* 时机未到 */

        WSEC_DateTimeCopy(&pExecInfo->stPreExecTime, pstLocalNow); /* 记录检测时刻 */

        pstRkInfo = &g_pKeystore->stRkInfo;
        pstCfg    = &g_pKmcCfg->stKmCfg;

        /* 检查是否预警 */
        nRemainLifeDays = WSEC_DateTimeDiff(dtpDay, pstUtcNow, &pstRkInfo->stRkExpiredTimeUtc);

        if (nRemainLifeDays > (WSEC_INT32)pstCfg->ulWarningBeforeKeyExpiredDays) {break;}

        WSEC_LOG_E1("WARNING: Root key will expire at %s(UTC)", WSEC_DateTime2String(&pstRkInfo->stRkExpiredTimeUtc, szTime, sizeof(szTime)));

        if (nRemainLifeDays <= 0) /* 密钥已经过期,  */
        {
            WSEC_LOG_W1("Root key expired, KMC_PRI_UpdateRootKeyPri()=%u", KMC_PRI_UpdateRootKeyPri(WSEC_NULL_PTR, 0, KMC_NOT_LOCK));
        }
        else
        {
            pstRkNtf = WSEC_CLONE_BUFF(pstRkInfo, sizeof(KMC_RK_ATTR_STRU));
        }
    }do_end;
    KMC_PRI_Unlock(KMC_LOCK_BOTH);

    if (pstRkNtf)
    {
        KMC_PRI_NtfRkExpire(pstRkNtf, nRemainLifeDays);
        WSEC_FREE(pstRkNtf);
    }

    return;
}

/*****************************************************************************
 函 数 名  : KMC_ChkMkStatus
 功能描述  : 检查所有Master Key是否过期
 纯 入 参  : pstLocalNow: 当前本地时间;
             pstUtcNow:   当前UTC时间;
 纯 出 参  : 无
 入参出参  : pExecInfo:   记录执行信息
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年11月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_ChkMkStatus(const WSEC_SYSTIME_T* pstLocalNow, const WSEC_SYSTIME_T* pstUtcNow, const WSEC_PROGRESS_RPT_STRU* pstRptProgress, WSEC_EXEC_INFO_STRU* pExecInfo)
{
    WSEC_INT32 nMkNum = 0, i = 0, nRemainLifeDays = 0, nRemainMkNum;
    KMC_MEM_MK_STRU* pMemMk = WSEC_NULL_PTR;
    KMC_MK_INFO_STRU* pMk = WSEC_NULL_PTR;
    KMC_MK_EXPIRE_NTF_STRU* pExpireNtf = WSEC_NULL_PTR;
    KMC_MK_NUM_OVERFLOW_STRU stMkOverflowNtf = {0};
    WSEC_ARRAY arrExpiredMk = WSEC_NULL_PTR, arrWarnMk = WSEC_NULL_PTR;
    WSEC_SPEND_TIME_STRU stTimer = {0};
    WSEC_BOOL bIsTime4ChkKey = WSEC_FALSE;
    const WSEC_UINT32 ulRemainMkNumPercent = 10; /* 剩余MK个数指标占总规格的百分比 */
    WSEC_ERR_T ulErrCode = WSEC_SUCCESS;

    if_oper(g_KmcSys.eState != WSEC_RUNNING, return);
    WSEC_ASSERT(pstLocalNow && pstUtcNow && pExecInfo);

    /* 1. 检测时机是否到? */
    KMC_PRI_Lock(KMC_LOCK_CFG);
    bIsTime4ChkKey = KMC_PRI_IsTime4ChkKey(pstLocalNow, &g_pKmcCfg->stKmCfg, pExecInfo);
    KMC_PRI_Unlock(KMC_LOCK_CFG);
    if (!bIsTime4ChkKey) {return;} /* 时机未到 */

    arrExpiredMk = WSEC_ARR_Initialize(0, 0, WSEC_NULL_PTR, WSEC_NULL_PTR);
    arrWarnMk = WSEC_ARR_Initialize(0, 0, WSEC_NULL_PTR, WSEC_ARR_StdRemoveElement);
    if ((!arrExpiredMk) || (!arrWarnMk))
    {
        WSEC_LOG_E("WSEC_ARR_Initialize() fail");
        if_oper(arrExpiredMk, WSEC_ARR_Finalize(arrExpiredMk));
        if_oper(arrWarnMk, WSEC_ARR_Finalize(arrWarnMk));
        return;
    }else{;}
    
    /* 2. 获取有哪些MK变更为过期了 */
    KMC_PRI_Lock(KMC_LOCK_BOTH);
    nMkNum = WSEC_ARR_GetCount(g_pKeystore->arrMk);
    for (i = 0; i < nMkNum; i++)
    {
        pMemMk = WSEC_ARR_GetAt(g_pKeystore->arrMk, i);
        continue_if(!pMemMk);

        pMk = &pMemMk->stMkInfo;
        continue_if(pMk->ucStatus != KMC_KEY_STATUS_ACTIVE);

        nRemainLifeDays = WSEC_DateTimeDiff(dtpDay, pstUtcNow, &pMk->stMkExpiredTimeUtc);
        continue_if((nRemainLifeDays > 0) && ((WSEC_UINT32)nRemainLifeDays > g_pKmcCfg->stKmCfg.ulWarningBeforeKeyExpiredDays));
        
        if ((nRemainLifeDays <= 0) && (KMC_MK_GEN_BY_INNER == pMk->ucGenStyle)) /* 内部密钥过期 */
        {
            WSEC_ARR_Add(arrExpiredMk, pMk);
        }
        else /* 外部密钥, 或者只需预警的内部密钥 */
        {
            pExpireNtf = (KMC_MK_EXPIRE_NTF_STRU*)WSEC_MALLOC(sizeof(KMC_MK_EXPIRE_NTF_STRU));
            break_oper_if(!pExpireNtf, WSEC_LOG_E4MALLOC(sizeof(KMC_MK_EXPIRE_NTF_STRU)), oper_null);
            break_oper_if(WSEC_MEMCPY(&pExpireNtf->stMkInfo, sizeof(pExpireNtf->stMkInfo), pMk, sizeof(KMC_MK_INFO_STRU)) != EOK, WSEC_LOG_E4MEMCPY, oper_null);
            pExpireNtf->nRemainDays = nRemainLifeDays;

            WSEC_UNCARE(WSEC_ARR_Add(arrWarnMk, pExpireNtf));
        }

        WSEC_RptProgress(pstRptProgress, &stTimer, (WSEC_UINT32)nMkNum, (WSEC_UINT32)(i + 1), WSEC_NULL_PTR); /* 不响应取消 */
    }

    /* 3. 处理过期MK */
    nMkNum = WSEC_ARR_GetCount(arrExpiredMk);
    if (nMkNum > 0)
    {
        for (i = 0; i < nMkNum; i++)
        {
            pMk = (KMC_MK_INFO_STRU*)WSEC_ARR_GetAt(arrExpiredMk, i);
            continue_if(!pMk);

            if (KMC_MK_GEN_BY_INNER == pMk->ucGenStyle) /* 内部MK, 需要重新创建, 对于外部MK则不做理会. */
            {
                ulErrCode = KMC_PRI_ReCreateMkItem(g_pKeystore, pMk);
                WSEC_LOG_E3("OnTiner: The MK(DomainId=%u, KeyId=%u) expired, ReCreateMk()=%u", pMk->ulDomainId, pMk->ulKeyId, ulErrCode);
            }
            WSEC_RptProgress(pstRptProgress, &stTimer, (WSEC_UINT32)nMkNum, (WSEC_UINT32)(i + 1), WSEC_NULL_PTR); /* 不响应取消 */
        }
        WSEC_ARR_QuickSort(g_pKeystore->arrMk);
        ulErrCode = KMC_PRI_WriteKsfSafety(g_pKeystore, WSEC_NULL_PTR);

        if (ulErrCode != WSEC_SUCCESS) {WSEC_LOG_E1("KMC_PRI_WriteKsfSafety()=%u", ulErrCode);}
    }
    stMkOverflowNtf.ulNum = WSEC_ARR_GetCount(g_pKeystore->arrMk);
    KMC_PRI_Unlock(KMC_LOCK_BOTH);
    WSEC_RptProgress(pstRptProgress, WSEC_NULL_PTR, (WSEC_UINT32)nMkNum, (WSEC_UINT32)nMkNum, WSEC_NULL_PTR); /* 确保进度100%上报 */

    /* 4. 发预警 */
    for (i = 0; i < WSEC_ARR_GetCount(arrWarnMk); i++)
    {
        pExpireNtf = (KMC_MK_EXPIRE_NTF_STRU*)WSEC_ARR_GetAt(arrWarnMk, i);
        WSEC_ASSERT(pExpireNtf);
        WSEC_NOTIFY(WSEC_KMC_NTF_MK_EXPIRE, pExpireNtf, sizeof(KMC_MK_EXPIRE_NTF_STRU));
    }

    /* 5. MK超规格告警 */
    nRemainMkNum = WSEC_MK_NUM_MAX - stMkOverflowNtf.ulNum;
    WSEC_ASSERT(nRemainMkNum >= 0); /* 如果为负数, 则KMC_PRI_ReCreateMkItem()处理逻辑错误 */
    if ((WSEC_INT32)(nRemainMkNum * 100) < (WSEC_INT32)(WSEC_MK_NUM_MAX * ulRemainMkNumPercent))
    {
        stMkOverflowNtf.ulMaxNum = WSEC_MK_NUM_MAX;
        WSEC_LOG_E2("MkNum(%d) will overflow(%d)", stMkOverflowNtf.ulNum, stMkOverflowNtf.ulMaxNum);
        WSEC_NOTIFY(WSEC_KMC_NTF_MK_NUM_OVERFLOW, &stMkOverflowNtf, sizeof(stMkOverflowNtf));
    }
    
    WSEC_ARR_Finalize(arrExpiredMk);
    WSEC_ARR_Finalize(arrWarnMk);

    /* Misinformation: FORTIFY.Memory_Leak (because 'pExpireNtf' freed when 'WSEC_ARR_Finalize(arrWarnMk)') */

    return;
}

/*****************************************************************************
 函 数 名  : KMC_Correct4Clock
 功能描述  : 校正因时间信息不正确的数据
             CBB初始化时需要访问时间, 如果这时系统时钟尚未就绪，则获取得时间信息
             不正确, 当APP检测到时钟系统正常后则告知CBB, 这时, CBB需校正时间数据.
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2015年5月25日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID KMC_Correct4Clock()
{
    const WSEC_BYTE bCorrected = 1;
    WSEC_BOOL bDirty = WSEC_FALSE;
    WSEC_SYSTIME_T tNowUtc = {0};
    KMC_MEM_MK_STRU* pMemMk = WSEC_NULL_PTR;
    KMC_MK_INFO_STRU* pMk = WSEC_NULL_PTR;
    WSEC_INT32 i = 0;
    WSEC_INT32 nDays = 0;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    if_oper(g_KmcSys.eState != WSEC_RUNNING, return);
    if_oper(bCorrected == g_KmcSys.bCorrectClockFlag, return);
    if (!WSEC_GetUtcDateTime(&tNowUtc))
    {
        WSEC_LOG_E("WSEC_GetUtcDateTime() fail");
        return;
    }
    if_oper(!WSEC_IS_NORMAL_YEAR(tNowUtc.uwYear), return); /* 时钟系统尚未就绪 */

    KMC_PRI_Lock(KMC_LOCK_KEYSTORE);
    do
    {
        if (!WSEC_IS_NORMAL_YEAR(g_pKeystore->stRkInfo.stRkCreateTimeUtc.uwYear)) /* 需要重置时间 */
        {
            nDays = WSEC_DateTimeDiff(dtpDay, &g_pKeystore->stRkInfo.stRkCreateTimeUtc, &g_pKeystore->stRkInfo.stRkExpiredTimeUtc);
            nErrCode = KMC_PRI_SetLifeTime(nDays, &g_pKeystore->stRkInfo.stRkCreateTimeUtc, &g_pKeystore->stRkInfo.stRkExpiredTimeUtc);
            break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC_PRI_SetLifeTime()=%u", nErrCode), oper_null);
            bDirty = WSEC_TRUE;
        }

        for (i = 0; i < WSEC_ARR_GetCount(g_pKeystore->arrMk); i++)
        {
            pMemMk = (KMC_MEM_MK_STRU*)WSEC_ARR_GetAt(g_pKeystore->arrMk, i);
            continue_if(!pMemMk); /* 容错: 数组受损 */

            pMk = &pMemMk->stMkInfo;
            continue_if(WSEC_IS_NORMAL_YEAR(pMk->stMkCreateTimeUtc.uwYear));

            nDays = WSEC_DateTimeDiff(dtpDay, &pMk->stMkCreateTimeUtc, &pMk->stMkExpiredTimeUtc);
            nErrCode = KMC_PRI_SetLifeTime(nDays, &pMk->stMkCreateTimeUtc, &pMk->stMkExpiredTimeUtc);
            break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC_PRI_SetLifeTime()=%u", nErrCode), oper_null);
            bDirty = WSEC_TRUE;
        }
    }do_end;
    if_oper((WSEC_SUCCESS == nErrCode) && bDirty, nErrCode = KMC_PRI_WriteKsfSafety(g_pKeystore, WSEC_NULL_PTR)); /* 保存文件 */
    KMC_PRI_Unlock(KMC_LOCK_KEYSTORE);

    if_oper(WSEC_SUCCESS == nErrCode, g_KmcSys.bCorrectClockFlag = bCorrected);
    return;
}

/*****************************************************************************
 函 数 名  : KMC_ShowStructSize
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
WSEC_VOID KMC_ShowStructSize(WSEC_FP_ShowStructSize pfShow)
{
    pfShow("KMC_MK_INFO_STRU", sizeof(KMC_MK_INFO_STRU));
    pfShow("KMC_CFG_ROOT_KEY_STRU", sizeof(KMC_CFG_ROOT_KEY_STRU));
    pfShow("KMC_CFG_KEY_MAN_STRU", sizeof(KMC_CFG_KEY_MAN_STRU));
    pfShow("KMC_CFG_DATA_PROTECT_STRU", sizeof(KMC_CFG_DATA_PROTECT_STRU));
    pfShow("KMC_CFG_KEY_TYPE_STRU", sizeof(KMC_CFG_KEY_TYPE_STRU));
    pfShow("KMC_CFG_DOMAIN_INFO_STRU", sizeof(KMC_CFG_DOMAIN_INFO_STRU));
    pfShow("KMC_RK_PARA_STRU", sizeof(KMC_RK_PARA_STRU));
    pfShow("KMC_KSF_RK_STRU", sizeof(KMC_KSF_RK_STRU));
    pfShow("KMC_MK_REAR_STRU", sizeof(KMC_MK_REAR_STRU));
    pfShow("KMC_KSF_MK_STRU", sizeof(KMC_KSF_MK_STRU));
    pfShow("KMC_MEM_MK_STRU", sizeof(KMC_MEM_MK_STRU));
    pfShow("KMC_MKF_HDR_STRU", sizeof(KMC_MKF_HDR_STRU));
    pfShow("KMC_MKF_HDR_WITH_HMAC_STRU", sizeof(KMC_MKF_HDR_WITH_HMAC_STRU));
    pfShow("KMC_MKF_MK_STRU", sizeof(KMC_MKF_MK_STRU));

    return;
}
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */


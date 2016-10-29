/******************************************************************************

版权所有 (C), 2001-2011, 华为技术有限公司

******************************************************************************
文 件 名   : KMC_Pri.h
版 本 号   : 初稿
作    者   : x00102361
生成日期   : 2014年6月16日
最近修改   :
功能描述   : KMC_Func.c 的内部接口头文件，不对外开放
函数列表   :
修改历史   :
1.日    期   : 2014年6月16日
作    者   : x00102361
修改内容   : 创建文件

******************************************************************************/
#ifndef __KMC_PRI_H_D13DA0FG2_DCRFKLAPSD32SF_4EHLPOC27__
#define __KMC_PRI_H_D13DA0FG2_DCRFKLAPSD32SF_4EHLPOC27__

#include "wsec_type.h"
#include "wsec_config.h"
#include "wsec_pri.h"
#include "wsec_array.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*=============================================================
                 文件版本号历史
1. Keystore文件版本号:
KMC_KSF_VER = 1     2014-12-31 首版本发布

2. MK文件版本号:
KMC_MKF_VER = 1     2014-12-31 首版本发布
=============================================================*/
#define KMC_KSF_VER (1) /* Keystore文件版本号 */
#define KMC_MKF_VER (1) /* MK文件版本号 */
#define KMC_KCF_VER (1) /* KMC配置文件版本号 */

#define KMC_RMK_LEN      (32) /* RMK密钥长度 */
#define KMC_EK4MKF_LEN   (32) /* MKF(MK文件)加密密钥长度 */
#define KMC_KEY4HMAC_LEN (32) /* 用于HMAC的密钥长度 */
#define KMC_HMAC_RST_LEN (32) /* HMAC计算结果长度 */

/* CBB专用Domain */
#define KMC_PRI_DOMAIN_ID_MIN (1024)
#define KMC_PRI_DOMAIN_ID_MAX (1056)

#define KMC_ITER_COUNT_MIN (1)
#define KMC_ITER_COUNT_MAX (100000)
#define KMC_IS_KEYITERATIONS_VALID(ulKeyIterations) ((KMC_ITER_COUNT_MIN <= (ulKeyIterations)) && ((ulKeyIterations) <= KMC_ITER_COUNT_MAX))

/*----------------------------------------------*
 * KMC配置文件TLV格式之Tag定义
 *----------------------------------------------*/
typedef enum 
{
    KMC_CFT_HDR = 1,         /* 文件头 */
    KMC_CFT_RK_CFG,          /* RootKey配置 */
    KMC_CFT_KEY_MAN,         /* 密钥管理相关的配置参数 */
    KMC_CFT_DP_CFG,          /* 数据保护配置 */
    KMC_CFT_DOMAIN_CFG,      /* Domain配置 */
    KMC_CFT_DOMAIN_KEY_TYPE  /* Domain的KeyType配置 */
} KMC_CFG_FILE_TLV_ENUM;

/*----------------------------------------------*
 * KMC锁保护类型
 *----------------------------------------------*/
typedef enum
{
    KMC_LOCK_NONE     = 0, /* 不锁 */
    KMC_LOCK_CFG      = 1, /* 锁配置 */
    KMC_LOCK_KEYSTORE = 2, /* 锁Keystore */
    KMC_LOCK_BOTH     = 3  /* 锁配置及Keystore */
} KMC_LOCK_TYPE_ENUM;

typedef enum
{
    KMC_NOT_LOCK, /* 不加锁 */
    KMC_NEED_LOCK /* 需加锁 */
} KMC_LOCK_OPER_ENUM;/* 子函数调用, 要求子函数是否对临界资源加锁 */

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                      Root Key(RK)数据结构
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
#pragma pack(1)
typedef struct tagKMC_RK_PARA
{
    WSEC_BYTE abRkMeterial1[32]; /* 根密钥物料1 */
    WSEC_BYTE abRkMeterial2[32]; /* 根密钥物料2 */
    WSEC_BYTE abReserved[32]; /* 预留 */
    WSEC_BYTE abRmkSalt[32];     /* 派生RMK盐值 */
} KMC_RK_PARA_STRU; /* Root Key导出密钥的基本参数 */
#pragma pack()

#pragma pack(1)
typedef struct tagKMC_KSF_RK
{
    KMC_RK_ATTR_STRU stRkAttr; /* RK基本属性 */
    KMC_RK_PARA_STRU stRkPara; /* RootKey密钥构造参数 */
    WSEC_UINT32     ulMkNum;         /* MasterKey总数 */
    WSEC_BYTE       abReserved[36];  /* 保留 */
    WSEC_BYTE       abAboveHash[32]; /* SHA256(stMemRk, ulMkNum, abReserved) */
} KMC_KSF_RK_STRU; /* Keystore中RootKey信息 */
#pragma pack()
/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                      Master Key(MK)数据结构
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
#pragma pack(1)
typedef struct tagKMC_MK_REAR
{
    WSEC_UINT32 ulPlainLen;        /* MK密钥明文长度 */
    WSEC_BYTE   abKey[WSEC_MK_LEN_MAX]; /* 密钥(1. 驻留文件: RMK加密后的密文, 2. 驻留内存: RMK解密后的加掩明文) */
} KMC_MK_REAR_STRU; /* MK尾信息 */
#pragma pack()

#pragma pack(1)
typedef struct tagKMC_KSF_MK
{
    KMC_MK_INFO_STRU stMkInfo;       /* MK基本信息 */
    WSEC_BYTE        abReserved[44]; /* 保留 */
    WSEC_BYTE        abIv[16];       /* MK使用RMK加密存储IV */
    WSEC_UINT32      ulCipherLen;    /* MK密钥密文长度 */
    KMC_MK_REAR_STRU stMkRear;       /* MK尾部信息 */
    WSEC_BYTE        abMkHash[32];   /* HMAC-SHA256(stMkInfo, abReserved, stMkRear) */
} KMC_KSF_MK_STRU; /* 存储于KSF中的MK信息(不驻留内存) */
#pragma pack()

#pragma pack(1)
typedef struct tagKMC_MEM_MK
{
    KMC_MK_INFO_STRU stMkInfo; /* MK基本信息 */
    KMC_MK_REAR_STRU stMkRear; /* MK尾部信息 */
} KMC_MEM_MK_STRU; /* 驻留内存中的MK信息或加密后写入MKF */
#pragma pack()
/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

typedef struct tagKMC_KSF_MEM
{
    WSEC_CHAR*       pszFromFile; /* 该数据来自哪个Keystore文件 */
    KMC_RK_ATTR_STRU stRkInfo; /* Root Key信息 */
    WSEC_ARRAY       arrMk;    /* MK数组, 其元素存储 KMC_MEM_MK_STRU 型地址, 按ulDomainId+usType可重复升序排列, ulDomainId+ulKeyId唯一 */
} KMC_KSF_MEM_STRU; /* 内存中的KSF文件 */

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                      KMC配置数据结构【内存】
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
typedef struct tagKMC_DOMAIN_CFG
{
    KMC_CFG_DOMAIN_INFO_STRU stDomainInfo;
    WSEC_ARRAY               arrKeyTypeCfg; /* KeyType数组, 其元素存储 KMC_CFG_KEY_TYPE_STRU 型地址, 按usKeyType升序排列 */
} KMC_DOMAIN_CFG_STRU;

typedef struct tagKMC_CFG
{
    KMC_CFG_ROOT_KEY_STRU     stRkCfg; /* RK相关配置信息(只针对CBB内部生成RK时有效) */
    KMC_CFG_KEY_MAN_STRU      stKmCfg; /* 密钥管理相关的配置参数 */
    KMC_CFG_DATA_PROTECT_STRU astDataProtectCfg[3];
    WSEC_ARRAY                arrDomainCfg;    /* Domain数组, 其元素存储 KMC_DOMAIN_CFG_STRU 型地址, 按DomainId升序排列 */
} KMC_CFG_STRU;

typedef struct tagKMC_BACKUP_PARA
{
    KMC_FP_CALLBACK_STRU   stCallbackFun;
    WSEC_PROGRESS_RPT_STRU stProgressRpt;
} KMC_BACKUP_PARA_STRU;

typedef struct tagKMC_SYS
{
    KMC_RK_GEN_FROM eRootKeyFrom;                  /* RootKey来源 */
    WSEC_CHAR*      apszKeystoreFile[2];           /* Keystore文件名(CBB申请内存存放文件名) */
    WSEC_CHAR*      apszKmcCfgFile[2];             /* KMC配置文件文件名(CBB申请内存存放文件名) */
    WSEC_BYTE       abMkMaskCode[WSEC_MK_LEN_MAX]; /* 对MK明文加掩处理的掩码 */
    WSEC_UINT32     ulMkPlainLenMax;               /* MK明文最大长度 */
    KMC_BACKUP_PARA_STRU stBackupPara;
    WSEC_RUN_STATE_ENUM  eState; /* KMC状态 */
    WSEC_BYTE            bCorrectClockFlag; /* 是否已校正过时间 */
} KMC_SYS_STRU;

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                      KMC配置数据结构【文件】
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
#pragma pack(1)
typedef struct tagKMC_KCF_HDR
{
    WSEC_BYTE   abFormatFlag[32]; /* 格式标记 */
    WSEC_UINT32 ulVer;            /* 版本号 */
    WSEC_UINT32 ulHashAlgId;      /* Hash算法 */
    WSEC_BYTE   abReserved[16];   /* 预留 */
    WSEC_BYTE   abHash[32];       /* 除文件头外其余数据的Hash值 */
} KMC_KCF_HDR_STRU; /* KMC配置文件头 */
#pragma pack()

typedef struct tagKMC_READ_KCF_CTX
{
    WSEC_CHAR* pszFile;      /* 配置文件名 */
    WSEC_FILE  pFile;        /* 打开的文件 */
    WSEC_BUFF  stReadBuff;   /* 读文件缓冲区 */
} KMC_READ_KCF_CTX_STRU; /* 读取KMC配置文件的上下文信息 */

typedef struct tagKMC_INI_CTX
{
    WSEC_BOOL              bCbbReadCfg;   /* 是否由CBB读KMC配置 */
    KMC_READ_KCF_CTX_STRU  stReadKcfCtx;  /* 读配置文件的上下文 */
    WSEC_PROGRESS_RPT_STRU stProgressRpt; /* 上报进度的回调函数等设置 */
} KMC_INI_CTX_STRU; /* KMC初始化阶段使用的上下文信息 */

/*==========================================================================================
                MK文件(简称MKF)数据结构(导出/入用)
格式:
    0. 32字节格式码
    1. KMC_MKF_HDR_WITH_HMAC_STRU
    2. n个MK数据密文(KMC_MKF_MK_STRU)
    3. 所有MK密文的HMAC
==========================================================================================*/
#pragma pack(1)
typedef struct tagKMC_MKF_HDR
{
    WSEC_UINT16 usVer;                 /* MK文件版本号 */
    WSEC_UINT16 usKsfVer;              /* Keystore文件版本号 */
    WSEC_UINT32 ulEncryptAlgId;        /* 加密算法ID */
    WSEC_UINT32 ulIteration4EncrypKey; /* 生成加密密钥时迭代次数 */
    WSEC_BYTE   abSalt4EncrypKey[16];  /* 生成加密密钥时使用的盐值 */
    WSEC_BYTE   abIv4EncrypMk[16];     /* 加密MK时使用的IV */
    WSEC_BYTE   Reserved1[16];         /* 预留 */

    WSEC_UINT32 ulHmacAlgId;           /* HMAC算法ID */
    WSEC_UINT32 ulIteration4HmacKey;   /* 生成HMAC密钥时迭代次数 */
    WSEC_BYTE   abSalt4HmacKey[16];    /* 生成HMAC密钥时使用的盐值 */
    WSEC_UINT32 ulCipherLenPerMk;      /* 单条MK密文长度 */
    WSEC_UINT32 ulMkNum;               /* MK个数 */
    WSEC_BYTE   Reserved2[16];         /* 预留 */
} KMC_MKF_HDR_STRU; /* MK文件头 */
#pragma pack()

#pragma pack(1)
typedef struct tagKMC_MKF_HDR_WITH_HMAC
{
    KMC_MKF_HDR_STRU stHdr;
    WSEC_BYTE        abHmac[KMC_HMAC_RST_LEN]; /* 上述数据的HMAC */
} KMC_MKF_HDR_WITH_HMAC_STRU;
#pragma pack()

#pragma pack(1)
typedef struct tagKMC_MKF_MK
{
    KMC_MK_INFO_STRU stMkInfo;   /* MK基本信息 */
    WSEC_BYTE        abIv[16];   /* MK使用RMK加密存储IV */
    WSEC_UINT32      ulPlainLen; /* 密钥明文长度, 不能超出 WSEC_KEY_LEN */
    WSEC_BYTE        abPlainText[WSEC_MK_LEN_MAX]; /* 密钥明文 */
} KMC_MKF_MK_STRU; /* MK文件中的MK记录(加密存储) */
#pragma pack()

/* KMC私有函数 */
KMC_KSF_MEM_STRU* KMC_PRI_FreeKsfSnapshot(KMC_KSF_MEM_STRU* pData);
KMC_CFG_STRU* KMC_PRI_FreeKmcCfg(KMC_CFG_STRU* pKmcCfg);
WSEC_VOID KMC_PRI_FreeGlobalMem(KMC_CFG_STRU** ppKmcCfg, KMC_KSF_MEM_STRU** ppKeystore);

WSEC_ERR_T KMC_PRI_Finalize(KMC_LOCK_OPER_ENUM eLock);
WSEC_ERR_T KMC_PRI_UpdateRootKeyPri(const WSEC_BYTE* pbKeyEntropy, WSEC_SIZE_T ulSize, KMC_LOCK_OPER_ENUM eLock);
WSEC_ERR_T KMC_PRI_GetMaxMkId(KMC_LOCK_OPER_ENUM eLock, WSEC_UINT32 ulDomainId, WSEC_UINT32* pulMaxKeyId);
WSEC_ERR_T KMC_PRI_GetMkDetail(KMC_LOCK_OPER_ENUM eLock, WSEC_UINT32 ulDomainId, WSEC_UINT32 ulKeyId, KMC_MK_INFO_STRU* pstMkInfo, WSEC_BYTE* pbKeyPlainText, WSEC_UINT32* pKeyLen);

WSEC_VOID KMC_PRI_MakeDefaultCfg4RootKey(KMC_CFG_ROOT_KEY_STRU* pstRkCfg);
WSEC_VOID KMC_PRI_MakeDefaultCfg4KeyMan(KMC_CFG_KEY_MAN_STRU* pstKmCfg);
WSEC_VOID KMC_PRI_MakeDefaultCfg4DataProtect(KMC_SDP_ALG_TYPE_ENUM eType, KMC_CFG_DATA_PROTECT_STRU *pstCfg);
WSEC_BOOL KMC_PRI_MakeDefaultCfg4Domain(KMC_CFG_STRU* pKmcCfg);

/* KMC管理配置数据时, 下列函数替代APP完成回调配置函数的功能 */
WSEC_BOOL KMC_PRI_ReadRootKeyCfg(KMC_CFG_ROOT_KEY_STRU* pstRkCfg); /* 读取RootKey配置 */
WSEC_BOOL KMC_PRI_ReadKeyManCfg(KMC_CFG_KEY_MAN_STRU* pstKmCfg); /* 读取KEY管理配置 */
WSEC_BOOL KMC_PRI_ReadCfgOfDomainCount(WSEC_UINT32* pulDomainCount); /* 读取KMC配置数据之 Domain个数 */
WSEC_BOOL KMC_PRI_ReadCfgOfDomainInfo(KMC_CFG_DOMAIN_INFO_STRU* pstAllDomainInfo, WSEC_UINT32 ulDomainCount); /* 读取所有Domain的配置信息 */
WSEC_BOOL KMC_PRI_ReadCfgOfDomainKeyTypeCount(WSEC_UINT32 ulDomainId, WSEC_UINT32* pulKeyTypeCount); /* 读取指定Domain有多少条KeyType配置 */
WSEC_BOOL KMC_PRI_ReadCfgOfDomainKeyType(WSEC_UINT32 ulDomainId, KMC_CFG_KEY_TYPE_STRU* pstDomainAllKeyType, WSEC_UINT32 ulKeyTypeCount); /* 读取指定Domain的所有KeyType配置 */
WSEC_BOOL KMC_PRI_ReadCfgOfDataProtection(KMC_SDP_ALG_TYPE_ENUM eType, KMC_CFG_DATA_PROTECT_STRU *pstPara); /* 读取指定类型的算法配置 */
WSEC_ERR_T KMC_PRI_OpenIniCtx(WSEC_BOOL bManCfgFile);
WSEC_VOID KMC_PRI_CloseIniCtx();

/* 检查配置参数合法性 */
WSEC_ERR_T KMC_PRI_ChkProtectCfg(KMC_SDP_ALG_TYPE_ENUM eType, const KMC_CFG_DATA_PROTECT_STRU *pstPara);

/* 写KMC配置文件(KCF: KMC Configure File) */
WSEC_ERR_T KMC_PRI_WriteKcfSafety(const KMC_CFG_STRU* pKmcCfg);
WSEC_ERR_T KMC_PRI_WriteKcf(const KMC_CFG_STRU* pKmcCfg, const WSEC_CHAR* pszKmcCfgFile, const WSEC_VOID* pvReserved);
WSEC_ERR_T KMC_PRI_ChkCfgFile(KMC_READ_KCF_CTX_STRU* pstCtx);

/* 读/写Keystore文件(KSF: KeyStore File) */
WSEC_ERR_T KMC_PRI_ReadKsfSafety(KMC_KSF_MEM_STRU** ppKeystore);
WSEC_ERR_T KMC_PRI_ReadKsf(const WSEC_CHAR* pszKeystoreFile, KMC_KSF_MEM_STRU* pKeystoreData);
WSEC_ERR_T KMC_PRI_ReadRootKey(const WSEC_CHAR* pszFile, KMC_KSF_RK_STRU* pRk);
WSEC_ERR_T KMC_PRI_ReadKsfBasedVer(const WSEC_CHAR* pszKeystoreFile, WSEC_UINT16 usVer, KMC_KSF_MEM_STRU* pKeystoreData);

WSEC_ERR_T KMC_PRI_WriteKsfSafety(const KMC_KSF_MEM_STRU* pKeystoreData, const KMC_KSF_RK_STRU* pRk);
WSEC_ERR_T KMC_PRI_WriteKsf(const KMC_KSF_MEM_STRU* pKeystoreData, const WSEC_CHAR* pszFile, const KMC_KSF_RK_STRU* pRk);

WSEC_ERR_T KMC_PRI_CreateRootKey(const WSEC_BUFF* pstEntropy, KMC_KSF_RK_STRU* pRk);
WSEC_ERR_T KMC_PRI_CreateMkArr(KMC_KSF_MEM_STRU* pKeystore);
WSEC_ERR_T KMC_PRI_CreateMkItem(KMC_KSF_MEM_STRU* pKeystore, const KMC_CFG_DOMAIN_INFO_STRU* pstDomain, const KMC_CFG_KEY_TYPE_STRU* pstKeyType, const WSEC_BUFF* pKeyPlain, WSEC_UINT32 ulKeyId);
WSEC_ERR_T KMC_PRI_ReCreateMkItem(KMC_KSF_MEM_STRU* pKeystore, KMC_MK_INFO_STRU* pMk);
WSEC_ERR_T KMC_PRI_CreateKsf(KMC_KSF_MEM_STRU* pKeystoreData, KMC_KSF_RK_STRU* pRk);

WSEC_BOOL KMC_PRI_MakeRmk(const KMC_KSF_RK_STRU* pstRk, WSEC_BUFF* pstBuff);
WSEC_ERR_T KMC_PRI_MakeMk(const KMC_CFG_DOMAIN_INFO_STRU* pstDomain, const KMC_CFG_KEY_TYPE_STRU* pstKeyType, const WSEC_BUFF* pKeyPlain, WSEC_UINT32 ulKeyId, KMC_MEM_MK_STRU* pMk);
WSEC_VOID KMC_PRI_GetRkMeterial(WSEC_BYTE* pMeterial, WSEC_SIZE_T nSize);
WSEC_ERR_T KMC_PRI_CalcMkPlainLenMax(WSEC_UINT32* pulLenMax);

WSEC_BOOL KMC_PRI_IsTime4ChkKey(const WSEC_SYSTIME_T* pstLocalNow, const KMC_CFG_KEY_MAN_STRU* pstCfg, WSEC_EXEC_INFO_STRU* pExecInfo);
WSEC_ERR_T KMC_PRI_SetLifeTime(WSEC_UINT32 ulLifeDays, WSEC_SYSTIME_T* pstCreateUtc, WSEC_SYSTIME_T* pstExpireUtc);

WSEC_VOID KMC_PRI_CvtByteOrder4KsfRk(KMC_KSF_RK_STRU* pstRk, WSEC_BYTEORDER_CVT_ENUM eOper);
WSEC_VOID KMC_PRI_CvtByteOrder4KsfMk(KMC_KSF_MK_STRU* pstMk, WSEC_BYTEORDER_CVT_ENUM eOper);
WSEC_VOID KMC_PRI_CvtByteOrder4MkInfo(KMC_MK_INFO_STRU* pstMkInfo, WSEC_BYTEORDER_CVT_ENUM eOper);
WSEC_VOID KMC_PRI_CvtByteOrder4MkfHdr(KMC_MKF_HDR_STRU* pstMkfHdr, WSEC_BYTEORDER_CVT_ENUM eOper);
WSEC_VOID KMC_PRI_CvtByteOrder4KcfHdr(KMC_KCF_HDR_STRU* pstKcfHdr, WSEC_BYTEORDER_CVT_ENUM eOper);
WSEC_VOID KMC_PRI_CvtByteOrder4RkCfg(KMC_CFG_ROOT_KEY_STRU* pstRkCfg, WSEC_BYTEORDER_CVT_ENUM eOper);
WSEC_VOID KMC_PRI_CvtByteOrder4KeyManCfg(KMC_CFG_KEY_MAN_STRU* pstKeyManCfg, WSEC_BYTEORDER_CVT_ENUM eOper);
WSEC_VOID KMC_PRI_CvtByteOrder4DataProtectCfg(KMC_CFG_DATA_PROTECT_STRU* pstCfg, WSEC_BYTEORDER_CVT_ENUM eOper);
WSEC_VOID KMC_PRI_CvtByteOrder4DomainInfo(KMC_CFG_DOMAIN_INFO_STRU* pstCfg, WSEC_BYTEORDER_CVT_ENUM eOper);
WSEC_VOID KMC_PRI_CvtByteOrder4KeyType(KMC_CFG_KEY_TYPE_STRU* pstCfg, WSEC_BYTEORDER_CVT_ENUM eOper);

WSEC_VOID KMC_PRI_NtfWriCfgFileFail(WSEC_ERR_T ulCause);
WSEC_VOID KMC_PRI_NtfMkChanged(const KMC_MK_INFO_STRU* pMk, KMC_KEY_CHANGE_TYPE_ENUM eType);
WSEC_VOID KMC_PRI_NtfRkExpire(const KMC_RK_ATTR_STRU* pRk, WSEC_INT32 nRemainLifeDays);
WSEC_ERR_T KMC_PRI_LoadData(KMC_CFG_STRU** ppKmcCfg, KMC_KSF_MEM_STRU** ppKeystore, const WSEC_PROGRESS_RPT_STRU* pstRptProgress, const KMC_FP_CFG_CALLBACK_STRU* pstCfgFun, WSEC_BOOL bCbbManCfg);
WSEC_ERR_T KMC_PRI_CfgDataInit(const KMC_FP_CFG_CALLBACK_STRU* pCallbackFun, KMC_CFG_STRU* pKmcCfg);
WSEC_ERR_T KMC_PRI_AddDomain2Array(KMC_CFG_STRU* pKmcCfg, const KMC_CFG_DOMAIN_INFO_STRU* pDomainInfo);
WSEC_ERR_T KMC_PRI_AddDomainKeyType2Array(KMC_CFG_STRU* pKmcCfg, WSEC_UINT32 ulDomainId,const KMC_CFG_KEY_TYPE_STRU* pKeyType);
WSEC_ERR_T KMC_PRI_AddMk2Array(KMC_KSF_MEM_STRU* pKeystore, KMC_MEM_MK_STRU* pMk);

WSEC_INT32 KMC_PRI_CompareDomain4Arr(const WSEC_VOID* p1, const WSEC_VOID* p2);
WSEC_INT32 KMC_PRI_CompareDomainKeyType4Arr(const WSEC_VOID* p1, const WSEC_VOID* p2);
WSEC_INT32 KMC_PRI_CompareMk4Arr(const WSEC_VOID* p1, const WSEC_VOID* p2);

WSEC_BOOL KMC_PRI_SearchDomainKeyTypeCfg(WSEC_UINT32 ulDomainId, WSEC_UINT16 usKeyType, KMC_DOMAIN_CFG_STRU** ppDomain, KMC_CFG_KEY_TYPE_STRU** ppKeyType);
KMC_DOMAIN_CFG_STRU* KMC_PRI_SearchDomain(const KMC_CFG_STRU* pKmcCfg, WSEC_UINT32 ulDomainId);
WSEC_INT32 KMC_PRI_SearchMkByKeyId(const KMC_KSF_MEM_STRU* pKeystore, WSEC_UINT32 ulDomainId, WSEC_UINT32 ulKeyId);

WSEC_VOID KMC_PRI_OnRemoveDomainArr(WSEC_VOID *pElement);
WSEC_VOID KMC_PRI_OnRemoveMkArr(WSEC_VOID *pElement);

WSEC_VOID KMC_PRI_Lock(KMC_LOCK_TYPE_ENUM eType);
WSEC_VOID KMC_PRI_Unlock(KMC_LOCK_TYPE_ENUM eType);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __KMC_PRI_H_D13DA0FG2_DCRFKLAPSD32SF_4EHLPOC27__ */


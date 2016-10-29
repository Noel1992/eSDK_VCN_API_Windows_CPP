/* 如下pc lint告警可忽略 */
/*lint -e413 -e506 -e522 -e526 -e533 -e534 -e545 -e550 -e573 -e602 -e603 -e628 -e632 -e633 -e638 -e639 -e634 -e641 -e655 -e665 -e668 -e701 -e702 -e750 -e775 -e785 -e794 -e830 -e960 */

#include "stdlib.h"
#include "wsec_securec.h"
#include "kmc_itf.h"
#include "kmc_pri.h"
#include "sdp_itf.h"
#include "sdp_pri.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#ifndef WSEC_COMPILE_SDP
#error Please defined 'WSEC_COMPILE_SDP' to compile this-file.
#endif

/***** 密文结构版本 */
#define SDP_CIPHER_TEXT_VER (1)
#define SDP_HMAC_VER        (1)
#define SDP_PWD_CIPHER_VER  (1)
#define SDP_CIPHER_FILE_VER (1) /* 密文文件版本号, 若格式变化则需要调整且解密时需要做兼容处理 */

#define SDP_MAX_ERR_FIELD_OF_CIPHER (1)
#define SDP_HMAC_FLAG_MAX           (1)

#define init_error_ctx(stErrCtx) do{ stErrCtx.ulErrCount = 0; stErrCtx.ulLastErrCode = WSEC_SUCCESS; }do_end
#define update_error_ctx(stErrCtx, condition, oper, err_code) do{if(condition) {stErrCtx.ulErrCount++; stErrCtx.ulLastErrCode = err_code; oper;}}do_end

WSEC_BYTE g_CipherFileFlag[] = {0x7F, 0x11, 0x3A, 0xBE, 0x84, 0x18, 0x20, 0x0D, 0xE0, 0x45, 0x2F, 0x61, 0x3D, 0x34, 0xD5, 0x16,
                                0x31, 0x99, 0x77, 0x74, 0x6F, 0xEE, 0x6D, 0x76, 0xC8, 0x92, 0xC4, 0x73, 0xF0, 0x17, 0x96, 0x87};

extern WSEC_ERR_T KMC_GetMkByType(WSEC_UINT32 ulDomainId, WSEC_UINT16 usKeyType, WSEC_BYTE* pKeyBuf, WSEC_UINT32 *pKeyLen, WSEC_UINT32 *pKeyId);
extern WSEC_ERR_T KMC_GetMkDetailInner(WSEC_UINT32 ulDomainId, WSEC_UINT32 ulKeyId, KMC_MK_INFO_STRU* pstMkInfo, WSEC_BYTE* pbKeyPlainText, WSEC_UINT32* pKeyLen);

WSEC_VOID SDP_CalcAllAlgPara(WSEC_UINT32 ulAlgId, const WSEC_CHAR* pszAlgName, WSEC_VOID* pReserved);

/*****************************************************************************
 函 数 名  : SDP_CalcAllAlgPara
 功能描述  : 遍历CAC所支持的算法集, 检查SDP常数是否满足加密、HMAC对参数的要求.
 输入参数  : ulAlgId、pszAlgName: 算法ID及名称
 输出参数  : pReserved: 如果检查失败则输出WSEC_FALSE，正常情况不输出.
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年3月18日
    作    者   : z00118096
    修改内容   : 新生成函数
  2.日    期   : 2015年7月31日
    作    者   : z00118096
    修改内容   : 新增对称分组加密最大块长度宏、最大HMAC值长度宏定义得是否过小.
*****************************************************************************/
WSEC_VOID SDP_CalcAllAlgPara(WSEC_UINT32 ulAlgId, const WSEC_CHAR* pszAlgName, WSEC_VOID* pReserved)
{
    WSEC_ALGTYPE_E eType;
    WSEC_UINT32 ulLen;
    WSEC_BOOL* pbOk = (WSEC_BOOL*)pReserved;

    eType = CAC_AlgId2Type(ulAlgId);

    do
    {
        if (WSEC_ALGTYPE_SYM == eType) /* 加密算法 */
        {
            /* 检查 SDP_KEY_MAX_LEN */
            ulLen = CAC_SYM_KeyLen(ulAlgId);
            break_oper_if(SDP_KEY_MAX_LEN < ulLen, WSEC_LOG_E2("'SDP_KEY_MAX_LEN' defined too small, it should not small than %d when used '%s' algorithmic.", ulLen, pszAlgName), *pbOk = WSEC_FALSE);

            /* 检查 SDP_IV_MAX_LEN */
            ulLen = CAC_SYM_IvLen(ulAlgId);
            break_oper_if(SDP_IV_MAX_LEN < ulLen, WSEC_LOG_E2("'SDP_IV_MAX_LEN' defined too small, it should not small than %d when used '%s' algorithmic.", ulLen, pszAlgName), *pbOk = WSEC_FALSE);

            ulLen = CAC_SYM_BlockSize(ulAlgId);
            break_oper_if(0 == ulLen, WSEC_LOG_E1("'%s' cannot support BlockSize.", pszAlgName), *pbOk = WSEC_FALSE);
            break_oper_if(ulLen > SDP_SYM_MAX_BLOCK_SIZE, WSEC_LOG_E1("'SDP_SYM_MAX_BLOCK_SIZE' defined too small, at least is %d.", ulLen), *pbOk = WSEC_FALSE);
        }
        else if (WSEC_ALGTYPE_HMAC == eType) /* HMAC算法 */
        {
            ulLen = CAC_HMAC_Size(ulAlgId);
            break_oper_if(SDP_PTMAC_MAX_LEN < ulLen, WSEC_LOG_E2("'SDP_PTMAC_MAX_LEN' defined too small, it should not small than %d when used '%s' algorithmic.", ulLen, pszAlgName), *pbOk = WSEC_FALSE);
            break_oper_if(SDP_HMAC_MAX_SIZE < ulLen, WSEC_LOG_E2("'SDP_HMAC_MAX_SIZE' defined too small, it should not small than %d when used '%s' algorithmic.", ulLen, pszAlgName), *pbOk = WSEC_FALSE);
        }else{;}
    }do_end;

    return;
}

/*****************************************************************************
 函 数 名  : SDP_Initialize
 功能描述  : SDP工作前, 检查常量设置是否合规, 这些常数有:
             SDP_KEY_MAX_LEN
             SDP_IV_MAX_LEN
             SDP_PTMAC_MAX_LEN
 输入参数  :
 输出参数  :
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2015年2月15日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T SDP_Initialize()
{
#ifdef WSEC_DEBUG
    WSEC_BOOL bOk = WSEC_TRUE;
#endif
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

#ifdef WSEC_DEBUG
    nErrCode = CAC_GetAlgList(SDP_CalcAllAlgPara, &bOk);
    if ((WSEC_SUCCESS == nErrCode) && (!bOk)) {nErrCode = WSEC_FAILURE;}
#endif

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : SDP_GetAlgProperty
 功能描述  : 查询加密算法适配，考虑适配IPSI、OpenSSL
 输入参数  :
 输出参数  :
 返 回 值  : 错误码

 修改历史      :
  1.日    期   : 2014年7月1日
    作    者   : j00265291
    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_GetAlgProperty(
    WSEC_UINT32 ulAlgID,
    WSEC_CHAR *pcAlgName,
    WSEC_UINT32 ulAlgNameLen,
    WSEC_ALGTYPE_E *peAlgType,
    WSEC_UINT32 *pulKeyLen,
    WSEC_UINT32 *pulIVLen,
    WSEC_UINT32 *pulBlockLen,
    WSEC_UINT32 *pulMACLen
    )
{
    WSEC_ALGTYPE_E eAlgType = WSEC_ALGTYPE_UNKNOWN;
    WSEC_UINT32 ulTempKeyLen   = 0;
    WSEC_UINT32 ulTempIVLen    = 0;
    WSEC_UINT32 ulTempBlockLen = 0;
    WSEC_UINT32 ulTempMACLen   = 0;
    const WSEC_CHAR* pszTempAlgName = WSEC_NULL_PTR;

    eAlgType = CAC_AlgId2Type(ulAlgID);
    return_oper_if((WSEC_ALGTYPE_UNKNOWN == eAlgType), WSEC_LOG_E("[SDP] CAC Get algorithm types failed."), WSEC_ERR_SDP_ALG_NOT_SUPPORTED);

    if (pcAlgName && (ulAlgNameLen > 0))
    {
        /* Misinformation: FORTIFY.Dead_Code */
        pszTempAlgName = CAC_AlgId2Name(ulAlgID);
        return_oper_if(!pszTempAlgName, WSEC_LOG_E("[SDP] CAC Get algorithm names failed."), WSEC_ERR_GET_ALG_NAME_FAIL);
        return_oper_if(strcpy_s(pcAlgName, ulAlgNameLen, pszTempAlgName) != EOK, WSEC_LOG_E("strcpy_s() fail"), WSEC_ERR_STRCPY_FAIL);
    }

    if (WSEC_NULL_PTR != pulKeyLen)
    {
        ulTempKeyLen = (WSEC_ALGTYPE_SYM == eAlgType) ? CAC_SYM_KeyLen(ulAlgID) : SDP_KEY_MAX_LEN;
        if (0 == ulTempKeyLen) {ulTempKeyLen = SDP_KEY_MAX_LEN;}
        return_oper_if((ulTempKeyLen > SDP_KEY_MAX_LEN), 
                       WSEC_LOG_E2("[SDP] Length of key exceeds the limit %d. Actually %d.", SDP_KEY_MAX_LEN, ulTempKeyLen), 
                       WSEC_ERR_INVALID_ARG);
    }

    if (WSEC_NULL_PTR != pulIVLen)
    {
        ulTempIVLen = CAC_SYM_IvLen(ulAlgID);
        return_oper_if((ulTempIVLen > SDP_IV_MAX_LEN), WSEC_LOG_E2("[SDP] Length of IV exceeds the limit %d. Actually %d.", SDP_IV_MAX_LEN, ulTempIVLen), WSEC_ERR_INVALID_ARG);
    }

    if (WSEC_NULL_PTR != pulBlockLen)
    {
        ulTempBlockLen = CAC_SYM_BlockSize(ulAlgID);
    }

    if (WSEC_NULL_PTR != pulMACLen)
    {
        ulTempMACLen = CAC_HMAC_Size(ulAlgID);
        return_oper_if(ulTempMACLen < 1, WSEC_LOG_E1("The alg(%d) connot support HMAC.", ulAlgID), WSEC_ERR_SDP_CONFIG_INCONSISTENT_WITH_USE);
        return_oper_if((ulTempMACLen > SDP_PTMAC_MAX_LEN), 
                       WSEC_LOG_E2("[SDP] Length of MAC exceeds the limit %d. Actually %d.", SDP_PTMAC_MAX_LEN, ulTempMACLen), 
                       WSEC_ERR_INVALID_ARG);
    }

    /* output param */
    WSEC_SAFE_ASSIGN(peAlgType, eAlgType);
    WSEC_SAFE_ASSIGN(pulKeyLen, ulTempKeyLen);
    WSEC_SAFE_ASSIGN(pulIVLen, ulTempIVLen);
    WSEC_SAFE_ASSIGN(pulBlockLen, ulTempBlockLen);
    WSEC_SAFE_ASSIGN(pulMACLen, ulTempMACLen);

    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : SDP_CvtByteOrder4CipherFileHdr
 功能描述  : 将密文文件头作字节序转换.
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
WSEC_VOID SDP_CvtByteOrder4CipherFileHdr(SDP_CIPHER_FILE_HDR_STRU* pstFileHdr, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    if (wbcHost2Network == eOper)
    {
        pstFileHdr->ulVer               = WSEC_H2N_L(pstFileHdr->ulVer);
        pstFileHdr->ulPlainBlockLenMax  = WSEC_H2N_L(pstFileHdr->ulPlainBlockLenMax);
        pstFileHdr->ulCipherBlockLenMax = WSEC_H2N_L(pstFileHdr->ulCipherBlockLenMax);
    }
    else if (wbcNetwork2Host == eOper)
    {
        pstFileHdr->ulVer               = WSEC_N2H_L(pstFileHdr->ulVer);
        pstFileHdr->ulPlainBlockLenMax  = WSEC_N2H_L(pstFileHdr->ulPlainBlockLenMax);
        pstFileHdr->ulCipherBlockLenMax = WSEC_N2H_L(pstFileHdr->ulCipherBlockLenMax);
    }
    else
    {
        WSEC_ASSERT_FALSE;
    }

    WSEC_CvtByteOrder4DateTime(&pstFileHdr->tCreateFileTimeUtc, eOper);
    WSEC_CvtByteOrder4DateTime(&pstFileHdr->tSrcFileCreateTime, eOper);
    WSEC_CvtByteOrder4DateTime(&pstFileHdr->tSrcFileEditTime, eOper);

    return;
}

/*****************************************************************************
 函 数 名  : SDP_CvtByteOrder4CipherTextHeader
 功能描述  : 将密文头作字节序转换.
 纯 入 参  : eOper: 字节序转换方法
 纯 出 参  : 无
 入参出参  : pstHdr: [in]字节序待转换数据; [out]转换后的数据
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年2月15日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID SDP_CvtByteOrder4CipherTextHeader(SDP_CIPHER_HEAD_STRU* pstHdr, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    WSEC_ASSERT(pstHdr);

    if (wbcHost2Network == eOper)
    {
        pstHdr->ulVersion   = WSEC_H2N_L(pstHdr->ulVersion);
        pstHdr->bHmacFlag   = WSEC_H2N_L((WSEC_UINT32)pstHdr->bHmacFlag);
        pstHdr->ulDomain    = WSEC_H2N_L(pstHdr->ulDomain);
        pstHdr->ulAlgId     = WSEC_H2N_L(pstHdr->ulAlgId);
        pstHdr->ulKeyId     = WSEC_H2N_L(pstHdr->ulKeyId);
        pstHdr->ulIterCount = WSEC_H2N_L(pstHdr->ulIterCount);
        pstHdr->ulCDLen     = WSEC_H2N_L(pstHdr->ulCDLen);
    }
    else if (wbcNetwork2Host == eOper)
    {
        pstHdr->ulVersion   = WSEC_N2H_L(pstHdr->ulVersion);
        pstHdr->bHmacFlag   = WSEC_N2H_L((WSEC_UINT32)pstHdr->bHmacFlag);
        pstHdr->ulDomain    = WSEC_N2H_L(pstHdr->ulDomain);
        pstHdr->ulAlgId     = WSEC_N2H_L(pstHdr->ulAlgId);
        pstHdr->ulKeyId     = WSEC_N2H_L(pstHdr->ulKeyId);
        pstHdr->ulIterCount = WSEC_N2H_L(pstHdr->ulIterCount);
        pstHdr->ulCDLen     = WSEC_N2H_L(pstHdr->ulCDLen);
    }
    else
    {
        WSEC_ASSERT_FALSE;
    }

    return;
}

/*****************************************************************************
 函 数 名  : SDP_CvtByteOrder4HmacTextHeader
 功能描述  : 将HMAC头作字节序转换.
 纯 入 参  : eOper: 字节序转换方法
 纯 出 参  : 无
 入参出参  : pstHdr: [in]字节序待转换数据; [out]转换后的数据
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年2月15日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID SDP_CvtByteOrder4HmacTextHeader(SDP_HMAC_HEAD_STRU* pstHdr, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    WSEC_ASSERT(pstHdr);

    if (wbcHost2Network == eOper)
    {
        pstHdr->ulVersion   = WSEC_H2N_L(pstHdr->ulVersion);
        pstHdr->ulDomain    = WSEC_H2N_L(pstHdr->ulDomain);
        pstHdr->ulAlgId     = WSEC_H2N_L(pstHdr->ulAlgId);
        pstHdr->ulKeyId     = WSEC_H2N_L(pstHdr->ulKeyId);
        pstHdr->ulIterCount = WSEC_H2N_L(pstHdr->ulIterCount);
    }
    else if (wbcNetwork2Host == eOper)
    {
        pstHdr->ulVersion   = WSEC_N2H_L(pstHdr->ulVersion);
        pstHdr->ulDomain    = WSEC_N2H_L(pstHdr->ulDomain);
        pstHdr->ulAlgId     = WSEC_N2H_L(pstHdr->ulAlgId);
        pstHdr->ulKeyId     = WSEC_N2H_L(pstHdr->ulKeyId);
        pstHdr->ulIterCount = WSEC_N2H_L(pstHdr->ulIterCount);
    }
    else
    {
        WSEC_ASSERT_FALSE;
    }

    return;
}

/*****************************************************************************
 函 数 名  : SDP_CvtByteOrder4PwdCipherTextHeader
 功能描述  : 将口令保护头作字节序转换.
 纯 入 参  : eOper: 字节序转换方法
 纯 出 参  : 无
 入参出参  : pstHdr: [in]字节序待转换数据; [out]转换后的数据
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年2月15日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID SDP_CvtByteOrder4PwdCipherTextHeader(SDP_PWD_HEAD_STRU* pstHdr, WSEC_BYTEORDER_CVT_ENUM eOper)
{
    WSEC_ASSERT(pstHdr);

    if (wbcHost2Network == eOper)
    {
        pstHdr->ulVersion   = WSEC_H2N_L(pstHdr->ulVersion);
        pstHdr->ulAlgId     = WSEC_H2N_L(pstHdr->ulAlgId);
        pstHdr->ulIterCount = WSEC_H2N_L(pstHdr->ulIterCount);
        pstHdr->ulCDLen     = WSEC_H2N_L(pstHdr->ulCDLen);
    }
    else if (wbcNetwork2Host == eOper)
    {
        pstHdr->ulVersion   = WSEC_N2H_L(pstHdr->ulVersion);
        pstHdr->ulAlgId     = WSEC_N2H_L(pstHdr->ulAlgId);
        pstHdr->ulIterCount = WSEC_N2H_L(pstHdr->ulIterCount);
        pstHdr->ulCDLen     = WSEC_N2H_L(pstHdr->ulCDLen);
    }
    else
    {
        WSEC_ASSERT_FALSE;
    }

    return;
}


/*****************************************************************************
 函 数 名  : SDP_FillCipherTextHeader
 功能描述  : 填充默认加密头信息和加密密钥信息
 输入参数  : KMC_SDP_ALG_TYPE_ENUM eIntfType      接口类型
             WSEC_UINT32 ulDomain                 作用域
 输出参数  : SDP_CIPHER_HEAD_STRU *pstCipherHead  密文头部结构
             WSEC_BYTE *pucKey                    工作密钥
             WSEC_UINT32 *pulKeyLen               工作密钥长度
             WSEC_UINT32 *pulIVLen                待生成IV的长度
 返 回 值  : WSEC_UINT32   错误码

 修改历史      :
  1.日    期   : 2014年7月2日
    作    者   : x00102361, j00265291
    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_FillCipherTextHeader(
    KMC_SDP_ALG_TYPE_ENUM eIntfType,
    WSEC_UINT32 ulDomain,
    SDP_CIPHER_HEAD_STRU *pstCipherHead,
    WSEC_BYTE *pucKey,
    WSEC_UINT32 *pulKeyLen,
    WSEC_UINT32 *pulIVLen
    )
{
    KMC_CFG_DATA_PROTECT_STRU stDP;
    WSEC_ALGTYPE_E eAlgType = WSEC_ALGTYPE_UNKNOWN;
    WSEC_ERR_T  ulRet       = WSEC_SUCCESS;
    WSEC_UINT32 ulIVLen     = 0;
    WSEC_UINT32 ulKeyLen    = 0;
    WSEC_UINT32 ulKeyId     = 0;
    WSEC_UINT32 ulIterCount = 0;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    /* check input */
    return_err_if_para_invalid("SDP_FillCipherTextHeader", pstCipherHead && pulKeyLen && pucKey && pulIVLen);

    /* Get interface */
    nErrCode = KMC_GetDataProtectCfg(eIntfType, &stDP);
    return_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC_GetDataProtectCfg() = %u.", nErrCode), nErrCode);
    return_oper_if((0 == (stDP.usKeyType & KMC_KEY_TYPE_ENCRPT)), WSEC_LOG_E("[SDP] Key type is incompatible with application. EK expected."), WSEC_ERR_SDP_CONFIG_INCONSISTENT_WITH_USE);

    /* 检查算法ID是否合法 */
    if (SDP_ALG_ENCRPT == eIntfType) {return_oper_if(CAC_AlgId2Type(stDP.ulAlgId) != WSEC_ALGTYPE_SYM, WSEC_LOG_E("KMC's Data-Protection CFG error."), WSEC_ERR_SDP_CONFIG_INCONSISTENT_WITH_USE);}
    if (SDP_ALG_INTEGRITY == eIntfType) {return_oper_if(CAC_AlgId2Type(stDP.ulAlgId) != WSEC_ALGTYPE_HMAC, WSEC_LOG_E("KMC's Data-Protection CFG error."), WSEC_ERR_SDP_CONFIG_INCONSISTENT_WITH_USE);}
    if (SDP_ALG_PWD_PROTECT == eIntfType) {return_oper_if(CAC_AlgId2Type(stDP.ulAlgId) != WSEC_ALGTYPE_PBKDF, WSEC_LOG_E("KMC's Data-Protection CFG error."), WSEC_ERR_SDP_CONFIG_INCONSISTENT_WITH_USE);}

    /* Get key length, IV length */
    ulRet = SDP_GetAlgProperty(stDP.ulAlgId, WSEC_NULL_PTR, 0, &eAlgType, &ulKeyLen, &ulIVLen, WSEC_NULL_PTR, WSEC_NULL_PTR);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] Get algorithm property failed."), ulRet);
    return_oper_if((WSEC_ALGTYPE_SYM != eAlgType), WSEC_LOG_E1("[SDP] AlgType(%d) is out of bounds.", eAlgType), WSEC_ERR_SDP_CONFIG_INCONSISTENT_WITH_USE);

    /* Get WK (work key) */
    ulRet = SDP_GetWorkKey(ulDomain, stDP.usKeyType, &ulKeyId, &ulIterCount,
                           pstCipherHead->aucSalt, SDP_SALT_LEN,
                           pstCipherHead->aucIV, ulIVLen, pucKey, ulKeyLen);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] Get WK failed."), ulRet);

    /* fill header except pstCipherHead->ulCDLen */
    pstCipherHead->ulVersion    = SDP_CIPHER_TEXT_VER;
    pstCipherHead->bHmacFlag    = stDP.bAppendMac;
    pstCipherHead->ulDomain     = ulDomain;
    pstCipherHead->ulAlgId      = stDP.ulAlgId;
    pstCipherHead->ulIterCount  = ulIterCount;
    pstCipherHead->ulKeyId      = ulKeyId;
    
    *pulKeyLen = ulKeyLen;
    *pulIVLen  = ulIVLen;

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : SDP_FillHmacTextHeader
 功能描述  : 填充默认计算HMAC信息头及密钥
 输入参数  : KMC_SDP_ALG_TYPE_ENUM eIntfType    接口类型
             WSEC_UINT32 ulDomain               作用域
 输出参数  : SDP_HMAC_HEAD_STRU *pstHmacHead    所生成HMAC的头部地址
             WSEC_BYTE *pucKey                  所生成的工作密钥[可空]
             WSEC_UINT32 *pulKeyLen             所生成的工作密钥长度[可空]
 返 回 值  : WSEC_UINT32 错误码

 修改历史      :
  1.日    期   : 2014年7月2日
    作    者   : x00102361, j00265291
    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_FillHmacTextHeader(
    KMC_SDP_ALG_TYPE_ENUM eIntfType,
    WSEC_UINT32 ulDomain,
    SDP_HMAC_HEAD_STRU *pstHmacHead,
    WSEC_BYTE *pucKey,
    WSEC_UINT32 *pulKeyLen
    )
{
    KMC_CFG_DATA_PROTECT_STRU stDP;
    WSEC_ALGTYPE_E eAlgType = WSEC_ALGTYPE_UNKNOWN;
    WSEC_ERR_T  ulRet       = WSEC_SUCCESS;
    WSEC_UINT32 ulKeyLen    = 0;
    WSEC_UINT32 ulKeyId     = 0;
    WSEC_UINT32 ulIterCount = 0;
    WSEC_ERR_T ulErrCode = WSEC_SUCCESS;

    /* check input */
    return_err_if_para_invalid("SDP_FillHmacTextHeader", pstHmacHead && pulKeyLen && pucKey);

    /* Get interface */
    ulErrCode = KMC_GetDataProtectCfg(eIntfType, &stDP);
    return_oper_if(ulErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC_GetDataProtectCfg()=%u", ulErrCode), ulErrCode);
    return_oper_if((0 == (stDP.usKeyType & KMC_KEY_TYPE_INTEGRITY)), WSEC_LOG_E("[SDP] Key type is incompatible with application. IK expected."), WSEC_ERR_SDP_CONFIG_INCONSISTENT_WITH_USE);

    /* Get key length */
    ulRet = SDP_GetAlgProperty(stDP.ulAlgId, WSEC_NULL_PTR, 0, &eAlgType, &ulKeyLen, WSEC_NULL_PTR, WSEC_NULL_PTR, WSEC_NULL_PTR);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] Get algorithm property failed."), ulRet);
    return_oper_if((WSEC_ALGTYPE_HMAC != eAlgType), WSEC_LOG_E1("[SDP] AlgType(%d) is out of bounds.", eAlgType), WSEC_ERR_SDP_CONFIG_INCONSISTENT_WITH_USE);

    /* Get WK (work key) */
    ulRet = SDP_GetWorkKey(ulDomain, stDP.usKeyType, &ulKeyId, &ulIterCount,
                           pstHmacHead->aucSalt, SDP_SALT_LEN, WSEC_NULL_PTR, 0, pucKey, ulKeyLen);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] Get WK failed."), ulRet);

    /* fill header */
    pstHmacHead->ulVersion      = SDP_HMAC_VER;
    pstHmacHead->ulDomain       = ulDomain;
    pstHmacHead->ulAlgId        = stDP.ulAlgId;
    pstHmacHead->ulIterCount    = ulIterCount;
    pstHmacHead->ulKeyId        = ulKeyId;

    /* output param */
    *pulKeyLen                  = ulKeyLen;

    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : SDP_FillPwdCipherTextHeader
 功能描述  : 填充默认口令单向加密头信息和加密密钥信息
 输入参数  : KMC_INTERFACE_TYPE_E eIntfType       接口类型
             WSEC_UINT32 ulDomain                 作用域
 输出参数  : SDP_PWD_HEAD_STRU *pstPwdHead        口令密文头部结构
 返 回 值  : WSEC_UINT32   错误码

 修改历史      :
  1.日    期   : 2014年7月2日
    作    者   : x00102361, j00265291
    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_FillPwdCipherTextHeader(
    KMC_SDP_ALG_TYPE_ENUM eIntfType,
    SDP_PWD_HEAD_STRU *pstPwdHead
    )
{
    KMC_CFG_DATA_PROTECT_STRU stDP;
    WSEC_ERR_T ulRet  = WSEC_SUCCESS;
    WSEC_ALGTYPE_E eAlgType = WSEC_ALGTYPE_UNKNOWN;
    WSEC_ERR_T ulErrCode = WSEC_SUCCESS;

    /* check input */
    return_err_if_para_invalid("SDP_FillPwdCipherTextHeader", pstPwdHead);

    /* Get interface */
    ulErrCode = KMC_GetDataProtectCfg(eIntfType, &stDP);
    return_oper_if(ulErrCode != WSEC_SUCCESS, WSEC_LOG_E1("KMC_GetDataProtectCfg()=%u", ulErrCode), ulErrCode);
    return_oper_if(!KMC_IS_KEYITERATIONS_VALID(stDP.ulKeyIterations), WSEC_LOG_E1("KeyIterations(%u) invalid.", stDP.ulKeyIterations), WSEC_ERR_KMC_KMCCFG_INVALID);

    /* Get Algorithm Property */
    ulRet = SDP_GetAlgProperty(stDP.ulAlgId, WSEC_NULL_PTR, 0, &eAlgType, WSEC_NULL_PTR, WSEC_NULL_PTR, WSEC_NULL_PTR, WSEC_NULL_PTR);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] Get algorithm property failed."), ulRet);
    return_oper_if((WSEC_ALGTYPE_PBKDF != eAlgType), WSEC_LOG_E1("[SDP] AlgType(%d) is out of bounds.", eAlgType), WSEC_ERR_SDP_CONFIG_INCONSISTENT_WITH_USE);
    
    /* generate salt */
    ulRet = CAC_Random(pstPwdHead->aucSalt, SDP_SALT_LEN);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] CAC calculate random failed."), WSEC_ERR_GET_RAND_FAIL);

    /* fill header except pstPwdHead->ulCDLen */
    pstPwdHead->ulVersion      = SDP_PWD_CIPHER_VER;
    pstPwdHead->ulAlgId        = stDP.ulAlgId;
    pstPwdHead->ulIterCount    = stDP.ulKeyIterations;

    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : SDP_GetWorkKey
 功能描述  : 获取WorkKey
 输入参数  :
 输出参数  :
 返 回 值  : 错误码

 修改历史      :
  1.日    期   : 2014年7月1日
    作    者   : j00265291
    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_GetWorkKey(
    WSEC_UINT32 ulDomain,
    WSEC_UINT16 usKeyType,
    WSEC_UINT32 *pulKeyId,
    WSEC_UINT32 *pulIterCount,
    WSEC_BYTE *pucSalt,
    WSEC_UINT32 ulSaltLen,
    WSEC_BYTE *pucIV,
    WSEC_UINT32 ulIVLen,
    WSEC_BYTE *pucKey,
    WSEC_UINT32 ulKeyLen
    )
{
    WSEC_BYTE szMasterKey[WSEC_MK_LEN_MAX];
    WSEC_ERR_T  ulRet           = WSEC_SUCCESS;
    WSEC_UINT32 ulMkLen         = 0;
    WSEC_UINT32 ulTempKeyId     = 0;
    WSEC_UINT32 ulTempIterCount = 1;

    /* check input */
    return_err_if_para_invalid("SDP_GetWorkKey", pucSalt && pucKey && pulKeyId && (ulSaltLen > 0) && (ulKeyLen > 0));

    /* generate salt */
    ulRet = CAC_Random(pucSalt, ulSaltLen);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] CAC calculate random failed."), WSEC_ERR_GET_RAND_FAIL);

    /* get Master Key */
    ulMkLen = sizeof(szMasterKey);
    ulRet = KMC_GetMkByType(ulDomain, usKeyType, szMasterKey, &ulMkLen, &ulTempKeyId);
    if ((WSEC_SUCCESS != ulRet) && (KMC_KEY_TYPE_ENCRPT_INTEGRITY != usKeyType))
    {
        ulRet = KMC_GetMkByType(ulDomain, KMC_KEY_TYPE_ENCRPT_INTEGRITY, szMasterKey, &ulMkLen, &ulTempKeyId);
    }
    return_oper_if((WSEC_SUCCESS != ulRet), WSEC_LOG_E("[SDP] KMC get MK by Key-Type failed."), ulRet);

    /* optionally generate IV */
    if ((WSEC_NULL_PTR != pucIV) && (ulIVLen > 0))
    {
        ulRet = CAC_Random(pucIV, ulIVLen);
        return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] CAC calculate random failed."), WSEC_ERR_GET_RAND_FAIL);
    }

    /* derive work key */
    ulRet = CAC_Pbkdf2(WSEC_ALGID_PBKDF2_HMAC_SHA256, szMasterKey, ulMkLen,
                       pucSalt, ulSaltLen, ulTempIterCount, ulKeyLen, pucKey);
    return_oper_if((WSEC_SUCCESS != ulRet), WSEC_LOG_E("[SDP] CAC pbkdf2 derive WK failed."), WSEC_ERR_PBKDF2_FAIL);

    /* output param */
    *pulKeyId = ulTempKeyId;
    WSEC_SAFE_ASSIGN(pulIterCount, ulTempIterCount);

    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : SDP_GetWorkKeyByID
 功能描述  : 获取WorkKey
 输入参数  :
 输出参数  :
 返 回 值  : 错误码

 修改历史      :
  1.日    期   : 2014年7月1日
    作    者   : j00265291
    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_GetWorkKeyByID(
    WSEC_UINT32 ulDomain,
    WSEC_UINT32 ulKeyId,
    WSEC_UINT32 ulIterCount,
    const WSEC_BYTE *pucSalt,
    WSEC_UINT32 ulSaltLen,
    WSEC_BYTE *pucKey,
    WSEC_UINT32 ulKeyLen
    )
{
    WSEC_BYTE szMasterKey[WSEC_MK_LEN_MAX];
    WSEC_ERR_T ulRet    = WSEC_SUCCESS;
    WSEC_UINT32 ulMkLen = 0;

    /* check input */
    return_err_if_para_invalid("SDP_GetWorkKeyByID", pucSalt && pucKey && (ulSaltLen > 0) && (ulKeyLen > 0) && (ulIterCount > 0));

    /* get Master Key */
    ulMkLen = sizeof(szMasterKey);
    ulRet = KMC_GetMkDetailInner(ulDomain, ulKeyId, WSEC_NULL_PTR, szMasterKey, &ulMkLen);
    return_oper_if((WSEC_SUCCESS != ulRet), WSEC_LOG_E("[SDP] KMC get MK by Key-Id failed."), ulRet);

    /* derive work key */
    ulRet = CAC_Pbkdf2(WSEC_ALGID_PBKDF2_HMAC_SHA256, szMasterKey, ulMkLen,
                       pucSalt, ulSaltLen, ulIterCount, ulKeyLen, pucKey);
    return_oper_if((WSEC_SUCCESS != ulRet), WSEC_LOG_E("[SDP] CAC pbkdf2 derive WK failed."), WSEC_ERR_PBKDF2_FAIL);

    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : SDP_FreeCtx
 功能描述  : 释放分段安全算法(加解密、HMAC)过程中使用的上下文.
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : pstSdpCtx: [in]指向上下信息的指针, [out]指针内容为NULL
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年5月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID SDP_FreeCtx(SDP_CRYPT_CTX *pstSdpCtx)
{
    SDP_CRYPT_CTX_STRU *pstSdpCtxObj = WSEC_NULL_PTR;

    if (!pstSdpCtx) {return;}

    pstSdpCtxObj = (SDP_CRYPT_CTX_STRU *)(*pstSdpCtx);
    if (!pstSdpCtxObj) {return;}

    do
    {
        break_oper_if(!pstSdpCtxObj->stWsecCtx, oper_null, oper_null);
        break_oper_if(WSEC_ALGTYPE_SYM == pstSdpCtxObj->eAlgType, CAC_CipherFree(&pstSdpCtxObj->stWsecCtx), oper_null);
        break_oper_if(WSEC_ALGTYPE_HMAC == pstSdpCtxObj->eAlgType, CAC_HmacFree(&pstSdpCtxObj->stWsecCtx), oper_null);
        break_oper_if(WSEC_ALGTYPE_DIGEST == pstSdpCtxObj->eAlgType, CAC_DigestFree(&pstSdpCtxObj->stWsecCtx), oper_null);
        WSEC_ASSERT(0);
    }do_end;

    WSEC_FREE(pstSdpCtxObj);
    *pstSdpCtx = WSEC_NULL_PTR;
}

/*****************************************************************************
 函 数 名  : SDP_GetCipherDataLen
 功能描述  : 预分配空间时使用，根据明文的长度获取密文数据块( cipher-data )的长度
 纯 入 参  : ulPlainTextLen 表示将加密的明文长度。
 纯 出 参  : pulCipherLen: 对应的密文长度, 
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 该函数适用于SDP_Encrypt、SDP_EncryptInit、SDP_EncryptUpdate、SDP_EncryptFinal等加密函数调用前进行密文长度计算和密文内存分配。
             同样适用于AES场景下SDP_DecryptInit、SDP_DecryptUpdate、SDP_DecryptFinal等解密函数调用前进行明文内存分配，
             确保至少具有一个加密块的长度。

 修改历史
  1.日    期   : 2014年7月2日
    作    者   : x00102361, j00265291
    修改内容   : 新生成函数
  2.日    期   : 2014年11月20日
    作    者   : z00118096
    修改内容   : 接口变更, 由出参传出长度, 返回值表达错误信息.
  3.日    期   : 2015年07月31日
    作    者   : z00118096
    修改内容   : 老版本的该接口无法解决竞争问题: 
                 1) 线程P1调用此函数获取密文长度;
                 2) 线程P2调用KMC_SetDataProtectCfg更改了算法配置;
                 3) P1调用SDP_Encrypt加密, 如果2)的算法更改导致实际需要的
                    密文长度较1)更长, 则出现内存越界写密文, 而程序无法感知.

                 修复方法: 使用最大的加密块长来计算密文长度, 与算法无关.
*****************************************************************************/
WSEC_ERR_T SDP_GetCipherDataLen(WSEC_UINT32 ulPlainTextLen, WSEC_UINT32* pulCipherLen)
{
    const WSEC_UINT32 ulBlkLen  = SDP_SYM_MAX_BLOCK_SIZE;
    WSEC_UINT32 ulHmacLen = 0;
    WSEC_ERR_T  ulRet = WSEC_SUCCESS;

    return_err_if_para_invalid("SDP_GetCipherDataLen", pulCipherLen);

    ulRet = SDP_GetHmacLen(&ulHmacLen);
    return_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E1("SDP_GetHmacLen()=%u", ulRet), ulRet);
    
    *pulCipherLen = SDP_CIPHER_HEAD_LEN + (((ulPlainTextLen / ulBlkLen) + 1) * ulBlkLen) + ulHmacLen;

    return WSEC_SUCCESS;
}

/*****************************************************************************
> 函 数 名  : SDP_Encrypt
> 功能描述  : 机密数据加密接口，能用于机密数据对称加密的场景。
> 密文结构  : cipher-head | cipher-data
>             cipher-head | cipher-data | hmac-head | hmac-data
> 输入参数  : ulDomain 表示作用域ID
>             pucPlainText 表示将要加密的明文内存块
>             ulPTLen 表示将要加密的明文内存块的长度
>             pulCTLen 表示将要生成加密后密文的内存块的分配长度
> 输出参数  : pucCipherText 表示将要生成加密后密文的内存块，包括密文头部
>             pulCTLen 表示将要生成加密后密文的内存块的实际长度，包括密文头部长度
> 返 回 值  : 错误码，WSEC_SUCCESS表示函数正常返回，密文已经输出。
> 先验条件  : 无
> 后验条件  : 当返回WSEC_SUCCESS时，*pulCTLen > 0
> 注意事项  : pucCipherText指向的内存块由调用者分配和释放，内存块的大小建议使用SDP_GetCipherDataLen获取。pulCTLen由调用者分配内存。

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_Encrypt(WSEC_UINT32 ulDomain,
                       const WSEC_BYTE *pucPlainText, WSEC_UINT32 ulPTLen,
                       WSEC_BYTE *pucCipherText, WSEC_UINT32 *pulCTLen)
{
    SDP_CIPHER_HEAD_STRU *pstCipherHead = WSEC_NULL_PTR;
    WSEC_BYTE aucKey[SDP_KEY_MAX_LEN];
    WSEC_ERR_T ulRet            = WSEC_SUCCESS;
    WSEC_UINT32 ulKeyLen        = 0;
    WSEC_UINT32 ulIVLen         = 0;
    WSEC_UINT32 ulMACLen        = 0;
    WSEC_UINT32 ulTempCTLen     = 0;
    WSEC_UINT32 ulCTMaxLen      = 0;


    /* check input */
    return_err_if_para_invalid("SDP_Encrypt", pucPlainText && pulCTLen && pucCipherText && (*pulCTLen >= ulPTLen) && (pucPlainText != pucCipherText));

    ulCTMaxLen = *pulCTLen;
    ulTempCTLen = ulCTMaxLen;

    /* construct header, get MK-derived work key */
    return_oper_if((ulCTMaxLen <= SDP_CIPHER_HEAD_LEN), WSEC_LOG_E("[SDP] Buffer for cipher text is not enough."), WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH);
    WSEC_MEMSET(pucCipherText, ulCTMaxLen, 0, SDP_CIPHER_HEAD_LEN);
    pstCipherHead = (SDP_CIPHER_HEAD_STRU *)pucCipherText;
    ulRet = SDP_FillCipherTextHeader(SDP_ALG_ENCRPT, ulDomain, pstCipherHead, aucKey, &ulKeyLen, &ulIVLen);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] Fill cipher text header failed."), ulRet);
    ulCTMaxLen -= SDP_CIPHER_HEAD_LEN;

    /* data encryption */
    ulRet = CAC_Encrypt(pstCipherHead->ulAlgId, aucKey, ulKeyLen, pstCipherHead->aucIV, ulIVLen,
                        pucPlainText, ulPTLen, (pucCipherText + SDP_CIPHER_HEAD_LEN),
                        /* suggest INOUT */ &ulTempCTLen);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] CAC encrypt failed."), WSEC_ERR_ENCRPT_FAIL);
    WSEC_ASSERT(ulTempCTLen <= ulCTMaxLen); /* assert: cipher space not overflow */

    /* destroy key */
    WSEC_DESTROY_KEY(aucKey, ulKeyLen);
    
    ulCTMaxLen -= ulTempCTLen;

    /* update header with real cipher length */
    pstCipherHead->ulCDLen = ulTempCTLen;

    /* compute cipher text length */
    ulTempCTLen += SDP_CIPHER_HEAD_LEN;

    /* compute data HMAC */
    if (pstCipherHead->bHmacFlag)
    {
        ulMACLen = ulCTMaxLen;
        ulRet = SDP_Hmac(ulDomain, pucPlainText, ulPTLen, pucCipherText + ulTempCTLen, &ulMACLen);
        return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] Compute hmac failed."), ulRet);

        ulTempCTLen += ulMACLen;
    }

    /* byte order process */
    SDP_CvtByteOrder4CipherTextHeader(pstCipherHead, wbcHost2Network);

    /* output param */
    *pulCTLen = ulTempCTLen;

    return WSEC_SUCCESS;
}

/*****************************************************************************
> 函 数 名  : SDP_Decrypt
> 功能描述  : 机密数据解密接口，提供解密机密数据密文的功能。通过密文头部获取解密所需的算法、IV、Salt、密钥ID，对数据进行解密，将明文返回给应用。
> 密文结构  : cipher-head | cipher-data
>             cipher-head | cipher-data | hmac-head | hmac-data
> 输入参数  : pucCipherText 表示将要解密的密文，包括数据保护的密文头部和追加HMAC；
>             ulCTLen 表示将要解密的密文的长度，包括数据保护的密文头和追加HMAC的长度
> 输出参数  : pucPlainText 表示表示将要生成解密后明文的内存块
>             pulPTLen 表示将要生成解密后明文的内存块的长度
> 返 回 值  : 错误码，WSEC_SUCCESS表示函数正常返回，明文已经输出。
> 先验条件  : 无
> 后验条件  : 当返回WSEC_SUCCESS时，* pulPTLen > 0
> 注意事项  : pucPlainText指向的内存块由调用者分配和释放，在AES场景下，明文长度不超过密文长度，
>             内存块的分配大小建议使用ulCTLen。pulPTLen由调用者分配内存。

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_Decrypt(
      WSEC_UINT32   ulDomain,
      const WSEC_BYTE *pucCipherText,
      WSEC_UINT32 ulCTLen,
      WSEC_BYTE *pucPlainText,
      WSEC_UINT32 *pulPTLen
    )
{
    SDP_CIPHER_HEAD_STRU    *pstCipherHead = WSEC_NULL_PTR;
    SDP_ERROR_CTX_STRU      stErrCtx;
    WSEC_ALGTYPE_E          eAlgType = WSEC_ALGTYPE_UNKNOWN;
    WSEC_BYTE aucKey[SDP_KEY_MAX_LEN] = {0};
    WSEC_ERR_T ulRet            = WSEC_SUCCESS;
    WSEC_UINT32 ulKeyLen        = 0;
    WSEC_UINT32 ulIVLen         = 0;
    WSEC_UINT32 ulTempPTLen     = 0;
    WSEC_UINT32 ulPTMaxLen      = 0;

    /* check input */
    return_err_if_para_invalid("SDP_Decrypt", pucCipherText && pucPlainText && pulPTLen && (ulCTLen > 0) && (pucPlainText != pucCipherText));

    ulPTMaxLen = *pulPTLen;
    ulTempPTLen = ulPTMaxLen;

    /* parse header */
    return_oper_if((ulCTLen < SDP_CIPHER_HEAD_LEN), WSEC_LOG_E("[SDP] Buffer for cipher text is not enough."), WSEC_ERR_INPUT_BUFF_NOT_ENOUGH);
    pstCipherHead = (SDP_CIPHER_HEAD_STRU *)pucCipherText;

    /* byte order process */
    SDP_CvtByteOrder4CipherTextHeader(pstCipherHead, wbcNetwork2Host);
    return_oper_if(pstCipherHead->ulCDLen < 1, *pulPTLen = 0, WSEC_SUCCESS); /* 密文长度为0, 则不需解密 */

    /* check format */
    init_error_ctx(stErrCtx);
    update_error_ctx(stErrCtx, (SDP_CIPHER_TEXT_VER != pstCipherHead->ulVersion),
                   WSEC_LOG_E2("[SDP] Cipher text version is incompatible, %d expected, %d actually.", SDP_CIPHER_TEXT_VER, pstCipherHead->ulVersion), 
                   WSEC_ERR_SDP_VERSION_INCOMPATIBLE);    
    update_error_ctx(stErrCtx, (ulDomain != pstCipherHead->ulDomain), 
                   WSEC_LOG_E1("[SDP] Cipher text are marked with an unexpected domain %d.", pstCipherHead->ulDomain), 
                   WSEC_ERR_SDP_DOMAIN_UNEXPECTED);
    update_error_ctx(stErrCtx, (0 == pstCipherHead->ulCDLen), 
                   WSEC_LOG_E("[SDP] Cipher data length cannot be 0."),
                   WSEC_ERR_SDP_INVALID_CIPHER_TEXT);
    update_error_ctx(stErrCtx, (pstCipherHead->bHmacFlag < 0 || pstCipherHead->bHmacFlag > SDP_HMAC_FLAG_MAX), 
                   WSEC_LOG_E("[SDP] Hmac flag is out of bounds."),
                   WSEC_ERR_SDP_INVALID_CIPHER_TEXT);
    update_error_ctx(stErrCtx, (!KMC_IS_KEYITERATIONS_VALID(pstCipherHead->ulIterCount)), 
                   WSEC_LOG_E("[SDP] Iterator count is out of bounds."),
                   WSEC_ERR_SDP_INVALID_CIPHER_TEXT);
    update_error_ctx(stErrCtx, (WSEC_ALGTYPE_UNKNOWN == CAC_AlgId2Type(pstCipherHead->ulAlgId)), 
                   WSEC_LOG_E("[SDP] CAC Get algorithm types failed."), 
                   WSEC_ERR_SDP_ALG_NOT_SUPPORTED);
    return_oper_if((stErrCtx.ulErrCount > SDP_MAX_ERR_FIELD_OF_CIPHER), WSEC_LOG_E("[SDP] Cipher text format is invalid."), WSEC_ERR_SDP_INVALID_CIPHER_TEXT);
    return_oper_if((stErrCtx.ulErrCount > 0), oper_null, stErrCtx.ulLastErrCode);

    return_oper_if((ulCTLen < (SDP_CIPHER_HEAD_LEN + pstCipherHead->ulCDLen)), WSEC_LOG_E("[SDP] Buffer for cipher text is not enough."), WSEC_ERR_INPUT_BUFF_NOT_ENOUGH);

    /* Get key length, IV length, MAC length */
    ulRet = SDP_GetAlgProperty(pstCipherHead->ulAlgId, WSEC_NULL_PTR, 0, &eAlgType, &ulKeyLen, &ulIVLen, WSEC_NULL_PTR, WSEC_NULL_PTR);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] Get algorithm property failed."), ulRet);
    return_oper_if((WSEC_ALGTYPE_SYM != eAlgType), WSEC_LOG_E1("[SDP] AlgType(%d) is out of bounds.", eAlgType), WSEC_ERR_SDP_INVALID_CIPHER_TEXT);

    /* Get WK (work key) By KeyID */
    ulRet = SDP_GetWorkKeyByID(pstCipherHead->ulDomain, pstCipherHead->ulKeyId, pstCipherHead->ulIterCount,
                               pstCipherHead->aucSalt, SDP_SALT_LEN, aucKey, ulKeyLen);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] Get WK by key id failed."), ulRet);

    /* data decryption */
    return_oper_if(ulTempPTLen < pstCipherHead->ulCDLen, WSEC_LOG_E2("Plain-Buff size(%d) at least is %d", ulTempPTLen, pstCipherHead->ulCDLen), WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH);
    ulRet = CAC_Decrypt(pstCipherHead->ulAlgId, aucKey, ulKeyLen, pstCipherHead->aucIV, ulIVLen,
                        (pucCipherText + SDP_CIPHER_HEAD_LEN), pstCipherHead->ulCDLen,
                        pucPlainText, /*suggested INOUT */ &ulTempPTLen);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] CAC decrypt failed."), WSEC_ERR_DECRPT_FAIL);

    /* destroy key */
    WSEC_DESTROY_KEY(aucKey, ulKeyLen);

    /* verify HMAC */
    if (pstCipherHead->bHmacFlag )
    {
        ulRet = SDP_VerifyHmac(ulDomain, pucPlainText, ulTempPTLen,
                               (pucCipherText + SDP_CIPHER_HEAD_LEN + pstCipherHead->ulCDLen),
                               (ulCTLen - SDP_CIPHER_HEAD_LEN - pstCipherHead->ulCDLen) );
        return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] Cipher text hmac verify failed."), ulRet);
    }

    /* output param */
    *pulPTLen = ulTempPTLen;

    return WSEC_SUCCESS;
}
/*****************************************************************************
> 函 数 名  : SDP_EncryptInit
> 功能描述  : 大块机密数据加密初始化接口。提供对称加密算法上下文初始化功能，初始化加密算法需要的密钥，IV等，
>             能用于大块机密数据需要分批加密的场景。为确保数据正确解密，要求应用保存密文头部。
> 密文结构  : cipher-head | hmac-head | cipher-data & hmac-data
> 输入参数  : ulDomain 表示作用域ID
> 输出参数  : pstSdpCtx 表示本轮数据加密过程建立的数据保护上下文
>             pstBodCipherHeader 表示将生成密文头部的内存块，如果追加HMAC，则包含HMAC头部
> 返 回 值  : 错误码，WSEC_SUCCESS表示函数正常返回，密文头部已经输出。
> 先验条件  : 无
> 后验条件  : 当返回WSEC_SUCCESS时，(*pulCHLen > 0) && (WSEC_NULL_PTR != *pstSdpCtx)
> 注意事项  : pucCipherHeader指向的内存块由调用者分配和释放，内存块的大小建议使用SDP_GetCipherDataLen获取。
>             pstSdpCtx，pulCHLen由调用者分配内存。

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_EncryptInit(
    WSEC_UINT32 ulDomain,
    SDP_CRYPT_CTX *pstSdpCtx,
    SDP_BOD_CIPHER_HEAD *pstBodCipherHeader
    )
{
    SDP_CRYPT_CTX_STRU *pstSdpCtxObj = WSEC_NULL_PTR;
    SDP_CIPHER_HEAD_STRU *pstCtxCipherHead = WSEC_NULL_PTR;
    WSEC_BYTE  aucKey[SDP_KEY_MAX_LEN] = {0};
    WSEC_ERR_T ulRet            = WSEC_SUCCESS;
    WSEC_UINT32 ulKeyLen        = 0;
    WSEC_UINT32 ulIVLen         = 0;

    /* check input */
    return_err_if_para_invalid("SDP_EncryptInit", pstSdpCtx && pstBodCipherHeader);

    /* construct context */
    pstSdpCtxObj = (SDP_CRYPT_CTX_STRU *)WSEC_MALLOC(SDP_CRYPT_CTX_LEN);
    return_oper_if((WSEC_NULL_PTR == pstSdpCtxObj), WSEC_LOG_E("[SDP] Memory for context allocate failed."), WSEC_ERR_MALLOC_FAIL);
    pstSdpCtxObj->eAlgType = WSEC_ALGTYPE_SYM;
    *pstSdpCtx = pstSdpCtxObj;
    pstCtxCipherHead = &(pstSdpCtxObj->stCipherHead);

    do
    {
        /* construct header, get MK-derived work key */
        /* header version is always the newest */
        WSEC_MEMSET(pstBodCipherHeader, sizeof(SDP_BOD_CIPHER_HEAD), 0, sizeof(SDP_BOD_CIPHER_HEAD));
        ulRet = SDP_FillCipherTextHeader(SDP_ALG_ENCRPT, ulDomain, pstCtxCipherHead,
                                         aucKey, &ulKeyLen, &ulIVLen);
        break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] Fill cipher text header failed."), oper_null);

        /* data encryption init */
        ulRet = CAC_EncryptInit(&(pstSdpCtxObj->stWsecCtx), pstCtxCipherHead->ulAlgId,
                                aucKey, ulKeyLen, pstCtxCipherHead->aucIV, ulIVLen);
        break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] CAC encrypt init failed."), ulRet = WSEC_ERR_ENCRPT_FAIL);

        /* destroy key */
        WSEC_DESTROY_KEY(aucKey, ulKeyLen);

        /* update header with real cipher length */
        pstCtxCipherHead->ulCDLen = 0u;

        /* output cipher header */
        break_oper_if(WSEC_MEMCPY(&pstBodCipherHeader->stCipherHeader, sizeof(pstBodCipherHeader->stCipherHeader), pstCtxCipherHead, sizeof(SDP_CIPHER_HEAD_STRU)) != EOK, 
                      WSEC_LOG_E4MEMCPY, ulRet = WSEC_ERR_MEMCPY_FAIL);

        /* compute data HMAC */
        if (pstCtxCipherHead->bHmacFlag )
        {
            ulRet = SDP_GetHmacAlgAttr(ulDomain, &pstBodCipherHeader->stHmacAlgAttr);
            break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E1("SDP_GetHmacAlgAttr() = %u.", ulRet), oper_null);

            ulRet = SDP_HmacInit(ulDomain, &pstBodCipherHeader->stHmacAlgAttr, &pstSdpCtxObj->stSdpCtxHmac);
            break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] SDP HMAC calculation init failed."), oper_null);
        }

        /* convert bytes-order */
        SDP_CvtByteOrder4CipherTextHeader((SDP_CIPHER_HEAD_STRU *)(&(pstBodCipherHeader->stCipherHeader)), wbcHost2Network);

        ulRet = WSEC_SUCCESS;
    } do_end;

    if (ulRet != WSEC_SUCCESS) { SDP_FreeCtx(pstSdpCtx); }

    return ulRet;
}

/*****************************************************************************
> 函 数 名  : SDP_EncryptUpdate
> 功能描述  : 大块机密数据加密更新接口。根据已建立的数据保护上下文对大块数据进行分段加密，
>             多次执行该函数来完成各个数据段的加密过程。大块机密数据的分段由调用者完成，
>             该函数只接收已经划分好的数据段作为参数，输出的密文应由调用者进行连接，构造完整密文。
> 密文结构  : cipher-head | hmac-head | cipher-data & hmac-data
> 输入参数  : pstSdpCtx 表示已建立的数据保护上下文，必须由SDP_EncryptInit输出
>             pucPlainText 表示将要加密的分段明文内存块
>             ulPTLen 表示将要加密的分段明文内存块的长度
>             pulCTLen 表示将要生成加密后密文的内存块的分配长度
> 输出参数  : pucCipherText 表示将要生成加密后密文的内存块
>             pulCTLen 表示将要生成加密后密文的内存块的实际长度
> 返 回 值  : 错误码，WSEC_SUCCESS表示函数正常返回，分段密文已经输出
> 先验条件  : (WSEC_NULL_PTR != pucPlainText) && (0 != ulPTLen)
>             && (WSEC_NULL_PTR != pucCipherText) && (WSEC_NULL_PTR != pulCTLen)
>             && (WSEC_NULL_PTR != pstSdpCtx) && (WSEC_NULL_PTR != *pstSdpCtx)
> 后验条件  : 当返回WSEC_SUCCESS时，*pulCTLen > 0
> 注意事项  : pucCipherText指向的内存块由调用者分配和释放，内存块的大小建议使用SDP_GetCipherDataLen获取。
>             pulCTLen由调用者分配内存。

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数
>  2.日    期   : 2014年11月6日
>    作    者   : j00265291
>    修改内容   : 修改函数，解决本函数失败时没有调用Final造成的内存泄露

*****************************************************************************/
WSEC_ERR_T SDP_EncryptUpdate (
      const SDP_CRYPT_CTX *pstSdpCtx,
      const WSEC_BYTE *pucPlainText,
      WSEC_UINT32 ulPTLen,
      WSEC_BYTE *pucCipherText,
      INOUT WSEC_UINT32 *pulCTLen
    )
{
    SDP_CRYPT_CTX_STRU      *pstSdpCtxObj = WSEC_NULL_PTR;
    SDP_CIPHER_HEAD_STRU    *pstCtxCipherHead = WSEC_NULL_PTR;
    WSEC_ERR_T  ulRet       = WSEC_SUCCESS;
    WSEC_UINT32 ulTempCTLen = 0;
    WSEC_UINT32 ulCTMaxLen  = 0;

    /* check input */
    return_err_if_para_invalid("SDP_EncryptUpdate", pucPlainText && pucCipherText && pstSdpCtx && pulCTLen && *pstSdpCtx && (ulPTLen > 0) && (*pulCTLen >= ulPTLen) && (pucPlainText != pucCipherText));

    ulCTMaxLen = *pulCTLen;
    ulTempCTLen = ulCTMaxLen;

    do 
    {
        /* construct context */
        pstSdpCtxObj = (SDP_CRYPT_CTX_STRU *)(*pstSdpCtx);
        pstCtxCipherHead = &(pstSdpCtxObj->stCipherHead);

        /* data encryption update */
        ulRet = CAC_EncryptUpdate(pstSdpCtxObj->stWsecCtx, pucPlainText, ulPTLen,
                                  pucCipherText, /* suggest INOUT */ &ulTempCTLen);
        if (ulRet!= WSEC_SUCCESS)
        {
            ulRet = WSEC_ERR_ENCRPT_FAIL;
            WSEC_LOG_E("[SDP] CAC encrypt update failed.");
            break;
        }

        /* update header with real cipher length, but not output to interface, avoiding rewrite */
        pstCtxCipherHead->ulCDLen += ulTempCTLen;

        /* compute data HMAC */
        if (pstCtxCipherHead->bHmacFlag )
        {
            ulRet = SDP_HmacUpdate(&(pstSdpCtxObj->stSdpCtxHmac), pucPlainText, ulPTLen);
            break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] SDP HMAC calculation update failed."), oper_null);
        }

        /* output param */
        *pulCTLen = ulTempCTLen;

        ulRet = WSEC_SUCCESS;
    } do_end;

    if (ulRet != WSEC_SUCCESS) { SDP_FreeCtx((SDP_CRYPT_CTX*)pstSdpCtx); }

    return ulRet;
}

/*****************************************************************************
> 函 数 名  : SDP_EncryptFinal
> 功能描述  : 大块机密数据加密结束接口，根据已建立的数据保护上下文，加密最后一个明文数据段。
>             生成最后更新的密文头部，根据配置选择是否生成HMAC密文。
> 密文结构  : cipher-head | hmac-head | cipher-data & hmac-data
> 输入参数  : pstSdpCtx 表示已建立的数据保护上下文，必须由SDP_EncryptInit输出；
>             pulCHLen表示将要生成加密结束后密文头部的内存块的分配长度；
>             pulCTLen表示将要生成加密结束后密文的内存块的分配长度；
>             pulHTLen表示将要生成加密结束后HMAC的内存块的分配长度；
> 输出参数  : pucCipherHeader 表示将要生成加密结束后密文头部的内存块；
>             pulCHLen 表示将要生成加密结束后密文头部的内存块的实际长度；
>             pucCipherText 表示将要生成加密结束后密文的内存块；
>             pulCTLen 表示将要生成加密结束后密文的内存块的实际长度；
>             pucHmacText表示将要生成加密结束后HMAC的内存块；
>             pulHTLen表示将要生成加密结束后HMAC的内存块的实际长度；
> 返 回 值  : 错误码，WSEC_SUCCESS表示函数正常返回，密文已经输出。
> 先验条件  : (WSEC_NULL_PTR != pucCipherText) && (WSEC_NULL_PTR != pulCTLen)
>             && (WSEC_NULL_PTR != pstSdpCtx) && (WSEC_NULL_PTR != *pstSdpCtx)
> 后验条件  : 当返回WSEC_SUCCESS时，(*pulCHLen> 0) && (*pulCTLen > 0)
> 注意事项  : pucHmacText，pucCipherHeader，pucCipherText指向的内存块由调用者分配和释放，
>             pucCipherHeader，pucCipherText内存块的大小建议使用SDP_GetCipherDataLen获取，
>             pucHmacText内存块的大小建议使用SDP_GetHmacLen获取。pulCHLen，pulCTLen，pulHTLen由调用者分配内存。
>             当配置要求追加HMAC时，参数满足(WSEC_NULL_PTR != pucHmacText) && (WSEC_NULL_PTR != pulHTLen)则输出HMAC，否则返回错误；
>             当配置不要求追加HMAC时，忽略参数pucHmacText，pulHTLen。

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_EncryptFinal(
      const SDP_CRYPT_CTX *pstSdpCtx,
      WSEC_BYTE *pucCipherText,
      INOUT WSEC_UINT32 *pulCTLen,
      WSEC_BYTE *pucHmacText,
      INOUT WSEC_UINT32 *pulHTLen
    )
{
    SDP_CRYPT_CTX_STRU          *pstSdpCtxObj = WSEC_NULL_PTR;
    SDP_CIPHER_HEAD_STRU        *pstCtxCipherHead = WSEC_NULL_PTR;
    WSEC_ERR_T  ulRet           = WSEC_SUCCESS;
    WSEC_UINT32 ulTempMacLen    = 0;
    WSEC_UINT32 ulTempCTLen     = 0;
    WSEC_UINT32 ulCTMaxLen      = 0;
    WSEC_UINT32 ulHTMaxLen      = 0;

    /* check input */
    return_err_if_para_invalid("SDP_EncryptFinal", pucCipherText && pstSdpCtx && (*pstSdpCtx) && (pulCTLen));

    ulCTMaxLen = *pulCTLen;
    ulTempCTLen = ulCTMaxLen;
    ulHTMaxLen = *pulHTLen;

    do
    {
        /* construct context */
        pstSdpCtxObj = (SDP_CRYPT_CTX_STRU *)(*pstSdpCtx);
        pstCtxCipherHead = &(pstSdpCtxObj->stCipherHead);
        break_oper_if((pstCtxCipherHead->bHmacFlag) && (WSEC_NULL_PTR == pucHmacText || WSEC_NULL_PTR == pulHTLen), 
                      WSEC_LOG_E("[SDP] Invalid parameter. Non-Null expected."), 
                      ulRet = WSEC_ERR_INVALID_ARG);

        /* data encryption final */
        ulRet = CAC_EncryptFinal(&(pstSdpCtxObj->stWsecCtx), pucCipherText, /* suggest INOUT */ &ulTempCTLen);
        break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] CAC encrypt update failed."), ulRet = WSEC_ERR_ENCRPT_FAIL);

        /* update header with real cipher length, but not output to interface, avoiding rewrite */
        pstCtxCipherHead->ulCDLen += ulTempCTLen;

        /* compute data HMAC */
        if (pstCtxCipherHead->bHmacFlag )
        {
            ulTempMacLen = ulHTMaxLen;
            ulRet = SDP_HmacFinal(&(pstSdpCtxObj->stSdpCtxHmac), pucHmacText, /* suggest INOUT */ &ulTempMacLen);
            break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] SDP HMAC calculation final failed."), oper_null);
        }

        /* output param */
        *pulCTLen = ulTempCTLen;
        *pulHTLen = ulTempMacLen;

        ulRet = WSEC_SUCCESS;
    } do_end;

    SDP_FreeCtx((SDP_CRYPT_CTX*)pstSdpCtx);
    return ulRet;
}

/*****************************************************************************
 函 数 名  : SDP_EncryptCancel
 功能描述  : 取消分段加密处理的后续过程.
             正常的分段加密处理过程是: Init ---> Update ---> Final. 如果在中间任意
                                              ↑________|
                                                   
             环节取消后续的处理, 则需要调用此函数.
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : pstSdpCtx: [in]指向上下信息的指针, [out]指针内容为NULL
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年5月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID SDP_EncryptCancel(SDP_CRYPT_CTX *pstSdpCtx)
{
    SDP_FreeCtx(pstSdpCtx);
}

/*****************************************************************************
> 函 数 名  : SDP_DecryptInit
> 功能描述  : 大块机密数据解密初始化接口。提供初始化数据保护上下文功能。
>             使用密文数据头部建立数据保护上下文，将建立好的数据保护上下文返回给应用。
>             根据密文头部选择是否校验HMAC密文。
> 密文结构  : cipher-head | hmac-head | cipher-data & hmac-data
> 输入参数  : pucCipherHeader 表示将要解析的密文头部；
>             pulCHLen 表示将要解析的密文头部的分配长度，该长度可能大于密文头部的实际长度；
>             pucHmacText 表示将要校验的HMAC密文，包括数据保护的HMAC头部；
>             ulHTLen表示将要校验的HMAC密文的长度；
> 输出参数  : pstSdpCtx 表示本轮数据解密过程建立的数据保护上下文；
>             pulCHLen 表示将要解析的密文头部的实际长度；
>             pulCDLen 表示将要解析的密文数据的实际长度；
> 返 回 值  : 错误码，WSEC_SUCCESS表示函数正常返回。
> 先验条件  : 无
> 后验条件  : 当返回WSEC_SUCCESS时，(*pulCHLen > 0) && (*pulCDLen > 0) && (WSEC_NULL_PTR != *pstSdpCtx)
> 注意事项  : pucCipherHeader指向的内存块的大小建议使用SDP_GetCipherDataLen获取。
>             pucHmacText指向的内存块的大小建议使用SDP_GetHmacLen获取。pstSdpCtx，pulCHLen，pulCDLen由调用者分配内存。
>             当密文包含追加HMAC时，参数满足(WSEC_NULL_PTR != pucHmacText) && (0 != ulHTLen)则校验HMAC，否则返回错误；
>             当密文不包含追加HMAC时，忽略参数pucHmacText，ulHTLen。

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_DecryptInit(
      WSEC_UINT32   ulDomain,
      SDP_CRYPT_CTX *pstSdpCtx,
      const SDP_BOD_CIPHER_HEAD* pstBodCipherHeader
    )
{
    SDP_CRYPT_CTX_STRU          *pstSdpCtxObj = WSEC_NULL_PTR;
    SDP_CIPHER_HEAD_STRU        *pstCtxCipherHead = WSEC_NULL_PTR;
    SDP_ERROR_CTX_STRU          stErrCtx;
    WSEC_ALGTYPE_E eAlgType     = WSEC_ALGTYPE_UNKNOWN;
    WSEC_BYTE aucKey[SDP_KEY_MAX_LEN];
    WSEC_ERR_T  ulRet           = WSEC_SUCCESS;
    WSEC_UINT32 ulKeyLen        = 0;
    WSEC_UINT32 ulIVLen         = 0;

    /* check input */
    return_err_if_para_invalid("SDP_DecryptInit", pstSdpCtx);

    /* construct context */
    pstSdpCtxObj = (SDP_CRYPT_CTX_STRU *)WSEC_MALLOC(SDP_CRYPT_CTX_LEN);
    return_oper_if(!pstSdpCtxObj, WSEC_LOG_E("[SDP] Memory for context allocate failed."), WSEC_ERR_MALLOC_FAIL);
    pstSdpCtxObj->eAlgType = WSEC_ALGTYPE_SYM;
    *pstSdpCtx = pstSdpCtxObj;
    pstCtxCipherHead = &(pstSdpCtxObj->stCipherHead);

    do
    {
        WSEC_ASSERT(sizeof(SDP_CIPHER_HEAD_STRU) <= sizeof(pstBodCipherHeader->stCipherHeader));
        break_oper_if(WSEC_MEMCPY(pstCtxCipherHead, sizeof(SDP_CIPHER_HEAD_STRU), &pstBodCipherHeader->stCipherHeader, sizeof(SDP_CIPHER_HEAD_STRU)) != EOK, 
                      WSEC_LOG_E4MEMCPY, ulRet = WSEC_ERR_MEMCPY_FAIL);
        
        /* parse header, its version is referred to cipher text */
        /* case is version 1 */
        SDP_CvtByteOrder4CipherTextHeader(pstCtxCipherHead, wbcNetwork2Host);

        /* check format */
        init_error_ctx(stErrCtx);
        update_error_ctx(stErrCtx, (SDP_CIPHER_TEXT_VER != pstCtxCipherHead->ulVersion),
                      WSEC_LOG_E2("[SDP] Cipher text version is incompatible, %d expected, %d actually.", SDP_CIPHER_TEXT_VER, pstCtxCipherHead->ulVersion), 
                      ulRet = WSEC_ERR_SDP_VERSION_INCOMPATIBLE);  
        update_error_ctx(stErrCtx, (ulDomain != pstCtxCipherHead->ulDomain), 
                      WSEC_LOG_E1("[SDP] Cipher text are marked with an unexpected domain %d.", pstCtxCipherHead->ulDomain), 
                      ulRet = WSEC_ERR_SDP_DOMAIN_UNEXPECTED);
        update_error_ctx(stErrCtx, (0 != pstCtxCipherHead->ulCDLen), 
                      WSEC_LOG_E("[SDP] Cipher data length must be 0."),
                      WSEC_ERR_SDP_INVALID_CIPHER_TEXT);
        update_error_ctx(stErrCtx, (pstCtxCipherHead->bHmacFlag < 0 || pstCtxCipherHead->bHmacFlag > SDP_HMAC_FLAG_MAX), 
                      WSEC_LOG_E("[SDP] Hmac flag is out of bounds."),
                      WSEC_ERR_SDP_INVALID_CIPHER_TEXT);
        update_error_ctx(stErrCtx, !KMC_IS_KEYITERATIONS_VALID(pstCtxCipherHead->ulIterCount), 
                      WSEC_LOG_E("[SDP] Iterator count is out of bounds."),
                      WSEC_ERR_SDP_INVALID_CIPHER_TEXT);
        update_error_ctx(stErrCtx, (WSEC_ALGTYPE_UNKNOWN == CAC_AlgId2Type(pstCtxCipherHead->ulAlgId)), 
                      WSEC_LOG_E("[SDP] CAC Get algorithm types failed."), 
                      WSEC_ERR_SDP_ALG_NOT_SUPPORTED);
        break_oper_if((stErrCtx.ulErrCount > SDP_MAX_ERR_FIELD_OF_CIPHER), WSEC_LOG_E("[SDP] Cipher text format is invalid."), ulRet = WSEC_ERR_SDP_INVALID_CIPHER_TEXT);
        break_oper_if((stErrCtx.ulErrCount > 0), oper_null, ulRet = stErrCtx.ulLastErrCode);

        /* Get key length, IV length, MAC length */
        ulRet = SDP_GetAlgProperty(pstCtxCipherHead->ulAlgId, WSEC_NULL_PTR, 0, 
                                   &eAlgType, &ulKeyLen, &ulIVLen, WSEC_NULL_PTR, WSEC_NULL_PTR);
        break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] Get algorithm property failed."), oper_null);
        break_oper_if(WSEC_ALGTYPE_SYM != eAlgType, 
                      WSEC_LOG_E1("[SDP] AlgType(%d) is out of bounds.", eAlgType), 
                      ulRet = WSEC_ERR_SDP_INVALID_CIPHER_TEXT);

        /* Get WK (work key) By KeyID */
        ulRet = SDP_GetWorkKeyByID(pstCtxCipherHead->ulDomain, pstCtxCipherHead->ulKeyId,
                                   pstCtxCipherHead->ulIterCount, pstCtxCipherHead->aucSalt,
                                   SDP_SALT_LEN, aucKey, ulKeyLen);
        break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] Get WK by KeyID failed."), oper_null);

        /* data decryption init */
        ulRet = CAC_DecryptInit(&(pstSdpCtxObj->stWsecCtx), pstCtxCipherHead->ulAlgId,
                                aucKey, ulKeyLen, pstCtxCipherHead->aucIV, ulIVLen);
        break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] CAC decrypt init failed."), ulRet = WSEC_ERR_DECRPT_FAIL);

        /* destroy key */
        WSEC_DESTROY_KEY(aucKey, ulKeyLen);

        /* verify HMAC init */
        if (pstCtxCipherHead->bHmacFlag )
        {
            ulRet = SDP_HmacInit(ulDomain, &pstBodCipherHeader->stHmacAlgAttr, &pstSdpCtxObj->stSdpCtxHmac);
            break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] Cipher text hmac verify init failed."), oper_null);
        }

        ulRet = WSEC_SUCCESS;
    } do_end;

    if (ulRet != WSEC_SUCCESS) { SDP_FreeCtx(pstSdpCtx); }

    return ulRet;
}

/*****************************************************************************
> 函 数 名  : SDP_DecryptUpdate
> 功能描述  : 大块机密数据解密更新接口，提供解密功能，根据数据保护上下文对大块密文进行分段解密。
>             通过数据保护上下文获取解密所需的算法、IV、Salt、密钥ID，对数据进行解密，将明文返回给应用。
>             多次执行该函数来完成各个密文段的解密过程。大块密文的分段由调用者完成，
>             该函数只接收已经划分好的密文段作为参数，输出的明文应由调用者进行连接，构造完整明文。
>             根据密文头部选择是否校验HMAC密文。
> 密文结构  : cipher-head | hmac-head | cipher-data & hmac-data
> 输入参数  : pstSdpCtx 表示已建立的数据保护上下文，必须由SDP_DecryptInit输出；
>             pucCipherText 表示将要解密的分段密文内存块；
>             ulCTLen 表示将要解密的分段密文内存块的长度；
>             pulPTLen 表示将要生成解密后明文的内存块的分配长度；
> 输出参数  : pucPlainText 表示将要生成解密后明文的内存块；
>             pulPTLen 表示将要生成解密后明文的内存块的实际长度；
> 返 回 值  : 错误码，WSEC_SUCCESS表示函数正常返回，分段明文已经输出。
> 先验条件  : (WSEC_NULL_PTR != pstSdpCtx) && (WSEC_NULL_PTR != *pstSdpCtx)
>             && (WSEC_NULL_PTR != pucCipherText) && (0 != ulCTLen)
>             && (WSEC_NULL_PTR != pucPlainText) && (WSEC_NULL_PTR != pulPTLen)
> 后验条件  : 当返回WSEC_SUCCESS时，*pulPTLen> 0
> 注意事项  : pucPlainText指向的内存块由调用者分配和释放，在AES场景下，明文长度至少包含一个解密明文块的大小，
>             内存块的大小建议使用SDP_GetCipherDataLen获取。pulPTLen由调用者分配内存。

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数
>  2.日    期   : 2014年11月6日
>    作    者   : j00265291
>    修改内容   : 修改函数，解决本函数失败时没有调用Final造成的内存泄露

*****************************************************************************/
WSEC_ERR_T SDP_DecryptUpdate(
      const SDP_CRYPT_CTX *pstSdpCtx,
      const WSEC_BYTE *pucCipherText,
      WSEC_UINT32 ulCTLen,
      WSEC_BYTE *pucPlainText,
      INOUT WSEC_UINT32 *pulPTLen
    )
{
    SDP_CRYPT_CTX_STRU          *pstSdpCtxObj = WSEC_NULL_PTR;
    SDP_CIPHER_HEAD_STRU        *pstCtxCipherHead = WSEC_NULL_PTR;
    WSEC_ERR_T  ulRet           = WSEC_SUCCESS;
    WSEC_UINT32 ulTempPTLen     = 0;
    WSEC_UINT32 ulPTMaxLen      = 0;

    /* check input */
    return_err_if_para_invalid("SDP_DecryptUpdate", pstSdpCtx && *pstSdpCtx && pucCipherText && pucPlainText && pulPTLen && (ulCTLen > 0) && (pucPlainText != pucCipherText));

    ulPTMaxLen = *pulPTLen;
    ulTempPTLen = ulPTMaxLen;

    do 
    {
        /* construct context */
        pstSdpCtxObj = (SDP_CRYPT_CTX_STRU *)(*pstSdpCtx);
        pstCtxCipherHead = &(pstSdpCtxObj->stCipherHead);

        /* data decryption update */
        ulRet = CAC_DecryptUpdate(pstSdpCtxObj->stWsecCtx,
                                  pucCipherText, ulCTLen, pucPlainText, /* suggest INOUT */ &ulTempPTLen);
        break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] CAC decrypt update failed."), ulRet = WSEC_ERR_DECRPT_FAIL);

        /* verify HMAC update */
        if ((pstCtxCipherHead->bHmacFlag) && (ulTempPTLen > 0))
        {
            ulRet = SDP_HmacUpdate(&pstSdpCtxObj->stSdpCtxHmac, pucPlainText, ulTempPTLen);
            break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] Cipher text hmac verify update failed."), oper_null);
        }

        /* output param */
        *pulPTLen = ulTempPTLen;

        ulRet = WSEC_SUCCESS;
    } do_end;

    if (ulRet != WSEC_SUCCESS) { SDP_FreeCtx((SDP_CRYPT_CTX*)pstSdpCtx); }

    return ulRet;
}

/*****************************************************************************
> 函 数 名  : SDP_DecryptFinal
> 功能描述  : 大块机密数据解密结束接口。提供解密功能，根据数据保护上下文对最后一个密文段进行解密。根据密文头部选择是否校验HMAC密文。
> 密文结构  : cipher-head | hmac-head | cipher-data & hmac-data
> 输入参数  : pstSdpCtx 表示已建立的数据保护上下文，必须由SDP_DecryptInit输出；
>             pucHmacText 表示将要校验的HMAC密文，包括数据保护的HMAC头部；
>             ulHTLen 表示将要校验的HMAC密文的长度；
>             pulPTLen 表示将要生成解密后明文的内存块的分配长度；
> 输出参数  : pucPlainText 表示将要生成解密后明文的内存块；
>             pulPTLen 表示将要生成解密后明文的内存块的实际长度；
> 返 回 值  : 错误码，WSEC_SUCCESS表示函数正常返回，明文已经输出。
> 先验条件  : (WSEC_NULL_PTR != pstSdpCtx) && (WSEC_NULL_PTR != *pstSdpCtx)
>             && (WSEC_NULL_PTR != pucPlainText) && (WSEC_NULL_PTR != pulPTLen)
> 后验条件  : 当返回WSEC_SUCCESS时，*pulPTLen > 0
> 注意事项  : pucPlainText指向的内存块由调用者分配和释放，在AES场景下，明文长度至少包含一个解密明文块的大小，
>             内存块的大小建议使用SDP_GetCipherDataLen获取。pulPTLen由调用者分配内存。
>             当密文包含追加HMAC时，参数满足(WSEC_NULL_PTR != pucHmacText) && (0 != ulHTLen)则校验HMAC，否则返回错误；
>             当密文不包含追加HMAC时，忽略参数pucHmacText，ulHTLen。

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_DecryptFinal(
      const SDP_CRYPT_CTX *pstSdpCtx,
      const WSEC_BYTE    *pucHmacText,
      WSEC_UINT32         ulHTLen,
      WSEC_BYTE          *pucPlainText,
      INOUT WSEC_UINT32   *pulPTLen
    )
{
    SDP_CRYPT_CTX_STRU          *pstSdpCtxObj = WSEC_NULL_PTR;
    SDP_CIPHER_HEAD_STRU        *pstCtxCipherHead = WSEC_NULL_PTR;
    WSEC_ERR_T  ulRet           = WSEC_SUCCESS;
    WSEC_UINT32 ulTempPTLen     = 0;
    WSEC_UINT32 ulPTMaxLen      = 0;
    WSEC_BUFF stHmacNew = {0};

    /* check input */
    return_err_if_para_invalid("SDP_DecryptFinal", pstSdpCtx && *pstSdpCtx && pucPlainText && pulPTLen);

    ulPTMaxLen = *pulPTLen;
    ulTempPTLen = ulPTMaxLen;

    do
    {
        /* construct context */
        pstSdpCtxObj = (SDP_CRYPT_CTX_STRU *)(*pstSdpCtx);
        pstCtxCipherHead = &(pstSdpCtxObj->stCipherHead);
        break_oper_if((pstCtxCipherHead->bHmacFlag) && ((WSEC_NULL_PTR == pucHmacText) || (0 == ulHTLen)), 
                      WSEC_LOG_E("[SDP] Invalid parameter. Non-Null HMAC pointer expected."), 
                      ulRet = WSEC_ERR_INVALID_ARG);

        /* data decryption final */
        ulRet = CAC_DecryptFinal(&(pstSdpCtxObj->stWsecCtx), pucPlainText, /* suggest INOUT */ &ulTempPTLen);
        break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] CAC decrypt final failed."), ulRet = WSEC_ERR_DECRPT_FAIL);

        /* verify HMAC final */
        if (pstCtxCipherHead->bHmacFlag)
        {
            if (ulTempPTLen > 0)
            {
                ulRet = SDP_HmacUpdate(&pstSdpCtxObj->stSdpCtxHmac, pucPlainText, ulTempPTLen);
                break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] Cipher text hmac verify update failed."), oper_null);
            }

            WSEC_BUFF_ALLOC(stHmacNew, WSEC_HMAC_LEN_MAX);
            break_oper_if(!stHmacNew.pBuff, WSEC_LOG_E4MALLOC(stHmacNew.nLen), ulRet = WSEC_ERR_MALLOC_FAIL);

            ulRet = SDP_HmacFinal(&pstSdpCtxObj->stSdpCtxHmac, (WSEC_BYTE*)stHmacNew.pBuff, &stHmacNew.nLen);
            break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] Cipher text hmac verify final failed."), oper_null);

            break_oper_if(stHmacNew.nLen != ulHTLen, WSEC_LOG_E("Verify hmac failed for HMAC-LEN is not same."), ulRet = WSEC_ERR_HMAC_AUTH_FAIL);
            break_oper_if(WSEC_MEMCMP(stHmacNew.pBuff, pucHmacText, ulHTLen) != 0, WSEC_LOG_E("Verify hmac failed for HMAC is not same."), ulRet = WSEC_ERR_HMAC_AUTH_FAIL);
        }

        /* output param */
        *pulPTLen = ulTempPTLen;
    } do_end;

    SDP_FreeCtx((SDP_CRYPT_CTX*)pstSdpCtx);
    return ulRet;
}

/*****************************************************************************
 函 数 名  : SDP_DecrypCancel
 功能描述  : 取消分段解密处理的后续过程.
             正常的分段解密处理过程是: Init ---> Update ---> Final. 如果在中间任意
                                              ↑________|
                                                   
             环节取消后续的处理, 则需要调用此函数.
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : pstSdpCtx: [in]指向上下信息的指针, [out]指针内容为NULL
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年5月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID SDP_DecrypCancel(SDP_CRYPT_CTX *pstSdpCtx)
{
    SDP_FreeCtx(pstSdpCtx);
}

/*****************************************************************************
 函 数 名  : SDP_FileEncrypt
 功能描述  : 将文件加密
 纯 入 参  : ulDomain: 功能域标识
             pszPlainFile:  明文文件
             pszCipherFile: 密文文件
             pfGetFileDateTime: 获取文件时间信息函数, 如果Caller提供, 则将原文件的
                                时间信息记录, 解密时可以还原其时间信息.[可NULL]
             pstRptProgress:    进度上报设置.[可NULL]
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月19日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T SDP_FileEncrypt(WSEC_UINT32 ulDomain, const WSEC_CHAR *pszPlainFile, const WSEC_CHAR *pszCipherFile, WSEC_FP_GetFileDateTime pfGetFileDateTime, const WSEC_PROGRESS_RPT_STRU* pstRptProgress)
{
    WSEC_FILE fRead = WSEC_NULL_PTR;
    WSEC_FILE fWri  = WSEC_NULL_PTR;
    WSEC_BUFF stPlain  = {0};
    WSEC_BUFF stCipher = {0};
    WSEC_BUFF stHmac   = {0};
    SDP_CIPHER_FILE_HDR_STRU stFileHdr = {0};
    WSEC_SIZE_T nPlainLen, nCipherLen, nHmacLen;
    WSEC_FILE_LEN ulSrcFileLen = 0, nReadSize = 0;
    SDP_CRYPT_CTX stSdpCtx = {0};
    SDP_BOD_CIPHER_HEAD stBodCipherHeader = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS, nTemp;
    WSEC_SPEND_TIME_STRU stTimer = {0};
    WSEC_BOOL bCancel = WSEC_FALSE; /* 操作过程中是否被APP取消 */
    WSEC_BOOL bRmvWhenFail = WSEC_FALSE; /* 操作失败后是否删除密文文件. */

    WSEC_ASSERT(sizeof(stFileHdr.abFormatFlag) == sizeof(g_CipherFileFlag));
    return_err_if_para_invalid("SDP_FileEncrypt", pszPlainFile && pszCipherFile);
    return_err_if_para_invalid("SDP_FileEncrypt", WSEC_NOT_IS_EMPTY_STRING(pszPlainFile) && WSEC_NOT_IS_EMPTY_STRING(pszCipherFile));

    return_oper_if(!WSEC_GetFileLen(pszPlainFile, &ulSrcFileLen), WSEC_LOG_E1("Cannot acess '%s'", pszPlainFile), WSEC_ERR_OPEN_FILE_FAIL);

    /* 1. 密文文件头准备 */
    stFileHdr.ulPlainBlockLenMax = WSEC_FILE_IO_SIZE_MAX;
    return_oper_if(!WSEC_GetUtcDateTime(&stFileHdr.tCreateFileTimeUtc), WSEC_LOG_E("WSEC_GetUtcDateTime() fail."), WSEC_ERR_GET_CURRENT_TIME_FAIL);
    nErrCode = SDP_GetCipherDataLen(stFileHdr.ulPlainBlockLenMax, &stFileHdr.ulCipherBlockLenMax);
    return_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("SDP_GetCipherDataLen()=%u", nErrCode), nErrCode);
    return_oper_if(WSEC_MEMCPY(stFileHdr.abFormatFlag, sizeof(stFileHdr.abFormatFlag), g_CipherFileFlag, sizeof(g_CipherFileFlag)) != EOK, WSEC_LOG_E4MEMCPY, WSEC_ERR_MEMCPY_FAIL);
    stFileHdr.ulVer = SDP_CIPHER_FILE_VER;

    if (pfGetFileDateTime)
    {
        if (!pfGetFileDateTime(pszPlainFile, &stFileHdr.tSrcFileCreateTime, &stFileHdr.tSrcFileEditTime)) /* 确保时间数据无效, 避免APP弄脏数据 */
        {
            stFileHdr.tSrcFileCreateTime.uwYear = 0;
            stFileHdr.tSrcFileEditTime.uwYear = 0;
        }
    }

    /* 2. 打开读&写文件 */
    fRead = WSEC_FOPEN(pszPlainFile, "rb");
    return_oper_if(!fRead, WSEC_LOG_E1("Open '%s' fail.", pszPlainFile), WSEC_ERR_OPEN_FILE_FAIL);
    fWri = WSEC_FOPEN(pszCipherFile, "wb");

    /* 3. 边读明文文件, 边生成密文并写密文 */
    do
    {
        break_oper_if(!fWri, WSEC_LOG_E1("Cannot write '%s'.", pszCipherFile), nErrCode = WSEC_ERR_WRI_FILE_FAIL);
        WSEC_BUFF_ALLOC(stPlain, stFileHdr.ulPlainBlockLenMax);
        WSEC_BUFF_ALLOC(stCipher, stFileHdr.ulCipherBlockLenMax);
        WSEC_BUFF_ALLOC(stHmac, SDP_HMAC_MAX_SIZE); /* 空间不做精确计算, 按最大空间分配 */
        break_oper_if(!stPlain.pBuff, WSEC_LOG_E4MALLOC(stPlain.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!stCipher.pBuff, WSEC_LOG_E4MALLOC(stCipher.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!stHmac.pBuff, WSEC_LOG_E4MALLOC(stHmac.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);

        /* 3.1 写密文文件头 */
        bRmvWhenFail = WSEC_TRUE;
        SDP_CvtByteOrder4CipherFileHdr(&stFileHdr, wbcHost2Network);
        nErrCode = WSEC_WriteTlv(fWri, SDP_CFT_FILE_HDR, sizeof(stFileHdr), &stFileHdr);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("WSEC_WriteTlv()=%u", nErrCode), oper_null);

        /* 3.2 大数据加密初始化 */
        nErrCode = SDP_EncryptInit(ulDomain, &stSdpCtx, &stBodCipherHeader);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("SDP_EncryptInit()=%u", nErrCode), oper_null);

        /* 3.2.1 写密文头 */
        nErrCode = WSEC_WriteTlv(fWri, SDP_CFT_CIPHER_HDR, sizeof(stBodCipherHeader), &stBodCipherHeader);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("WSEC_WriteTlv()=%u", nErrCode), oper_null);

        /* 3.3 读明文文件, 加密, 写密文文件 */
        while (!WSEC_FEOF(fRead))
        {
            nPlainLen = WSEC_FREAD(stPlain.pBuff, 1, stPlain.nLen, fRead);
            break_oper_if(WSEC_FERROR(fRead), WSEC_LOG_E1("Read file(%s) fail.", pszPlainFile), nErrCode = WSEC_ERR_READ_FILE_FAIL);
            if (0 == nPlainLen) {break;}
            nReadSize += nPlainLen; /* 已经处理明文长度 */
            nCipherLen = stCipher.nLen;
            nErrCode = SDP_EncryptUpdate(&stSdpCtx, (const WSEC_BYTE*)stPlain.pBuff, nPlainLen, (WSEC_BYTE*)stCipher.pBuff, &nCipherLen);
            break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("SDP_EncryptUpdate()=%u", nErrCode), oper_null);

            /* 写密文体 */
            if (nCipherLen > 0)
            {
                nErrCode = WSEC_WriteTlv(fWri, SDP_CFT_CIPHER_BODY, nCipherLen, stCipher.pBuff);
                WSEC_FFLUSH(fWri);
                break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("WSEC_WriteTlv()=%u", nErrCode), oper_null);
            }

            /* 进度上报 */
            WSEC_RptProgress(pstRptProgress, &stTimer, ulSrcFileLen, nReadSize, &bCancel);
            if (bCancel) {break;}
        }
        if (!bCancel) {WSEC_RptProgress(pstRptProgress, WSEC_NULL_PTR, ulSrcFileLen, ulSrcFileLen, WSEC_NULL_PTR);} /* 确保进度按100%上报 */

        /* 3.4 加密结束 */
        nCipherLen = stCipher.nLen;
        nHmacLen = stHmac.nLen;
        nTemp = SDP_EncryptFinal(&stSdpCtx, (WSEC_BYTE*)stCipher.pBuff, &nCipherLen, (WSEC_BYTE*)stHmac.pBuff, &nHmacLen);
        break_oper_if(bCancel, WSEC_LOG_E("Encrypt progress canceled."), nErrCode = WSEC_ERR_CANCEL_BY_APP);
        break_oper_if(nTemp != WSEC_SUCCESS, WSEC_LOG_E1("SDP_EncryptFinal()=%u", nErrCode), nErrCode = nTemp);
        
        if (nErrCode != WSEC_SUCCESS) {break;}

        if (nCipherLen > 0)
        {
            nErrCode = WSEC_WriteTlv(fWri, SDP_CFT_CIPHER_BODY, nCipherLen, stCipher.pBuff);
            WSEC_FFLUSH(fWri);
            break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("WSEC_WriteTlv()=%u", nErrCode), oper_null);
        }

        /* 写HMAC */
        if (nHmacLen > 0)
        {
            nErrCode = WSEC_WriteTlv(fWri, SDP_CFT_HMAC_VAL, nHmacLen, stHmac.pBuff);
            WSEC_FFLUSH(fWri);
            break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("WSEC_WriteTlv()=%u", nErrCode), oper_null);
        }
    }do_end;    

    /* 4. 释放资源 */
    WSEC_FCLOSE(fRead);
    WSEC_FFLUSH(fWri);
    WSEC_FCLOSE(fWri);
    WSEC_BUFF_FREE(stPlain);
    WSEC_BUFF_FREE(stCipher);
    WSEC_BUFF_FREE(stHmac);
    SDP_FreeCtx(&stSdpCtx);

    /* Misinformation: FORTIFY.Race_Condition--File_System_Access */
    if ((nErrCode != WSEC_SUCCESS) && bRmvWhenFail) {WSEC_UNCARE(WSEC_FREMOVE(pszCipherFile));}/* 加密失败, 则清除密文文件 */

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : SDP_FileDecrypt
 功能描述  : 将文件解密
 纯 入 参  : pszCipherFile: 密文文件
             pszPlainFile: 明文文件
             pfSetFileDateTime: 设置文件时间信息函数, 如果Caller提供, 则将原文件的
                                时间信息记录还原[可NULL]
             pstRptProgress:    进度上报设置.[可NULL]
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年12月19日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T SDP_FileDecrypt(WSEC_UINT32 ulDomain, const WSEC_CHAR *pszCipherFile, const WSEC_CHAR *pszPlainFile, WSEC_FP_SetFileDateTime pfSetFileDateTime, const WSEC_PROGRESS_RPT_STRU* pstRptProgress)
{
    WSEC_FILE fRead = WSEC_NULL_PTR;
    WSEC_FILE fWri  = WSEC_NULL_PTR;
    WSEC_BUFF stPlain  = {0};
    WSEC_BUFF stCipher = {0};
    WSEC_BUFF stHmac   = {0};
    SDP_CIPHER_FILE_HDR_STRU stFileHdr = {0};
    SDP_BOD_CIPHER_HEAD stBodCipherHeader = {0};
    SDP_CRYPT_CTX stSdpCtx = {0};
    WSEC_TLV_STRU stTlv = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS, nTemp;
    WSEC_FILE_LEN ulSrcFileLen = 0, nReadSize = 0;
    WSEC_SPEND_TIME_STRU stTimer = {0};
    WSEC_BOOL bCancel = WSEC_FALSE; /* 操作过程中是否被APP取消 */

    return_err_if_para_invalid("SDP_FileEncrypt", pszPlainFile && pszCipherFile);
    return_err_if_para_invalid("SDP_FileEncrypt", WSEC_NOT_IS_EMPTY_STRING(pszPlainFile) && WSEC_NOT_IS_EMPTY_STRING(pszCipherFile));

    return_oper_if(!WSEC_GetFileLen(pszCipherFile, &ulSrcFileLen), WSEC_LOG_E1("Cannot acess '%s'", pszCipherFile), WSEC_ERR_OPEN_FILE_FAIL);

    do
    {
        fRead = WSEC_FOPEN(pszCipherFile, "rb");
        fWri = WSEC_FOPEN(pszPlainFile, "wb");
        break_oper_if(!fRead, WSEC_LOG_E1("Cannot open '%s'", pszCipherFile), nErrCode = WSEC_ERR_OPEN_FILE_FAIL);
        break_oper_if(!fWri, WSEC_LOG_E1("Cannot write '%s'", pszPlainFile), nErrCode = WSEC_ERR_WRI_FILE_FAIL);

        /* 1. 密文文件检测 */
        /* 1.1 读密文文件头 */
        if (!WSEC_ReadTlv(fRead, &stFileHdr, sizeof(stFileHdr), &stTlv, &nErrCode)) {break;}
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("Read '%s' fail.", pszCipherFile), oper_null);
        break_oper_if(stTlv.ulTag != SDP_CFT_FILE_HDR, WSEC_LOG_E1("'%s' is not expected encrpt file for TLV's tag is not SDP_CFT_FILE_HDR.", pszCipherFile), nErrCode = WSEC_ERR_FILE_FORMAT);
        SDP_CvtByteOrder4CipherFileHdr(&stFileHdr, wbcNetwork2Host);
        nReadSize += sizeof(stFileHdr);

        stFileHdr.ulPlainBlockLenMax += 100; /* OpenSSL要求明文缓冲区长度大于实际长度一个Block, 为安全考虑, 将明文长度放大以便分配足够的缓冲区给OpenSSL使用. */

        /* 1) 检查格式 */
        break_oper_if(WSEC_MEMCMP(g_CipherFileFlag, stFileHdr.abFormatFlag, sizeof(g_CipherFileFlag)) != 0,
                      WSEC_LOG_E1("The format of '%s' is incorrect.", pszCipherFile), nErrCode = WSEC_ERR_FILE_FORMAT);

        /* 2) 检查版本 */
        if (stFileHdr.ulVer != SDP_CIPHER_FILE_VER) /* 版本不正确, 需要自动升级, 后续开发者完成. */
        {
            WSEC_LOG_E1("The version of '%s' is incorrect.", pszCipherFile);
            nErrCode = WSEC_ERR_SDP_VERSION_INCOMPATIBLE;
            break;
        }

        /* 1.2 读密文头 */
        if (!WSEC_ReadTlv(fRead, &stBodCipherHeader, sizeof(stBodCipherHeader), &stTlv, &nErrCode)) {break;}
        break_oper_if(stTlv.ulTag != SDP_CFT_CIPHER_HDR, WSEC_LOG_E1("'%s' is not expected encrpt file for TLV's tag not is SDP_CFT_CIPHER_HDR.", pszCipherFile), nErrCode = WSEC_ERR_FILE_FORMAT);

        nReadSize += sizeof(stBodCipherHeader);

        /* 2 解密密文文件密文段 */
        WSEC_BUFF_ALLOC(stCipher, stFileHdr.ulCipherBlockLenMax);
        WSEC_BUFF_ALLOC(stPlain, stFileHdr.ulPlainBlockLenMax);

        break_oper_if(!stCipher.pBuff, WSEC_LOG_E4MALLOC(stCipher.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);
        break_oper_if(!stPlain.pBuff, WSEC_LOG_E4MALLOC(stPlain.nLen), nErrCode = WSEC_ERR_MALLOC_FAIL);

        nErrCode = SDP_DecryptInit(ulDomain, &stSdpCtx, &stBodCipherHeader);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("SDP_DecryptInit()=%u", nErrCode), oper_null);

        while (WSEC_ReadTlv(fRead, stCipher.pBuff, stCipher.nLen, &stTlv, &nErrCode))
        {
            break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("WSEC_ReadTlv()=%u", nErrCode), oper_null);
            nReadSize += stTlv.ulLen; /* 已经处理密文长度 */

            if (SDP_CFT_HMAC_VAL == stTlv.ulTag)
            {
                WSEC_BUFF_ASSIGN(stHmac, stTlv.pVal, stTlv.ulLen);
                break;
            }
            break_oper_if(stTlv.ulTag != SDP_CFT_CIPHER_BODY, WSEC_LOG_E2("Unexpected TLV's Tag(%u) in %s", stTlv.ulTag, pszCipherFile), nErrCode = WSEC_ERR_FILE_FORMAT);

            /* 解密密文段 */
            stPlain.nLen = stFileHdr.ulPlainBlockLenMax;
            nErrCode = SDP_DecryptUpdate(&stSdpCtx, stTlv.pVal, stTlv.ulLen, stPlain.pBuff, &stPlain.nLen);

            break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("SDP_DecryptUpdate()=%u", nErrCode), oper_null);
            break_oper_if(!WSEC_FWRITE_MUST(stPlain.pBuff, stPlain.nLen, fWri), WSEC_LOG_E1("Write '%s' fail.", pszPlainFile), nErrCode = WSEC_ERR_WRI_FILE_FAIL);
            WSEC_FFLUSH(fWri);

            /* 进度上报 */
            WSEC_RptProgress(pstRptProgress, &stTimer, ulSrcFileLen, nReadSize, &bCancel);
            if (bCancel) {break;}
        }
        if ((!bCancel)) {WSEC_RptProgress(pstRptProgress, WSEC_NULL_PTR, ulSrcFileLen, ulSrcFileLen, WSEC_NULL_PTR);} /* 确保进度是100% */
        
        stPlain.nLen = stFileHdr.ulPlainBlockLenMax;
        nTemp = SDP_DecryptFinal(&stSdpCtx, stHmac.pBuff, stHmac.nLen, stPlain.pBuff, &stPlain.nLen);
        break_oper_if(bCancel, WSEC_LOG_E("Decrypt progress canceled."), nErrCode = WSEC_ERR_CANCEL_BY_APP);
        break_oper_if(nTemp != WSEC_SUCCESS, WSEC_LOG_E1("SDP_DecryptFinal()=%u", nTemp), nErrCode = nTemp);

        if (nErrCode != WSEC_SUCCESS) {break;}
        if (0 == stPlain.nLen) {break;}

        break_oper_if(!WSEC_FWRITE_MUST(stPlain.pBuff, stPlain.nLen, fWri), WSEC_LOG_E1("Write '%s' fail.", pszPlainFile), nErrCode = WSEC_ERR_WRI_FILE_FAIL);
    }do_end;

    WSEC_FCLOSE(fRead);
    WSEC_FCLOSE(fWri);
    WSEC_BUFF_FREE(stPlain);
    WSEC_BUFF_FREE(stCipher);
    SDP_FreeCtx(&stSdpCtx);

    if (WSEC_SUCCESS == nErrCode) /* 酌情还原明文文件的日期信息 */
    {
        if ((pfSetFileDateTime) && WSEC_IsDateTime(&stFileHdr.tSrcFileCreateTime) && WSEC_IsDateTime(&stFileHdr.tSrcFileEditTime))
        {
            pfSetFileDateTime(pszPlainFile, &stFileHdr.tSrcFileCreateTime, &stFileHdr.tSrcFileEditTime);
        }
    }
    else
    {
        /* Misinformation: FORTIFY.Race_Condition--File_System_Access */
        WSEC_UNCARE(WSEC_FREMOVE(pszPlainFile));
    }
    
    return nErrCode;
}


/*****************************************************************************
> 函 数 名  : SDP_GetHmacLen
> 功能描述  : 计算HMAC密文（hmac-data）长度。
> 输入参数  : 无
> 输出参数  : 无
> 返 回 值  : 加密后HMAC密文的长度，包括HMAC头部的长度。
> 注意事项  : 无

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数
>  2.日    期   : 2015年7月31日
>    作    者   : z00118096
>    修改内容   : 老版本的该接口无法解决竞争问题: 
         1) 线程P1调用此函数获取HMAC长度(假若此时的算法配置为HMAC256);
         2) 线程P2调用KMC_SetDataProtectCfg更改了算法配置为HMAC512;
         3) P1调用SDP_Hmac, 由于APP处理1)时提供的HMAC空间过小, 则出现
            内存越界写HMAC, 而程序无法感知.

         修复方法: 使用最大的HMAC长来计算密文长度, 与算法无关.
*****************************************************************************/
WSEC_ERR_T SDP_GetHmacLen(WSEC_UINT32* pulHmacLen)
{
    return_err_if_para_invalid("SDP_GetHmacLen", pulHmacLen);

    *pulHmacLen = SDP_HMAC_HEAD_LEN + SDP_HMAC_MAX_SIZE;

    return WSEC_SUCCESS;
}

/*****************************************************************************
> 函 数 名  : SDP_Hmac
> 功能描述  : 机密数据计算Hmac接口。
> 密文结构  : hmac-head | hmac-data
> 输入参数  : ulDomain 表示作用域ID
>             pucPlainText 表示将要计算HMAC的明文内存块
>             ulPTLen 表示将要计算HMAC的明文内存块的长度
>             pulHTLen 表示将要生成HMAC的内存块的分配长度
> 输出参数  : pucHmacText 表示将要生成HMAC的内存块，包括HMAC头部
>             pulHTLen 表示将要生成HMAC的内存块的实际长度，包括HMAC头部的长度
> 返 回 值  : 错误码，WSEC_SUCCESS表示函数正常返回，HMAC已经输出
> 先验条件  : 无
> 后验条件  : 当返回WSEC_SUCCESS时，*pulHTLen> 0
> 注意事项  : pucHmacText指向的内存块由调用者分配和释放，内存块的大小建议使用SDP_GetHmacLen获取。
>             pulHTLen由调用者分配内存。

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_Hmac(
      WSEC_UINT32   ulDomain,
      const WSEC_BYTE *pucPlainText,
      WSEC_UINT32   ulPTLen,
      WSEC_BYTE    *pucHmacText,
      INOUT WSEC_UINT32   *pulHTLen
    )
{
    SDP_HMAC_HEAD_STRU          *pstHmacHead = WSEC_NULL_PTR;
    WSEC_BYTE aucKey[SDP_KEY_MAX_LEN];
    WSEC_ERR_T  ulRet       = WSEC_SUCCESS;
    WSEC_UINT32 ulKeyLen    = 0;
    WSEC_UINT32 ulTempHTLen = 0;
    WSEC_UINT32 ulHTMaxLen  = 0;

    /* check input */
    return_err_if_para_invalid("SDP_Hmac", pucPlainText && pucHmacText && pulHTLen && (pucPlainText != pucHmacText));

    ulHTMaxLen = *pulHTLen;
    ulTempHTLen = ulHTMaxLen;

    /* construct header, get MK-derived work key */
    return_oper_if((ulHTMaxLen <= SDP_HMAC_HEAD_LEN), WSEC_LOG_E("[SDP] Buffer for hmac text is not enough."), WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH);
    WSEC_MEMSET(pucHmacText, ulHTMaxLen, 0, SDP_HMAC_HEAD_LEN);
    pstHmacHead = (SDP_HMAC_HEAD_STRU *)pucHmacText;
    ulRet = SDP_FillHmacTextHeader(SDP_ALG_INTEGRITY, ulDomain, pstHmacHead, aucKey, &ulKeyLen);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] Fill HMAC text header failed."), ulRet);

    /* HMAC calculation */
    ulRet = CAC_Hmac(pstHmacHead->ulAlgId, aucKey, ulKeyLen, pucPlainText, ulPTLen,
                     pucHmacText + SDP_HMAC_HEAD_LEN, /* suggest INOUT */ &ulTempHTLen);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] CAC calculate hmac failed."), WSEC_ERR_HMAC_FAIL);

    /* destroy key */
    WSEC_DESTROY_KEY(aucKey, ulKeyLen);

    /* header not filled with real HMAC length, allocate fixed max space for HMAC */
    /* compute HMAC length, including header length */
    ulTempHTLen += SDP_HMAC_HEAD_LEN;

    /* byte order process */
    SDP_CvtByteOrder4HmacTextHeader(pstHmacHead, wbcHost2Network);

    /* output param */
    *pulHTLen = ulTempHTLen;

    return WSEC_SUCCESS;
}

/*****************************************************************************
> 函 数 名  : SDP_VerifyHmac
> 功能描述  : HMAC校验接口。提供校验机密数据HMAC的功能。
>             通过HMAC头部获取校验所需的算法、Salt、密钥ID，校验HMAC是否正确。
> 密文结构  : hmac-head | hmac-data
> 输入参数  : pucPlainText 表示将要校验HMAC密文对应的明文内存块
>             ulPTLen 表示将要校验HMAC密文对应的明文内存块的长度
>             pucHmacText 表示将要校验的HMAC密文，包括数据保护的HMAC头部
>             ulHTLen 表示将要校验的HMAC密文的长度
> 输出参数  : 无
> 返 回 值  : 错误码，WSEC_SUCCESS表示函数正常返回，HMAC密文校验结果正确。

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_VerifyHmac(
      WSEC_UINT32       ulDomain,
      const WSEC_BYTE  *pucPlainText,
      WSEC_UINT32       ulPTLen,
      const WSEC_BYTE  *pucHmacText,
      WSEC_UINT32       ulHTLen
    )
{
    SDP_HMAC_HEAD_STRU          *pstHmacHead = WSEC_NULL_PTR;
    SDP_ERROR_CTX_STRU          stErrCtx;
    WSEC_ALGTYPE_E eAlgType = WSEC_ALGTYPE_UNKNOWN;
    WSEC_BYTE aucKey[SDP_KEY_MAX_LEN];
    WSEC_BYTE aucTempPTMac[SDP_PTMAC_MAX_LEN];
    WSEC_ERR_T  ulRet           = WSEC_SUCCESS;
    WSEC_UINT32 ulTempPTMacLen  = 0;
    WSEC_UINT32 ulKeyLen        = 0;

    /* check input */
    return_err_if_para_invalid("SDP_VerifyHmac", pucHmacText && pucPlainText && (ulHTLen > 0) && (pucPlainText != pucHmacText));

    return_oper_if((ulHTLen <= SDP_HMAC_HEAD_LEN), WSEC_LOG_E("[SDP] Invalid parameter. Buffer for hmac text is not enough."), WSEC_ERR_INPUT_BUFF_NOT_ENOUGH);
    return_oper_if((ulHTLen > SDP_HMAC_HEAD_LEN + SDP_PTMAC_MAX_LEN), WSEC_LOG_E("[SDP] Invalid parameter. Buffer for hmac text is out of bounds."), WSEC_ERR_INVALID_ARG);

    /* parse header */
    pstHmacHead = (SDP_HMAC_HEAD_STRU *)pucHmacText;

    /* byte order process */
    SDP_CvtByteOrder4HmacTextHeader(pstHmacHead, wbcNetwork2Host);

    /* check format */
    init_error_ctx(stErrCtx);
    update_error_ctx(stErrCtx, (SDP_HMAC_VER != pstHmacHead->ulVersion),
                   WSEC_LOG_E2("[SDP] Cipher text version is incompatible, %d expected, %d actually.", SDP_HMAC_VER, pstHmacHead->ulVersion), 
                   WSEC_ERR_SDP_VERSION_INCOMPATIBLE);    
    update_error_ctx(stErrCtx, (ulDomain != pstHmacHead->ulDomain), 
                   WSEC_LOG_E1("[SDP] Cipher text are marked with an unexpected domain %d.", pstHmacHead->ulDomain), 
                   WSEC_ERR_SDP_DOMAIN_UNEXPECTED);
    update_error_ctx(stErrCtx, (!KMC_IS_KEYITERATIONS_VALID(pstHmacHead->ulIterCount)), 
                   WSEC_LOG_E("[SDP] Iterator count is out of bounds."),
                   WSEC_ERR_SDP_INVALID_CIPHER_TEXT);
    update_error_ctx(stErrCtx, (WSEC_ALGTYPE_UNKNOWN == CAC_AlgId2Type(pstHmacHead->ulAlgId)), 
                   WSEC_LOG_E("[SDP] CAC Get algorithm types failed."), 
                   WSEC_ERR_SDP_ALG_NOT_SUPPORTED);
    return_oper_if((stErrCtx.ulErrCount > SDP_MAX_ERR_FIELD_OF_CIPHER), WSEC_LOG_E("[SDP] Cipher text format is invalid."), WSEC_ERR_SDP_INVALID_CIPHER_TEXT);
    return_oper_if((stErrCtx.ulErrCount > 0), oper_null, stErrCtx.ulLastErrCode);

    /* Get key length, MAC length */
    ulRet = SDP_GetAlgProperty(pstHmacHead->ulAlgId, WSEC_NULL_PTR, 0, &eAlgType, &ulKeyLen, WSEC_NULL_PTR, WSEC_NULL_PTR, &ulTempPTMacLen);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] Get algorithm property failed."), ulRet);
    return_oper_if((WSEC_ALGTYPE_HMAC != eAlgType), WSEC_LOG_E1("[SDP] AlgType(%d) is out of bounds.", eAlgType), WSEC_ERR_SDP_INVALID_CIPHER_TEXT);
    return_oper_if((ulHTLen < SDP_HMAC_HEAD_LEN + ulTempPTMacLen), WSEC_LOG_E("[SDP] Invalid parameter. Buffer for hmac text is not enough."), WSEC_ERR_INPUT_BUFF_NOT_ENOUGH);

    /* Get WK (work key) By KeyID */
    ulRet = SDP_GetWorkKeyByID(pstHmacHead->ulDomain, pstHmacHead->ulKeyId, pstHmacHead->ulIterCount,
                               pstHmacHead->aucSalt, SDP_SALT_LEN, aucKey, ulKeyLen);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] Get WK by KeyID failed."), ulRet);

    /* HMAC calculation */
    ulRet = CAC_Hmac(pstHmacHead->ulAlgId, aucKey, ulKeyLen, pucPlainText, ulPTLen,
                     aucTempPTMac, &ulTempPTMacLen);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] CAC calculate hmac failed."), WSEC_ERR_HMAC_FAIL);

    /* destroy key */
    WSEC_DESTROY_KEY(aucKey, ulKeyLen);

    /* verify HMAC */
    return_oper_if(WSEC_MEMCMP(aucTempPTMac, pucHmacText + SDP_HMAC_HEAD_LEN, ulTempPTMacLen) != 0, 
                   WSEC_LOG_E("[SDP] HMAC failed to pass the verification."), WSEC_ERR_HMAC_AUTH_FAIL);

    return WSEC_SUCCESS;
}

/*****************************************************************************
 函 数 名  : SDP_GetHmacAlgAttr
 功能描述  : 获取HMAC算法信息
 纯 入 参  : ulDomain: 被HMAC保护数据所属功能域
 纯 出 参  : pstHmacAlgAttr: 回传HMAC算法属性信息
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2015年2月15日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T SDP_GetHmacAlgAttr(WSEC_UINT32 ulDomain, SDP_HMAC_ALG_ATTR *pstHmacAlgAttr)
{
    SDP_HMAC_HEAD_STRU* pHdr = WSEC_NULL_PTR;
    WSEC_BYTE aucKey[SDP_KEY_MAX_LEN] = {0};
    WSEC_UINT32 ulKeyLen = 0;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    return_err_if_para_invalid("SDP_GetHmacAlgAttr", pstHmacAlgAttr);
    pHdr = (SDP_HMAC_HEAD_STRU*)pstHmacAlgAttr->abyBuffer;

    nErrCode = SDP_FillHmacTextHeader(SDP_ALG_INTEGRITY, ulDomain, pHdr, aucKey, &ulKeyLen);
    return_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("SDP_FillHmacTextHeader() = %u", nErrCode), nErrCode);
    
    SDP_CvtByteOrder4HmacTextHeader(pHdr, wbcHost2Network);

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : SDP_HmacInit
 功能描述  : 对大数据, 或者分段数据进行HMAC的初始化内。
 纯 入 参  : ulDomain: 被HMAC保护数据所属功能域
             pstHmacAlgAttr: HMAC算法属性信息
 纯 出 参  : pstSdpCtx: HMAC分段计算使用的上下文.
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2015年2月15日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T SDP_HmacInit(WSEC_UINT32 ulDomain, const SDP_HMAC_ALG_ATTR *pstHmacAlgAttr, SDP_CRYPT_CTX *pstSdpCtx)
{
    SDP_HMAC_HEAD_STRU* pstHdr = WSEC_NULL_PTR;
    SDP_CRYPT_CTX_STRU *pstSdpCtxObj = WSEC_NULL_PTR;
    WSEC_ALGTYPE_E eAlgType = WSEC_ALGTYPE_UNKNOWN;
    WSEC_BYTE aucKey[SDP_KEY_MAX_LEN];
    WSEC_UINT32 ulTempPTMacLen  = 0;
    WSEC_UINT32 ulKeyLen        = 0;
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    return_err_if_para_invalid("SDP_HmacInit", pstHmacAlgAttr && pstSdpCtx);

    pstSdpCtxObj = (SDP_CRYPT_CTX_STRU *)WSEC_MALLOC(SDP_CRYPT_CTX_LEN);
    return_oper_if(!pstSdpCtxObj, WSEC_LOG_E4MALLOC(SDP_CRYPT_CTX_LEN), WSEC_ERR_MALLOC_FAIL);

    *pstSdpCtx = pstSdpCtxObj;
    pstSdpCtxObj->eAlgType = WSEC_ALGTYPE_HMAC;
    pstHdr = &pstSdpCtxObj->stHmacHead;

    do
    {
        break_oper_if(WSEC_MEMCPY(pstHdr, sizeof(SDP_HMAC_HEAD_STRU), pstHmacAlgAttr, sizeof(SDP_HMAC_HEAD_STRU)) != EOK, 
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);

        SDP_CvtByteOrder4HmacTextHeader(pstHdr, wbcNetwork2Host);
        
        /* 参数检查 */
        break_oper_if(pstHdr->ulVersion != SDP_HMAC_VER, WSEC_LOG_E2("HMAC version is incompatible, %d expected, %d actually.", SDP_HMAC_VER, pstHdr->ulVersion), nErrCode = WSEC_ERR_SDP_VERSION_INCOMPATIBLE);
        break_oper_if((ulDomain != pstHdr->ulDomain), WSEC_LOG_E1("Cipher text are marked with an unexpected domain %d.", pstHdr->ulDomain), nErrCode = WSEC_ERR_SDP_DOMAIN_UNEXPECTED);
        break_oper_if(!KMC_IS_KEYITERATIONS_VALID(pstHdr->ulIterCount), WSEC_LOG_E("Iterator count is out of bounds."), nErrCode = WSEC_ERR_SDP_INVALID_CIPHER_TEXT);
        break_oper_if(CAC_AlgId2Type(pstHdr->ulAlgId) == WSEC_ALGTYPE_UNKNOWN, WSEC_LOG_E("CAC Get algorithm types failed."), nErrCode = WSEC_ERR_SDP_ALG_NOT_SUPPORTED);

        /* 构造HMAC密钥 */
        nErrCode = SDP_GetAlgProperty(pstHdr->ulAlgId, WSEC_NULL_PTR, 0, &eAlgType, &ulKeyLen, WSEC_NULL_PTR, WSEC_NULL_PTR, &ulTempPTMacLen);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("SDP_GetAlgProperty() = %u.", nErrCode), oper_null);
        break_oper_if(eAlgType != WSEC_ALGTYPE_HMAC, WSEC_LOG_E1("AlgType(%d) cannot support HMAC.", eAlgType), nErrCode = WSEC_ERR_SDP_INVALID_CIPHER_TEXT);

        /* Get WK (work key) By KeyID */
        nErrCode = SDP_GetWorkKeyByID(pstHdr->ulDomain, pstHdr->ulKeyId, pstHdr->ulIterCount, pstHdr->aucSalt, SDP_SALT_LEN, aucKey, ulKeyLen);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("SDP_GetWorkKeyByID() = %u", nErrCode), oper_null);

        /* HMAC calculation init */
        nErrCode = CAC_HmacInit(&(pstSdpCtxObj->stWsecCtx), pstHdr->ulAlgId, aucKey, ulKeyLen);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("CAC_HmacInit() = %u", nErrCode), oper_null);
    }do_end;

    if (nErrCode != WSEC_SUCCESS) {SDP_FreeCtx(pstSdpCtx);}

    /* destroy key */
    WSEC_DESTROY_KEY(aucKey, ulKeyLen);

    /* Misinformation: FORTIFY.Memory_Leak ('pstSdpCtxObj' output by 'pstSdpCtx' when succ.) */

    return nErrCode;
}


/*****************************************************************************
> 函 数 名  : SDP_HmacUpdate
> 功能描述  : 大块机密数据计算HMAC更新接口。根据已建立的数据保护上下文对大块数据进行分段更新HMAC。
>             多次执行该函数来完成各个数据段的HMAC更新过程。大块机密数据的分段由调用者完成，
>             该函数只接收已经划分好的数据段作为参数。
> 输入参数  : pstSdpCtx 表示已建立的数据保护上下文，必须由SDP_HmacInit输出；
>             pucPlainText 表示将要计算HMAC的分段明文内存块；
>             ulPTLen 表示将要计算HMAC的分段明文内存块的长度；
> 输出参数  : 无
> 返 回 值  : 错误码，WSEC_SUCCESS表示函数正常返回。
> 先验条件  : (WSEC_NULL_PTR != pstSdpCtx) && (WSEC_NULL_PTR != *pstSdpCtx)
>             && (WSEC_NULL_PTR != pucPlainText) && (0 != ulPTLen)
> 后验条件  : 无

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数
>  2.日    期   : 2014年11月6日
>    作    者   : j00265291
>    修改内容   : 修改函数，解决本函数失败时没有调用Final造成的内存泄露

*****************************************************************************/
WSEC_ERR_T SDP_HmacUpdate(
      const SDP_CRYPT_CTX *pstSdpCtx,
      const WSEC_BYTE *pucPlainText,
      WSEC_UINT32 ulPTLen
    )
{
    SDP_CRYPT_CTX_STRU          *pstSdpCtxObj = WSEC_NULL_PTR;
    WSEC_ERR_T ulRet = WSEC_SUCCESS;

    /* check input */
    return_err_if_para_invalid("SDP_HmacUpdate", pstSdpCtx && *pstSdpCtx && pucPlainText && (ulPTLen > 0));

    do 
    {
        /* construct context */
        pstSdpCtxObj = (SDP_CRYPT_CTX_STRU *)(*pstSdpCtx);

        /* HMAC calculation update */
        ulRet = CAC_HmacUpdate(pstSdpCtxObj->stWsecCtx, pucPlainText, ulPTLen);
        if (ulRet!= WSEC_SUCCESS)
        {
            ulRet = WSEC_ERR_HMAC_FAIL;
            WSEC_LOG_E("[SDP] CAC calculate hmac update failed.");
            break;
        }

        ulRet = WSEC_SUCCESS;
    } do_end;

    if (ulRet != WSEC_SUCCESS) { SDP_FreeCtx((SDP_CRYPT_CTX*)pstSdpCtx); }

    return ulRet;
}

/*****************************************************************************
> 函 数 名  : SDP_HmacFinal
> 功能描述  : 大块机密数据计算HMAC结束接口。根据已建立的数据保护上下文，完成最后一个数据段的HMAC更新，
>             最终输出大块机密数据的HMAC。
> 输入参数  : pstSdpCtx 表示已建立的数据保护上下文，必须由SDP_HmacInit输出；
>             pulHDLen 表示将要生成计算得到HMAC的内存块的分配长度；
> 输出参数  : pucHmacData 表示将要生成计算得到HMAC的内存块；
>             pulHDLen 表示将要生成计算得到HMAC的内存块的实际长度；
> 返 回 值  : 错误码，WSEC_SUCCESS表示函数正常返回，HMAC已经输出。
> 先验条件  : (WSEC_NULL_PTR != pstSdpCtx) && (WSEC_NULL_PTR != *pstSdpCtx)
>             && (WSEC_NULL_PTR != pucHmacData) && (WSEC_NULL_PTR != pulHDLen)
> 后验条件  : 当返回WSEC_SUCCESS时，*pulHDLen> 0
> 注意事项  : pucHmacData指向的内存块由调用者分配和释放，内存块的大小建议使用SDP_GetHmacLen获取。
>             pulHDLen由调用者分配内存。

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数

>  2.日    期   : 2015年3月20日
>    作    者   : z00118096
>    修改内容   : 增加对HMAC输出缓冲区的检查
*****************************************************************************/
WSEC_ERR_T SDP_HmacFinal(
      const SDP_CRYPT_CTX *pstSdpCtx,
      WSEC_BYTE    *pucHmacData,
      INOUT WSEC_UINT32   *pulHDLen
    )
{
    SDP_CRYPT_CTX_STRU *pstSdpCtxObj = WSEC_NULL_PTR;
    SDP_HMAC_HEAD_STRU* pstHdr = WSEC_NULL_PTR;
    WSEC_ERR_T ulRet = WSEC_SUCCESS;
    WSEC_UINT32 ulTempHDLen    = 0;
    WSEC_UINT32 ulHDMaxLen     = 0;
    WSEC_UINT32 ulNeedHmacSize = 0;

    /* check input */
    return_err_if_para_invalid("SDP_HmacFinal", pstSdpCtx && (*pstSdpCtx) && pucHmacData && pulHDLen);

    ulHDMaxLen = *pulHDLen;
    ulTempHDLen = ulHDMaxLen;

    do
    {
        /* construct context */
        pstSdpCtxObj = (SDP_CRYPT_CTX_STRU *)(*pstSdpCtx);

        /* 检查HMAC输出缓冲区是否足够 */
        pstHdr = &pstSdpCtxObj->stHmacHead;
        ulNeedHmacSize = CAC_HMAC_Size(pstHdr->ulAlgId);

        break_oper_if(ulHDMaxLen < ulNeedHmacSize, WSEC_LOG_E2("HMAC buffer too small(%d), need %d", ulHDMaxLen, ulNeedHmacSize), ulRet = WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH);
        ulRet = CAC_HmacFinal(&(pstSdpCtxObj->stWsecCtx), pucHmacData, &ulTempHDLen);
        break_oper_if(ulRet != WSEC_SUCCESS, WSEC_LOG_E("[SDP] CAC calculate hmac final failed."), ulRet = WSEC_ERR_HMAC_FAIL);

        /* output param */
        *pulHDLen = ulTempHDLen;

        ulRet = WSEC_SUCCESS;
    } do_end;

    SDP_FreeCtx((SDP_CRYPT_CTX*)pstSdpCtx);

    return ulRet;
}

/*****************************************************************************
 函 数 名  : SDP_HmacCancel
 功能描述  : 取消分段HMAC处理的后续过程.
             正常的分段HMAC处理过程是: Init ---> Update ---> Final. 如果在中间任意
                                              ↑________|
                                                   
             环节取消后续的处理, 则需要调用此函数.
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : pstSdpCtx: [in]指向上下信息的指针, [out]指针内容为NULL
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2015年5月13日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID SDP_HmacCancel(SDP_CRYPT_CTX *pstSdpCtx)
{
    SDP_FreeCtx(pstSdpCtx);
}

/*****************************************************************************
 函 数 名  : SDP_FileHmac
 功能描述  : 对文件进行HMAC计算.
 纯 入 参  : ulDomain: 被HMAC保护文件数据所属功能域
             pszFile:  待保护文件
             pstRptProgress: 进度上报回调函数[可空]
             pstHmacAlgAttr: HMAC算法属性信息
 纯 出 参  : pvHmacData: 输出HMAC(调用者提供空间)
 入参出参  : pulHDLen: [in]pvHmacData输出缓冲区长度; [out]HMAC实际长度
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2015年2月15日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T SDP_FileHmac(WSEC_UINT32 ulDomain, const WSEC_CHAR* pszFile, const WSEC_PROGRESS_RPT_STRU* pstRptProgress, const SDP_HMAC_ALG_ATTR* pstHmacAlgAttr, WSEC_VOID* pvHmacData, WSEC_UINT32* pulHDLen)
{
    SDP_CRYPT_CTX stSdpCtx = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    WSEC_FILE_LEN nFileLen = 0, nReadLen = 0, nReadLenStat = 0;
    WSEC_VOID *pvReadBuff = WSEC_NULL_PTR;
    WSEC_FILE  fRead;
    WSEC_SPEND_TIME_STRU stTimer = {0};
    WSEC_BOOL bCancel = WSEC_FALSE, bNeedHmacFinal = WSEC_FALSE;
    
    return_err_if_para_invalid("SDP_FileHmac", pszFile && pstHmacAlgAttr && pvHmacData && pulHDLen);
    return_err_if_para_invalid("SDP_FileHmac", WSEC_NOT_IS_EMPTY_STRING(pszFile));
    return_oper_if(!WSEC_GetFileLen(pszFile, &nFileLen), WSEC_LOG_E1("Cannot access '%s'", pszFile), WSEC_ERR_OPEN_FILE_FAIL);

    fRead = WSEC_FOPEN(pszFile, "rb");
    return_oper_if(!fRead, WSEC_LOG_E1("Cannot open '%s'", pszFile), WSEC_ERR_OPEN_FILE_FAIL);

    pvReadBuff = WSEC_MALLOC(WSEC_FILE_IO_SIZE_MAX);

    do
    {
        break_oper_if(!pvReadBuff, WSEC_LOG_E4MALLOC(WSEC_FILE_IO_SIZE_MAX), nErrCode = WSEC_ERR_MALLOC_FAIL);
        
        nErrCode = SDP_HmacInit(ulDomain, pstHmacAlgAttr, &stSdpCtx);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("SDP_HmacInit() = %u", nErrCode), oper_null);

        bNeedHmacFinal = WSEC_TRUE;
        while (!WSEC_FEOF(fRead))
        {
            nReadLen = WSEC_FREAD(pvReadBuff, 1, WSEC_FILE_IO_SIZE_MAX, fRead);
            continue_if(nReadLen <= 0);

            nReadLenStat += nReadLen;

            nErrCode = SDP_HmacUpdate(&stSdpCtx, pvReadBuff, nReadLen);
            break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("SDP_HmacUpdate() = %u", nErrCode), bNeedHmacFinal = WSEC_FALSE);

            WSEC_RptProgress(pstRptProgress, &stTimer, nFileLen, nReadLenStat, &bCancel);
            break_oper_if(bCancel, WSEC_LOG_E("App canceled."), nErrCode = WSEC_ERR_CANCEL_BY_APP);
        }

        if (!bCancel) {WSEC_RptProgress(pstRptProgress, WSEC_NULL_PTR, nFileLen, nReadLenStat, WSEC_NULL_PTR);}
        if (bNeedHmacFinal) {nErrCode = SDP_HmacFinal(&stSdpCtx, (WSEC_BYTE*)pvHmacData, pulHDLen);}
    }do_end;

    SDP_FreeCtx(&stSdpCtx);

    WSEC_FCLOSE(fRead);
    WSEC_FREE(pvReadBuff);

    return nErrCode;
}

/*****************************************************************************
 函 数 名  : SDP_VerifyFileHmac
 功能描述  : 对文件进行HMAC认证.
 纯 入 参  : ulDomain: 被HMAC保护文件数据所属功能域
             pszFile:  待保护文件
             pstRptProgress: 进度上报回调函数[可空]
             pstHmacAlgAttr: HMAC算法属性信息
             pvHmacData: 该文件原HMAC值
             ulHDLen:    该文件原HMAC长度
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 错误码(WSEC_SUCCESS为成功, 其余为错误码)
 特别注意  : 无

 修改历史
  1.日    期   : 2015年2月15日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ERR_T SDP_VerifyFileHmac(WSEC_UINT32 ulDomain, const WSEC_CHAR *pszFile, const WSEC_PROGRESS_RPT_STRU* pstRptProgress, const SDP_HMAC_ALG_ATTR* pstHmacAlgAttr, const WSEC_VOID *pvHmacData, WSEC_UINT32 ulHDLen)
{
    WSEC_BUFF stHmacNew = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    return_err_if_para_invalid("SDP_VerifyFileHmac", pszFile && pstHmacAlgAttr && pvHmacData && (ulHDLen > 0));
    WSEC_BUFF_ALLOC(stHmacNew, WSEC_HMAC_LEN_MAX);
    return_oper_if(!stHmacNew.pBuff, WSEC_LOG_E4MALLOC(stHmacNew.nLen), WSEC_ERR_MALLOC_FAIL);

    do
    {
        nErrCode = SDP_FileHmac(ulDomain, pszFile, pstRptProgress, pstHmacAlgAttr, stHmacNew.pBuff, &stHmacNew.nLen);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("SDP_FileHmac() = %u", nErrCode), oper_null);

        break_oper_if(ulHDLen != stHmacNew.nLen, oper_null, nErrCode = WSEC_ERR_HMAC_AUTH_FAIL);
        break_oper_if(WSEC_MEMCMP(pvHmacData, stHmacNew.pBuff, ulHDLen) != 0, oper_null, nErrCode = WSEC_ERR_HMAC_AUTH_FAIL);
    }do_end;

    WSEC_BUFF_FREE(stHmacNew);
    return nErrCode;
}

/*****************************************************************************
> 函 数 名  : SDP_GetPwdCipherLen
> 功能描述  : 根据指定的加密后数据长度，计算口令密文全部长度。
> 输入参数  : ulPwdLen Password长度
> 输出参数  : 无
> 返 回 值  : 口令密文的长度，包括口令密文头部。
> 注意事项  : 该函数适用于SDP_ProtectPwd函数调用前进行口令密文长度计算和口令密文内存分配。

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数

*****************************************************************************/
WSEC_SIZE_T SDP_GetPwdCipherLen(WSEC_SIZE_T ulPwdLen)
{
    /* sum of length */
    return (WSEC_SIZE_T)(SDP_PWD_HEAD_LEN + ulPwdLen);
}


/*****************************************************************************
> 函 数 名  : SDP_ProtectPwd
> 功能描述  : 口令单向加密接口。提供口令单向加密功能，
> 密文结构  : pwd-cipher-head | pwd-cipher-data
> 输入参数  : pucPlainText 表示将要单向加密的明文口令
>             ulPTLen 表示将要单向加密的明文口令的长度
>             ulCTLen 表示生成的口令密文的最大长度，包括口令密文头部长度。
> 输出参数  : pucCipherText 表示将要生成单向加密后口令密文的内存块，包括口令密文头部
> 返 回 值  : 错误码，WSEC_SUCCESS表示函数正常返回，口令密文已经输出。
> 注意事项  : pucCipherText指向的内存块由调用者分配和释放，内存块的大小建议使用SDP_GetPwdCipherLen获取。

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_ProtectPwd(
      const WSEC_BYTE *pucPlainText,
      WSEC_UINT32 ulPTLen,
      WSEC_BYTE *pucCipherText,
      WSEC_UINT32 ulCTLen
    )
{
    SDP_PWD_HEAD_STRU *pstPwdHead = WSEC_NULL_PTR;
    WSEC_ERR_T  ulRet = WSEC_SUCCESS;

    /* check input */
    return_err_if_para_invalid("SDP_ProtectPwd", pucPlainText && pucCipherText && (ulCTLen >= SDP_PWD_HEAD_LEN) && (pucPlainText != pucCipherText));

    /* construct header, get MK-derived work key */
    WSEC_MEMSET(pucCipherText, ulCTLen, 0, SDP_PWD_HEAD_LEN);
    pstPwdHead = (SDP_PWD_HEAD_STRU *)pucCipherText;
    ulRet = SDP_FillPwdCipherTextHeader(SDP_ALG_PWD_PROTECT, pstPwdHead);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] Fill password cipher text header failed."), ulRet);

    /* update header with real cipher length */
    pstPwdHead->ulCDLen = ulCTLen - SDP_PWD_HEAD_LEN;

    /* password encryption */
    ulRet = CAC_Pbkdf2(pstPwdHead->ulAlgId, (WSEC_BYTE *)pucPlainText, ulPTLen,
                       pstPwdHead->aucSalt, SDP_SALT_LEN,
                       pstPwdHead->ulIterCount, pstPwdHead->ulCDLen, pucCipherText + SDP_PWD_HEAD_LEN);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] CAC pbkdf2 encrypt password failed."), WSEC_ERR_PBKDF2_FAIL);

    /* byte order process */
    SDP_CvtByteOrder4PwdCipherTextHeader(pstPwdHead, wbcHost2Network);

    return WSEC_SUCCESS;
}

/*****************************************************************************
> 函 数 名  : SDP_VerifyPwd
> 功能描述  : 口令密文校验接口。提供校验口令密文的功能
> 输入参数  : pucPlainText 表示将要校验口令密文对应的明文口令
>             ulPTLen 表示将要校验口令密文对应的明文口令的长度
>             pucCipherText 表示将要校验的口令密文，包括数据保护的口令密文头部
>             ulCTLen 表示将要校验的口令密文的长度
> 输出参数  : 无
> 返 回 值  : 错误码，WSEC_SUCCESS表示函数正常返回，口令密文校验结果正确。

> 修改历史      :
>  1.日    期   : 2014年7月2日
>    作    者   : x00102361, j00265291
>    修改内容   : 新生成函数

*****************************************************************************/
WSEC_ERR_T SDP_VerifyPwd(
      const WSEC_BYTE *pucPlainText,
      WSEC_UINT32 ulPTLen,
      const WSEC_BYTE *pucCipherText,
      WSEC_UINT32 ulCTLen
    )
{
    SDP_PWD_HEAD_STRU           *pstPwdHead = WSEC_NULL_PTR;
    SDP_ERROR_CTX_STRU          stErrCtx;
    WSEC_ERR_T  ulRet           = WSEC_SUCCESS;
    WSEC_BYTE *pTempCipherData = WSEC_NULL_PTR;
    WSEC_ALGTYPE_E eAlgType = WSEC_ALGTYPE_UNKNOWN;

    /* check input */
    return_err_if_para_invalid("SDP_VerifyPwd", pucPlainText && pucCipherText && (ulCTLen >= SDP_PWD_HEAD_LEN) && (pucPlainText != pucCipherText));

    /* check header */
    pstPwdHead = (SDP_PWD_HEAD_STRU *)pucCipherText;

    /* byte order process */
    SDP_CvtByteOrder4PwdCipherTextHeader(pstPwdHead, wbcNetwork2Host);

    /* check format */
    init_error_ctx(stErrCtx);
    update_error_ctx(stErrCtx, (SDP_PWD_CIPHER_VER != pstPwdHead->ulVersion),
                   WSEC_LOG_E2("[SDP] Cipher text version is incompatible, %d expected, %d actually.", SDP_PWD_CIPHER_VER, pstPwdHead->ulVersion), 
                   WSEC_ERR_SDP_VERSION_INCOMPATIBLE);    
/*    update_error_ctx(stErrCtx, (0 == pstPwdHead->ulCDLen), 
                   WSEC_LOG_E("[SDP] Cipher data length cannot be 0."),
                   WSEC_ERR_SDP_INVALID_CIPHER_TEXT); */
    update_error_ctx(stErrCtx, (!KMC_IS_KEYITERATIONS_VALID(pstPwdHead->ulIterCount)), 
                   WSEC_LOG_E("[SDP] Iterator count is out of bounds."),
                   WSEC_ERR_SDP_INVALID_CIPHER_TEXT);
    update_error_ctx(stErrCtx, (WSEC_ALGTYPE_UNKNOWN == CAC_AlgId2Type(pstPwdHead->ulAlgId)), 
                   WSEC_LOG_E("[SDP] CAC Get algorithm types failed."), 
                   WSEC_ERR_SDP_ALG_NOT_SUPPORTED);
    return_oper_if((stErrCtx.ulErrCount > SDP_MAX_ERR_FIELD_OF_CIPHER), WSEC_LOG_E("[SDP] Cipher text format is invalid."), WSEC_ERR_SDP_INVALID_CIPHER_TEXT);
    return_oper_if((stErrCtx.ulErrCount > 0), oper_null, stErrCtx.ulLastErrCode);

    return_oper_if((ulCTLen < SDP_PWD_HEAD_LEN + pstPwdHead->ulCDLen), WSEC_LOG_E("[SDP] Invalid parameter. Buffer for cipher text is not enough."), WSEC_ERR_INPUT_BUFF_NOT_ENOUGH);

    /* Get algorithm property */
    ulRet = SDP_GetAlgProperty(pstPwdHead->ulAlgId, WSEC_NULL_PTR, 0, &eAlgType, WSEC_NULL_PTR, WSEC_NULL_PTR, WSEC_NULL_PTR, WSEC_NULL_PTR);
    return_oper_if((ulRet!= WSEC_SUCCESS), WSEC_LOG_E("[SDP] Get algorithm property failed."), ulRet);
    return_oper_if((WSEC_ALGTYPE_PBKDF != eAlgType), WSEC_LOG_E1("[SDP] AlgType(%d) is out of bounds.", eAlgType), WSEC_ERR_SDP_INVALID_CIPHER_TEXT);

    pTempCipherData = (WSEC_BYTE*)WSEC_MALLOC(pstPwdHead->ulCDLen > 0 ? pstPwdHead->ulCDLen : 32); /* 解决0长口令 */
    return_oper_if((WSEC_NULL_PTR == pTempCipherData), WSEC_LOG_E("[SDP] Memory for cipher data allocate failed."), WSEC_ERR_MALLOC_FAIL);

    do
    {
        /* password encryption */
        ulRet = CAC_Pbkdf2(pstPwdHead->ulAlgId, (WSEC_BYTE *)pucPlainText, ulPTLen,
                           pstPwdHead->aucSalt, SDP_SALT_LEN, pstPwdHead->ulIterCount, pstPwdHead->ulCDLen, pTempCipherData);
        if (ulRet!= WSEC_SUCCESS)
        {
            ulRet = WSEC_ERR_PBKDF2_FAIL;
            WSEC_LOG_E("[SDP] CAC pbkdf2 encrypt password failed.");
            break;
        }

        /* verify cipher text */
        if (0 != WSEC_MEMCMP(pTempCipherData, pucCipherText + SDP_PWD_HEAD_LEN, pstPwdHead->ulCDLen) )
        {
            ulRet = WSEC_ERR_SDP_PWD_VERIFY_FAIL;
            WSEC_LOG_E("[SDP] Password cipher text failed to pass the verification.");
            break;
        }

        ulRet = WSEC_SUCCESS;
    } do_end;

    WSEC_FREE(pTempCipherData);
    return ulRet;
}

/*****************************************************************************
 函 数 名  : SDP_ShowStructSize
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
WSEC_VOID SDP_ShowStructSize(WSEC_FP_ShowStructSize pfShow)
{
    pfShow("SDP_CIPHER_HEAD", sizeof(SDP_CIPHER_HEAD));
    pfShow("SDP_HMAC_ALG_ATTR", sizeof(SDP_HMAC_ALG_ATTR));
    pfShow("SDP_PWD_HEAD", sizeof(SDP_PWD_HEAD));
    pfShow("SDP_BOD_CIPHER_HEAD", sizeof(SDP_BOD_CIPHER_HEAD));
    pfShow("SDP_CIPHER_HEAD_STRU", sizeof(SDP_CIPHER_HEAD_STRU));
    pfShow("SDP_HMAC_HEAD_STRU", sizeof(SDP_HMAC_HEAD_STRU));
    pfShow("SDP_PWD_HEAD_STRU", sizeof(SDP_PWD_HEAD_STRU));
    pfShow("SDP_CIPHER_FILE_HDR_STRU", sizeof(SDP_CIPHER_FILE_HDR_STRU));
}
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */


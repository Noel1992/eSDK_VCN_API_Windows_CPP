/******************************************************************************

                 Copyright (C), 2001-2014, Huawei Tech. Co., Ltd.

 ******************************************************************************
  FileName      : xcapbase.c
  Version       : 1.0.0
  Author        :  l00171031 
  Date          : 2014-6-16
  Description   : WSEC基本功能函数 (WSEC Basic Functions)
  Function List :


  History   :
        1.Date            : 2014-6-16
          Author          :  l00171031 
          Modification    :　创建文件

******************************************************************************/
/*PClint warning ignore*/	/*lint -e522 -e550 -e533 -e534 -e641 -e506 */

#include "wsec_config.h"

#ifdef WSEC_COMPILE_CAC_IPSI

#include "cac_pri.h"
#include "ipsi_types.h"
#include "wsec_type.h"
#include "sec_crypto.h"
#include "crypto_def.h"
#include "sec_def.h"
#include "sec_sys.h"
#include "wsec_itf.h"
#include "wsec_pri.h"
#include "wsec_securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define CAC_HMAC_LEN_MAX 64
WSEC_BOOL g_bCrptoLibInit = WSEC_FALSE;


WSEC_VOID CAC_DigestFree(WSEC_CRYPT_CTX *pCtx)
{
	if(!pCtx || !(*pCtx))
	{	return; }
    CRYPT_digestFree((CRYPT_CTX*)pCtx);
}
WSEC_VOID CAC_HmacFree(WSEC_CRYPT_CTX *pCtx)
{
	WSEC_BYTE buffer[CAC_HMAC_LEN_MAX]={0};
	WSEC_UINT32 len = 0;
	if(!pCtx || !(*pCtx))
	{	return; }
    WSEC_UNCARE(CRYPT_hmacFinal((CRYPT_CTX*)pCtx,buffer,(SEC_UINT32*)(&len)));
}
WSEC_VOID CAC_CipherFree(WSEC_CRYPT_CTX *pCtx)
{
	if(!pCtx || !(*pCtx))
	{	return; }
	crypt_freeCtx((CRYPT_CTX*)pCtx);
}

 /******************************************************************************
  FunctionName      : 加密库初始化
  Version       : 1.0.0
  Author        :  l00171031 
  Date          : 2014-6-16
  Description   : CAC基本功能函数 (CAC Basic Functions)
  Function List :


  History   :
        1.Date            : 2014-6-16
          Author          :  l00171031 
          Modification    :　创建文件

******************************************************************************/
WSEC_ERR_T CAC_LibraryInit(const WSEC_VOID *pvEntropyBuf, WSEC_SIZE_T ulLen)
{

    if(!g_bCrptoLibInit)
    {
        if(SEC_SUCCESS != CRYPT_libraryInit())
        {
            WSEC_LOG_E("CRYPT_addEntropy() fail.");
            return WSEC_ERR_CRPTO_LIB_FAIL;
        }	
		
        if( !pvEntropyBuf && !ulLen )
        {
	        if(SEC_SUCCESS != CRYPT_addEntropy((const SEC_UCHAR *)pvEntropyBuf, (SEC_UINT32)ulLen))
	        {
	            WSEC_LOG_E("CRYPT_addEntropy() fail.");
	            return WSEC_ERR_CRPTO_LIB_FAIL;
	        }
        }

		g_bCrptoLibInit = WSEC_TRUE;

    }

    return WSEC_SUCCESS;
}

WSEC_ERR_T CAC_Random(WSEC_VOID * pBuf, WSEC_SIZE_T ulLen)
{
    WSEC_INT32 iRet = 0;
    iRet = CRYPT_random((SEC_UCHAR*)pBuf, ulLen);
    if(SEC_SUCCESS != iRet)
    {
        WSEC_LOG_E1("Call CRYPT_random failed : %d\n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
    else
    {
        return WSEC_SUCCESS;
    }
}

/******************************************************************************
  FunctionName        : CAC_2IPSIAlg
  Version        : 1.0.0
  Author        :  l00171031 
  Date            : 2014-6-16
  Description    : 转换ID,从密钥管理模块的ID 转换成IPSI 的ID
  :


  History    :
        1.Date              : 2014-6-16
          Author          :  l00171031 
          Modification      :　创建文件

******************************************************************************/

WSEC_UINT32 CAC_Alg2IPSIAlg_digest(WSEC_UINT32 AlgID)
{
    switch (AlgID)
    {    
        case(WSEC_ALGID_MD5): { return ALGID_MD5; }
        case(WSEC_ALGID_SHA1): { return ALGID_SHA1; }
        case(WSEC_ALGID_SHA224): { return ALGID_SHA224; }
        case(WSEC_ALGID_SHA256): { return ALGID_SHA256; }
        case(WSEC_ALGID_SHA384): { return ALGID_SHA384; }
        case(WSEC_ALGID_SHA512): { return ALGID_SHA512; }  

        default:
        {
            return 0;
        }
    }

}

WSEC_UINT32 CAC_Alg2IPSIAlg_hmac(WSEC_UINT32 AlgID)
{
    switch (AlgID)
    {    
        case(WSEC_ALGID_HMAC_MD5): { return ALGID_HMAC_MD5; }
        case(WSEC_ALGID_HMAC_SHA1): { return ALGID_HMAC_SHA1; }
        case(WSEC_ALGID_HMAC_SHA224): { return ALGID_HMAC_SHA224; }
        case(WSEC_ALGID_HMAC_SHA256): { return ALGID_HMAC_SHA256; }
        case(WSEC_ALGID_HMAC_SHA384): { return ALGID_HMAC_SHA384; }
        case(WSEC_ALGID_HMAC_SHA512): { return ALGID_HMAC_SHA512; }

        default:
        {
            return 0;
        }

    }
}


WSEC_UINT32 CAC_Alg2IPSIAlg_sym(WSEC_UINT32 AlgID)
{
    switch (AlgID)
    {    
        case(WSEC_ALGID_DES_EDE3_ECB): { return ALGID_DES_EDE3_ECB; }
        case(WSEC_ALGID_DES_EDE3_CBC): { return ALGID_DES_EDE3_CBC; }
        case(WSEC_ALGID_AES128_ECB): { return ALGID_AES128_ECB; }
        case(WSEC_ALGID_AES128_CBC): { return ALGID_AES128_CBC; }
        case(WSEC_ALGID_AES256_ECB): { return ALGID_AES256_ECB; }
        case(WSEC_ALGID_AES256_CBC): { return ALGID_AES256_CBC; }

        default:
        {
            return 0;
        }

    }
}      

WSEC_UINT32 CAC_Alg2IPSIAlg_pbkdf(WSEC_UINT32 AlgID)
{

    switch (AlgID)
    {    
        case(WSEC_ALGID_PBKDF2_HMAC_MD5): { return ALGID_HMAC_MD5; }
        case(WSEC_ALGID_PBKDF2_HMAC_SHA1): { return ALGID_HMAC_SHA1; }
        case(WSEC_ALGID_PBKDF2_HMAC_SHA224): { return ALGID_HMAC_SHA224; }
        case(WSEC_ALGID_PBKDF2_HMAC_SHA256): { return ALGID_HMAC_SHA256; }
        case(WSEC_ALGID_PBKDF2_HMAC_SHA384): { return ALGID_HMAC_SHA384; }
        case(WSEC_ALGID_PBKDF2_HMAC_SHA512): { return ALGID_HMAC_SHA512; }

        default:
        {
            return 0;
        }

    }
}

const WSEC_CHAR* CAC_AlgId2Name(WSEC_UINT32 ulAlgID)
{
    const WSEC_CHAR* pszName = WSEC_NULL_PTR;
    
    switch (ulAlgID)
    {
        case(WSEC_ALGID_DES_EDE3_ECB): { pszName = WSEC_ALGNAME_DES_EDE3_ECB; break; }
        case(WSEC_ALGID_DES_EDE3_CBC): { pszName = WSEC_ALGNAME_DES_EDE3_CBC; break; }
        case(WSEC_ALGID_AES128_ECB): { pszName = WSEC_ALGNAME_AES128_ECB; break; }
        case(WSEC_ALGID_AES128_CBC): { pszName = WSEC_ALGNAME_AES128_CBC; break; }
        case(WSEC_ALGID_AES256_ECB): { pszName = WSEC_ALGNAME_AES256_ECB; break; }
        case(WSEC_ALGID_AES256_CBC): { pszName = WSEC_ALGNAME_AES256_CBC; break; }
        case(WSEC_ALGID_MD5): { pszName = WSEC_ALGNAME_MD5; break; }
        case(WSEC_ALGID_SHA1): { pszName = WSEC_ALGNAME_SHA1; break; }
        case(WSEC_ALGID_SHA224): { pszName = WSEC_ALGNAME_SHA224; break; }
        case(WSEC_ALGID_SHA256): { pszName = WSEC_ALGNAME_SHA256; break; }
        case(WSEC_ALGID_SHA384): { pszName = WSEC_ALGNAME_SHA384; break; }
        case(WSEC_ALGID_SHA512): { pszName = WSEC_ALGNAME_SHA512; break; }
        case(WSEC_ALGID_HMAC_MD5): { pszName = WSEC_ALGNAME_HMAC_MD5; break; }
        case(WSEC_ALGID_HMAC_SHA1): { pszName = WSEC_ALGNAME_HMAC_SHA1; break; }
        case(WSEC_ALGID_HMAC_SHA224): { pszName = WSEC_ALGNAME_HMAC_SHA224; break; }
        case(WSEC_ALGID_HMAC_SHA256): { pszName = WSEC_ALGNAME_HMAC_SHA256; break; }
        case(WSEC_ALGID_HMAC_SHA384): { pszName = WSEC_ALGNAME_HMAC_SHA384; break; }
        case(WSEC_ALGID_HMAC_SHA512): { pszName = WSEC_ALGNAME_HMAC_SHA512; break; }
        case(WSEC_ALGID_PBKDF2_HMAC_MD5): { pszName = WSEC_ALGNAME_PBKDF2_HMAC_MD5; break; }
        case(WSEC_ALGID_PBKDF2_HMAC_SHA1): { pszName = WSEC_ALGNAME_PBKDF2_HMAC_SHA1; break; }
        case(WSEC_ALGID_PBKDF2_HMAC_SHA224): { pszName = WSEC_ALGNAME_PBKDF2_HMAC_SHA224; break; }
        case(WSEC_ALGID_PBKDF2_HMAC_SHA256): { pszName = WSEC_ALGNAME_PBKDF2_HMAC_SHA256; break; }
        case(WSEC_ALGID_PBKDF2_HMAC_SHA384): { pszName = WSEC_ALGNAME_PBKDF2_HMAC_SHA384; break; }
        case(WSEC_ALGID_PBKDF2_HMAC_SHA512): { pszName = WSEC_ALGNAME_PBKDF2_HMAC_SHA512; break; }

        default:
            {
                WSEC_LOG_E("Wrong Encryption AlgID\n");
            }
    }

    return pszName;
}

WSEC_ERR_T CAC_GetAlgList(
    CAC_FP_ProcAlg pfProcAlg,
    WSEC_VOID   *pReserved)
{
    if (WSEC_NULL_PTR == pfProcAlg)
    {
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }

    pfProcAlg(WSEC_ALGID_DES_EDE3_ECB, WSEC_ALGNAME_DES_EDE3_ECB, pReserved);
    pfProcAlg(WSEC_ALGID_DES_EDE3_CBC, WSEC_ALGNAME_DES_EDE3_CBC, pReserved);
    pfProcAlg(WSEC_ALGID_AES128_ECB, WSEC_ALGNAME_AES128_ECB, pReserved);
    pfProcAlg(WSEC_ALGID_AES128_CBC, WSEC_ALGNAME_AES128_CBC, pReserved);
    pfProcAlg(WSEC_ALGID_AES256_ECB, WSEC_ALGNAME_AES256_ECB, pReserved);
    pfProcAlg(WSEC_ALGID_AES256_CBC, WSEC_ALGNAME_AES256_CBC, pReserved);
    pfProcAlg(WSEC_ALGID_MD5, WSEC_ALGNAME_MD5, pReserved);
    pfProcAlg(WSEC_ALGID_SHA1, WSEC_ALGNAME_SHA1, pReserved);
    pfProcAlg(WSEC_ALGID_SHA224, WSEC_ALGNAME_SHA224, pReserved);
    pfProcAlg(WSEC_ALGID_SHA256, WSEC_ALGNAME_SHA256, pReserved);
    pfProcAlg(WSEC_ALGID_SHA384, WSEC_ALGNAME_SHA384, pReserved);
    pfProcAlg(WSEC_ALGID_SHA512, WSEC_ALGNAME_SHA512, pReserved);
    pfProcAlg(WSEC_ALGID_HMAC_MD5, WSEC_ALGNAME_HMAC_MD5, pReserved);
    pfProcAlg(WSEC_ALGID_HMAC_SHA1, WSEC_ALGNAME_HMAC_SHA1, pReserved);
    pfProcAlg(WSEC_ALGID_HMAC_SHA224, WSEC_ALGNAME_HMAC_SHA224, pReserved);
    pfProcAlg(WSEC_ALGID_HMAC_SHA256, WSEC_ALGNAME_HMAC_SHA256, pReserved);
    pfProcAlg(WSEC_ALGID_HMAC_SHA384, WSEC_ALGNAME_HMAC_SHA384, pReserved);
    pfProcAlg(WSEC_ALGID_HMAC_SHA512, WSEC_ALGNAME_HMAC_SHA512, pReserved);
    pfProcAlg(WSEC_ALGID_PBKDF2_HMAC_MD5, WSEC_ALGNAME_PBKDF2_HMAC_MD5, pReserved);
    pfProcAlg(WSEC_ALGID_PBKDF2_HMAC_SHA1, WSEC_ALGNAME_PBKDF2_HMAC_SHA1, pReserved);
    pfProcAlg(WSEC_ALGID_PBKDF2_HMAC_SHA224, WSEC_ALGNAME_PBKDF2_HMAC_SHA224, pReserved);
    pfProcAlg(WSEC_ALGID_PBKDF2_HMAC_SHA256, WSEC_ALGNAME_PBKDF2_HMAC_SHA256, pReserved);
    pfProcAlg(WSEC_ALGID_PBKDF2_HMAC_SHA384, WSEC_ALGNAME_PBKDF2_HMAC_SHA384, pReserved);
    pfProcAlg(WSEC_ALGID_PBKDF2_HMAC_SHA512, WSEC_ALGNAME_PBKDF2_HMAC_SHA512, pReserved);

    return WSEC_SUCCESS;
}

WSEC_ALGTYPE_E CAC_AlgId2Type(WSEC_UINT32 ulAlgID)
{
    if (ALGID_UNKNOWN != CAC_Alg2IPSIAlg_sym(ulAlgID))
    {
        return WSEC_ALGTYPE_SYM;
    }

    if (ALGID_UNKNOWN != CAC_Alg2IPSIAlg_hmac(ulAlgID))
    {
        return WSEC_ALGTYPE_HMAC;
    }

    if (ALGID_UNKNOWN != CAC_Alg2IPSIAlg_pbkdf(ulAlgID))
    {
        return WSEC_ALGTYPE_PBKDF;
    }

    if (ALGID_UNKNOWN != CAC_Alg2IPSIAlg_digest(ulAlgID))
    {
        return WSEC_ALGTYPE_DIGEST;
    }

    return WSEC_ALGTYPE_UNKNOWN;
}


/******************************************************************************
    FunctionName      : CAC_Digest
                                  CAC_DigestInit
                                  CAC_DigestUpdate
                                  CAC_DigestFinal
    Version       : 1.0.0
    Author          :  l00171031 
    Date          : 2014-6-16
    Description   : 密钥管理模块摘要相关函数，此处要封装IPSI 的相关函数。
                         输入的算法ID 采用密钥管理模块的ID , 封装IPSI 或者 openssl 的
                         接口。
    :
  
  
    History   :
          1.Date            : 2014-6-16
            Author            :  l00171031 
            Modification    :　创建文件
  
******************************************************************************/

WSEC_ERR_T CAC_Digest(WSEC_UINT32 ulAlgID,
                      const WSEC_VOID* pvData, WSEC_SIZE_T ulDataLen,
                      WSEC_VOID* pvDigest, WSEC_SIZE_T* pulDigestLen)
{
    WSEC_UINT32 AlgType = 0 ;
    WSEC_UINT32 iRet = 0;

    AlgType = CAC_Alg2IPSIAlg_digest(ulAlgID);

    if(0 == AlgType)
    {
        WSEC_LOG_E("Wrong Digest AlgID\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	
	if(!pvData  || !pvDigest )
	{
		return WSEC_ERR_INVALID_ARG;
	}
	
    iRet = CRYPT_digest((SEC_UINT32)AlgType, (SEC_UCHAR*)pvData, ulDataLen, pvDigest, (SEC_UINT32 *)pulDigestLen);
    if (SEC_SUCCESS == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_digest: %d\n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }

}



WSEC_ERR_T CAC_DigestInit(WSEC_CRYPT_CTX* pCtx, WSEC_UINT32 ulAlgID)
{
    WSEC_UINT32 AlgType = 0 ;
    WSEC_UINT32 iRet = 0;
    AlgType = CAC_Alg2IPSIAlg_digest(ulAlgID);

    if(0 == AlgType)
    {
        WSEC_LOG_E("Wrong Digest AlgID \n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	if(!pCtx )
	{
	    return WSEC_ERR_INVALID_ARG;
	}

    iRet = CRYPT_digestInit((CRYPT_CTX*)pCtx,AlgType);

    if (SEC_SUCCESS == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_digestinit , return value: %d\n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }      

}

WSEC_ERR_T CAC_DigestUpdate( 
    const WSEC_CRYPT_CTX pCtx, 
    const WSEC_VOID *pvData, WSEC_SIZE_T ulDataLen)
{
    WSEC_UINT32 iRet = 0 ;
	if(!pCtx || !pvData )
	{
	    return WSEC_ERR_INVALID_ARG;
	}
    iRet = CRYPT_digestUpdate((CRYPT_CTX)pCtx, (SEC_UCHAR*)pvData, ulDataLen);

    if (SEC_SUCCESS == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_digestUpdate: %d\n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }    

}

WSEC_ERR_T CAC_DigestFinal( 
    WSEC_CRYPT_CTX* pCtx,
    WSEC_VOID* pvDigest, WSEC_SIZE_T* pulLen)
{

    WSEC_UINT32 iRet = 0 ;
	if(!pCtx)
	{
		WSEC_LOG_E("Wrong Input pCtx for CAC_DigestFinal \n");
	    return WSEC_ERR_INVALID_ARG;
	}
		
	if(!pvDigest || !pulLen)
	{
		CAC_DigestFree((WSEC_CRYPT_CTX*)pCtx);
		WSEC_LOG_E("Wrong Input pvDigest or pulLen for CAC_DigestFinal \n");
	    return WSEC_ERR_INVALID_ARG;
	}

    iRet = CRYPT_digestFinal((CRYPT_CTX*)pCtx, pvDigest, (SEC_UINT32 *)pulLen);

    if (SEC_SUCCESS == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_DigestFinal: %d \n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }  

}




/******************************************************************************
    FunctionName      : CAC_Hmac
                                  CAC_HmacInit
                                  CAC_HmacUpdate
                                  CAC_HmacFinal
    Version       : 1.0.0
    Author          :  l00171031 
    Date          : 2014-6-16
    Description   : 密钥管理模块hmac 摘要相关函数，此处要封装IPSI 的相关函数。
                         输入的算法ID 采用密钥管理模块的ID , 封装IPSI 或者 openssl 的
                         接口。
    :
  
  
    History   :
          1.Date            : 2014-6-16
            Author            :  l00171031 
            Modification    :　创建文件
  
******************************************************************************/
WSEC_UINT32 CAC_HMAC_Size (WSEC_UINT32 ulAlgId)
{
    return CRYPT_HMAC_size(CAC_Alg2IPSIAlg_hmac(ulAlgId));
}


WSEC_ERR_T CAC_Hmac(WSEC_UINT32 ulAlgID,
                    const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,   
                    const WSEC_VOID* pvData, WSEC_SIZE_T ulDataLen,
                    WSEC_VOID* pvHmac, WSEC_SIZE_T* pulHmacLen)
{
    WSEC_UINT32 AlgType = 0 ;
    WSEC_UINT32 iRet = 0;

    AlgType = CAC_Alg2IPSIAlg_hmac(ulAlgID);

    if(0 == AlgType)
    {
        WSEC_LOG_E("Wrong hmac AlgID\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	
	if(!pvKey || !pvData || !pvHmac || !pulHmacLen)
	{
		return WSEC_ERR_INVALID_ARG;
	}

    iRet = CRYPT_hmac((SEC_UINT32)AlgType,(SEC_UCHAR*)pvKey, (SEC_UINT32)ulKeyLen, 
						(SEC_UCHAR*)pvData, (SEC_UINT32)ulDataLen, (SEC_UCHAR*)pvHmac, (SEC_UINT32 *)pulHmacLen);
    if (SEC_SUCCESS == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_hmac: %d\n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
}

WSEC_ERR_T CAC_HmacInit(WSEC_CRYPT_CTX* pCtx,
                        WSEC_UINT32 ulAlgID,
                        const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen)
{
    WSEC_UINT32 AlgType = 0 ;
    WSEC_UINT32 iRet = 0;

    AlgType = CAC_Alg2IPSIAlg_hmac(ulAlgID);

    if(0 == AlgType)
    {
        WSEC_LOG_E("Wrong hmac AlgID\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	
	if(!pCtx || !pvKey)
	{
	    return WSEC_ERR_INVALID_ARG;
	}

    iRet = CRYPT_hmacInit((CRYPT_CTX*)pCtx,AlgType, pvKey, ulKeyLen);

    if (SEC_SUCCESS == iRet )
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_hmacInit: %d\n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
}

WSEC_ERR_T CAC_HmacUpdate(const WSEC_CRYPT_CTX pCtx, const WSEC_VOID* pvData, WSEC_SIZE_T ulDataLen)
{
    WSEC_UINT32 iRet = 0;
	if(!pCtx || !pvData)
	{
	    return WSEC_ERR_INVALID_ARG;
	}

    iRet = CRYPT_hmacUpdate((CRYPT_CTX)pCtx, (SEC_UCHAR*)pvData, ulDataLen);

    if (SEC_SUCCESS == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_hmacInit :%d\n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }

}

WSEC_ERR_T CAC_HmacFinal(WSEC_CRYPT_CTX* pCtx, WSEC_VOID* pvHmac, WSEC_SIZE_T* pulHmacLen)
{
    WSEC_UINT32 iRet = 0;
	if( !pCtx )
	{
		WSEC_LOG_E("Wrong Input pCtx for CAC_HmacFinal \n");
	    return WSEC_ERR_INVALID_ARG;
	}

	if(!pvHmac || !pulHmacLen)
	{
		CAC_HmacFree((WSEC_CRYPT_CTX*)pCtx);
		WSEC_LOG_E("Wrong Input pvHmac or pulHmacLen for CAC_HmacFinal \n");
		return  WSEC_ERR_INVALID_ARG;
	}
	
    iRet = CRYPT_hmacFinal((CRYPT_CTX*)pCtx, pvHmac,(SEC_UINT32 *)pulHmacLen);

    if (SEC_SUCCESS == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_DigestFinal :%d\n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }  

}


  /******************************************************************************
   FunctionName      : CAC_Pbkdf2
   Version         : 1.0.0
   Author         :    l00171031 
   Date          : 2014-6-16
   Description     : WSEC pbkdf2 迭代生成的函数。用于使用pbkdf2 迭代生成密钥
                       参数较多，只依赖ipsi 或者openssl 的加解密接口。其中还有
                       一些其他的函数配合使用。
   :
 
 
   History     :
         1.Date            : 2014-6-16
           Author           :  l00171031 
           Modification    :　创建文件
 
 ******************************************************************************/

WSEC_BOOL CAC_pbkdf2_ForPBES2_chkKDFAlgID(WSEC_UINT32 ulKDFAlg)
{


    /* Valid KDF algorithms are ALGID_HMAC_SHA1 and ALGID_HMAC_MD5
    ALGID_HMAC_SHA224 and ALGID_HMAC_SHA256 and ALGID_HMAC_SHA384 
    *ALGID_HMAC_SHA512 */

    if ((ALGID_HMAC_SHA1 == ulKDFAlg)
        || (ALGID_HMAC_MD5 == ulKDFAlg)
        || (ALGID_HMAC_SHA224 == ulKDFAlg)
        || (ALGID_HMAC_SHA256 == ulKDFAlg)
        || (ALGID_HMAC_SHA384 == ulKDFAlg)
        || (ALGID_HMAC_SHA512 == ulKDFAlg))
    {
        return WSEC_TRUE;
    }

    return WSEC_FALSE;
}



WSEC_ERR_T CAC_pbkdf2_deriveKey_forPBES2(
    WSEC_UINT32 ulKDFAlg,
    const WSEC_BYTE* pucPassword, WSEC_UINT32 ulPwdLen,
    const WSEC_BYTE* pucSalt, WSEC_UINT32 ulSaltLen,
    WSEC_UINT32 ulDKLen,
    WSEC_INT32 iIteration,
    WSEC_BYTE* pucDerivedKey)
{
    WSEC_UINT32 ulBlocksCount = 1;
    WSEC_INT32  ulTempCount1=0;
    WSEC_UINT32 ulTempCount2=0;
    WSEC_UINT32 ulCurrLen = 0;
    WSEC_BYTE acTempDigest[MAX_DIGEST_SIZE]= {0};
    WSEC_UINT32 ulTempDigestLen = 0;
    WSEC_UINT32 ulHLEN = 0;
    WSEC_BYTE ucTemp[4] = {0};
    CRYPT_CTX stCtx = WSEC_NULL_PTR; 
    WSEC_ERR_T ulRetVal = WSEC_SUCCESS;

    if (!CAC_pbkdf2_ForPBES2_chkKDFAlgID(ulKDFAlg)) {return WSEC_ERR_CRPTO_LIB_FAIL;}

    ulHLEN = CRYPT_HMAC_size(ulKDFAlg); 
    /* BEGIN:  Tests of a value against zero should be made explicit
    : while(ulDKLen)*/
    while (0 != ulDKLen)
    {
        if (ulDKLen > ulHLEN)
        {
            ulCurrLen = ulHLEN;
        }
        else
        {
            ulCurrLen = ulDKLen;
        }
        /* We are unlikely to ever use more than 
        256 blocks (5120 bits!)
        * but just   case...
        To find the Block Index */

        ucTemp[0] = (unsigned char)((ulBlocksCount >> 24) & 0xff);
        ucTemp[1] = (unsigned char)((ulBlocksCount >> 16) & 0xff);
        ucTemp[2] = (unsigned char)((ulBlocksCount >> 8) & 0xff);
        ucTemp[3] = (unsigned char)(ulBlocksCount & 0xff);

        ulRetVal = CRYPT_hmacInit(&stCtx, ulKDFAlg,
            pucPassword, ulPwdLen);
        if (SEC_SUCCESS == ulRetVal)
        {
            ulRetVal=CRYPT_hmacUpdate(stCtx, (SEC_UCHAR *)pucSalt, ulSaltLen);
            if (SEC_SUCCESS == ulRetVal)
            {
                ulRetVal= CRYPT_hmacUpdate(stCtx, ucTemp, 4);
                if (SEC_SUCCESS == ulRetVal)
                {
                    ulRetVal=CRYPT_hmacFinal(&stCtx, acTempDigest, (SEC_UINT32 *)&ulTempDigestLen);
                }
            }
        }

        if (SEC_SUCCESS != ulRetVal)
        {
            return WSEC_ERR_CRPTO_LIB_FAIL;
        }

		return_oper_if(WSEC_MEMCPY(pucDerivedKey, ulCurrLen, acTempDigest, ulCurrLen) != EOK, WSEC_LOG_E4MEMCPY, WSEC_ERR_MEMCPY_FAIL);

        for (ulTempCount1 = 1; ulTempCount1 < iIteration;
            ulTempCount1++)
        {

            ulRetVal = CRYPT_hmac(ulKDFAlg, pucPassword, ulPwdLen,
                acTempDigest, ulHLEN,
                acTempDigest, (SEC_UINT32 *)&ulTempDigestLen);

            if (SEC_SUCCESS != ulRetVal)
            {
                return WSEC_ERR_CRPTO_LIB_FAIL;
            }

            for (ulTempCount2 = 0; ulTempCount2 < ulCurrLen;
                ulTempCount2++)
            {
                pucDerivedKey[ulTempCount2] ^= acTempDigest[ulTempCount2];
            }
        }

        ulDKLen -= ulCurrLen;
        ulBlocksCount++;
        pucDerivedKey += ulCurrLen;
    }    

    return ulRetVal;
}

/*功能接口*/

WSEC_ERR_T CAC_Pbkdf2(WSEC_UINT32 ulKDFAlg,
                      const WSEC_VOID* pvPassword, WSEC_SIZE_T ulPwdLen,
                      const WSEC_VOID* pvSalt, WSEC_SIZE_T ulSaltLen,
                      WSEC_INT32 iIter, 
                      WSEC_SIZE_T ulDKLen, WSEC_VOID* pvDerivedKey)
{
    WSEC_UINT32 ulHashLen = 0;
    WSEC_UINT32 ulRetVal = 0;
    WSEC_BYTE* pucHashData = WSEC_NULL_PTR;
    WSEC_BYTE* pucTemp = WSEC_NULL_PTR;
    WSEC_UINT32 ulTempKDFAlg = 0;

	return_err_if_para_invalid("CAC_Pbkdf2", pvPassword && pvSalt && pvDerivedKey && ulSaltLen);
    
    if (iIter <= 0)
    {
        ulRetVal = WSEC_ERR_INVALID_ARG;
        goto HANDLE_ERR;
    }
	
    ulTempKDFAlg = CAC_Alg2IPSIAlg_pbkdf(ulKDFAlg);
    if (ulTempKDFAlg == 0)
    {
        ulRetVal = WSEC_ERR_INVALID_ARG;
        goto HANDLE_ERR;
    }

    ulRetVal = CAC_pbkdf2_deriveKey_forPBES2(ulTempKDFAlg, pvPassword,
        ulPwdLen, pvSalt, ulSaltLen,
        ulDKLen, iIter,
        pvDerivedKey);

    if (WSEC_SUCCESS != ulRetVal)
    {
        goto HANDLE_ERR;
    }

    WSEC_CLEANSE_DATA(pucTemp, (ulHashLen * sizeof(WSEC_BYTE)));
    WSEC_FREE(pucTemp);
    WSEC_CLEANSE_DATA(pucHashData, (ulHashLen * sizeof(WSEC_BYTE)));
    WSEC_FREE(pucHashData);

    return WSEC_SUCCESS;

HANDLE_ERR:
    WSEC_FREE(pucTemp);
    WSEC_FREE(pucHashData);
    if (WSEC_ERR_MALLOC_FAIL != ulRetVal)
    {
        WSEC_LOG_E1("The failure reason is : %d\n", ulRetVal);
    }

    return ulRetVal;
}

WSEC_UINT32 CAC_SYM_BlockSize(WSEC_UINT32 ulAlgId)
{
    return CRYPT_SYM_blockSize(CAC_Alg2IPSIAlg_sym(ulAlgId));
}

WSEC_UINT32 CAC_SYM_IvLen(WSEC_UINT32 ulAlgId)
{
    return CRYPT_SYM_ivLen(CAC_Alg2IPSIAlg_sym(ulAlgId));
}

WSEC_UINT32 CAC_SYM_KeyLen(WSEC_UINT32 ulAlgId)
{
	return CRYPT_SYM_keyLen(CAC_Alg2IPSIAlg_sym(ulAlgId));
}

WSEC_ERR_T CAC_Encrypt(WSEC_UINT32 ulAlgID,
                       const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,
                       const WSEC_VOID* pvIV, WSEC_SIZE_T ulIVLen,
                       const WSEC_VOID* pvPlainText, WSEC_SIZE_T ulPlainLen,
                       WSEC_VOID* pvCipherText, WSEC_SIZE_T* pulCLen)
{
    WSEC_UINT32 AlgType = 0 ;
    WSEC_UINT32 iRet = 0;
	WSEC_UINT32 blocksize = 0;

    AlgType = CAC_Alg2IPSIAlg_sym(ulAlgID);

    if(0 == AlgType)
    {
        WSEC_LOG_E("Wrong Encryption AlgID\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	
	if( !pvKey || !pvIV || !pvPlainText || !pvCipherText || !pulCLen)
	{
	    return WSEC_ERR_INVALID_ARG;
	}

	blocksize = CAC_SYM_BlockSize( ulAlgID);

	if(*pulCLen < (blocksize ? ((ulPlainLen/blocksize+1)*blocksize):ulPlainLen))
	{
		WSEC_LOG_E("The Input ciphertext buffer len is not enough ,make sure it is at least (ulPlainLen/blocksize+1)*ulPlainLen \n");
		return WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH;
	}

    CRYPT_SET_PAD_MODE(AlgType, BLOCK_PADDING_NORMAL);
    iRet = CRYPT_encrypt(AlgType,
                        (SEC_UCHAR*)pvKey, ulKeyLen,
                        (SEC_UCHAR*)pvIV, ulIVLen,
                        (SEC_UCHAR*)pvPlainText, ulPlainLen,
                        pvCipherText, (SEC_UINT32 *)pulCLen);
    if (SEC_SUCCESS == iRet )
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("CRYPT_encrypt() fail:%d", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
}

WSEC_ERR_T CAC_EncryptInit(WSEC_CRYPT_CTX* pCtx, WSEC_UINT32 ulAlgID,
                           const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,
                           const WSEC_VOID* pvIV, WSEC_SIZE_T ulIVLen)
{
    WSEC_UINT32 AlgType = 0 ;
    WSEC_UINT32 iRet = 0;

    AlgType = CAC_Alg2IPSIAlg_sym(ulAlgID);

    if(0 == AlgType)
    {
        WSEC_LOG_E("Wrong Encryption AlgID\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	
	if(!pCtx ||!pvKey ||!pvIV)
	{
	    return WSEC_ERR_INVALID_ARG;
	}

    CRYPT_SET_PAD_MODE(AlgType, BLOCK_PADDING_NORMAL);
    iRet = CRYPT_encryptInit((CRYPT_CTX*)pCtx, AlgType,
                             (SEC_UCHAR*)pvKey, (SEC_UINT32)ulKeyLen,
                             (SEC_UCHAR*)pvIV, (SEC_UINT32)ulIVLen);
    if (SEC_SUCCESS == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_encryptInit :%d \n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
}


WSEC_ERR_T CAC_EncryptUpdate(const WSEC_CRYPT_CTX ctx,
                             const WSEC_VOID* pvPlainText, WSEC_SIZE_T ulPlainLen,
                             WSEC_VOID* pvCipherText, WSEC_SIZE_T* pulCLen)
{

    WSEC_UINT32 iRet = 0;

	if(!ctx || !pvPlainText || !pvCipherText || !pulCLen)
	{
	    return WSEC_ERR_INVALID_ARG;
	}

	if(*pulCLen < ulPlainLen)
	{
		WSEC_LOG_E("The Input ciphertext buffer len for update is not enough ,make sure it is at least ulPlainLen \n");
		return WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH;
	}

    iRet = CRYPT_encryptUpdate((CRYPT_CTX)ctx,
           (SEC_UCHAR*)pvPlainText, ulPlainLen,
           pvCipherText, (SEC_UINT32*)pulCLen);
    if (SEC_SUCCESS == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_encryptUpdate :%d \n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }

}

WSEC_ERR_T CAC_EncryptFinal(WSEC_CRYPT_CTX* pCtx, WSEC_VOID* pvCipherText, WSEC_SIZE_T* pulCLen)
{
    WSEC_UINT32 iRet = 0;
	if(!pCtx )
	{
		WSEC_LOG_E("Got Wrong pCtx from CAC_EncryptFinal  \n");
	    return WSEC_ERR_INVALID_ARG;
	}

	if(!pvCipherText || !pulCLen)
	{
		CAC_CipherFree((WSEC_CRYPT_CTX*)pCtx);
		return WSEC_ERR_INVALID_ARG;
	}

    iRet = CRYPT_encryptFinal((CRYPT_CTX*)pCtx, pvCipherText, (SEC_UINT32 *)pulCLen);
    if (SEC_SUCCESS == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_encryptFinal :%d \n ", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
}


/******************************************************************************
      FunctionName        : CAC_Decrypt
                                      CAC_DecryptInit
                                      CAC_DecryptUpdate
                                      CAC_DecryptFinal
      Version        : 1.0.0
      Author        :  l00171031 
      Date            : 2014-6-16
      Description    : WSEC 解密 迭代生成的函数。用于使用解密 迭代生成密钥
                          参数较多，只依赖ipsi 或者openssl 的加解密接口。其中还有
                          一些其他的函数配合使用。
      :
    
    
      History    :
            1.Date              : 2014-6-16
              Author          :  l00171031 
              Modification      :　创建文件
    
******************************************************************************/

WSEC_ERR_T CAC_Decrypt(WSEC_UINT32 ulAlgID,
                        const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,
                        const WSEC_VOID* pvIV, WSEC_SIZE_T ulIVLen,
                        const WSEC_VOID* pvCipherText, WSEC_SIZE_T ulCipherLen,
                        WSEC_VOID* pvPlainText, WSEC_SIZE_T* pulPLen)
{
    WSEC_UINT32 AlgType = 0 ;
    WSEC_UINT32 iRet = 0;

    AlgType = CAC_Alg2IPSIAlg_sym(ulAlgID);

    if(0 == AlgType)
    {
        WSEC_LOG_E("Wrong Encryption AlgID \n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	
	if( !pvKey || !pvIV|| !pvCipherText || !pvPlainText || !pulPLen)
	{
	    return WSEC_ERR_INVALID_ARG;
	}
	if(*pulPLen < ulCipherLen )
	{
		WSEC_LOG_E("The Input plaintext buffer len is not enough ,make sure it is at least ulCipherLen+blocksize \n");
		return WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH;
	}

    CRYPT_SET_PAD_MODE(AlgType, BLOCK_PADDING_NORMAL);
    iRet = CRYPT_decrypt(AlgType, pvKey,ulKeyLen,pvIV,ulIVLen,
        (SEC_UCHAR *)pvCipherText,ulCipherLen,pvPlainText,(SEC_UINT32 *)pulPLen);
    if (SEC_SUCCESS == iRet )
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_decrypt:%d \n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
}

WSEC_ERR_T CAC_DecryptInit(WSEC_CRYPT_CTX* pCtx, WSEC_UINT32 ulAlgID,
                           const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,
                           const WSEC_VOID* pvIV, WSEC_SIZE_T ulIVLen)
{
    WSEC_UINT32 AlgType = 0 ;
    WSEC_UINT32 iRet = 0;

    AlgType = CAC_Alg2IPSIAlg_sym(ulAlgID);

    if(0 == AlgType)
    {
        WSEC_LOG_E("Wrong Encryption AlgID\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	
	if(!pCtx || !pvKey || !pvIV )
	{
	    return WSEC_ERR_INVALID_ARG;
	}

    CRYPT_SET_PAD_MODE(AlgType,BLOCK_PADDING_NORMAL);
    iRet = CRYPT_decryptInit((CRYPT_CTX*)pCtx,AlgType,pvKey,ulKeyLen,pvIV,ulIVLen);
    if (SEC_SUCCESS == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_decryptInit :%d \n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }

}


WSEC_ERR_T CAC_DecryptUpdate(const WSEC_CRYPT_CTX ctx,
                             const WSEC_VOID* pvCipherText, WSEC_SIZE_T ulCipherLen,
                             WSEC_VOID* pvPlainText, WSEC_SIZE_T* pulPLen)
{
    WSEC_UINT32 iRet = 0;

    return_err_if_para_invalid("CAC_DecryptUpdate", (pvCipherText && pvPlainText && pulPLen));
    
	if(!ctx || !pvCipherText || !pvPlainText || !pulPLen)
	{
	    return WSEC_ERR_INVALID_ARG;
	}
	if(*pulPLen < ulCipherLen )
	{
		WSEC_LOG_E("The Input plaintext buffer len is not enough ,make sure it is at least ulCipherLen+blocksize \n");
		return WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH;
	}

    iRet = CRYPT_decryptUpdate((CRYPT_CTX)ctx,(SEC_UCHAR*)pvCipherText,ulCipherLen,pvPlainText,(SEC_UINT32 *)pulPLen);
    if (SEC_SUCCESS == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_decryptUpdate :%d \n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
}

WSEC_ERR_T CAC_DecryptFinal(WSEC_CRYPT_CTX* pCtx, WSEC_VOID* pvPlainText, WSEC_SIZE_T* pulPLen)
{

    WSEC_UINT32 iRet = 0;
	if(!pCtx )
	{
		WSEC_LOG_E("Got Wrong pCtx from CRYPT_DecryptFinal  \n");
	    return WSEC_ERR_INVALID_ARG;
	}

	if(!pvPlainText || !pulPLen)
	{
		CAC_CipherFree((WSEC_CRYPT_CTX*)pCtx);
		return WSEC_ERR_INVALID_ARG;
	}

    iRet = CRYPT_decryptFinal((CRYPT_CTX*)pCtx,(SEC_UCHAR*)pvPlainText,(SEC_UINT32 *)pulPLen);
    if (SEC_SUCCESS == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_decryptFinal :%d \n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* WSEC_COMPILE_CAC_IPSI */


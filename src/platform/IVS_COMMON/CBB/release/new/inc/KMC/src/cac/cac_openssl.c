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
        1.Date            : 2015-2-6
          Author          :  l00171031 
          Modification    :　创建文件

******************************************************************************/
/*PClint warning ignore*//*lint -e522 -e550 -e533 -e534 -e641 -e506 -e539*/

#include "wsec_config.h"

#ifdef WSEC_COMPILE_CAC_OPENSSL

#include "cac_pri.h"
#include "openssl/err.h"
#include "openssl/evp.h"
#include "openssl/rand.h"
#include "openssl/aes.h"
#include "openssl/hmac.h"
#include "wsec_type.h"
#include "wsec_itf.h"
#include "wsec_pri.h"
#include "wsec_securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */
#define CAC_ALGID_UNKNOWN 0
#define CAC_OPENSSL_CRPYT_BUFF_OVER_SIZE (32)

WSEC_BOOL g_bCrptoLibInit = WSEC_FALSE;

WSEC_ERR_T CAC_EncryptPri(WSEC_UINT32 ulAlgID,
                       const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,
                       const WSEC_VOID* pvIV, WSEC_SIZE_T ulIVLen,
                       const WSEC_VOID* pvPlainText, WSEC_SIZE_T ulPlainLen,
                       WSEC_VOID* pvCipherText, WSEC_SIZE_T* pulCLen);
WSEC_ERR_T CAC_EncryptUpdatePri(const WSEC_CRYPT_CTX ctx,
                             const WSEC_VOID* pvPlainText, WSEC_SIZE_T ulPlainLen,
                             WSEC_VOID* pvCipherText, WSEC_SIZE_T* pulCLen);
WSEC_ERR_T CAC_EncryptFinalPri(const WSEC_CRYPT_CTX* pCtx, WSEC_VOID* pvCipherText, WSEC_SIZE_T* pulCLen);
WSEC_ERR_T CAC_DecryptPri(WSEC_UINT32 ulAlgID,
                       const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,
                       const WSEC_VOID* pvIV, WSEC_SIZE_T ulIVLen,
                        const WSEC_VOID* pvCipherText, WSEC_SIZE_T ulCipherLen,
                        WSEC_VOID* pvPlainText, WSEC_SIZE_T* pulPLen);
WSEC_ERR_T CAC_DecryptUpdatePri(const WSEC_CRYPT_CTX ctx,
                             const WSEC_VOID* pvCipherText, WSEC_SIZE_T ulCipherLen,
                             WSEC_VOID* pvPlainText, WSEC_SIZE_T* pulPLen);
WSEC_ERR_T CAC_DecryptFinalPri(const WSEC_CRYPT_CTX* pCtx, WSEC_VOID* pvPlainText, WSEC_SIZE_T* pulPLen);

/*内存释放函数*/
WSEC_VOID CAC_DigestFree(WSEC_CRYPT_CTX *pCtx)
{
	if(!pCtx || !(*pCtx))
	{	return; }
	EVP_MD_CTX_destroy((EVP_MD_CTX*)(*pCtx));
	*pCtx = 0;
}

WSEC_VOID CAC_HmacFree(WSEC_CRYPT_CTX *pCtx)
{
	if(!pCtx || !(*pCtx))
	{	return; }
	HMAC_CTX_cleanup((HMAC_CTX*)(*pCtx));
	WSEC_FREE(*pCtx);
}

WSEC_VOID CAC_CipherFree(WSEC_CRYPT_CTX *pCtx)
{
	if(!pCtx || !(*pCtx))
	{	return; }
    EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)(*pCtx));
	*pCtx = 0;
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
	WSEC_UNREFER(pvEntropyBuf);
	WSEC_UNREFER(ulLen);
	if(!g_bCrptoLibInit)
	{
		/* Initialise the library */
		ERR_load_crypto_strings();
	    /* Load up the software EVP_CIPHER and EVP_MD definitions */
		OpenSSL_add_all_algorithms();
        g_bCrptoLibInit = WSEC_TRUE;
	}

    return WSEC_SUCCESS;
}

WSEC_ERR_T CAC_Random(WSEC_VOID * pBuf, WSEC_SIZE_T ulLen)
{
    WSEC_INT32 iRet = 0;

	if(!pBuf)
		return WSEC_ERR_INVALID_ARG;
	
    iRet = RAND_bytes((WSEC_BYTE*)pBuf, (int)ulLen);
    if(1 != iRet)
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
  FunctionName        : CAC_2OpensslAlg
  Version        : 1.0.0
  Author        :  l00171031 
  Date            : 2014-6-16
  Description    : 转换ID,从密钥管理模块的ID 转换成Openssl 的Method  :


  History    :
        1.Date              : 2014-6-16
          Author          :  l00171031 
          Modification      :　创建文件

******************************************************************************/

const EVP_MD* CAC_Alg2OpensslAlg_digest(WSEC_UINT32 AlgID)
{
    switch (AlgID)
    {    
        case(WSEC_ALGID_MD5): { return EVP_md5(); }
        case(WSEC_ALGID_SHA1): { return EVP_sha1(); }
        case(WSEC_ALGID_SHA224): { return EVP_sha224(); }
        case(WSEC_ALGID_SHA256): { return EVP_sha256(); }
        case(WSEC_ALGID_SHA384): { return EVP_sha384(); }
        case(WSEC_ALGID_SHA512): { return EVP_sha512(); }  

        default:
        {
           return WSEC_NULL_PTR;
        }
    }
    

}

const EVP_MD* CAC_Alg2OpensslAlg_hmac(WSEC_UINT32 AlgID)
{
    switch (AlgID)
    {    
        case(WSEC_ALGID_HMAC_MD5): { return EVP_md5(); }
        case(WSEC_ALGID_HMAC_SHA1): { return EVP_sha1(); }
        case(WSEC_ALGID_HMAC_SHA224): { return EVP_sha224(); }
        case(WSEC_ALGID_HMAC_SHA256): { return EVP_sha256(); }
        case(WSEC_ALGID_HMAC_SHA384): { return EVP_sha384(); }
        case(WSEC_ALGID_HMAC_SHA512): { return EVP_sha512(); }

        default:
        {
            return WSEC_NULL_PTR;
        }

    }
}


const EVP_CIPHER* CAC_Alg2OpensslAlg_sym(WSEC_UINT32 AlgID)
{
    switch (AlgID)
    {    
        case(WSEC_ALGID_DES_EDE3_ECB): { return EVP_des_ede3_ecb(); }
        case(WSEC_ALGID_DES_EDE3_CBC): { return EVP_des_ede3_cbc(); }
        case(WSEC_ALGID_AES128_ECB): { return EVP_aes_128_ecb(); }
        case(WSEC_ALGID_AES128_CBC): { return EVP_aes_128_cbc();}
        case(WSEC_ALGID_AES256_ECB): { return EVP_aes_256_ecb(); }
        case(WSEC_ALGID_AES256_CBC): { return EVP_aes_256_cbc(); }

        default:
        {
            return WSEC_NULL_PTR;
        }

    }
}      

const EVP_MD* CAC_Alg2OpensslAlg_pbkdf(WSEC_UINT32 AlgID)
{

    switch (AlgID)
    {    
        case(WSEC_ALGID_PBKDF2_HMAC_MD5): { return EVP_md5(); }
        case(WSEC_ALGID_PBKDF2_HMAC_SHA1): { return EVP_sha1(); }
        case(WSEC_ALGID_PBKDF2_HMAC_SHA224): { return EVP_sha224(); }
        case(WSEC_ALGID_PBKDF2_HMAC_SHA256): { return EVP_sha256(); }
        case(WSEC_ALGID_PBKDF2_HMAC_SHA384): { return EVP_sha384(); }
        case(WSEC_ALGID_PBKDF2_HMAC_SHA512): { return EVP_sha512(); }

        default:
        {
            return WSEC_NULL_PTR;
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
    if (CAC_ALGID_UNKNOWN != CAC_Alg2OpensslAlg_sym(ulAlgID))
    {
        return WSEC_ALGTYPE_SYM;
    }

    if (CAC_ALGID_UNKNOWN != CAC_Alg2OpensslAlg_hmac(ulAlgID))
    {
        return WSEC_ALGTYPE_HMAC;
    }

    if (CAC_ALGID_UNKNOWN != CAC_Alg2OpensslAlg_pbkdf(ulAlgID))
    {
        return WSEC_ALGTYPE_PBKDF;
    }

    if (CAC_ALGID_UNKNOWN != CAC_Alg2OpensslAlg_digest(ulAlgID))
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
    Description   : 密钥管理模块摘要相关函数，此处要封装openssl 的相关函数。
                         输入的算法ID 采用密钥管理模块的ID , 封装openssl  的
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
    const EVP_MD* AlgType = 0 ;
    WSEC_UINT32 iRet = 0;

    AlgType = CAC_Alg2OpensslAlg_digest(ulAlgID);

    if(WSEC_NULL_PTR == AlgType)
    {
        WSEC_LOG_E("Wrong Digest AlgID\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;

    }
	if(!pvData  || !pvDigest )
	{
		return WSEC_ERR_INVALID_ARG;
	}
    iRet = EVP_Digest(pvData, (size_t)ulDataLen,(WSEC_BYTE *)pvDigest, pulDigestLen, AlgType, WSEC_NULL_PTR);
    if (1 == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E("Got failure from EVP_digest \n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }

}



WSEC_ERR_T CAC_DigestInit(WSEC_CRYPT_CTX* pCtx, WSEC_UINT32 ulAlgID)
{
    const EVP_MD* AlgType = 0 ;
    WSEC_UINT32 iRet = 0;
    AlgType = CAC_Alg2OpensslAlg_digest(ulAlgID);

    if(0 == AlgType)
    {
        WSEC_LOG_E("Wrong Digest AlgID \n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	if(!pCtx )
	{
	    return WSEC_ERR_INVALID_ARG;
	}
    *pCtx = EVP_MD_CTX_create();
	if(*pCtx != WSEC_NULL_PTR)
	{ 
    	iRet = EVP_DigestInit_ex(*pCtx,AlgType,WSEC_NULL_PTR); 
	}

    if (1 == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
    	EVP_MD_CTX_destroy(*pCtx);
        WSEC_LOG_E("Got failure from EVP_DigestInit_ex \n");
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
    iRet = EVP_DigestUpdate((EVP_MD_CTX*)pCtx, pvData, ulDataLen);

    if (1 == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E1("Got failure from CRYPT_digestUpdate: %d\n", iRet);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }    
/*DigestUpdate 失败，需要调用 CAC_DigestFree 释放上下文pCtx*/
}

WSEC_ERR_T CAC_DigestFinal( 
    WSEC_CRYPT_CTX* pCtx,
    WSEC_VOID* pvDigest, WSEC_SIZE_T* pulLen)
{

    WSEC_UINT32 iRet = 0 ;
	if(!pCtx || !(*pCtx))
	{
		WSEC_LOG_E("Got failure from CRYPT_DigestFinal,The Input pCtx or content of pCtx is NULL \n");
		return WSEC_ERR_INVALID_ARG;
	}
	if(!pvDigest || !pulLen )
	{
		CAC_DigestFree((WSEC_CRYPT_CTX *)pCtx);
	    return WSEC_ERR_INVALID_ARG;
	}
    iRet = EVP_DigestFinal_ex((EVP_MD_CTX*)(*pCtx), (WSEC_BYTE*)pvDigest, (WSEC_UINT32 *)pulLen);
    CAC_DigestFree((WSEC_CRYPT_CTX *)pCtx);
    if (1 == iRet)
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
                         接口。Openssl 0.9.8 中，HMAC 系列函数没有返回值，Openssl1.0.0以上，有返回值
                         这里考虑suse的默认openssl用的是0.9.8 ，我们先用void,后续要讨论
    :
  
  
    History   :
          1.Date            : 2014-6-16
            Author            :  l00171031 
            Modification    :　创建文件
  
******************************************************************************/
WSEC_UINT32 CAC_HMAC_Size (WSEC_UINT32 ulAlgId)
{
    if(0 != CAC_Alg2OpensslAlg_hmac(ulAlgId))
        return EVP_MD_size(CAC_Alg2OpensslAlg_hmac(ulAlgId));
    else
        return 0;
}


WSEC_ERR_T CAC_Hmac(WSEC_UINT32 ulAlgID,
                    const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,   
                    const WSEC_VOID* pvData, WSEC_SIZE_T ulDataLen,
                    WSEC_VOID* pvHmac, WSEC_SIZE_T* pulHmacLen)
{
    const EVP_MD* AlgType = WSEC_NULL_PTR;
    WSEC_VOID* ptr = WSEC_NULL_PTR;

    AlgType = CAC_Alg2OpensslAlg_hmac(ulAlgID);

    if(WSEC_NULL_PTR == AlgType)
    {
        WSEC_LOG_E("Wrong hmac AlgID\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	if(!pvKey || !pvData || !pvHmac || !pulHmacLen)
	{
		return WSEC_ERR_INVALID_ARG;
	}

    ptr = HMAC(AlgType, pvKey, ulKeyLen, (WSEC_BYTE*)pvData, ulDataLen, pvHmac, (WSEC_UINT32 *)pulHmacLen);
    if (WSEC_NULL_PTR != ptr)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E("Got failure from HMAC \n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }

}

WSEC_ERR_T CAC_HmacInit(WSEC_CRYPT_CTX* pCtx,
                        WSEC_UINT32 ulAlgID,
                        const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen)
{
    const EVP_MD* AlgType = 0 ;
    HMAC_CTX* hmac_ctx = 0;

    AlgType = CAC_Alg2OpensslAlg_hmac(ulAlgID);

    if(0 == AlgType)
    {
        WSEC_LOG_E("Wrong hmac AlgID .\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	if(!pCtx || !pvKey)
	{
	    return WSEC_ERR_INVALID_ARG;
	}
	
	hmac_ctx = (HMAC_CTX*)WSEC_MALLOC(sizeof(HMAC_CTX));
	if(hmac_ctx == WSEC_NULL_PTR)
	{
        WSEC_LOG_E("Got failure from malloc.\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
	}	
	
	HMAC_CTX_init(hmac_ctx);

    if(1 != HMAC_Init_ex(hmac_ctx, pvKey, ulKeyLen ,AlgType, WSEC_NULL_PTR))
    {
        WSEC_LOG_E("Got failure from HMAC_Init_ex. \n");
		WSEC_FREE(hmac_ctx);
        return WSEC_ERR_CRPTO_LIB_FAIL;     
    }

	*pCtx = (WSEC_VOID*)hmac_ctx;

    return WSEC_SUCCESS;

}

WSEC_ERR_T CAC_HmacUpdate(const WSEC_CRYPT_CTX pCtx, const WSEC_VOID* pvData, WSEC_SIZE_T ulDataLen)
{
	if(!pCtx || !pvData)
	{
	    return WSEC_ERR_INVALID_ARG;
	}

	if(1 != HMAC_Update((HMAC_CTX*)pCtx, (WSEC_BYTE*)pvData, ulDataLen))
	{
		WSEC_LOG_E("Got failure from HMAC_Update. \n");

		return WSEC_ERR_CRPTO_LIB_FAIL;
	}
		
    return WSEC_SUCCESS;
}

WSEC_ERR_T CAC_HmacFinal(WSEC_CRYPT_CTX* pCtx, WSEC_VOID* pvHmac, WSEC_SIZE_T* pulHmacLen)
{
	WSEC_UINT32 iRet = WSEC_SUCCESS;
	if(!pCtx || !(*pCtx))
	{
		WSEC_LOG_E("The Input pCtx of CAC_HmacFinal is NULL. \n");
	    return WSEC_ERR_INVALID_ARG;
	}
	
	if( !pvHmac || !pulHmacLen )
	{
		CAC_HmacFree((WSEC_CRYPT_CTX*)pCtx);
	    return WSEC_ERR_INVALID_ARG;
	}

    if(1 != HMAC_Final((HMAC_CTX*)(*pCtx), (unsigned char *)pvHmac,(WSEC_UINT32 *)pulHmacLen))
    {
		WSEC_LOG_E("Got failure from HMAC_Final. \n");
		iRet = WSEC_ERR_CRPTO_LIB_FAIL;
    }

	CAC_HmacFree((WSEC_CRYPT_CTX*)pCtx);

    return iRet;

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

WSEC_ERR_T CAC_Pbkdf2(WSEC_UINT32 ulKDFAlg,
                      const WSEC_VOID* pvPassword, WSEC_SIZE_T ulPwdLen,
                      const WSEC_VOID* pvSalt, WSEC_SIZE_T ulSaltLen,
                      WSEC_INT32 iIter, 
                      WSEC_SIZE_T ulDKLen, WSEC_VOID* pvDerivedKey)
{
    const EVP_MD *ctx = WSEC_NULL_PTR;
	
    return_err_if_para_invalid("CAC_Pbkdf2", pvPassword && pvSalt && pvDerivedKey && ulSaltLen);
	if(iIter <= 0)
		return WSEC_ERR_INVALID_ARG;
    return_oper_if((ulPwdLen > 0) && (ulDKLen < 1), oper_null, WSEC_ERR_INVALID_ARG);
    return_oper_if((ulPwdLen < 1) && (ulDKLen > 0), oper_null, WSEC_ERR_INVALID_ARG);

    ctx = CAC_Alg2OpensslAlg_pbkdf(ulKDFAlg);
    return_oper_if(!ctx, WSEC_LOG_E("Wrong KDF AlgID"), WSEC_ERR_CRPTO_LIB_FAIL);

	if(1 == PKCS5_PBKDF2_HMAC((const char *)pvPassword,(int )ulPwdLen,
		(WSEC_BYTE*) pvSalt,(int)ulSaltLen,iIter,ctx,(int)ulDKLen,(WSEC_BYTE *) pvDerivedKey))
	{
		return WSEC_SUCCESS;
	}
	else
	{
		return WSEC_ERR_CRPTO_LIB_FAIL;
	}
}

WSEC_UINT32 CAC_SYM_BlockSize(WSEC_UINT32 ulAlgId)
{
    if(0 != CAC_Alg2OpensslAlg_sym(ulAlgId))
        return EVP_CIPHER_block_size(CAC_Alg2OpensslAlg_sym(ulAlgId));
    else
        return 0;
}

WSEC_UINT32 CAC_SYM_IvLen(WSEC_UINT32 ulAlgId)
{
    if(0!= CAC_Alg2OpensslAlg_sym(ulAlgId))
        return EVP_CIPHER_iv_length(CAC_Alg2OpensslAlg_sym(ulAlgId));
    else
        return 0;
}

WSEC_UINT32 CAC_SYM_KeyLen(WSEC_UINT32 ulAlgId)
{
    if(0 != CAC_Alg2OpensslAlg_sym(ulAlgId))
        return EVP_CIPHER_key_length(CAC_Alg2OpensslAlg_sym(ulAlgId));
    else
        return 0;
}

WSEC_ERR_T CAC_Encrypt(WSEC_UINT32 ulAlgID,
                       const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,
                       const WSEC_VOID* pvIV, WSEC_SIZE_T ulIVLen,
                       const WSEC_VOID* pvPlainText, WSEC_SIZE_T ulPlainLen,
                       WSEC_VOID* pvCipherText, WSEC_SIZE_T* pulCLen)
{
    WSEC_BUFF stCipher = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    
    return_oper_if((*pulCLen < ulPlainLen), WSEC_LOG_E("Cipher buff len too small"), WSEC_ERR_INVALID_ARG);

    /* 根据OpenSSL实现的特点, 需将密文缓冲区放大 */
    WSEC_BUFF_ALLOC(stCipher, (ulPlainLen + CAC_OPENSSL_CRPYT_BUFF_OVER_SIZE));
    return_oper_if(!stCipher.pBuff, WSEC_LOG_E4MALLOC(stCipher.nLen), WSEC_ERR_MALLOC_FAIL);

    do
    {
        nErrCode = CAC_EncryptPri(ulAlgID, pvKey, ulKeyLen, pvIV, ulIVLen, pvPlainText, ulPlainLen, stCipher.pBuff, &stCipher.nLen);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("Above function return %u", nErrCode), oper_null);

        break_oper_if(*pulCLen < stCipher.nLen, WSEC_LOG_E2("%u bytes cipher-buff needed, but only %u provided.", stCipher.nLen, *pulCLen), nErrCode = WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH);

        break_oper_if(WSEC_MEMCPY(pvCipherText, *pulCLen, stCipher.pBuff, stCipher.nLen) != EOK, 
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
        *pulCLen = stCipher.nLen;
    }do_end;

    WSEC_BUFF_FREE(stCipher);
    return nErrCode;
}

WSEC_ERR_T CAC_EncryptPri(WSEC_UINT32 ulAlgID,
                       const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,
                       const WSEC_VOID* pvIV, WSEC_SIZE_T ulIVLen,
                       const WSEC_VOID* pvPlainText, WSEC_SIZE_T ulPlainLen,
                       WSEC_VOID* pvCipherText, WSEC_SIZE_T* pulCLen)
{
    const EVP_CIPHER* AlgType = 0 ;
	EVP_CIPHER_CTX *ctx ;
	int len = 0;
	WSEC_UINT32 blocksize = 0;
	int totallen = 0;

    AlgType = CAC_Alg2OpensslAlg_sym(ulAlgID);

    if(0 == AlgType)
    {
        WSEC_LOG_E("Wrong Encryption AlgID\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	if( !pvKey || !pvIV || !pvPlainText || !pvCipherText || !pulCLen)
	{
	    return WSEC_ERR_INVALID_ARG;
	}

	if(ulKeyLen != CAC_SYM_KeyLen(ulAlgID) || ulIVLen != CAC_SYM_IvLen(ulAlgID))
	{
	    WSEC_LOG_E("Wrong Encryption IV len or Keylen\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
	}
	
	blocksize = CAC_SYM_BlockSize( ulAlgID);

	if(*pulCLen < (blocksize ? ((ulPlainLen/blocksize+1)*blocksize):ulPlainLen))
	{
		WSEC_LOG_E("The Input ciphertext buffer len is not enough ,make sure it is at least ulPlainLen+blocksize \n");
		return WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH;
	}

	/* Create and initialise the context */
	if(!(ctx = EVP_CIPHER_CTX_new())) 
	{
        WSEC_LOG_E("CIPHER CTX creation failed. \n");
        return WSEC_ERR_CRPTO_LIB_FAIL;		
	}
	
	if(1 != EVP_EncryptInit_ex(ctx, AlgType, WSEC_NULL_PTR, (WSEC_BYTE*)pvKey, (WSEC_BYTE*)pvIV))
	{
		WSEC_LOG_E("Failed when the encryption ctx init\n");
		EVP_CIPHER_CTX_free(ctx);
		return WSEC_ERR_CRPTO_LIB_FAIL;
	}	
	len = ulPlainLen + CAC_SYM_BlockSize(ulAlgID);

	if(1 != EVP_EncryptUpdate(ctx, (WSEC_BYTE*)pvCipherText, &len, (WSEC_BYTE*)pvPlainText, (int)ulPlainLen))
	{
		WSEC_LOG_E("Failed when the encryption ctx Update \n");
		EVP_CIPHER_CTX_free(ctx);
		return WSEC_ERR_CRPTO_LIB_FAIL;
	}	

	totallen = len;

	
	/* Finalise the Encryption. Further plaintext bytes may be written at
	 * this stage.
	 */
	if(1 != EVP_EncryptFinal_ex(ctx, ((WSEC_BYTE*)pvCipherText)+ totallen, &len)) 
	{
		WSEC_LOG_E("Failed when the encryption ctx Final \n");
		EVP_CIPHER_CTX_free(ctx);
		return WSEC_ERR_CRPTO_LIB_FAIL;
	}	
	
	*pulCLen = (WSEC_SIZE_T)(totallen + len);
	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return WSEC_SUCCESS;
}

WSEC_ERR_T CAC_EncryptInit(WSEC_CRYPT_CTX* pCtx, WSEC_UINT32 ulAlgID,
                           const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,
                           const WSEC_VOID* pvIV, WSEC_SIZE_T ulIVLen)
{
    const EVP_CIPHER* AlgType = 0 ;
	EVP_CIPHER_CTX *ctx ;	


    AlgType = CAC_Alg2OpensslAlg_sym(ulAlgID);

    if(0 == AlgType)
    {
        WSEC_LOG_E("Wrong Encryption AlgID\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	if(!pCtx ||!pvKey ||!pvIV)
	{
	    return WSEC_ERR_INVALID_ARG;
	}
	if(ulKeyLen != CAC_SYM_KeyLen(ulAlgID) || ulIVLen != CAC_SYM_IvLen(ulAlgID))
	{
	    WSEC_LOG_E("Wrong Encryption IV len or Keylen. \n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
	}
	
	ctx = EVP_CIPHER_CTX_new();
	if( 0 == ctx)
	{
        WSEC_LOG_E( "CIPHER CTX creation failed\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;		
	}
   
    if(1 != EVP_EncryptInit_ex(ctx, AlgType, WSEC_NULL_PTR,(WSEC_BYTE*)pvKey, (WSEC_BYTE*)pvIV))
    {
        WSEC_LOG_E("Got failure from EVP_EncryptInit \n");
		EVP_CIPHER_CTX_free(ctx);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	*pCtx = ctx;
	return WSEC_SUCCESS;
}

WSEC_ERR_T CAC_EncryptUpdate(const WSEC_CRYPT_CTX ctx,
                             const WSEC_VOID* pvPlainText, WSEC_SIZE_T ulPlainLen,
                             WSEC_VOID* pvCipherText, WSEC_SIZE_T* pulCLen)
{
    WSEC_BUFF stCipher = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    return_oper_if((*pulCLen < ulPlainLen), WSEC_LOG_E("Cipher buff len too small"), WSEC_ERR_INVALID_ARG);

    /* 根据OpenSSL实现的特点, 需将密文缓冲区放大 */
    WSEC_BUFF_ALLOC(stCipher, (ulPlainLen + CAC_OPENSSL_CRPYT_BUFF_OVER_SIZE));
    return_oper_if(!stCipher.pBuff, WSEC_LOG_E4MALLOC(stCipher.nLen), WSEC_ERR_MALLOC_FAIL);

    do
    {
        nErrCode = CAC_EncryptUpdatePri(ctx, pvPlainText, ulPlainLen, stCipher.pBuff, &stCipher.nLen);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("Above function return %u", nErrCode), oper_null);

        break_oper_if(*pulCLen < stCipher.nLen, WSEC_LOG_E2("%u bytes cipher-buff needed, but only %u provided.", stCipher.nLen, *pulCLen), nErrCode = WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH);

        break_oper_if(WSEC_MEMCPY(pvCipherText, *pulCLen, stCipher.pBuff, stCipher.nLen) != EOK, 
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
        *pulCLen = stCipher.nLen;
    }do_end;

    WSEC_BUFF_FREE(stCipher);
    return nErrCode;
}

WSEC_ERR_T CAC_EncryptUpdatePri(const WSEC_CRYPT_CTX ctx,
                             const WSEC_VOID* pvPlainText, WSEC_SIZE_T ulPlainLen,
                             WSEC_VOID* pvCipherText, WSEC_SIZE_T* pulCLen)
{

    WSEC_UINT32 iRet = 0;  

	if(!ctx || !pvPlainText || !pvCipherText || !pulCLen)
	{
	    return WSEC_ERR_INVALID_ARG;
	}
	if(*pulCLen < ulPlainLen + EVP_CIPHER_block_size(((EVP_CIPHER_CTX*)ctx)->cipher))
	{
		WSEC_LOG_E("The Input ciphertext buffer len is not enough ,make sure it is at least ulPlainLen+blocksize \n");
		return WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH;
	}

    iRet = EVP_EncryptUpdate((EVP_CIPHER_CTX*)ctx,  
           (WSEC_BYTE *)pvCipherText, (int*)pulCLen,(WSEC_BYTE*)pvPlainText, (int)ulPlainLen);
    if (1 == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E("Got failure from EVP_EncryptUpdate \n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }

}

WSEC_ERR_T CAC_EncryptFinal(WSEC_CRYPT_CTX* pCtx, WSEC_VOID* pvCipherText, WSEC_SIZE_T* pulCLen)
{
    WSEC_BUFF stCipher = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

    /* 根据OpenSSL实现的特点, 需将密文缓冲区放大 */
    WSEC_BUFF_ALLOC(stCipher, (*pulCLen + CAC_OPENSSL_CRPYT_BUFF_OVER_SIZE));
    if(!stCipher.pBuff)
	{
		CAC_CipherFree((WSEC_CRYPT_CTX*)pCtx); /*释放内存*/
		return WSEC_ERR_MALLOC_FAIL;
	}
	
    do
    {
        nErrCode = CAC_EncryptFinalPri(pCtx, stCipher.pBuff, &stCipher.nLen);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("Above function return %u", nErrCode), oper_null);

        break_oper_if(*pulCLen < stCipher.nLen, WSEC_LOG_E2("%u bytes cipher-buff needed, but only %u provided.", stCipher.nLen, *pulCLen), nErrCode = WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH);

        break_oper_if(WSEC_MEMCPY(pvCipherText, *pulCLen, stCipher.pBuff, stCipher.nLen) != EOK, 
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
        *pulCLen = stCipher.nLen;
    }do_end;

	CAC_CipherFree((WSEC_CRYPT_CTX*)pCtx); /*释放内存*/

    WSEC_BUFF_FREE(stCipher);
    return nErrCode;
}

WSEC_ERR_T CAC_EncryptFinalPri(const WSEC_CRYPT_CTX* pCtx, WSEC_VOID* pvCipherText, WSEC_SIZE_T* pulCLen)
{
    WSEC_UINT32 iRet = 0;
	if(!pCtx || !pvCipherText || !pulCLen || !(*pCtx))
	{
	    return WSEC_ERR_INVALID_ARG;
	}
    iRet = EVP_EncryptFinal_ex((EVP_CIPHER_CTX*)(*pCtx), (WSEC_BYTE*)pvCipherText, (int *)pulCLen);
    if (1 == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E("Got failure from EVP_EncryptFinal.\n");
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
    WSEC_BUFF stPlain = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;

	if(!pulPLen || !pvCipherText || !pvPlainText )
		{return WSEC_ERR_INVALID_ARG;}
    
    return_oper_if((*pulPLen < ulCipherLen), WSEC_LOG_E("Plain buff len too small"), WSEC_ERR_INVALID_ARG);

    /* 根据OpenSSL实现的特点, 需将明文缓冲区放大 */
    WSEC_BUFF_ALLOC(stPlain, (ulCipherLen + CAC_OPENSSL_CRPYT_BUFF_OVER_SIZE));
    return_oper_if(!stPlain.pBuff, WSEC_LOG_E4MALLOC(stPlain.nLen), WSEC_ERR_MALLOC_FAIL);

    do
    {
        nErrCode = CAC_DecryptPri(ulAlgID, pvKey, ulKeyLen, pvIV, ulIVLen, pvCipherText, ulCipherLen, stPlain.pBuff, &stPlain.nLen);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("Above function return %u", nErrCode), oper_null);

        break_oper_if(*pulPLen < stPlain.nLen, WSEC_LOG_E2("%u bytes output-buff needed, but only %u provided.", stPlain.nLen, *pulPLen), nErrCode = WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH);

        break_oper_if(WSEC_MEMCPY(pvPlainText, *pulPLen, stPlain.pBuff, stPlain.nLen) != EOK, 
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
        *pulPLen = stPlain.nLen;
    }do_end;

    WSEC_BUFF_FREE(stPlain);
    return nErrCode;
}

WSEC_ERR_T CAC_DecryptPri(WSEC_UINT32 ulAlgID,
                       const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,
                       const WSEC_VOID* pvIV, WSEC_SIZE_T ulIVLen,
                        const WSEC_VOID* pvCipherText, WSEC_SIZE_T ulCipherLen,
                        WSEC_VOID* pvPlainText, WSEC_SIZE_T* pulPLen)
{
    const EVP_CIPHER* AlgType = 0 ;
	EVP_CIPHER_CTX *ctx ;
	int len = 0;
	int totallen = 0;

    AlgType = CAC_Alg2OpensslAlg_sym(ulAlgID);

    if(0 == AlgType)
    {
        WSEC_LOG_E("Wrong Encryption AlgID\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	if( !pvKey || !pvIV|| !pvCipherText || !pvPlainText || !pulPLen)
	{
	    return WSEC_ERR_INVALID_ARG;
	}

	if(ulKeyLen != CAC_SYM_KeyLen(ulAlgID) || ulIVLen != CAC_SYM_IvLen(ulAlgID))
	{
	    WSEC_LOG_E("Wrong Encryption IV len or Keylen\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
	}

	if(*pulPLen < ulCipherLen + CAC_SYM_BlockSize(ulAlgID) )
	{
		WSEC_LOG_E("The Input plaintext buffer len is not enough ,make sure it is at least ulCipherLen+blocksize \n");
		return WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH;
	}
	
	/* Create and initialise the context */
	if(!(ctx = EVP_CIPHER_CTX_new())) 
	{
        WSEC_LOG_E("CIPHER CTX creation failed. \n");
        return WSEC_ERR_CRPTO_LIB_FAIL;		
	}

	len = CAC_SYM_BlockSize(ulAlgID) + ulCipherLen;
	
	if(1 != EVP_DecryptInit_ex(ctx, AlgType, WSEC_NULL_PTR, (WSEC_BYTE*)pvKey, (WSEC_BYTE*)pvIV))
	{
		WSEC_LOG_E("Failed when the Decryption ctx init\n");
		EVP_CIPHER_CTX_free(ctx);
		return WSEC_ERR_CRPTO_LIB_FAIL;
	}	

	if(1 != EVP_DecryptUpdate(ctx, (WSEC_BYTE*)pvPlainText, &len, (WSEC_BYTE*)pvCipherText, (int)ulCipherLen))
	{
		WSEC_LOG_E("Failed when the Decryption ctx Update \n");
		EVP_CIPHER_CTX_free(ctx);
		return WSEC_ERR_CRPTO_LIB_FAIL;
	}	

	totallen = len;
		
	/* Finalise the Encryption. Further plaintext bytes may be written at
	 * this stage.
	 */
	if(1 != EVP_DecryptFinal_ex(ctx,((WSEC_BYTE*)pvPlainText) + totallen, &len)) 
	{
		WSEC_LOG_E("Failed when the Decryption ctx Final \n");
		EVP_CIPHER_CTX_free(ctx);
		return WSEC_ERR_CRPTO_LIB_FAIL;
	}	
	
	*pulPLen= (WSEC_SIZE_T)(totallen + len);
	
	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return WSEC_SUCCESS;
}

WSEC_ERR_T CAC_DecryptInit(WSEC_CRYPT_CTX* pCtx, WSEC_UINT32 ulAlgID,
                           const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,
                           const WSEC_VOID* pvIV, WSEC_SIZE_T ulIVLen)
{
    const EVP_CIPHER* AlgType = 0 ;
	EVP_CIPHER_CTX *ctx ;	

    AlgType = CAC_Alg2OpensslAlg_sym(ulAlgID);
	if(!pCtx || !pvKey || !pvIV )
	{
	    return WSEC_ERR_INVALID_ARG;
	}
    if(0 == AlgType)
    {
        WSEC_LOG_E("Wrong Encryption AlgID\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	
	if(ulKeyLen != CAC_SYM_KeyLen(ulAlgID) || ulIVLen != CAC_SYM_IvLen(ulAlgID))
	{
	    WSEC_LOG_E("Wrong Encryption IV len or Keylen\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
	}
	
	ctx = EVP_CIPHER_CTX_new();
	if( 0 == ctx)
	{
        WSEC_LOG_E( "CIPHER CTX creation failed\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;		
	}
   
    if(1!= EVP_DecryptInit_ex(ctx, AlgType, WSEC_NULL_PTR,(WSEC_BYTE *)pvKey, (WSEC_BYTE *)pvIV))
    {
        WSEC_LOG_E("Got failure from EVP_EncryptInit \n");
		EVP_CIPHER_CTX_free(ctx);
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
	*pCtx = ctx;
	return WSEC_SUCCESS;
}

WSEC_ERR_T CAC_DecryptUpdate(const WSEC_CRYPT_CTX ctx,
                             const WSEC_VOID* pvCipherText, WSEC_SIZE_T ulCipherLen,
                             WSEC_VOID* pvPlainText, WSEC_SIZE_T* pulPLen)
{
    WSEC_BUFF stPlain = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
	
	if(!pulPLen || !pvCipherText || !pvPlainText )
		{return WSEC_ERR_INVALID_ARG;}
    
    return_oper_if((*pulPLen < ulCipherLen), WSEC_LOG_E("Plain buff len too small"), WSEC_ERR_INVALID_ARG);

    /* 根据OpenSSL实现的特点, 需将明文缓冲区放大 */
    WSEC_BUFF_ALLOC(stPlain, (ulCipherLen + CAC_OPENSSL_CRPYT_BUFF_OVER_SIZE));
    return_oper_if(!stPlain.pBuff, WSEC_LOG_E4MALLOC(stPlain.nLen), WSEC_ERR_MALLOC_FAIL);

    do
    {
        nErrCode = CAC_DecryptUpdatePri(ctx, pvCipherText, ulCipherLen, stPlain.pBuff, &stPlain.nLen);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("Above function return %u", nErrCode), oper_null);

        break_oper_if(*pulPLen < stPlain.nLen, WSEC_LOG_E2("%u bytes output-buff needed, but only %u provided.", stPlain.nLen, *pulPLen), nErrCode = WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH);

        break_oper_if(WSEC_MEMCPY(pvPlainText, *pulPLen, stPlain.pBuff, stPlain.nLen) != EOK, 
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
        *pulPLen = stPlain.nLen;
    }do_end;
	
    WSEC_BUFF_FREE(stPlain);
    return nErrCode;
}

WSEC_ERR_T CAC_DecryptUpdatePri(const WSEC_CRYPT_CTX ctx,
                             const WSEC_VOID* pvCipherText, WSEC_SIZE_T ulCipherLen,
                             WSEC_VOID* pvPlainText, WSEC_SIZE_T* pulPLen)
{

    WSEC_UINT32 iRet = 0;

	if(!ctx || !pvCipherText || !pvPlainText || !pulPLen)
	{
	    return WSEC_ERR_INVALID_ARG;
	}

	if(*pulPLen < ulCipherLen + EVP_CIPHER_block_size(((EVP_CIPHER_CTX*)ctx)->cipher))
	{
		WSEC_LOG_E("The Input plaintext buffer len is not enough ,make sure it is at least ulCipherLen+blocksize \n");
		return WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH;
	}

    iRet = EVP_DecryptUpdate((EVP_CIPHER_CTX*)ctx,  
           (WSEC_BYTE *)pvPlainText, (int *)pulPLen, (WSEC_BYTE *)pvCipherText, (int)ulCipherLen);
    if (1 == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E("Got failure from EVP_EncryptUpdate \n");		
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }

}

WSEC_ERR_T CAC_DecryptFinal(WSEC_CRYPT_CTX* pCtx, WSEC_VOID* pvPlainText, WSEC_SIZE_T* pulPLen)
{
    WSEC_BUFF stPlain = {0};
    WSEC_ERR_T nErrCode = WSEC_SUCCESS;
    
    /* 根据OpenSSL实现的特点, 需将明文缓冲区放大 */
    WSEC_BUFF_ALLOC(stPlain, (*pulPLen + CAC_OPENSSL_CRPYT_BUFF_OVER_SIZE));
    if(!stPlain.pBuff)
	{
		CAC_CipherFree((WSEC_CRYPT_CTX*)pCtx); /*释放内存*/
		return WSEC_ERR_MALLOC_FAIL;
	}

    do
    {
        nErrCode = CAC_DecryptFinalPri(pCtx, stPlain.pBuff, &stPlain.nLen);
        break_oper_if(nErrCode != WSEC_SUCCESS, WSEC_LOG_E1("Above function return %u", nErrCode), oper_null);

        break_oper_if(*pulPLen < stPlain.nLen, WSEC_LOG_E2("%u bytes output-buff needed, but only %u provided.", stPlain.nLen, *pulPLen), nErrCode = WSEC_ERR_OUTPUT_BUFF_NOT_ENOUGH);

        break_oper_if(WSEC_MEMCPY(pvPlainText, *pulPLen, stPlain.pBuff, stPlain.nLen) != EOK, 
                      WSEC_LOG_E4MEMCPY, nErrCode = WSEC_ERR_MEMCPY_FAIL);
        *pulPLen = stPlain.nLen;
    }do_end;

	CAC_CipherFree((WSEC_CRYPT_CTX*)pCtx); /*释放内存*/

    WSEC_BUFF_FREE(stPlain);
    return nErrCode;
}

WSEC_ERR_T CAC_DecryptFinalPri(const WSEC_CRYPT_CTX* pCtx, WSEC_VOID* pvPlainText, WSEC_SIZE_T* pulPLen)
{
    WSEC_UINT32 iRet = 0;

	if(!pCtx || !pvPlainText || !pulPLen || !(*pCtx))
	{
	    return WSEC_ERR_INVALID_ARG;
	}

    iRet = EVP_DecryptFinal_ex((EVP_CIPHER_CTX*)(*pCtx), (WSEC_BYTE*)pvPlainText, (int *)pulPLen);
    if (1 == iRet)
    {
        return WSEC_SUCCESS;
    }
    else
    {
        WSEC_LOG_E("Got failure from EVP_EncryptFinal.\n");
        return WSEC_ERR_CRPTO_LIB_FAIL;
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* WSEC_COMPILE_CAC_OPENSSL */


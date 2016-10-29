/******************************************************************************

                 Copyright (C), 2001-2014, Huawei Tech. Co., Ltd.

 ******************************************************************************
  FileName      : xcapbase.h
  Version       : 1.0.0
  Author        :  ,l00171031
  Date          : 2014-6-14
  Description   : WSEC基础头文件，基本类型定义
  Function List :


  History   :
        1.Date            : 2014-6-14
          Author          :  ,l00171031
          Modification    :　创建文件

******************************************************************************/
#ifndef _WSEC_BASE_H_
#define _WSEC_BASE_H_

#include "wsec_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

typedef WSEC_VOID* WSEC_CRYPT_CTX;


#define WSEC_MIN(a,b) ((a) < (b) ? (a) : (b))

#define WSEC_CLEANSE_DATA(a,b)  if(a != 0 ){WSEC_MEMSET(a,b,0,b);}

#define WSEC_ALGNAME_DES_EDE3_ECB       "DES_EDE3_ECB"
#define WSEC_ALGNAME_DES_EDE3_CBC       "DES_EDE3_CBC"
#define WSEC_ALGNAME_AES128_ECB         "AES128_ECB"
#define WSEC_ALGNAME_AES128_CBC         "AES128_CBC"
#define WSEC_ALGNAME_AES256_ECB         "AES256_ECB"
#define WSEC_ALGNAME_AES256_CBC         "AES256_CBC"
#define WSEC_ALGNAME_MD5                "MD5"
#define WSEC_ALGNAME_SHA1               "SHA1"
#define WSEC_ALGNAME_SHA224             "SHA224"
#define WSEC_ALGNAME_SHA256             "SHA256"
#define WSEC_ALGNAME_SHA384             "SHA384"
#define WSEC_ALGNAME_SHA512             "SHA512"
#define WSEC_ALGNAME_HMAC_MD5           "HMAC_MD5"
#define WSEC_ALGNAME_HMAC_SHA1          "HMAC_SHA1"
#define WSEC_ALGNAME_HMAC_SHA224        "HMAC_SHA224"
#define WSEC_ALGNAME_HMAC_SHA256        "HMAC_SHA256"
#define WSEC_ALGNAME_HMAC_SHA384        "HMAC_SHA384"
#define WSEC_ALGNAME_HMAC_SHA512        "HMAC_SHA512"
#define WSEC_ALGNAME_PBKDF2_HMAC_MD5    "PBKDF2_HMAC_MD5"
#define WSEC_ALGNAME_PBKDF2_HMAC_SHA1   "PBKDF2_HMAC_SHA1"
#define WSEC_ALGNAME_PBKDF2_HMAC_SHA224 "PBKDF2_HMAC_SHA224"
#define WSEC_ALGNAME_PBKDF2_HMAC_SHA256 "PBKDF2_HMAC_SHA256"
#define WSEC_ALGNAME_PBKDF2_HMAC_SHA384 "PBKDF2_HMAC_SHA384"
#define WSEC_ALGNAME_PBKDF2_HMAC_SHA512 "PBKDF2_HMAC_SHA512"
#define WSEC_ALGNAME_PKCS8              "PKCS8"

typedef enum
{
    WSEC_ALGTYPE_UNKNOWN,
    WSEC_ALGTYPE_SYM,
    WSEC_ALGTYPE_HMAC,
    WSEC_ALGTYPE_DIGEST,
    WSEC_ALGTYPE_PBKDF,
}WSEC_ALGTYPE_E;

#define WSEC_IS_ENCRYPT_ALGID(id) (CAC_AlgId2Type(id) == WSEC_ALGTYPE_SYM)
#define WSEC_IS_HASH_ALGID(id)    (CAC_AlgId2Type(id) == WSEC_ALGTYPE_DIGEST)
#define WSEC_IS_HMAC_ALGID(id)    (CAC_AlgId2Type(id) == WSEC_ALGTYPE_HMAC)
#define WSEC_IS_PBKDF_ALGID(id)   (CAC_AlgId2Type(id) == WSEC_ALGTYPE_PBKDF)

/* function pointer */
typedef WSEC_VOID (*CAC_FP_ProcAlg)(WSEC_UINT32 ulAlgId, const WSEC_CHAR* pszAlgName, WSEC_VOID* pReserved);

/*Fuction list of Encryption / Decryption /hmac /digest /hmac */
WSEC_ERR_T CAC_LibraryInit(const WSEC_VOID* pvEntropyBuf, WSEC_SIZE_T ulLen);
WSEC_ERR_T CAC_Random(WSEC_VOID* pvBuf, WSEC_SIZE_T ulLen);
WSEC_ERR_T CAC_Digest(WSEC_UINT32 ulAlgID,
                      const WSEC_VOID* pvData, WSEC_SIZE_T ulDataLen,
                      WSEC_VOID* pvDigest, WSEC_SIZE_T* pulDigestLen);
WSEC_ERR_T CAC_DigestInit(WSEC_CRYPT_CTX* pCtx,  WSEC_UINT32 ulAlgID);
WSEC_ERR_T CAC_DigestUpdate(const WSEC_CRYPT_CTX pCtx, const WSEC_VOID* pvData, WSEC_SIZE_T ulDataLen);
WSEC_ERR_T CAC_DigestFinal(WSEC_CRYPT_CTX* pCtx, WSEC_VOID* pvDigest, WSEC_SIZE_T* pulLen);
WSEC_VOID CAC_DigestFree(WSEC_CRYPT_CTX* pCtx);
WSEC_VOID CAC_HmacFree(WSEC_CRYPT_CTX* pCtx);
WSEC_VOID CAC_CipherFree(WSEC_CRYPT_CTX* pCtx);

WSEC_ERR_T CAC_Hmac(WSEC_UINT32 ulAlgID,
                    const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,   
                    const WSEC_VOID* pvData, WSEC_SIZE_T ulDataLen,
                    WSEC_VOID* pvHmac, WSEC_SIZE_T* pulHmacLen);
WSEC_ERR_T CAC_HmacInit(WSEC_CRYPT_CTX* pCtx,
                        WSEC_UINT32 ulAlgID,
                        const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen);
WSEC_ERR_T CAC_HmacUpdate(const WSEC_CRYPT_CTX pCtx, const WSEC_VOID* pvData, WSEC_SIZE_T ulDataLen);
WSEC_ERR_T CAC_HmacFinal(WSEC_CRYPT_CTX* pCtx, WSEC_VOID* pvHmac, WSEC_SIZE_T* pulHmacLen);
WSEC_ERR_T CAC_Pbkdf2(WSEC_UINT32 ulKDFAlg,
                      const WSEC_VOID* pvPassword, WSEC_SIZE_T ulPwdLen,
                      const WSEC_VOID* pvSalt, WSEC_SIZE_T ulSaltLen,
                      WSEC_INT32 iIter, 
                      WSEC_SIZE_T ulDKLen, WSEC_VOID* pvDerivedKey);
WSEC_ERR_T CAC_Encrypt(WSEC_UINT32 ulAlgID,
                       const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,
                       const WSEC_VOID* pvIV, WSEC_SIZE_T ulIVLen,
                       const WSEC_VOID* pvPlainText, WSEC_SIZE_T ulPlainLen,
                       WSEC_VOID* pvCipherText, WSEC_SIZE_T* pulCLen);
WSEC_ERR_T CAC_EncryptInit(WSEC_CRYPT_CTX* pCtx, WSEC_UINT32 ulAlgID,
                           const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,
                           const WSEC_VOID* pvIV, WSEC_SIZE_T ulIVLen);
WSEC_ERR_T CAC_EncryptUpdate(const WSEC_CRYPT_CTX ctx,
                             const WSEC_VOID* pvPlainText, WSEC_SIZE_T ulPlainLen,
                             WSEC_VOID* pvCipherText, WSEC_SIZE_T* pulCLen);
WSEC_ERR_T CAC_EncryptFinal(WSEC_CRYPT_CTX* pCtx, WSEC_VOID* pvCipherText, WSEC_SIZE_T* pulCLen);
WSEC_ERR_T CAC_Decrypt(WSEC_UINT32 ulAlgID,
                        const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,
                        const WSEC_VOID* pvIV, WSEC_SIZE_T ulIVLen,
                        const WSEC_VOID* pvCipherText, WSEC_SIZE_T ulCipherLen,
                        WSEC_VOID* pvPlainText, WSEC_SIZE_T* pulPLen);
WSEC_ERR_T CAC_DecryptInit(WSEC_CRYPT_CTX* pCtx, WSEC_UINT32 ulAlgID,
                           const WSEC_VOID* pvKey, WSEC_SIZE_T ulKeyLen,
                           const WSEC_VOID* pvIV, WSEC_SIZE_T ulIVLen);
WSEC_ERR_T CAC_DecryptUpdate(const WSEC_CRYPT_CTX ctx,
                             const WSEC_VOID* pvCipherText, WSEC_SIZE_T ulCipherLen,
                             WSEC_VOID* pvPlainText, WSEC_SIZE_T* pulPLen);
WSEC_ERR_T CAC_DecryptFinal(WSEC_CRYPT_CTX* pCtx, WSEC_VOID* pvPlainText, WSEC_SIZE_T* pulPLen);

WSEC_UINT32 CAC_SYM_BlockSize(WSEC_UINT32 ulAlgId);
WSEC_UINT32 CAC_SYM_IvLen(WSEC_UINT32 ulAlgId);
WSEC_UINT32 CAC_SYM_KeyLen(WSEC_UINT32 ulAlgId);
WSEC_UINT32 CAC_HMAC_Size(WSEC_UINT32 ulAlgId);

const WSEC_CHAR* CAC_AlgId2Name(WSEC_UINT32 ulAlgID);
WSEC_ERR_T CAC_GetAlgList(CAC_FP_ProcAlg pfProcAlg, WSEC_VOID* pReserved);
WSEC_ALGTYPE_E CAC_AlgId2Type(WSEC_UINT32 ulAlgID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*_WSEC_BASE_H_*/


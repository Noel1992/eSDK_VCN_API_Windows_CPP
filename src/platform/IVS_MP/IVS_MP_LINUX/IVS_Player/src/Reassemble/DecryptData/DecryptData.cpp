#include "DecryptData.h"
#ifdef WIN32
#include "..\..\inc\common\log\ivs_log.h"
#else
#include "ivs_log.h"
#endif 
#include "ivs_error.h"
#include <string>
#include <string.h>

/*****************************************************************************
 函数名称：DecryptDataXOR
 功能描述：采用XOR方法解密数据流
 输入参数：
    @pSecretKey：         密钥
    @ulSecretKeyLen：     密钥长度
    @pEncryptData：       数据流
    @ulEncryptDataLen :   数据长度
 输出参数：NA
 返 回 值：void
*****************************************************************************/


int DecryptDataXOR(const char* pSecretKey, const unsigned long ulSecretKeyLen, char* pEncryptData,
                                const unsigned long ulEncryptDataLen)
{
    if ((NULL == pSecretKey)
        || (0 == ulSecretKeyLen)
        || (NULL == pEncryptData)
        || (0 == ulEncryptDataLen))
    {
        IVS_LOG(IVS_LOG_ERR, "XORDecryptData", "Splitter decrypt data error, parameter is invalid.");
        return IVS_PARA_INVALID;
    }

	unsigned long j = 0;
	for (unsigned long i = 0; i < ulEncryptDataLen; i++)
	{
		if (j == ulSecretKeyLen)
		{
			j = 0;
		}
		pEncryptData[i] ^= pSecretKey[j++];
	}

    return IVS_SUCCEED;
}

/*****************************************************************************
 函数名称：DecryptDataAES
 功能描述：采用AES方法解密数据流
 输入参数：
    @pSecretKey：         密钥
    @ulIKeyLen：          前24位为ikey
    @ulcbufLen：          后16位为cbuf
    @pEncryptData :       数据流
    @ulEncryptDataLen :   数据长度
 输出参数：NA
 返 回 值：void
*****************************************************************************/
int DecryptDataAES( const char* pSecretKey, unsigned long ulIKeyLen, unsigned long ulcbufLen,
                                 char* pEncryptData,
                                 unsigned long ulEncryptDataLen,unsigned char * m_pDecryptDataTmpBuf,double selective_enc_ratio,unsigned int uiEncryptExLen)
{
    //分解加密存储密钥，前24位为ikey，后16位为cbuf
    unsigned char ikey[33] = {0};

    //memcpy_s(ikey,ulIKeyLen, pSecretKey, ulIKeyLen);
	memcpy(ikey,pSecretKey,ulIKeyLen);
    unsigned char cbuf[17] = {0};

    //memcpy_s(cbuf, ulcbufLen, pSecretKey + ulIKeyLen, ulcbufLen);
	memcpy(cbuf,pSecretKey + ulIKeyLen,ulcbufLen);

    if (NULL == pEncryptData)
    {
        IVS_LOG(IVS_LOG_ERR, "AESDecryptData", "Decoder buffer is NULL");
        return IVS_PLAYER_RET_ALLOC_MEM_ERROR;
    }

    if (NULL == m_pDecryptDataTmpBuf)
    {
        IVS_LOG(IVS_LOG_ERR, "AESDecryptData", "Malloc memory for Decrypt buffer failed.");
        return IVS_PLAYER_RET_ALLOC_MEM_ERROR;
    }

	//memset(m_pDecryptDataBuf,0,DERYPT_BUFF_LENTH_HD);
    //AES256解密
	AesCtrSelectiveDecry((unsigned char*)(pEncryptData),(int)ulEncryptDataLen,m_pDecryptDataTmpBuf,cbuf, ikey, 256, selective_enc_ratio);
    memcpy(pEncryptData, m_pDecryptDataTmpBuf, ulEncryptDataLen - uiEncryptExLen);

    return IVS_SUCCEED;
}

int AesCtrEncry( const unsigned char * ibuf, int len, unsigned char * obuf, 
	unsigned char * cbuf, const unsigned char * key, int nbits, 
	double selective_enc_ratio)
{
	if (NULL == ibuf || NULL == obuf || NULL == cbuf || NULL == key)
	{
		return IVS_PARA_INVALID;
	}
	AesCtrSelectiveEncry(ibuf,len,obuf,cbuf,key,nbits,selective_enc_ratio);
	return IVS_SUCCEED;
}

int DecryptDataGetAesKey(aes_context *ctx, const unsigned char* key, int nbits)
{
	GetAesKey(ctx,key,nbits);
	return IVS_SUCCEED;
}

int DecryptDataAesALLValue(unsigned char * pszInBuff,  unsigned long ulLen, 
	unsigned char * pszOutBuff,aes_context *ctx,unsigned int uiEncryptExLen)
{
	//需要加上扩展位的长度
	AesALLDataDecrypt(pszInBuff,(int)ulLen,pszOutBuff,ctx);
	if (ulLen > uiEncryptExLen)
	{
		memcpy(pszInBuff, pszOutBuff, (unsigned int)ulLen - uiEncryptExLen);
		return IVS_SUCCEED;
	}

	return IVS_FAIL;
}



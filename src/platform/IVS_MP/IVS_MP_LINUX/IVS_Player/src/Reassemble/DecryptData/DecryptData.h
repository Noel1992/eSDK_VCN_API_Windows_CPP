/*******************************************************************************
//  版权所有    华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  DecryptData.h
//  文件说明:
//  版    本:   IVS V100R002C01
//  作    者:   chenxianxiao/00206592
//  创建日期:   2012/11/08
//  修改标识：
//  修改说明：
*******************************************************************************/

#ifndef __DECRYPT_DATA_H__
#define __DECRYPT_DATA_H__

#include "Aes256_ctr_partial.h"

int DecryptDataXOR(const char* pSecretKey, const unsigned long ulSecretKeyLen, char* pEncryptData,
		const unsigned long ulEncryptDataLen);


/*****************************************************************************************
 * 部分码流AES256解密
 * pEncryptData : 要解密的数据流, 以字节为单位 (如果不满一个字节, 可以在后面补足; 解密后删除补足的字节即可)
 * ulEncryptDataLen  : 要解密的数据流长度 (字节数)
 * m_pDecryptDataTmpBuf : 解密后的数据流, 以字节为单位 (如果解密前补足过, 解密后可删除补足的部分)
 * pSecretKey   : 解密密钥, 可自主选定 (字节数可以是: 16, 24, 32)
 * ulIKeyLen	: 解密密钥的长度
 * selective_enc_ratio: 解密的数据比例, 变化范围为(0, 1). 
 *                      例如: 0.5表示每两个字节中解密一个字节; 0.1表示每10个字节中解密一个字节
 ****************************************************************************************/
int DecryptDataAES( const char* pSecretKey, unsigned long ulIKeyLen, unsigned long ulcbufLen, char* pEncryptData,
		unsigned long ulEncryptDataLen,unsigned char * m_pDecryptDataTmpBuf,double selective_enc_ratio,unsigned int uiEncryptExLen);


/*****************************************************************************************
 * 部分码流AES256加密
 * ibuf : 要加密的数据流, 以字节为单位 (如果不满一个字节, 可以在后面补足; 加密后删除补足的字节即可)
 * len  : 要加密的数据流长度 (字节数)
 * obuf : 加密后的数据流, 以字节为单位 (如果加密前补足过, 加密后可删除补足的部分)
 * cbuf : 加密用的初始化向量，可自主选定 (16个字节)
 * key  : 加密密钥, 可自主选定 (字节数可以是: 16, 24, 32)
 * nbits: 加密密钥的比特长度, 与密钥key的字节数匹配 (比特数可以是: 128, 192, 256)
 * selective_enc_ratio: 加密的数据比例, 变化范围为(0, 1). 
 *                      例如: 0.5表示每两个字节中加密一个字节; 0.1表示每10个字节中加密一个字节
 ****************************************************************************************/
int AesCtrEncry( const unsigned char * ibuf, int len, unsigned char * obuf, 
	unsigned char * cbuf, const unsigned char * key, int nbits, 
	double selective_enc_ratio);

/*****************************************************************************************
 * 获取AES解密子密钥
 * ctx  : 子密钥
 * key  : 加密密钥
 * nbits: pszKey的长度, 必须为16, 24, 32中之一
 ****************************************************************************************/
int DecryptDataGetAesKey(aes_context *ctx, const unsigned char* key, int nbits); 

 /*****************************************************
     * AES解密
     * pszInBuff:  密文
     * pszOutBuff: 明文
     * iLen:       数据长度
     *             首先, 必须为16的整数倍
     *             另外, 因为是对称加密算法, 输入输出长度是一样
	 * ctx:        子密钥
	 * uiEncryptExLen: 补足的字节数
     * 返回值:     数据长度不是16的整数倍时, 会失败
     *****************************************************/
int DecryptDataAesALLValue(unsigned char * pszInBuff, unsigned long ulLen, 
	unsigned char * pszOutBuff,aes_context *ctx,unsigned int uiEncryptExLen);

#endif/*__DECRYPT_DATA_H__*/

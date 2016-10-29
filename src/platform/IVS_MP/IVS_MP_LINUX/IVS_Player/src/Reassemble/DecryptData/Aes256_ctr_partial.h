/****************************************************************************************
filename    :    ivs_aes256_ctr_partial.h
author      :    lianshiguo 00174213
created     :    2012/11/22 (直接用以前版本的, 兼容C30, C60)
description :    AES-CTR加解密, 部分码流AES256加解密
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/22 (从之前版本上除去测试代码)
*****************************************************************************************/

#ifndef __AES256_CTR_PARTIAL_H__
#define __AES256_CTR_PARTIAL_H__


typedef unsigned char u_int_8;
typedef unsigned long int u_int_32;

typedef struct
{
	u_int_32 erk[64];     /* encryption round keys */
	u_int_32 drk[64];     /* decryption round keys */
	int      nr;          /* number of rounds */
} aes_context;

/*****************************************************************************************
 * AES-CTR加解密
 * ibuf : 要加/解密的数据流, 以字节为单位 (如果不满一个字节, 可以在后面补足; 加/解密后删除补足的字节即可)
 * len  : 要加/解密的数据流长度 (字节数)
 * obuf : 加/解密后的数据流, 以字节为单位 (如果加/解密前补足过, 加/解密后可删除补足的部分)
 * cbuf : 加/解密用的初始化向量，可自主选定 (16个字节)
 * key  : 加/解密密钥, 可自主选定 (字节数可以是: 16, 24, 32)
 * nbits: 加/解密密钥的比特长度, 与密钥key的字节数匹配 (比特数可以是: 128, 192, 256)
 ****************************************************************************************/
void  AesCtrCry(
    const unsigned char * ibuf, int len, unsigned char * obuf, 
    unsigned char * cbuf, const unsigned char * key, int nbits);

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
void  AesCtrSelectiveEncry(
    const unsigned char * ibuf, int len, unsigned char * obuf, 
    unsigned char * cbuf, const unsigned char * key, int nbits, 
    double selective_enc_ratio);

/*****************************************************************************************
 * 部分码流AES256解密
 * ibuf : 要解密的数据流, 以字节为单位 (如果不满一个字节, 可以在后面补足; 解密后删除补足的字节即可)
 * len  : 要解密的数据流长度 (字节数)
 * obuf : 解密后的数据流, 以字节为单位 (如果解密前补足过, 解密后可删除补足的部分)
 * cbuf : 解密用的初始化向量，可自主选定 (16个字节)
 * key  : 解密密钥, 可自主选定 (字节数可以是: 16, 24, 32)
 * nbits: 解密密钥的比特长度, 与密钥key的字节数匹配 (比特数可以是: 128, 192, 256)
 * selective_enc_ratio: 解密的数据比例, 变化范围为(0, 1). 
 *                      例如: 0.5表示每两个字节中解密一个字节; 0.1表示每10个字节中解密一个字节
 ****************************************************************************************/
void  AesCtrSelectiveDecry(
    const unsigned char * ibuf, int len, unsigned char * obuf, 
    unsigned char * cbuf, const unsigned char * key, int nbits, 
    double selective_enc_ratio);

/*****************************************************************************************
 * 获取AES解密子密钥
 * ctx  : 子密钥
 * key  : 加密密钥
 * nbits: pszKey的长度, 必须为16, 24, 32中之一
 ****************************************************************************************/
void GetAesKey(aes_context *ctx, const u_int_8 *key, int nbits);

 /*****************************************************
     * AES解密
     * pszInBuff:  密文
     * pszOutBuff: 明文
     * iLen:       数据长度
     *             首先, 必须为16的整数倍
     *             另外, 因为是对称加密算法, 输入输出长度是一样
	 * ctx:        子密钥
     * 返回值:     数据长度不是16的整数倍时, 会失败
     *****************************************************/
void AesALLDataDecrypt(unsigned char * pszInBuff, int iLen, 
	unsigned char * pszOutBuff,aes_context *ctx);

#endif // __AES256_CTR_PARTIAL_H__

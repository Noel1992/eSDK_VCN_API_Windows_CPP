/*******************************************************************************
//  ��Ȩ����    ��Ϊ�������޹�˾
//  �� �� ����  IVS_Player
//  �� �� ����  DecryptData.h
//  �ļ�˵��:
//  ��    ��:   IVS V100R002C01
//  ��    ��:   chenxianxiao/00206592
//  ��������:   2012/11/08
//  �޸ı�ʶ��
//  �޸�˵����
*******************************************************************************/

#ifndef __DECRYPT_DATA_H__
#define __DECRYPT_DATA_H__

#include "Aes256_ctr_partial.h"

int DecryptDataXOR(const char* pSecretKey, const unsigned long ulSecretKeyLen, char* pEncryptData,
		const unsigned long ulEncryptDataLen);


/*****************************************************************************************
 * ��������AES256����
 * pEncryptData : Ҫ���ܵ�������, ���ֽ�Ϊ��λ (�������һ���ֽ�, �����ں��油��; ���ܺ�ɾ��������ֽڼ���)
 * ulEncryptDataLen  : Ҫ���ܵ����������� (�ֽ���)
 * m_pDecryptDataTmpBuf : ���ܺ��������, ���ֽ�Ϊ��λ (�������ǰ�����, ���ܺ��ɾ������Ĳ���)
 * pSecretKey   : ������Կ, ������ѡ�� (�ֽ���������: 16, 24, 32)
 * ulIKeyLen	: ������Կ�ĳ���
 * selective_enc_ratio: ���ܵ����ݱ���, �仯��ΧΪ(0, 1). 
 *                      ����: 0.5��ʾÿ�����ֽ��н���һ���ֽ�; 0.1��ʾÿ10���ֽ��н���һ���ֽ�
 ****************************************************************************************/
int DecryptDataAES( const char* pSecretKey, unsigned long ulIKeyLen, unsigned long ulcbufLen, char* pEncryptData,
		unsigned long ulEncryptDataLen,unsigned char * m_pDecryptDataTmpBuf,double selective_enc_ratio,unsigned int uiEncryptExLen);


/*****************************************************************************************
 * ��������AES256����
 * ibuf : Ҫ���ܵ�������, ���ֽ�Ϊ��λ (�������һ���ֽ�, �����ں��油��; ���ܺ�ɾ��������ֽڼ���)
 * len  : Ҫ���ܵ����������� (�ֽ���)
 * obuf : ���ܺ��������, ���ֽ�Ϊ��λ (�������ǰ�����, ���ܺ��ɾ������Ĳ���)
 * cbuf : �����õĳ�ʼ��������������ѡ�� (16���ֽ�)
 * key  : ������Կ, ������ѡ�� (�ֽ���������: 16, 24, 32)
 * nbits: ������Կ�ı��س���, ����Կkey���ֽ���ƥ�� (������������: 128, 192, 256)
 * selective_enc_ratio: ���ܵ����ݱ���, �仯��ΧΪ(0, 1). 
 *                      ����: 0.5��ʾÿ�����ֽ��м���һ���ֽ�; 0.1��ʾÿ10���ֽ��м���һ���ֽ�
 ****************************************************************************************/
int AesCtrEncry( const unsigned char * ibuf, int len, unsigned char * obuf, 
	unsigned char * cbuf, const unsigned char * key, int nbits, 
	double selective_enc_ratio);

/*****************************************************************************************
 * ��ȡAES��������Կ
 * ctx  : ����Կ
 * key  : ������Կ
 * nbits: pszKey�ĳ���, ����Ϊ16, 24, 32��֮һ
 ****************************************************************************************/
int DecryptDataGetAesKey(aes_context *ctx, const unsigned char* key, int nbits); 

 /*****************************************************
     * AES����
     * pszInBuff:  ����
     * pszOutBuff: ����
     * iLen:       ���ݳ���
     *             ����, ����Ϊ16��������
     *             ����, ��Ϊ�ǶԳƼ����㷨, �������������һ��
	 * ctx:        ����Կ
	 * uiEncryptExLen: ������ֽ���
     * ����ֵ:     ���ݳ��Ȳ���16��������ʱ, ��ʧ��
     *****************************************************/
int DecryptDataAesALLValue(unsigned char * pszInBuff, unsigned long ulLen, 
	unsigned char * pszOutBuff,aes_context *ctx,unsigned int uiEncryptExLen);

#endif/*__DECRYPT_DATA_H__*/

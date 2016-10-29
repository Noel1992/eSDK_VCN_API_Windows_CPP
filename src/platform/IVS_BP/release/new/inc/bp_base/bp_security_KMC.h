
/********************************************************************
filename    :    ivs_security_KMC.h
author      :    yWX150774
created     :    2012/11/06
description :    SHA(256)摘要算法
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/06 初始版本
*********************************************************************/

#ifndef BP_SECURITY_KMC_H
#define BP_SECURITY_KMC_H

//#include "ivs_xml.h"
//#include "bp_key_config.h"
#include <string>
#include "bp_def.h"

class EXPORT_DLL CKMC
{
public:

	CKMC ( void );
    virtual ~CKMC (void );
	
	//char find_pos(char ch);
	//void base64_decode(const unsigned char *data, unsigned int data_len,unsigned char *ret);
	void getEncryptKeyLen(int srclen,int* outlen );
	bool EncryptKey(unsigned char*Inkey, int Inlen, unsigned char*OutKey,int Outlen);
	bool DecryptKey(unsigned char*Inkey,int Inlen, unsigned char* OutKey);//OutKey由调用者提供，unsigned char outkey[33]={}

    bool getKey(unsigned char* pInEncryptKey,unsigned char* pOutKey);	
    bool getKeyFromxml(std::string & pDevstr,const char* element);

};

#endif // CBB_SECURITY_KMC_H

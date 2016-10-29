/*Copyright 2015 Huawei Technologies Co., Ltd. All rights reserved.
eSDK is licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
		http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

/********************************************************************
 filename			:    DecoderSDKFactory.h 
 author				:    ywx313149
 created			:    2016/08/27
 description	:    SDK解码器工厂类
 copyright		:    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history				:    2016/08/27 初始版本
*********************************************************************/


#ifndef __DECODER_SDK_FACTORY_H__
#define  __DECODER_SDK_FACTORY_H__

#include "DecoderSDK.h"
#include "HikDecoderSDK.h"
#include "vos_common.h"
#include "vos.h"

class CDecoderSDKFactory
{
public:
	CDecoderSDKFactory(void);
	~CDecoderSDKFactory(void);
	static CDecoderSDK* GetDecoder(CUMW_DECODER_CONNECT_TYPE emConnectType);

private:
	static CHikDecoderSDK* m_pHikDecoderSDK;
	static VOS_Mutex* m_pMutex; //解码器登录信息表锁
};

#endif //__DECODER_SDK_FACTORY_H__



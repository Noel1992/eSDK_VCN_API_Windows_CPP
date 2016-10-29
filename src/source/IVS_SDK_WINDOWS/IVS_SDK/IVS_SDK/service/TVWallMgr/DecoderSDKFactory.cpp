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

#include "StdAfx.h"
#include "DecoderSDKFactory.h"
#include "vos_common.h"
//#include "CumwCommon.h"   //lint !e537

CHikDecoderSDK* CDecoderSDKFactory::m_pHikDecoderSDK = NULL;
VOS_Mutex* CDecoderSDKFactory::m_pMutex = NULL;;

CDecoderSDKFactory::CDecoderSDKFactory(void)
{
	m_pMutex = VOS_CreateMutex(); //lint !e1938
}


CDecoderSDKFactory::~CDecoderSDKFactory(void)
{
	if (NULL != m_pHikDecoderSDK)
	{
		try
		{
			VOS_DELETE(m_pHikDecoderSDK);
		}
		catch (...)
		{
		}

		m_pHikDecoderSDK = NULL;
	}

	if (NULL != m_pMutex)
	{
		VOS_DestroyMutex(m_pMutex);
		m_pMutex = NULL;
	}
}

CDecoderSDK* CDecoderSDKFactory::GetDecoder(CUMW_DECODER_CONNECT_TYPE emConnectType)
{
	CLockGuard autoLock(m_pMutex); 
	if(CUMW_DECODER_CONNECT_HIK == emConnectType)
	{
		if(NULL == m_pHikDecoderSDK)
		{
			m_pHikDecoderSDK = VOS_NEW((CHikDecoderSDK*&)m_pHikDecoderSDK);
		}
		return m_pHikDecoderSDK;
	}

	return NULL;
}




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

#include "DecoderSDK.h"
//#include "CumwCommon.h"   //lint !e537

CDecoderSDK::CDecoderSDK(void)
{
}


CDecoderSDK::~CDecoderSDK(void)
{
}

IVS_INT32 CDecoderSDK::Login(IVS_LONG& ulSessionID, const DECODER_DEV* pDecoder, const IVS_CHAR* pPwd)
{
	return 1;
}//lint !e715 !e1961

IVS_INT32 CDecoderSDK::Logout(IVS_LONG ulSessionID)
{
	return 1;
}//lint !e715 !e1961

IVS_INT32 CDecoderSDK::GetChannels(IVS_LONG ulSessionID, DECODER_DEV* pDecoder)
{
	return 1;
}//lint !e715 !e1961

IVS_INT32 CDecoderSDK::StartPlayURL(IVS_LONG ulSessionID, const IVS_CHAR* pUrl, CUMW_DECODER_CHANNEL_INFO* pChannel)
{
	return 1;
}//lint !e715 !e1961
IVS_INT32 CDecoderSDK::StopPlayURL(IVS_LONG ulSessionID, CUMW_DECODER_CHANNEL_INFO* pChannel) 
{
	return 1;
}//lint !e715 !e1961

//通过解码器ID获取用户的登录号
IVS_INT32 CDecoderSDK::GetUserIDbyDecorderID(const IVS_CHAR * pszDecoderID, IVS_LONG& lUserID)
{
	return 1;
}//lint !e715 !e1961

IVS_INT32 CDecoderSDK::InitHik(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData)
{
	return 1;
}//lint !e715 !e1961

bool CDecoderSDK::IsHikDecOnline(const char* pszDecoderID)
{
	return false;
}//lint !e715 !e1961
//IVS_INT32 CDecoderSDK::CheckDecoderChannel(const IVS_CHAR* pDecoderID,IVS_ULONG uiChannel) const
//{
//    return false;
//}//lint !e715


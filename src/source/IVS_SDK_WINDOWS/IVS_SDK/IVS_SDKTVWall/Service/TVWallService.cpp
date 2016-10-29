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
#include "TVWallService.h"

IMPLEMENT_SINGLE_INSTANCE(CTVWallService)

CTVWallService::CTVWallService(void)
{
}

CTVWallService::~CTVWallService(void)
{
}

//初始化电视墙模块
IVS_INT32 CTVWallService::InitTVWallService(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData)
{    
	return m_ControlBus.InitControlBus(pFunNotifyCallBack, pUserData);
}

IVS_INT32 CTVWallService::ReleaseTVWallService()
{
	return m_ControlBus.ReleaseControlBus();
}

IVS_INT32 CTVWallService::IniBusiness(IPI_INIT_BUSINESS_REQ* pParaInfo)
{
	return m_ControlBus.IniBusiness(pParaInfo);
}

IVS_INT32 CTVWallService::ReIniBusiness(IPI_INIT_BUSINESS_REQ* pParaInfo)
{
	return m_ControlBus.ReIniBusiness(pParaInfo);
}

IVS_INT32 CTVWallService::StartBusiness(IPI_START_BUSINESS_REQ* pParaInfo)
{
	return m_ControlBus.StartBusiness(pParaInfo);
}

IVS_INT32 CTVWallService::ReStartBusiness(IPI_START_BUSINESS_REQ* pParaInfo)
{
	return m_ControlBus.ReStartBusiness(pParaInfo);
}

IVS_INT32 CTVWallService::StopBusiness(IPI_STOP_BUSINESS_REQ* pParaInfo)
{
	return m_ControlBus.StopBusiness(pParaInfo);
}

IVS_INT32 CTVWallService::PlayAudio(IPI_AUDIO_CONTROL_REQ* pParaInfo)
{
	return m_ControlBus.PlayAudio(pParaInfo);
}

IVS_INT32 CTVWallService::SetVolume(IPI_TUNEUP_VOLUME_REQ* pParaInfo)
{
	return m_ControlBus.SetVolume(pParaInfo);
}

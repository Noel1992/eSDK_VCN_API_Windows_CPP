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
	filename	: 	TVWallService.h
	author		:	z00201790
	created		:	2012/12/12	
	description	:	定义电视墙业务管理类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/12 初始版本;
*********************************************************************/

#ifndef __TVWALL_SERVICE_H__
#define __TVWALL_SERVICE_H__

#include "hwsdk.h"
#include "ivs_error.h"
#include "ControlBus.h"

#include <string>
using namespace std;
/*!< 单键实例头文件 */
#include "SingleInst.h"
/*!< 电视上墙功能实现类 */
#include "ControlBus.h"


class CTVWallService
{
    DECLARE_SINGLE_INSTANCE(CTVWallService)

public:
    virtual ~CTVWallService(void);

    // 初始化TV Wall模块
    IVS_INT32 InitTVWallService(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);

    // 释放TV Wall模块
    IVS_INT32 ReleaseTVWallService();

    // 初始化解码器
    IVS_INT32 IniBusiness(IPI_INIT_BUSINESS_REQ* pParaInfo);

	// 重新初始化解码器
	IVS_INT32 ReIniBusiness(IPI_INIT_BUSINESS_REQ* pParaInfo);

    // 命令解码器开始解码
    IVS_INT32 StartBusiness(IPI_START_BUSINESS_REQ* pParaInfo);

	// 命令解码器重新开始解码
	IVS_INT32 ReStartBusiness(IPI_START_BUSINESS_REQ* pParaInfo);

    // 命令解码器停止解码
    IVS_INT32 StopBusiness(IPI_STOP_BUSINESS_REQ* pParaInfo);

    // 播放/停止播放声音
    IVS_INT32 PlayAudio(IPI_AUDIO_CONTROL_REQ* pParaInfo);

    // 设置音量
    IVS_INT32 SetVolume(IPI_TUNEUP_VOLUME_REQ* pParaInfo);

private:
    CTVWallService(void);

private:
    CControlBus m_ControlBus;//电视上墙控制对象

};
#endif //__TVWALL_SERVICE_H__


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
	description	:	�������ǽҵ�������;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/12 ��ʼ�汾;
*********************************************************************/

#ifndef __TVWALL_SERVICE_H__
#define __TVWALL_SERVICE_H__

#include "hwsdk.h"
#include "ivs_error.h"
#include "ControlBus.h"

#include <string>
using namespace std;
/*!< ����ʵ��ͷ�ļ� */
#include "SingleInst.h"
/*!< ������ǽ����ʵ���� */
#include "ControlBus.h"


class CTVWallService
{
    DECLARE_SINGLE_INSTANCE(CTVWallService)

public:
    virtual ~CTVWallService(void);

    // ��ʼ��TV Wallģ��
    IVS_INT32 InitTVWallService(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);

    // �ͷ�TV Wallģ��
    IVS_INT32 ReleaseTVWallService();

    // ��ʼ��������
    IVS_INT32 IniBusiness(IPI_INIT_BUSINESS_REQ* pParaInfo);

	// ���³�ʼ��������
	IVS_INT32 ReIniBusiness(IPI_INIT_BUSINESS_REQ* pParaInfo);

    // �����������ʼ����
    IVS_INT32 StartBusiness(IPI_START_BUSINESS_REQ* pParaInfo);

	// ������������¿�ʼ����
	IVS_INT32 ReStartBusiness(IPI_START_BUSINESS_REQ* pParaInfo);

    // ���������ֹͣ����
    IVS_INT32 StopBusiness(IPI_STOP_BUSINESS_REQ* pParaInfo);

    // ����/ֹͣ��������
    IVS_INT32 PlayAudio(IPI_AUDIO_CONTROL_REQ* pParaInfo);

    // ��������
    IVS_INT32 SetVolume(IPI_TUNEUP_VOLUME_REQ* pParaInfo);

private:
    CTVWallService(void);

private:
    CControlBus m_ControlBus;//������ǽ���ƶ���

};
#endif //__TVWALL_SERVICE_H__


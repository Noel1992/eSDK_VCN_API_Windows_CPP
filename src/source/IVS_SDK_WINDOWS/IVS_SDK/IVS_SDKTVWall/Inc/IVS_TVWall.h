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
filename	: 	IVS_TVWall.h
author		:	z00201790
created		:	2012/12/14
description	:	���� DLL Ӧ�ó���ĵ�������;
copyright	:	Copyright (C) 2011-2015
history		:	2012/10/23 ��ʼ�汾;
*********************************************************************/

#ifndef IVS_TVWALL_H
#define IVS_TVWALL_H

#include "hwsdk.h"
#include "ivs_error.h"
#include "IVS_SDKTVWall.h"

#ifdef IVS_IVSSDKTVWALL_EXPORTS
#define IVS_IVSSDKTVWALL_API __declspec(dllexport)
#else
#define IVS_IVSSDKTVWALL_API __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif
	/*****************************************************************************
	�������ƣ�IVS_TVWall_Init
	������������ʼ��;
	���������NA
	���������NA
	�� �� ֵ���ɹ�:IVS_SUCCEED;
	ʧ��:������;
	*****************************************************************************/
	IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_Init(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);

	/*****************************************************************************
	�������ƣ�IVS_TVWall_Release
	������������ʼ��;
	���������NA
	���������NA
	�� �� ֵ���ɹ�:IVS_SUCCEED;
	ʧ��:������;
	*****************************************************************************/
	IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_Release();
	
    /**************************************************************************
    * name       : IVS_TVWall_InitBusiness
    * description: ��ʼ��������
    * input      : pParaInfo   ��ʼ������
    * output     : NA
    * return     : long.       ������
    * remark     : NA
    **************************************************************************/
    IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_InitBusiness(IPI_INIT_BUSINESS_REQ* pParaInfo);

	/**************************************************************************
    * name       : IVS_TVWall_ReInitBusiness
    * description: ���³�ʼ��������
    * input      : pParaInfo   ��ʼ������
    * output     : NA
    * return     : long.       ������
    * remark     : NA
    **************************************************************************/
    IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_ReInitBusiness(IPI_INIT_BUSINESS_REQ* pParaInfo);

	/**************************************************************************
    * name       : IVS_TVWall_StartBusiness
    * description: ����Զ�̽�������ʼ����
    * input      : pParaInfo ��ʼ������
    * output     : void
    * return     : �ɹ�:IVS_SUCCEED;
    * remark     : NA
    **************************************************************************/
    IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_StartBusiness(IPI_START_BUSINESS_REQ* pParaInfo);

	/**************************************************************************
    * name       : IVS_TVWall_ReStartBusiness
    * description: ����Զ�̽��������¿�ʼ����
    * input      : pParaInfo ��ʼ������
    * output     : void
    * return     : �ɹ�:IVS_SUCCEED;
    * remark     : NA
    **************************************************************************/
    IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_ReStartBusiness(IPI_START_BUSINESS_REQ* pParaInfo);

    /**************************************************************************
    * name       : IVS_TVWall_StopBusiness
    * description: ����Զ�̽�����ֹͣ����
    * input      : pParaInfo ��ʼ������
    * output     : void
    * return     : �ɹ�:IVS_SUCCEED;
    * remark     : NA
    **************************************************************************/
    IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_StopBusiness(IPI_STOP_BUSINESS_REQ* pParaInfo);

    /**************************************************************************
    * name       : IVS_TVWall_PlayAudio
    * description: ����/ֹͣ��������
    * input      : pParaInfo ��ʼ������
    * output     : void
    * return     : �ɹ�:IVS_SUCCEED;
    * remark     : NA
    **************************************************************************/
    //IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_PlayAudio(IPI_AUDIO_CONTROL_REQ *pParaInfo);

    /**************************************************************************
    * name       : IVS_TVWall_SetVolume
    * description: ��������
    * input      : pParaInfo ��ʼ������
    * output     : void
    * return     : �ɹ�:IVS_SUCCEED;
    * remark     : NA
    **************************************************************************/
    //IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_SetVolume(IPI_TUNEUP_VOLUME_REQ* pParaInfo);
}

#endif //IVS_TVWALL_H

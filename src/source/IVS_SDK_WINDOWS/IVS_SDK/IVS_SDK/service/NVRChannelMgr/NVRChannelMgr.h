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
 filename			:    NVRChannelMgr.h
 author				:    ywx313149
 created			:    2016/08/17
 description	:    ����NVRCode �� NVR Channel��ת��
 copyright		:    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history				:    2016/08/17 ��ʼ�汾
*********************************************************************/

#ifndef __NVR_CHANNEL_MGR_H__
#define __NVR_CHANNEL_MGR_H__

#include "ivs_error.h"
#include "hwsdk.h"
#include "NSSOperator.h"
#include "SDKDef.h"

class CUserMgr;
class CNVRChannelMgr
{
public:
	CNVRChannelMgr(void);
	~CNVRChannelMgr(void);

	void SetUserMgr(CUserMgr *pUserMgr);

	/******************************************************************
     function			: GetCameraCodeByChannel
     description	: ����uiChannelID����Ϣ�н�������Ҫ������
     input				: uiChannelID NVRͨ����
     output				: NA
     return				: IVS_INT32
    *******************************************************************/
	IVS_INT32 GetCameraCodeByChannel(const IVS_UINT32 uiChannelID);


	/******************************************************************
     function			: GetMultiCameraCodeByChannel
     description	: ����uiChannelID����Ϣ�н��������cameracode
     input				: uiChannelID NVRͨ����
     output				: stCameraCodeVct  �豸���������
     return				: IVS_INT32
    *******************************************************************/
	IVS_INT32 GetMultiCameraCodeByChannel(const IVS_UINT32 uiChannelID, std::vector<IVS_DEVICE_CODE>& stCameraCodeVct);


	/******************************************************************
     function			: GetUniCameraCodeByChannel
     description	: ����uiChannelID����Ϣ�н�����Ψһһ��cameracode
     input				: uiChannelID NVRͨ����
     output				: pDevCode �豸�����ַ�����
     return				: IVS_INT32
    *******************************************************************/
	IVS_INT32 GetUniCameraCodeByChannel(const IVS_UINT32 uiChannelID, IVS_CHAR* pDevCode);


	/******************************************************************
     function			: GetUniCameraCodeByChannel
     description	: ����uiChannel�б����Ϣ�н��������cameracode,cameracode��uiChannel һһ��Ӧ
     input				: pChannelList NVRͨ�����б�
								  uiDeviceNum �豸��Ŀ
     output				: pDeviceList �豸�����б�
     return				: IVS_INT32
    *******************************************************************/
	IVS_INT32 GetMultiCameraCodeByChannelList(const IVS_UINT32  uiDeviceNum, const IVS_CHANNEL  *pChannelList,  IVS_DEVICE_CODE** pDeviceList);


	/******************************************************************
     function			: ClearChannelCameraCodeMap
     description	: ��ʼ��MAP
     input				: NA
     output				: NA
     return				: void
    *******************************************************************/
	void ClearChannelCameraCodeMap(void);


	/******************************************************************
     function			: GetChannelByCameraCode
     description	: ��devcodeɸѡ����Ӧ��channel
     input				: pDevCode �豸�����ַ�����
     output				: uiChannelID NVRͨ����
     return				: IVS_INT32
    *******************************************************************/
	IVS_INT32 GetChannelByCameraCode(const IVS_CHAR* pDevCode, IVS_UINT32& uiChannelID);


	/******************************************************************
     function			: ���Ӳ�ѯ����CameraCode
     description	: ��devcodeɸѡ����Ӧ��channel
     input				: pDevCode �豸�����ַ�����
								: pUnifiedQuery ��ѯ����
     output				: pNewUnifiedQuery ����CameraCode�Ĳ�ѯ����,��ָ����Ҫ�ڷ������ͷ�
     return				: IVS_INT32
    *******************************************************************/
	IVS_INT32 AddCamerCodeOfQuery(const IVS_CHAR* pDevCode, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_QUERY_UNIFIED_FORMAT** pNewUnifiedQuery);



	
private:
	CUserMgr* m_pUserMgr;

	// TODO ���߳� ��
	ChannelCameraCodeMap m_stChannelCameraCodeMap; 
	VOS_Mutex*                             m_pMutexChannelCameraCodeMap;      // ������

};

#endif



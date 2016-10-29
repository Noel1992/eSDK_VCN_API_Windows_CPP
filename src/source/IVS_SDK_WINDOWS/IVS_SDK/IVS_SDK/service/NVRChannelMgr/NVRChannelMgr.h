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
 description	:    处理NVRCode 和 NVR Channel的转换
 copyright		:    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history				:    2016/08/17 初始版本
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
     description	: 根据uiChannelID从消息中解析出需要的数据
     input				: uiChannelID NVR通道号
     output				: NA
     return				: IVS_INT32
    *******************************************************************/
	IVS_INT32 GetCameraCodeByChannel(const IVS_UINT32 uiChannelID);


	/******************************************************************
     function			: GetMultiCameraCodeByChannel
     description	: 根据uiChannelID从消息中解析出多个cameracode
     input				: uiChannelID NVR通道号
     output				: stCameraCodeVct  设备编码的数组
     return				: IVS_INT32
    *******************************************************************/
	IVS_INT32 GetMultiCameraCodeByChannel(const IVS_UINT32 uiChannelID, std::vector<IVS_DEVICE_CODE>& stCameraCodeVct);


	/******************************************************************
     function			: GetUniCameraCodeByChannel
     description	: 根据uiChannelID从消息中解析出唯一一个cameracode
     input				: uiChannelID NVR通道号
     output				: pDevCode 设备编码字符数组
     return				: IVS_INT32
    *******************************************************************/
	IVS_INT32 GetUniCameraCodeByChannel(const IVS_UINT32 uiChannelID, IVS_CHAR* pDevCode);


	/******************************************************************
     function			: GetUniCameraCodeByChannel
     description	: 根据uiChannel列表从消息中解析出多个cameracode,cameracode与uiChannel 一一对应
     input				: pChannelList NVR通道号列表
								  uiDeviceNum 设备数目
     output				: pDeviceList 设备编码列表
     return				: IVS_INT32
    *******************************************************************/
	IVS_INT32 GetMultiCameraCodeByChannelList(const IVS_UINT32  uiDeviceNum, const IVS_CHANNEL  *pChannelList,  IVS_DEVICE_CODE** pDeviceList);


	/******************************************************************
     function			: ClearChannelCameraCodeMap
     description	: 初始化MAP
     input				: NA
     output				: NA
     return				: void
    *******************************************************************/
	void ClearChannelCameraCodeMap(void);


	/******************************************************************
     function			: GetChannelByCameraCode
     description	: 由devcode筛选出对应的channel
     input				: pDevCode 设备编码字符数组
     output				: uiChannelID NVR通道号
     return				: IVS_INT32
    *******************************************************************/
	IVS_INT32 GetChannelByCameraCode(const IVS_CHAR* pDevCode, IVS_UINT32& uiChannelID);


	/******************************************************************
     function			: 增加查询条件CameraCode
     description	: 由devcode筛选出对应的channel
     input				: pDevCode 设备编码字符数组
								: pUnifiedQuery 查询条件
     output				: pNewUnifiedQuery 增加CameraCode的查询条件,此指针需要在方法外释放
     return				: IVS_INT32
    *******************************************************************/
	IVS_INT32 AddCamerCodeOfQuery(const IVS_CHAR* pDevCode, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_QUERY_UNIFIED_FORMAT** pNewUnifiedQuery);



	
private:
	CUserMgr* m_pUserMgr;

	// TODO 多线程 锁
	ChannelCameraCodeMap m_stChannelCameraCodeMap; 
	VOS_Mutex*                             m_pMutexChannelCameraCodeMap;      // 保护锁

};

#endif



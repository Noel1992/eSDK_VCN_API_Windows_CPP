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
	filename	: 	RecordPlanMgr.h
	author		:	z00201790
	created		:	2012/11/05	
	description	:	定义录像计划管理类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/11/05 初始版本;
*********************************************************************/

#ifndef __RECORD_PLAN_MGR_H__
#define __RECORD_PLAN_MGR_H__

#include "RecordXMLProcess.h"


class CUserMgr;
class CRecordPlanMgr
{
public:
    CRecordPlanMgr(void);
    ~CRecordPlanMgr(void);

    void SetUserMgr(CUserMgr *pUserMgr);

public:
	/******************************************************************
	function : AddRecordPlan
	description: 添加平台录像计划;
	input : uiDeviceNum,摄像机数量；pDeviceList，摄像机编号；pRecordPlan，录像计划内容；
	pResultList，录像计划批量新增结果；uiResultBufferSize，结果内存大小校验值
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 AddRecordPlan(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
		const IVS_RECORD_PLAN* pRecordPlan,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize)const;

	/******************************************************************
	function : ModifyRecordPlan
	description: 修改平台录像计划;
	input : uiDeviceNum,摄像机数量；pDeviceList，摄像机编号；pRecordPlan，录像计划内容；
	pResultList，录像计划批量修改结果；uiResultBufferSize，结果内存大小校验值
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 ModifyRecordPlan(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
		const IVS_RECORD_PLAN* pRecordPlan,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize)const;

	/******************************************************************
	function : DeleteRecordPlan
	description: 删除平台录像计划;
	input : uiDeviceNum,摄像机数量；pDeviceList，摄像机编号；uiRecordMethod，录像计划方式；
	pResultList，录像计划批量删除结果；uiResultBufferSize，结果内存大小校验值
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 DeleteRecordPlan(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
		IVS_UINT32 uiRecordMethod,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize)const;

	/******************************************************************
	function : GetRecordPlan
	description: 查询平台录像计划;
	input : pCameraCode,摄像机编号；uiRecordMethod，录像计划方式；pRecordPlan，录像计划查询结果；
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 GetRecordPlan(const char* pCameraCode,IVS_UINT32 uiRecordMethod,
		IVS_RECORD_PLAN* pRecordPlan);

    /******************************************************************
    function : GetCameraPlanInfo
    description: 查询摄像机计划设置信息
    input : IVS_UINT32 uiPlanType       录像类型
    input : IVS_UINT32 uiDeviceNum      摄像机个数
    output : IVS_CHAR** pRspXm          响应消息
    return : 成功-0；失败-1
    *******************************************************************/
    IVS_INT32 GetCameraPlanInfo(IVS_UINT32 uiPlanType, IVS_CHAR** pRspXml);

	/******************************************************************
	function : SetWaterMarkState
	description: 设置开启、关闭水印校验;
	input : iHandle,句柄；bIsStart，是否开启
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 SetWaterMarkState(IVS_INT32 /*iHandle*/,IVS_BOOL /*bIsStart*/)const;

private:
    CUserMgr *m_UserMgr;
};

#endif

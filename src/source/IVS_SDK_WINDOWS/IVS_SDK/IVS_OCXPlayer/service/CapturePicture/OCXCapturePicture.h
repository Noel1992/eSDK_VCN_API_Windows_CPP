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
	filename	: 	OCXCapturePicture.h
	author		:	s00193168
	created		:	2013/1/25	
	description	:	定义图片抓拍类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2013/1/25 初始版本;
*********************************************************************/
#ifndef __OCX_CAPTUREPICTURE_H__ 
#define __OCX_CAPTUREPICTURE_H__

#include "hwsdk.h"
#include "stdafx.h"
#include "SDKDef.h"

class OCXCapturePicture
{
public:
	OCXCapturePicture(void);
	~OCXCapturePicture(void);

	// 获取平台抓拍列表
	/*****************************************************************
	     function   : GetSnapshotList
	     description: 获取平台抓拍列表
		 input      : IVS_INT32 iSessionId  会话ID
		 const IVS_CHAR * pQueryParam  抓拍图片查询参数
	     output     : CString & strResult  抓拍图片列表信息
	     return     : 成功-0 失败-错误码 
	*****************************************************************/
	
	static IVS_INT32 GetSnapshotList(IVS_INT32 iSessionId, const IVS_CHAR* pQueryParam, CString &strResult);

	/**********************************************************************
	* name			: SetSnapshotPlan
	* description	: 设置平台抓拍计划
	* input			: strResult 平台抓拍计划xml
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	static LONG SetSnapshotPlan(const CString& strSnapshotPlan,const IVS_INT32 iSessionID);

	/**********************************************************************
	* name			: GetSnapshotPlan
	* description	: 查询平台抓拍计划
	* input			: strCameraCode 摄像机编号
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	static CString GetSnapshotPlan(const CString& strCameraCode,const IVS_INT32 iSessionID);

	
	/*****************************************************************
	     function   : DelSnapshotPlan
	     description: 删除平台抓拍计划
	     output     : IVS_INT32 iSessionId   会话ID
	     output     : const IVS_CHAR * pCameraCode 摄像机编码
	     return     : 成功-0 失败-错误码 
	*****************************************************************/
	static IVS_INT32 DelSnapshotPlan(IVS_INT32 iSessionId, const IVS_CHAR* pCameraCode);

	/*****************************************************************
	     function   : DeleteSnapshot
	     description: 删除平台抓拍图片
	     inputv     : IVS_INT32 iSessionID           会话ID
	     input      : const IVS_CHAR * pCameraCode   摄像机编码
	     input      : IVS_UINT32 uiPictureID         图片ID
	     return     : 成功-0 失败-错误码
	*****************************************************************/
	static IVS_INT32 DeleteSnapshot(IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_UINT32 uiPictureID);

};


#endif


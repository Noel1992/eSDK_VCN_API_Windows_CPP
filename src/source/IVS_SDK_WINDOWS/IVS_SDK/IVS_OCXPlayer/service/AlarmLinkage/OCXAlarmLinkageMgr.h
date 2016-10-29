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
	filename	: 	OCXAlarmLinkageMgr.h
	author		:	z00193167
	created		:	2012/12/06	
	description	:	处理告警OCX
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/06 初始版本
*********************************************************************/

#ifndef __OCX_ALARMLINKAGE_MGR_H__
#define __OCX_ALARMLINKAGE_MGR_H__

#include "hwsdk.h"

class COCXAlarmLinkageMgr
{
public:
	COCXAlarmLinkageMgr();
	~COCXAlarmLinkageMgr();

public:
	/***********************************************************************************
    * name       : AddAlarmLinkage
    * description: 新增告警联动策略ocx
    * input      : iSessionId:登录Id; pReqXml:CU传参数; strResult:返回值
    * output     : strResult:返回给CU的xml
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AddAlarmLinkage(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString& strResult);

	/***********************************************************************************
    * name       : ModifyAlarmLinkage
    * description: 修改告警联动策略ocx
    * input      : iSessionId:登录Id; pReqXml:CU传参数; 
    * output     : NA
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 ModifyAlarmLinkage(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml);

	/***********************************************************************************
    * name       : DeleteAlarmLinkage
    * description: 删除告警联动策略ocx，iAlarmLinkageID，告警联动策略ID
    * input      : iSessionId:登录Id; 
    * output     : NA
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 DeleteAlarmLinkage(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml);


	/***********************************************************************************
    * name       : GetAlarmLinkage
	* description: 获取联动策略详细信息
	* input      : iSessionId:登录Id; uiAlarmLinkageID:联动策略Id; strResult:返回值;
	* output     : strResult:返回给CU的xml
	* return     : NA
	* remark     : NA
	***********************************************************************************/
	static IVS_VOID GetAlarmLinkage(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString& strResult);

	/***********************************************************************************
    * name       : GetAlarmLinkageList
	* description: 获取联动策略列表
	* input      : iSessionId:登录Id; pReqXml:CU传参数; strResult:返回值;
	* output     : strResult:返回给CU的xml
	* return     : NA
	* remark     : NA
	***********************************************************************************/
	static IVS_VOID GetAlarmLinkageList(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString& strResult);

	/***********************************************************************************
    * name       : GetAlarmLinkageAction
	* description: 查询告警联动动作详情
	* input      : iSessionId:登录Id; pReqXml，查询请求xml
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 GetAlarmLinkageAction(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult);
};

#endif //__OCX_ALARMLINKAGE_MGR_H__


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
filename	: 	AlarmHelpMgr.h
author		:	pkf57481
created		:	2012/11/7
description	:	告警帮助管理;
copyright	:	Copyright (C) 2011-2015
history		:	2012/11/7 初始版本;
*********************************************************************/

#ifndef __ALARMHELP_MGR_H
#define __ALARMHELP_MGR_H
#include "hwsdk.h"

class CUserMgr;
class CAlarmHelpMgr
{
public:
	CAlarmHelpMgr(void);
	~CAlarmHelpMgr(void);
	void SetUserMgr(CUserMgr *pUserMgr);
public:
	// 新增告警帮助
	IVS_INT32 AddHelpInfo(const IVS_CHAR* pDomainCode, const IVS_HELP_INFO *helpInfo) const;

	// 修改告警帮助
	IVS_INT32 ModifyHelpInfo(const IVS_CHAR* pDomainCode, const IVS_HELP_INFO *helpInfo) const;

	// 删除告警帮助
	IVS_INT32 DeleteHelpInfo(IVS_UINT32 uiSourceId,IVS_UINT32 uiSourceType, const IVS_CHAR* pDomainCode) const;

	// 查询告警帮助详情
	IVS_INT32 QueryHelpInfo(IVS_UINT32 uiAlarmHelpSourceType,IVS_UINT32 uiAlarmHelpSourceId,const IVS_CHAR* pDomainCode, IVS_HELP_INFO *helpInfo) const;

    /******************************************************************
     function   : QueryHelpInfoText
     description: 查询帮助信息正文
     input      : const IVS_CHAR * pAlarmInCode
     input      : const IVS_CHAR * pAlarmType
     input      : IVS_CHAR * pHelpInfo
     input      : IVS_UINT32 uiBufferSize
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 QueryHelpInfoText(const IVS_CHAR* pAlarmInCode, const IVS_CHAR* pAlarmType, IVS_CHAR* pHelpInfo, IVS_UINT32 uiBufferSize);

private:
	CAlarmHelpMgr* m_AlarmHelpMgr;
	CUserMgr* m_UserMgr;
};	

#endif

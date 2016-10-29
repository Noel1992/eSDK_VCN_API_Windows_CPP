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
	filename	: 	OCXLogMgr.h
	author		:	wWX155351
	created		:	2012/12/10	
	description	:	处理告警OCX
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2012/12/10 初始版本
*********************************************************************/

#ifndef __OCX_LOG_MGR_H__
#define __OCX_LOG_MGR_H__

#include "hwsdk.h"

class COCXLogMgr
{
public:
	COCXLogMgr();
	~COCXLogMgr();

	// 查询操作日志
	static IVS_INT32 GetOperationLog(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString &strResult);

};

#endif // __OCX_LOG_MGR_H__

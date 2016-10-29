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
filename 	: COcxIntelligenceAnalysisMgr.h
author   	: z00227377
created 	: 2012/12/06	
description : OCX智能分析管理
copyright 	: Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history 	: 2012/12/06 初始版本
*********************************************************************/

#ifndef __OCX_INTELLIGENCE_ANALYSIS_MGR_H__
#define __OCX_INTELLIGENCE_ANALYSIS_MGR_H__

#include "hwsdk.h"
#include "stdafx.h"

class COcxIntelligenceAnalysisMgr
{
public:
    COcxIntelligenceAnalysisMgr(void);
    ~COcxIntelligenceAnalysisMgr(void);

public:

	// 智能分析统一透传接口
	static void IntelligenceAnalysis(CString &strResult, IVS_INT32 iSessionID, ULONG ulInterfaceType, const IVS_CHAR* pReqXml);

private:

	// 构建返回消息
    static void RetToCUMsg(CString &strResult, IVS_INT32 iRet, IVS_CHAR* pRspXml);

};


#endif

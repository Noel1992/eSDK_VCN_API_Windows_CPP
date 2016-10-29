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
filename	: 	CIntelligenceAnalysisMgr.h
author		:	zwx211831
created		:	2014/12/5
description	:	���ܷ����������;
copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history		:	2014/12/5 ��ʼ�汾;
*********************************************************************/

#ifndef __INTELLIGENCE_ANALYSIS_MGR_H__
#define __INTELLIGENCE_ANALYSIS_MGR_H__

#include "hwsdk.h"
#include "nss_mt.h"

class CIntelligenceAnalysisMgr
{
public:
    CIntelligenceAnalysisMgr(void);
    ~CIntelligenceAnalysisMgr(void);
    
    // �����û�����ָ��
    void SetUserMgr(CUserMgr *pUserMgr);

	// ���ܷ���ͳһ͸���ӿ�
	IVS_INT32 IntelligenceAnalysis(IVS_UINT32 uiInterfaceType, IVS_CHAR* pTransID, IVS_UINT32 uiTransIDLen, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

private:
    CUserMgr   *m_pUserMgr;       // �û�����ָ��
};

#endif



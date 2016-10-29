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
filename : ReportMgr.h
author : pkf57481
created : 2013/03/06
description : ���������
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/03/06 ��ʼ�汾
*********************************************************************/


#ifndef __REPORT_MGR_H_
#define __REPORT_MGR_H_

#include "hwsdk.h"
#include "IVSCommon.h"

class CUserMgr;
class CReportMgr
{
public:
    CReportMgr(void);
    ~CReportMgr(void);

    void SetUserMgr(CUserMgr *pUserMgr);

public:
    // ʵʱ�����ѯ
    IVS_INT32 OMUQueryDevReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // ʵʱ���������ѯ
    IVS_INT32 OMUQueryDevReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // ʵʱ�����ѯ
    IVS_INT32 OMUQueryRealtimeReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // ʵʱ���������ѯ
    IVS_INT32 OMUQueryRealtimeReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // ��Ƶ������ϲ�ѯ
    IVS_INT32 OMUQueryQDReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // ��Ƶ������������ѯ
    IVS_INT32 OMUQueryQDReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // ¼��ִ�мƻ������ѯ
    IVS_INT32 OMUQueryRecordPlanReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // ¼��ִ�мƻ����������ѯ
    IVS_INT32 OMUQueryRecordPlanReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // ¼�������Ա����ѯ
    IVS_INT32 OMUQueryRecordIntegralityReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // ¼�������Ա��������ѯ
    IVS_INT32 OMUQueryRecordIntegralityReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // �����ʱ����ѯ
    IVS_INT32 OMUQueryPacketLossRateReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // �����ʱ��������ѯ
    IVS_INT32 OMUQueryPacketLossRateReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // ���̴洢ռ���ʱ����ѯ
    IVS_INT32 OMUQueryDiskUsageRateReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    IVS_INT32 SendQueryCmd(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml, IVS_INT32 iMsgType, IVS_INT32 iTimeOut = -1);

	// ��ѯ������Ϣ�ӿ�
	IVS_INT32 OMUQueryDiskProperty(IVS_CHAR** pRspXml);

private:
    CUserMgr *m_UserMgr;
};

#endif

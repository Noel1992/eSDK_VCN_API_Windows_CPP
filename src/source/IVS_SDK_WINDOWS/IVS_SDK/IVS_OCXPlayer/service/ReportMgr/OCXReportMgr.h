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
filename : OCXReportMgr.h
author : s00191067
created : 2013/3/7
description : ���������
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/3/7 ��ʼ�汾
*********************************************************************/


#ifndef __OCX_REPORT_MGR_H_
#define __OCX_REPORT_MGR_H_

#include "hwsdk.h"
#include "IVSCommon.h"

class COCXReportMgr
{
public:
    COCXReportMgr(void);
    ~COCXReportMgr(void);

public:
    // ʵʱ�����ѯ
    static CString OMUQueryDevReport(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // ʵʱ���������ѯ
    static CString OMUQueryDevReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // ʵʱ�����ѯ
    static CString OMUQueryRealtimeReport(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // ʵʱ���������ѯ
    static CString OMUQueryRealtimeReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // ��Ƶ������ϲ�ѯ
    static CString OMUQueryQDReport(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // ��Ƶ������������ѯ
    static CString OMUQueryQDReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // ¼��ִ�мƻ������ѯ
    static CString OMUQueryRecordPlanReport(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // ¼��ִ�мƻ����������ѯ
    static CString OMUQueryRecordPlanReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // ¼�������Ա����ѯ
    static CString OMUQueryRecordIntegralityReport(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // ¼�������Ա��������ѯ
    static CString OMUQueryRecordIntegralityReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // �����ʱ����ѯ
    static CString OMUQueryPacketLossRateReport(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // �����ʱ��������ѯ
    static CString OMUQueryPacketLossRateReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // ���̴洢ռ���ʱ����ѯ
    static CString OMUQueryDiskUsageRateReport(IVS_INT32 iSessionID, LPCTSTR pReqXml);

};

#endif

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
description : 报表管理类
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/3/7 初始版本
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
    // 实时报表查询
    static CString OMUQueryDevReport(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // 实时报表详情查询
    static CString OMUQueryDevReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // 实时报表查询
    static CString OMUQueryRealtimeReport(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // 实时报表详情查询
    static CString OMUQueryRealtimeReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // 视频质量诊断查询
    static CString OMUQueryQDReport(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // 视频质量诊断详情查询
    static CString OMUQueryQDReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // 录像执行计划报表查询
    static CString OMUQueryRecordPlanReport(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // 录像执行计划报表详情查询
    static CString OMUQueryRecordPlanReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // 录像完整性报表查询
    static CString OMUQueryRecordIntegralityReport(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // 录像完整性报表详情查询
    static CString OMUQueryRecordIntegralityReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // 丢包率报表查询
    static CString OMUQueryPacketLossRateReport(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // 丢包率报表详情查询
    static CString OMUQueryPacketLossRateReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // 磁盘存储占用率报表查询
    static CString OMUQueryDiskUsageRateReport(IVS_INT32 iSessionID, LPCTSTR pReqXml);

};

#endif

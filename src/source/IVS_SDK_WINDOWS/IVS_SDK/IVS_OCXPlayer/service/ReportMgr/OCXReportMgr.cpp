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

#include "stdafx.h"
#include "SDKDef.h"
#include "IVS_OCXPlayerCtrl.h"
#include "hwsdk.h"
#include "ToolsHelp.h"
#include "ivs_error.h"
#include "OCXReportMgr.h"
#include "IVS_Trace.h"
#include "OCXXmlProcess.h"

COCXReportMgr::COCXReportMgr(void)
{
}

COCXReportMgr::~COCXReportMgr(void)
{
}

// 实时报表查询
CString COCXReportMgr::OMUQueryDevReport(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
	COCXXmlProcess::GetErrString(strResult, IVS_FAIL);
	return strResult;
  //  IVS_CHAR* pRspXml = NULL;
  //  IVS_INT32 iRet = IVS_SDK_OMUQueryDevReport(iSessionID, pReqXml, &pRspXml);
  //  if (IVS_SUCCEED != iRet || NULL == pRspXml)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "OMUQueryDevReport SDK Interface fail", "NA");
		//if (NULL != pRspXml)
		//{
		//	IVS_SDK_ReleaseBuffer(pRspXml);
		//}
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  std::string strInXml = pRspXml;
  //  std::string strOutXml;
  //  iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
  //  if (IVS_SUCCEED != iRet)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
		//if (NULL != pRspXml)//lint !e774
		//{
		//	IVS_SDK_ReleaseBuffer(pRspXml);
		//}
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  IVS_SDK_ReleaseBuffer(pRspXml);
   // return strOutXml.c_str();
}

// 实时报表详情查询
CString COCXReportMgr::OMUQueryDevReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
	COCXXmlProcess::GetErrString(strResult, IVS_FAIL);
	return strResult;
  //  IVS_CHAR* pRspXml = NULL;
  //  IVS_INT32 iRet = IVS_SDK_OMUQueryDevReportDetail(iSessionID, pReqXml, &pRspXml);
  //  if (IVS_SUCCEED != iRet || NULL == pRspXml)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "OMUQueryDevReportDetail SDK Interface fail", "NA");
  //      if (NULL != pRspXml)
  //      {
  //          IVS_SDK_ReleaseBuffer(pRspXml);
  //      }
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  std::string strInXml = pRspXml;
  //  std::string strOutXml;
  //  iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
  //  if (IVS_SUCCEED != iRet)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
		//if (NULL != pRspXml)//lint !e774
		//{
		//	IVS_SDK_ReleaseBuffer(pRspXml);
		//}
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  IVS_SDK_ReleaseBuffer(pRspXml);
    //return strOutXml.c_str();
}

// 实时报表查询
CString COCXReportMgr::OMUQueryRealtimeReport(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
	COCXXmlProcess::GetErrString(strResult, IVS_FAIL);
	return strResult;
  //  IVS_CHAR* pRspXml = NULL;
  //  IVS_INT32 iRet = IVS_SDK_OMUQueryRealtimeReport(iSessionID, pReqXml, &pRspXml);
  //  if (IVS_SUCCEED != iRet || NULL == pRspXml)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "OMUQueryRealtimeReport SDK Interface fail", "NA");
  //      if (NULL != pRspXml)
  //      {
  //          IVS_SDK_ReleaseBuffer(pRspXml);
  //      }
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  std::string strInXml = pRspXml;
  //  std::string strOutXml;
  //  iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
  //  if (IVS_SUCCEED != iRet)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
		//if (NULL != pRspXml)//lint !e774
		//{
		//	IVS_SDK_ReleaseBuffer(pRspXml);
		//}
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  IVS_SDK_ReleaseBuffer(pRspXml);
    //return strOutXml.c_str();
}

// 实时报表详情查询
CString COCXReportMgr::OMUQueryRealtimeReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
	COCXXmlProcess::GetErrString(strResult, IVS_FAIL);
	return strResult;
  //  IVS_CHAR* pRspXml = NULL;
  //  IVS_INT32 iRet = IVS_SDK_OMUQueryRealtimeReportDetail(iSessionID, pReqXml, &pRspXml);
  //  if (IVS_SUCCEED != iRet || NULL == pRspXml)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "OMUQueryRealtimeReportDetail SDK Interface fail", "NA");
  //      if (NULL != pRspXml)
  //      {
  //          IVS_SDK_ReleaseBuffer(pRspXml);
  //      }
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  std::string strInXml = pRspXml;
  //  std::string strOutXml;
  //  iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
  //  if (IVS_SUCCEED != iRet)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
		//if (NULL != pRspXml)//lint !e774
		//{
		//	IVS_SDK_ReleaseBuffer(pRspXml);
		//}
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  IVS_SDK_ReleaseBuffer(pRspXml);
    //return strOutXml.c_str();
}

// 视频质量诊断查询
CString COCXReportMgr::OMUQueryQDReport(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
	COCXXmlProcess::GetErrString(strResult, IVS_FAIL);
	return strResult;
  //  IVS_CHAR* pRspXml = NULL;
  //  IVS_INT32 iRet = IVS_SDK_OMUQueryQDReport(iSessionID, pReqXml, &pRspXml);
  //  if (IVS_SUCCEED != iRet || NULL == pRspXml)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "OMUQueryQDReport SDK Interface fail", "NA");
  //      if (NULL != pRspXml)
  //      {
  //          IVS_SDK_ReleaseBuffer(pRspXml);
  //      }
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  std::string strInXml = pRspXml;
  //  std::string strOutXml;
  //  iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
  //  if (IVS_SUCCEED != iRet)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
		//if (NULL != pRspXml)//lint !e774
		//{
		//	IVS_SDK_ReleaseBuffer(pRspXml);
		//}
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  IVS_SDK_ReleaseBuffer(pRspXml);
  //  return strOutXml.c_str();
}

// 视频质量诊断详情查询
CString COCXReportMgr::OMUQueryQDReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
	COCXXmlProcess::GetErrString(strResult, IVS_FAIL);
	return strResult;
  //  IVS_CHAR* pRspXml = NULL;
  //  IVS_INT32 iRet = IVS_SDK_OMUQueryQDReportDetail(iSessionID, pReqXml, &pRspXml);
  //  if (IVS_SUCCEED != iRet || NULL == pRspXml)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "OMUQueryQDReportDetail SDK Interface fail", "NA");
  //      if (NULL != pRspXml)
  //      {
  //          IVS_SDK_ReleaseBuffer(pRspXml);
  //      }
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  std::string strInXml = pRspXml;
  //  std::string strOutXml;
  //  iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
  //  if (IVS_SUCCEED != iRet)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
		//if (NULL != pRspXml)//lint !e774
		//{
		//	IVS_SDK_ReleaseBuffer(pRspXml);
		//}
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  IVS_SDK_ReleaseBuffer(pRspXml);
  //  return strOutXml.c_str();
}

// 录像执行计划报表查询
CString COCXReportMgr::OMUQueryRecordPlanReport(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
	COCXXmlProcess::GetErrString(strResult, IVS_FAIL);
	return strResult;
  //  IVS_CHAR* pRspXml = NULL;
  //  IVS_INT32 iRet = IVS_SDK_OMUQueryRecordPlanReport(iSessionID, pReqXml, &pRspXml);
  //  if (IVS_SUCCEED != iRet || NULL == pRspXml)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "OMUQueryRecordPlanReport SDK Interface fail", "NA");
  //      if (NULL != pRspXml)
  //      {
  //          IVS_SDK_ReleaseBuffer(pRspXml);
  //      }
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  std::string strInXml = pRspXml;
  //  std::string strOutXml;
  //  iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
  //  if (IVS_SUCCEED != iRet)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
		//if (NULL != pRspXml)//lint !e774
		//{
		//	IVS_SDK_ReleaseBuffer(pRspXml);
		//}
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  IVS_SDK_ReleaseBuffer(pRspXml);
  //  return strOutXml.c_str();
}

// 录像执行计划报表详情查询
CString COCXReportMgr::OMUQueryRecordPlanReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");

	CString strResult = NULL;
	COCXXmlProcess::GetErrString(strResult, IVS_FAIL);
	return strResult;
  //  IVS_CHAR* pRspXml = NULL;
  //  IVS_INT32 iRet = IVS_SDK_OMUQueryRecordPlanReportDetail(iSessionID, pReqXml, &pRspXml);
  //  if (IVS_SUCCEED != iRet || NULL == pRspXml)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "OMUQueryRecordPlanReportDetail SDK Interface fail", "NA");
  //      if (NULL != pRspXml)
  //      {
  //          IVS_SDK_ReleaseBuffer(pRspXml);
  //      }
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  std::string strInXml = pRspXml;
  //  std::string strOutXml;
  //  iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
  //  if (IVS_SUCCEED != iRet)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
		//if (NULL != pRspXml)//lint !e774
		//{
		//	IVS_SDK_ReleaseBuffer(pRspXml);
		//}
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  IVS_SDK_ReleaseBuffer(pRspXml);
  //  return strOutXml.c_str();
}

// 录像完整性报表查询
CString COCXReportMgr::OMUQueryRecordIntegralityReport(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
    IVS_CHAR* pRspXml = NULL;
    IVS_INT32 iRet = IVS_SDK_OMUQueryRecordIntegralityReport(iSessionID, pReqXml, &pRspXml);
    if (IVS_SUCCEED != iRet || NULL == pRspXml)
    {
        BP_RUN_LOG_ERR(iRet, "OMUQueryRecordIntegralityReport SDK Interface fail", "NA");
        if (NULL != pRspXml)
        {
            IVS_SDK_ReleaseBuffer(pRspXml);
        }
        COCXXmlProcess::GetErrString(strResult, iRet);
        return strResult;
    }
    std::string strInXml = pRspXml;
    std::string strOutXml;
    iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
		if (NULL != pRspXml)//lint !e774
		{
			IVS_SDK_ReleaseBuffer(pRspXml);
		}
        COCXXmlProcess::GetErrString(strResult, iRet);
        return strResult;
    }
    IVS_SDK_ReleaseBuffer(pRspXml);
    return strOutXml.c_str();
}

// 录像完整性报表详情查询
CString COCXReportMgr::OMUQueryRecordIntegralityReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");

	CString strResult = NULL;
	COCXXmlProcess::GetErrString(strResult, IVS_FAIL);
	return strResult;
  //  IVS_CHAR* pRspXml = NULL;
  //  IVS_INT32 iRet = IVS_SDK_OMUQueryRecordIntegralityReportDetail(iSessionID, pReqXml, &pRspXml);
  //  if (IVS_SUCCEED != iRet || NULL == pRspXml)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "OMUQueryRecordIntegralityReportDetail SDK Interface fail", "NA");
  //      if (NULL != pRspXml)
  //      {
  //          IVS_SDK_ReleaseBuffer(pRspXml);
  //      }
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  std::string strInXml = pRspXml;
  //  std::string strOutXml;
  //  iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
  //  if (IVS_SUCCEED != iRet)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
		//if (NULL != pRspXml)//lint !e774
		//{
		//	IVS_SDK_ReleaseBuffer(pRspXml);
		//}
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  IVS_SDK_ReleaseBuffer(pRspXml);
  //  return strOutXml.c_str();
}

// 丢包率报表查询
CString COCXReportMgr::OMUQueryPacketLossRateReport(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");

	CString strResult = NULL;
	COCXXmlProcess::GetErrString(strResult, IVS_FAIL);
	return strResult;

  //  IVS_CHAR* pRspXml = NULL;
  //  IVS_INT32 iRet = IVS_SDK_OMUQueryPacketLossRateReport(iSessionID, pReqXml, &pRspXml);
  //  if (IVS_SUCCEED != iRet || NULL == pRspXml)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "OMUQueryPacketLossRateReport SDK Interface fail", "NA");
  //      if (NULL != pRspXml)
  //      {
  //          IVS_SDK_ReleaseBuffer(pRspXml);
  //      }
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  std::string strInXml = pRspXml;
  //  std::string strOutXml;
  //  iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
  //  if (IVS_SUCCEED != iRet)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
		//if (NULL != pRspXml)//lint !e774
		//{
		//	IVS_SDK_ReleaseBuffer(pRspXml);
		//}
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  IVS_SDK_ReleaseBuffer(pRspXml);
  //  return strOutXml.c_str();
}

// 丢包率报表详情查询
CString COCXReportMgr::OMUQueryPacketLossRateReportDetail(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");

	CString strResult = NULL;
	COCXXmlProcess::GetErrString(strResult, IVS_FAIL);
	return strResult;

  //  IVS_CHAR* pRspXml = NULL;
  //  IVS_INT32 iRet = IVS_SDK_OMUQueryPacketLossRateReportDetail(iSessionID, pReqXml, &pRspXml);
  //  if (IVS_SUCCEED != iRet || NULL == pRspXml)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "OMUQueryPacketLossRateReportDetail SDK Interface fail", "NA");
  //      if (NULL != pRspXml)
  //      {
  //          IVS_SDK_ReleaseBuffer(pRspXml);
  //      }
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  std::string strInXml = pRspXml;
  //  std::string strOutXml;
  //  iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
  //  if (IVS_SUCCEED != iRet)
  //  {
  //      BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
		//if (NULL != pRspXml)//lint !e774
		//{
		//	IVS_SDK_ReleaseBuffer(pRspXml);
		//}
  //      COCXXmlProcess::GetErrString(strResult, iRet);
  //      return strResult;
  //  }
  //  IVS_SDK_ReleaseBuffer(pRspXml);
  //  return strOutXml.c_str();
}

// 磁盘存储占用率报表查询
CString COCXReportMgr::OMUQueryDiskUsageRateReport(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
    IVS_CHAR* pRspXml = NULL;
    IVS_INT32 iRet = IVS_SDK_OMUQueryDiskUsageRateReport(iSessionID, pReqXml, &pRspXml);
    if (IVS_SUCCEED != iRet || NULL == pRspXml)
    {
        BP_RUN_LOG_ERR(iRet, "OMUQueryDiskUsageRateReport SDK Interface fail", "NA");
        if (NULL != pRspXml)
        {
            IVS_SDK_ReleaseBuffer(pRspXml);
        }
        COCXXmlProcess::GetErrString(strResult, iRet);
        return strResult;
    }
    std::string strInXml = pRspXml;
    std::string strOutXml;
    iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
		if (NULL != pRspXml)//lint !e774
		{
			IVS_SDK_ReleaseBuffer(pRspXml);
		}
        COCXXmlProcess::GetErrString(strResult, iRet);
        return strResult;
    }
    IVS_SDK_ReleaseBuffer(pRspXml);
    return strOutXml.c_str();
}

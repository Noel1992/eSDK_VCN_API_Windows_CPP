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

#include "OCXCapturePicture.h"
#include "OCXXmlProcess.h"
#include "OCXCapturePictureXMLProcess.h"
#include "IVS_Trace.h"
#include <time.h>

OCXCapturePicture::OCXCapturePicture(void)
{
}


OCXCapturePicture::~OCXCapturePicture(void)
{
}

// 获取平台抓拍列表
IVS_INT32 OCXCapturePicture::GetSnapshotList(IVS_INT32 iSessionId, const IVS_CHAR* pQueryParam, CString &strResult)
{
	if (NULL == pQueryParam)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Snapshot List", "param is null");
		return IVS_XML_INVALID;
	}

	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	CXml xmlRsp;
 	IVS_CHAR cCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	if (!xmlReq.Parse(pQueryParam))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"IVS OCX Query Record List Failed", "Reason: Parse pUnifiedQuery to Xml Failed.");
		return IVS_XML_INVALID;
	}

	IVS_QUERY_SNAPSHOT_PARAM pSnapshotParam;
	eSDK_MEMSET(&pSnapshotParam, 0, sizeof(IVS_QUERY_SNAPSHOT_PARAM));
    IVS_INT32 iRet = OCXCapturePictureXMLProcess::GetSnapshotListParseXml(xmlReq, &pSnapshotParam, cCameraCode);

	if (IVS_SUCCEED !=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Snapshot List", "failed");
		return iRet;
	}
	IVS_UINT32 iTotalNum = (pSnapshotParam.stIndexRange.uiToIndex) - (pSnapshotParam.stIndexRange.uiFromIndex);
	iTotalNum = iTotalNum + 1;

	IVS_UINT32 uiBufSize = sizeof(IVS_SNAPSHOT_INFO_LIST) + (iTotalNum - 1) * sizeof(IVS_SNAPSHOT_INFO);
	IVS_VOID* pBuff = IVS_NEW((IVS_CHAR *&)pBuff, uiBufSize);
	eSDK_MEMSET(pBuff, 0, uiBufSize);
	IVS_SNAPSHOT_INFO_LIST* pSnapshotList = reinterpret_cast<IVS_SNAPSHOT_INFO_LIST*>(pBuff);
	pSnapshotList->uiTotal = iTotalNum;
	iRet = IVS_SDK_GetSnapshotList(iSessionId, cCameraCode, &pSnapshotParam, pSnapshotList, uiBufSize);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Get Snapshot List", "SDK failed");
		IVS_DELETE(pBuff, MUILI);
		return iRet;
	}

	//拼装图片列表xml
	iRet = OCXCapturePictureXMLProcess::GetSnapshotListGetXml(xmlRsp, pSnapshotList, iTotalNum);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"OCX Get Snapshot List Get Xml", "failed");
		IVS_DELETE(pBuff, MUILI);
		return iRet;
	}
	IVS_UINT32 iLen = 0;
	IVS_DELETE(pBuff, MUILI);
	//把xml生成string
	strResult = xmlRsp.GetXMLStream(iLen);
	return iRet;
}

// 设置平台抓拍计划
LONG OCXCapturePicture:: SetSnapshotPlan(const CString& strSnapshotPlan,const IVS_INT32 iSessionID)
{
	IVS_DEBUG_TRACE("");

	//CXml xmlReq;
	//if (!xmlReq.Parse(strSnapshotPlan))
	//{
	//	BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Set Snapshot Plan Failed.", "Reason:xml Parse Fail.");
	//	return IVS_XML_INVALID;
	//}

	//IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	//IVS_SNAPSHOT_PLAN stSnapshotPlan;
	//IVS_CHAR pCameraCode[IVS_DEV_CODE_LEN+1] = {0};
	////eSDK_MEMSET(pCameraCode,0,sizeof(IVS_CHAR));
	//eSDK_MEMSET(&stSnapshotPlan,0,sizeof(IVS_SNAPSHOT_PLAN));
	//if(IVS_SUCCEED!=OCXCapturePictureXMLProcess::SetSnapshotPlanGetXML(pCameraCode,&stSnapshotPlan,xmlReq))
	//{
	//	iRet = static_cast<IVS_INT32>(IVS_XML_INVALID);
	//	BP_RUN_LOG_ERR(IVS_XML_INVALID,"Set Snapshot Plan", "ErrorCode =%d", iRet);
	//	return iRet;
	//}
	////IVS_UINT32 uiPlanInfoNodeNum = 0;
	////iRet = static_cast<IVS_INT32>(COCXXmlProcess::GetElemCount("Content/SnapshotPlanInfo/PlanList",uiPlanInfoNodeNum,xmlReq));
	////stSnapshotPlan->uiPlanInfoNum = uiPlanInfoNodeNum;

	//iRet = IVS_SDK_SetSnapshotPlan(iSessionID,pCameraCode,&stSnapshotPlan);
	return IVS_FAIL;
}

// 删除平台抓拍计划
IVS_INT32 OCXCapturePicture::DelSnapshotPlan(IVS_INT32 iSessionId, const IVS_CHAR* pCameraCode)
{
	if (NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Delelte Snapshot", "param is null");
		return IVS_XML_INVALID;
	}

	//IVS_DEBUG_TRACE("");
	//IVS_INT32 iRet = IVS_SDK_DelSnapshotPlan(iSessionId, pCameraCode);

	//if (IVS_SUCCEED != iRet)
	//{
	//	BP_RUN_LOG_ERR(iRet, "OCX Delelte Snapshot Plan", "IVS SDK Del Snapshot Plan failed");
	//	return iRet;
	//}

	return IVS_FAIL;
}

//查询平台抓拍计划
CString OCXCapturePicture::GetSnapshotPlan(const CString& strCameraCode,const IVS_INT32 iSessionID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);	

	CString strResult;
	COCXXmlProcess::GetErrString(strResult, IVS_FAIL);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_NO_SESSION(reqTime,rspTime,IVS_FAIL,"");
	return strResult;

	//CString strResult;
	//CXml xmlRsp;
	//IVS_UINT32 uiXmlLen = 0;
	//IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	//IVS_SNAPSHOT_PLAN stSnapshotPlan;
	//eSDK_MEMSET(&stSnapshotPlan,0,sizeof(IVS_SNAPSHOT_PLAN));
	//CString cCameraCode = strCameraCode;
	//IVS_CHAR* pCameraCode = cCameraCode.GetBuffer();

	//iRet = IVS_SDK_GetSnapshotPlan(iSessionID,pCameraCode,&stSnapshotPlan);
	//if (IVS_SUCCEED != iRet)
	//{
	//	BP_RUN_LOG_ERR(iRet,"Get Snapshot Plan", "failed");
	//	COCXXmlProcess::GetErrString(strResult,iRet);
	//	return strResult;
	//}

	//iRet = OCXCapturePictureXMLProcess::GetSnapshotPlanGetXML(iRet,pCameraCode,&stSnapshotPlan,xmlRsp);
	//if (IVS_SUCCEED != iRet)
	//{
	//	BP_RUN_LOG_ERR(iRet,"Get Snapshot Plan", "Get Snapshot Plan Rsp XML failed");
	//	COCXXmlProcess::GetErrString(strResult,iRet);
	//	return strResult;
	//}

	//strResult = xmlRsp.GetXMLStream(uiXmlLen);
	//return strResult;
}


// 删除平台抓拍图片
IVS_INT32 OCXCapturePicture::DeleteSnapshot(IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_UINT32 uiPictureID)
{
	if (NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Delete Snapshot", "Camera Code is null");
		return IVS_XML_INVALID;
	}

	IVS_DEBUG_TRACE("");
	return IVS_SDK_DeleteSnapshot(iSessionID, pCameraCode, uiPictureID);
}


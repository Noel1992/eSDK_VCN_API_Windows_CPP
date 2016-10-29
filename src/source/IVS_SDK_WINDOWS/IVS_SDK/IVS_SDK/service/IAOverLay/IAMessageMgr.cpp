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

#include "IAMessageMgr.h"
#include "ToolsHelp.h"
#include "IVSCommon.h"
#include "IVS_SDK.h"
#include "IAXMLProcess.h"
#include "IVS_Trace.h"
#include "SDKConfig.h"
#include "UserMgr.h"
#include "SDKInterface.h"

#define IA_VIDEO_DELAY_TIME_MIN 20
#define IA_VIDEO_DELAY_TIME_MAX 2000

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

extern CSDKInterface *g_pNvs ;//lint -e526

/****************************************************************************/
/*								CIAMessageMgr																*/
/*								管理轨迹消息与Panel之间的对应关系										*/
/****************************************************************************/

CIAMessageMgr::CIAMessageMgr()
{
	m_bInited = true;
	m_ulRequestID = IA_INVALID_REQUEST_ID;
	m_uiPUIAVideoDelayValue = 1000;
	eSDK_MEMSET(&m_IAReqParam, 0x0, sizeof(IA_REQ_PARAM));

	m_MutexMap = VOS_CreateMutex();
}

CIAMessageMgr::~CIAMessageMgr()
{
	if (NULL != m_MutexMap)
	{
		(void)VOS_DestroyMutex(m_MutexMap);
		m_MutexMap = NULL;
	}

	try
	{
		IAMESSAGE_MAP_ITER iterMap = m_IAMessageMap.begin();
		IAMESSAGE_MAP_ITER iterMapEnd  = m_IAMessageMap.end();
		for ( ; iterMap != iterMapEnd; iterMap++)
		{
			CIAObject *pIAObject = iterMap->second;
			if (NULL != pIAObject)
			{
				IVS_DELETE(pIAObject);
			}
		}
		m_bInited  = false;
	}
	catch (...)
	{
	}
}

CIAMessageMgr &CIAMessageMgr::Instance()
{
	static CIAMessageMgr Mgr;
	return Mgr;
}

//Start Playback IA OverLay
IVS_ULONG CIAMessageMgr::StartOverlay(CIAOverLayMgr *pVideoPane)
{
	CHECK_POINTER(pVideoPane, IA_INVALID_REQUEST_ID);
	IVS_DEBUG_TRACE("CameraCode: %s, NVRCode: %s, PlayHandle: %lu, RuleType: %lu, QueryType: %lu", 
		pVideoPane->GetCameraID(), pVideoPane->GetNVRCode(), pVideoPane->GetPlayHandle(), pVideoPane->GetRuleType(), pVideoPane->GetQueryType());

	//get request param.
	IA_REQ_PARAM IAReqParam;
	eSDK_MEMSET(&IAReqParam, 0x0, sizeof(IAReqParam));

	IVS_INT32 iRet = GetReqParam(pVideoPane, IAReqParam);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IA_INVALID_REQUEST_ID, "StartOverlay", "GetReqParam Failed");
		return IA_INVALID_REQUEST_ID;
	}

	return StartRealtimeOverlay(pVideoPane, IAReqParam);
}

IVS_ULONG CIAMessageMgr::AllocRequestID()
{
	return ++m_ulRequestID;
}

IVS_INT32 CIAMessageMgr::GetReqParam(const CIAOverLayMgr *pVideoPane, IA_REQ_PARAM &IAReqParam) const
{
	CHECK_POINTER(pVideoPane, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("CameraCode: %s, NVRCode: %s, RealPlayHandle: %lu, RuleType: %lu, QueryType: %lu", 
		pVideoPane->GetCameraID(), pVideoPane->GetNVRCode(), pVideoPane->GetPlayHandle(), pVideoPane->GetRuleType(), pVideoPane->GetQueryType());

	IAReqParam.ulQueryType = pVideoPane->GetQueryType();
	IAReqParam.ulRuleType = pVideoPane->GetRuleType();
	CHECK_SUCCESS(CToolsHelp::Strncpy(IAReqParam.CameraID, IVS_DEV_CODE_LEN + 1, pVideoPane->GetCameraID(), IVS_DEV_CODE_LEN));
	CHECK_SUCCESS(CToolsHelp::Strncpy(IAReqParam.NVRCode, IVS_NVR_CODE_LEN + 1, pVideoPane->GetNVRCode(), IVS_NVR_CODE_LEN));
	CHECK_SUCCESS(CToolsHelp::Strncpy(IAReqParam.StartTime, IVS_TIME_LEN + 1, pVideoPane->GetTimeSpan()->cStart, IVS_TIME_LEN));
	CHECK_SUCCESS(CToolsHelp::Strncpy(IAReqParam.StartTime, IVS_TIME_LEN + 1, pVideoPane->GetTimeSpan()->cEnd, IVS_TIME_LEN));

	return IVS_SUCCEED;
}

IVS_ULONG CIAMessageMgr::StartRealtimeOverlay(CIAOverLayMgr *pVideoPane, IA_REQ_PARAM &IAReqParam)
{
	CHECK_POINTER(pVideoPane, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("CameraCode: %s, NVRCode: %s, RealPlayHandle: %lu", pVideoPane->GetCameraID(), pVideoPane->GetNVRCode(), pVideoPane->GetPlayHandle());

	//send request message
	IVS_INT32 iResult = SendenableMessage(pVideoPane->GetSessionID(), IAReqParam, NULL);
	if (IVS_SUCCEED != iResult)
	{
		BP_RUN_LOG_ERR(IA_INVALID_REQUEST_ID, "StartRealtimeOverlay", "SendenableMessage failed, return: %d", iResult);
		return IA_INVALID_REQUEST_ID;
	}

	//set realplay dealy time
	iResult = SetPlayDelayTime(pVideoPane);
	if (IVS_SUCCEED != iResult)
	{
		BP_RUN_LOG_ERR(IA_INVALID_REQUEST_ID, "StartRealtimeOverlay", "SetPlayDelayTime failed, return: %d", iResult);
		return IA_INVALID_REQUEST_ID;
	}

	//insert panel into map, used to insert parse overlay message
	iResult = InsertIAObject(pVideoPane, IAReqParam);
	if (IVS_SUCCEED != iResult)
	{
		return IA_INVALID_REQUEST_ID;
	}
	return IAReqParam.ulRequestID;

}

// 尝试流复用
IVS_ULONG CIAMessageMgr::TryRealStreamShare(CIAOverLayMgr *pVideoPane, 
	const IA_REQ_PARAM &IAReqParam)
{
	CHECK_POINTER(pVideoPane, IA_INVALID_REQUEST_ID);
	IVS_DEBUG_TRACE("CameraCode: %s, NVRCode: %s, RealPlayHandle: %lu", pVideoPane->GetCameraID(), pVideoPane->GetNVRCode(), pVideoPane->GetPlayHandle());
	(void)VOS_MutexLock(m_MutexMap);

	IAMESSAGE_MAP_ITER iterMap = m_IAMessageMap.begin();
	IAMESSAGE_MAP_ITER iterMapEnd = m_IAMessageMap.end();
	while (iterMap != iterMapEnd) 
	{
		CIAObject *pIAObject = iterMap->second;
		if (NULL == pIAObject)
		{
			iterMap++;
			continue;
		}
		if ((IAReqParam.ulRuleType == pIAObject->m_ulRuleType) 
			&& (IAReqParam.ulQueryType == pIAObject->m_ulQueryType) 
			&& (0 == strcmp(IAReqParam.CameraID, pIAObject->m_szCameraID))
			&& (0 == strcmp(IAReqParam.NVRCode, pIAObject->m_szNVRCode))
			&& (0 == strcmp(IAReqParam.StartTime, pIAObject->m_szStartTime)))
		{
			(void)pIAObject->InsertPanel(pVideoPane);
			(void)VOS_MutexUnlock(m_MutexMap);
			return iterMap->first;
		}
		iterMap++;
	}

	(void)VOS_MutexUnlock(m_MutexMap);
	return IA_INVALID_REQUEST_ID;
}

IVS_INT32 CIAMessageMgr::SendenableMessage(IVS_INT32 iSessionID, IA_REQ_PARAM &IAReqParam, IVS_CHAR *pTransID)const
{
	IVS_DEBUG_TRACE("");
	CXml xml;
	IVS_INT32 iRet = CIAXMLProcess::GetAnalyzeResultBuildXML(IAReqParam, xml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "SendMessageReqXML", "GetAnalyzeResultBuildXML failed");
		return iRet;
	}

	// send cmd
	IVS_UINT32 uiLen = 0;
	const IVS_CHAR *ReqXml = xml.GetXMLStream(uiLen);
	IVS_CHAR* pRspXml = NULL;

	// 获取用户管理类
	CHECK_POINTER(g_pNvs, IVS_NOT_INIT);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	// 发送NSS智能分析消息
	if (NULL == pTransID)	//lint -e774
	{
		iRet = pUserMgr->GetIntelligenceAnalysisMgr().IntelligenceAnalysis(NSS_GET_ANALYZE_RESULT_REQ, NULL, 0, ReqXml, &pRspXml);
	} 
	else 
	{
		iRet = pUserMgr->GetIntelligenceAnalysisMgr().IntelligenceAnalysis(NSS_GET_ANALYZE_RESULT_REQ, pTransID, TRANS_ID_LENTH + 1, ReqXml, &pRspXml);
	}
	// 如果发送NSS智能分析消息成功，并且响应的数据不为空
	if (IVS_SUCCEED == iRet && pRspXml != NULL)
	{
		IVS_INT32 ulResultCode = IVS_FAIL;
		iRet = CIAXMLProcess::GetAnalyzeResultPraseXML(pRspXml, ulResultCode, IAReqParam);
	}
	else
	{
		// 如果返回成功，但是响应的数据为空
		if (IVS_SUCCEED == iRet)
		{
			BP_RUN_LOG_ERR(IVS_SMU_CMD_XML_IS_NULL, "SendMessageStopXML", "IntelligenceAnalysis success, but RspXml is NULL");
			return IVS_SMU_CMD_XML_IS_NULL;
		}
		// 如果返回失败
		else
		{
			BP_RUN_LOG_ERR(iRet, "SendMessageStopXML", "IntelligenceAnalysis failed");
			// 释放内部开辟的内存
			(void)g_pNvs->ReleaseBuffer(pRspXml);
			return iRet;
		}
	}

	// 解析智能分析请求轨迹报文失败
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "SendMessageStopXML", "GetAnalyzeResultPraseXML failed");
	}
	// 释放内部开辟的内存
	(void)g_pNvs->ReleaseBuffer(pRspXml);
	return iRet;
}

// 发送停止请求轨迹XML消息
IVS_INT32 CIAMessageMgr::SendMessageStopXML(const CIAOverLayMgr *pVideoPane, IVS_ULONG ulRequestID, IVS_CHAR *pTransID)const
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pVideoPane, IVS_PARA_INVALID);

	CXml xml;
	IVS_INT32 iRet = CIAXMLProcess::StopAnalyzeResultBuildXML(ulRequestID, pVideoPane->GetRuleType(), 
		pVideoPane->GetQueryType(), pVideoPane->GetCameraID(), xml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "SendMessageStopXML", "StopAnalyzeResultBuildXML failed");
		return iRet;
	}

	IVS_UINT32 uiLen = 0;
	const IVS_CHAR *ReqXml = xml.GetXMLStream(uiLen);
	IVS_CHAR* pRspXml = NULL;

	// 获取用户管理类
	CHECK_POINTER(g_pNvs, IVS_NOT_INIT);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(pVideoPane->GetSessionID());
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	// 发送NSS智能分析消息
	if (NULL == pTransID)	//lint -e774
	{
		iRet = pUserMgr->GetIntelligenceAnalysisMgr().IntelligenceAnalysis(NSS_STOP_PUSH_ANALYZE_RESULT_REQ, NULL, 0, ReqXml, &pRspXml);
	} 
	else 
	{
		iRet = pUserMgr->GetIntelligenceAnalysisMgr().IntelligenceAnalysis(NSS_STOP_PUSH_ANALYZE_RESULT_REQ, pTransID, TRANS_ID_LENTH + 1, ReqXml, &pRspXml);
	}
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "SendMessageStopXML", "IntelligenceAnalysis failed");
	}

	// 释放内部开辟的内存
	iRet = g_pNvs->ReleaseBuffer(pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "SendMessageStopXML", "ReleaseBuffer failed");
	}

	return iRet;

}


IVS_INT32 CIAMessageMgr::InsertIAObject(CIAOverLayMgr *pVideoPane, IA_REQ_PARAM &IAReqParam)
{
	CHECK_POINTER(pVideoPane, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("CameraCode: %s, NVRCode: %s, RealPlayHandle: %lu", pVideoPane->GetCameraID(), pVideoPane->GetNVRCode(), pVideoPane->GetPlayHandle());

	(void)VOS_MutexLock(m_MutexMap);
	IAMESSAGE_MAP_ITER iterMap = m_IAMessageMap.find(IAReqParam.ulRequestID);
	if (m_IAMessageMap.end() != iterMap)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Insert IA Object", "RequestID Exists");
		(void)VOS_MutexUnlock(m_MutexMap);
		return IVS_FAIL;
	}

	CIAObject *pNewIAObject = NULL;
	try	{
		pNewIAObject = new CIAObject(IAReqParam.ulRuleType, IAReqParam.ulQueryType);
	} 
	catch(...)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Insert IA Object", "Create IAObject Failed");
		(void)VOS_MutexUnlock(m_MutexMap);
		return IVS_ALLOC_MEMORY_ERROR;
	}

	(void)pNewIAObject->SetCameraID(IAReqParam.CameraID);
	(void)pNewIAObject->SetNVRCode(IAReqParam.NVRCode);
	(void)pNewIAObject->InsertPanel(pVideoPane);
	(void)m_IAMessageMap.insert(std::make_pair(IAReqParam.ulRequestID, pNewIAObject));

	(void)VOS_MutexUnlock(m_MutexMap);
	return IVS_SUCCEED;
}

IVS_INT32 CIAMessageMgr::IAPushAlarmLocus(const IVS_CHAR *pIAMessage)
{
	IVS_DEBUG_TRACE("");

	IA_LAYOUT_MESSAGE* pIALayoutMessage = (IA_LAYOUT_MESSAGE* )IVS_NEW((CHAR* &)pIALayoutMessage, sizeof(IA_LAYOUT_MESSAGE));	//lint !e826
	if (NULL == pIALayoutMessage)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IAPushAlarmLocus", "new IA_LAYOUT_MESSAGE failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	IA_REQ_PARAM	  IAReqParameters;
	eSDK_MEMSET(pIALayoutMessage, 0, sizeof(IA_LAYOUT_MESSAGE));
	eSDK_MEMSET(&IAReqParameters, 0, sizeof(IAReqParameters));
	IVS_INT32 iRet = CIAXMLProcess::TrackInfoContentPraseXML(pIAMessage, 
		*pIALayoutMessage, IAReqParameters);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IA Push Alarm Locus", "Prase Track Info XML Failed");
		IVS_DELETE(pIALayoutMessage, MUILI);
		return iRet;
	}
	iRet = InsertTrackInfo(IAReqParameters, *pIALayoutMessage);
	IVS_DELETE(pIALayoutMessage, MUILI);
	return iRet;
}


IVS_INT32 CIAMessageMgr::InsertTrackInfo(const IA_REQ_PARAM &IAReqParameters, 
	const IA_LAYOUT_MESSAGE &IALayoutMessage)
{
	IVS_DEBUG_TRACE("RequestID: %lu, QueryType: %lu, FrameID: %I64u", IAReqParameters.ulRequestID, IAReqParameters.ulQueryType, IALayoutMessage.ulFrameID);
	
	(void)VOS_MutexLock(m_MutexMap);
	CIAObject *pIAObject = NULL;

	if (IA_QUERY_TYPE_REALTIME == IAReqParameters.ulQueryType)
	{
		IAMESSAGE_MAP_ITER iterMap = m_IAMessageMap.begin();
		IAMESSAGE_MAP_ITER iterMapEnd = m_IAMessageMap.end();
		for (; iterMap != iterMapEnd; iterMap++) 
		{
			CIAObject *ptmpIAObject = iterMap->second;
			if (NULL == ptmpIAObject)
			{
				continue;
			}
			if ((IAReqParameters.ulRuleType == ptmpIAObject->m_ulRuleType) 
				&& (IAReqParameters.ulQueryType == ptmpIAObject->m_ulQueryType) 
				&& (0 == strncmp(IAReqParameters.CameraID, ptmpIAObject->m_szCameraID, strlen(IAReqParameters.CameraID)))
				&& (0 == strncmp(IAReqParameters.NVRCode, ptmpIAObject->m_szNVRCode, strlen(IAReqParameters.NVRCode))))
			{
				pIAObject = ptmpIAObject;
			}
		}
		if (NULL == pIAObject)
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "Insert Track Info", "RealPlay Panel Not Found");
			(void)VOS_MutexUnlock(m_MutexMap);
			return IVS_FAIL;
		}
	} 
	else 
	{
		// 根据RequestID查找节点
		IAMESSAGE_MAP_ITER iterMap = m_IAMessageMap.find(IAReqParameters.ulRequestID);
		if (m_IAMessageMap.end() == iterMap)
		{
			(void)VOS_MutexUnlock(m_MutexMap);
			BP_RUN_LOG_ERR(IVS_FAIL, "Insert Track Info", "Record Panel Not Found");
			return IVS_FAIL;
		}
		pIAObject = iterMap->second;
		if (NULL == pIAObject)
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "Insert Track Info", "IAObject NULL");
			(void)VOS_MutexUnlock(m_MutexMap);
			return IVS_FAIL;
		}
	}

	PANEL_ITER IterPanel = pIAObject->m_VideoPaneList.begin();
	PANEL_ITER IterPanelEnd = pIAObject->m_VideoPaneList.end();
	while (IterPanel != IterPanelEnd)
	{
		CIAOverLayMgr *pVideoPane = *IterPanel++;
		if (NULL == pVideoPane)
		{
			continue;
		}

		IVS_INT32 iRet = pVideoPane->InsertIALayoutMessage(IALayoutMessage);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Insert Track Info", "Insert Failed");
			(void)VOS_MutexUnlock(m_MutexMap);
			return iRet;
		}
	}

	(void)VOS_MutexUnlock(m_MutexMap);
	return IVS_SUCCEED;
}

IVS_INT32 CIAMessageMgr::StopOverlay(const CIAOverLayMgr *pVideoPane, IVS_ULONG &ulRequestID)
{
	CHECK_POINTER(pVideoPane, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("CameraCode: %s, NVRCode: %s, PlayHandle: %lu, RequestID: %lu", 
		pVideoPane->GetCameraID(), pVideoPane->GetNVRCode(), pVideoPane->GetPlayHandle(), ulRequestID);

	DeletePanelInMap(ulRequestID, pVideoPane);

	// 获取用户管理类
	CHECK_POINTER(g_pNvs, IVS_NOT_INIT);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(pVideoPane->GetSessionID());
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	// 根据用户获取媒体基类
	CMediaBase* mediaBase = pUserMgr->GetRealPlayMgr().GetMediaBase(pVideoPane->GetPlayHandle());
	if (NULL == mediaBase)
	{
		mediaBase = pUserMgr->GetPlaybackMgr().GetMediaBase(pVideoPane->GetPlayHandle());
	}
	// 设置延迟
	if(NULL != mediaBase)
	{
		(void)mediaBase->SetDelayFrameNum(0);
	}

	ulRequestID = IA_INVALID_REQUEST_ID;
	return IVS_SUCCEED;
}

void CIAMessageMgr::DeletePanelInMap(IVS_ULONG ulRequestID, const CIAOverLayMgr *pVideoPane) 
{
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Delete Panel In Map", "Panel is NULL");
		return;
	}
	IVS_DEBUG_TRACE("CameraCode: %s, NVRCode: %s, RealPlayHandle: %lu, RequestID: %lu", 
		pVideoPane->GetCameraID(), pVideoPane->GetNVRCode(), pVideoPane->GetPlayHandle(), ulRequestID);

	(void)VOS_MutexLock(m_MutexMap);
	// 根据RequestID查找节点
	IAMESSAGE_MAP_ITER iterMap = m_IAMessageMap.find(ulRequestID);
	if (m_IAMessageMap.end() == iterMap)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Delete Panel In Map", "Panel Not Found");
		(void)VOS_MutexUnlock(m_MutexMap);
		return;
	}

	CIAObject *pIAObject = iterMap->second;
	if (NULL == pIAObject)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Delete Panel In Map", "IAObject is NULL");
		(void)VOS_MutexUnlock(m_MutexMap);
		return;
	}
	if (IA_QUERY_TYPE_REALTIME == pIAObject->m_ulQueryType 
		&& 1 == pIAObject->GetPanelNum() 
		&& IVS_SUCCEED != SendMessageStopXML(pVideoPane, 0, NULL))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Delete Panel In Map", "Send Stop Push Failed");
	}

	// 删除map节点中PanelList中对应Panel指针
	(void)pIAObject->DeletePanel(pVideoPane);
	// 若仅存在一个Panel指针,则删除整个map节点
	if (pIAObject->CheckPanelListNull())
	{
		IVS_DELETE(pIAObject);
		iterMap = m_IAMessageMap.erase(iterMap);
	}

	(void)VOS_MutexUnlock(m_MutexMap);
	return;
}

//set realplay dealy time
IVS_INT32 CIAMessageMgr::SetPlayDelayTime(const CIAOverLayMgr *pVideoPane) const
{
	IVS_DEBUG_TRACE("");
	IVS_MEDIA_INFO strMediaInfo;
	eSDK_MEMSET(&strMediaInfo,0,sizeof(IVS_MEDIA_INFO));
	IVS_INT32 iResult = IVS_SDK_GetMediaInfo(pVideoPane->GetSessionID(), pVideoPane->GetPlayHandle(), &strMediaInfo);
	if (IVS_SUCCEED != iResult)
	{
		BP_RUN_LOG_ERR(iResult, "StartRealtimeOverlay", "IVS_SDK_GetMediaInfo failed");
		return iResult;
	}

	IVS_UINT32 uiIAVideoDelayTime = 0;
	if (IVS_ABILITY_IA_QD == pVideoPane->GetRuleType())
	{
		//获取前端延时时间设置
		uiIAVideoDelayTime = CSDKConfig::instance().GetPUIAVideoDelayValue();
		uiIAVideoDelayTime = min(uiIAVideoDelayTime, IA_VIDEO_DELAY_TIME_MAX);
		uiIAVideoDelayTime = max(uiIAVideoDelayTime, IA_VIDEO_DELAY_TIME_MIN);
	}
	else
	{
		//获取平台延时时间设置
		uiIAVideoDelayTime = CSDKConfig::instance().GetPlatformIAVideoDelayValue();
		uiIAVideoDelayTime = min(uiIAVideoDelayTime, IA_VIDEO_DELAY_TIME_MAX);
		uiIAVideoDelayTime = max(uiIAVideoDelayTime, IA_VIDEO_DELAY_TIME_MIN);
	}

	IVS_UINT32 uiIAVideoDelayFrame = (strMediaInfo.uiVideoFrameRate 
		* uiIAVideoDelayTime) / 1000;

	// 获取用户管理类
	CHECK_POINTER(g_pNvs, IVS_NOT_INIT);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(pVideoPane->GetSessionID());
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	// 根据用户获取媒体基类
	CMediaBase* mediaBase = pUserMgr->GetRealPlayMgr().GetMediaBase(pVideoPane->GetPlayHandle());
	if (NULL == mediaBase)
	{
		mediaBase = pUserMgr->GetPlaybackMgr().GetMediaBase(pVideoPane->GetPlayHandle());
	}
	// 设置延迟
	if(NULL != mediaBase)
	{
		(void)mediaBase->SetDelayFrameNum(uiIAVideoDelayFrame);
	}

	return IVS_SUCCEED;
}

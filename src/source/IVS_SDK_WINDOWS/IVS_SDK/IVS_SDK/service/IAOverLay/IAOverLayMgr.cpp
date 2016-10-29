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

#include "IAOverLayMgr.h"
#include "IVS_IATrackAPI.h"
#include "IAMessageMgr.h"
#include "IVS_Trace.h"
#include "ToolsHelp.h"
#include "IVS_SDK.h"
#include "IVSCommon.h"
#include "IAXMLProcess.h"
#include "UserMgr.h"
#include "SDKInterface.h"

extern CSDKInterface *g_pNvs ;//lint -e526

//callback function of show IA track
static IVS_VOID __SDK_CALL OverlayProc(IVS_VOID *hDc, IVS_VOID *rc, ULONG64 ulFrameID, IVS_VOID *pUser)
{
	DRAW_HANDLE h = (DRAW_HANDLE)pUser;

	if (!h)
	{
		return;
	}
	(void)IATrack_ShowIATrack(h, (HDC)hDc, (RECT *)rc, ulFrameID);
}//lint !e818

bool CIAOverLayMgr::m_bStartPlaybackOverlayThread = false;

CIAOverLayMgr::CIAOverLayMgr()
	: m_hWndHandle(NULL)
	, m_ulPlayHandle(0)
	, m_ulOverlayHandle(0)
	, m_ulRequestID(IA_INVALID_REQUEST_ID)
	, m_ulQueryType(IA_INVALID_QUERY_TYPE)
	, m_ulRuleType(0)
	, m_iSessionID(SDK_SESSIONID_INIT)
	, m_pStartPlaybackOverlayThread(NULL)
{
	eSDK_MEMSET(m_szCameraID, 0x0, sizeof(m_szCameraID));
	eSDK_MEMSET(m_szNVRCode , 0x0, sizeof(m_szNVRCode));
	eSDK_MEMSET(&m_tmTimeSpan , 0x0, sizeof(m_tmTimeSpan));
}

CIAOverLayMgr::~CIAOverLayMgr()
{
	if (0 != m_ulOverlayHandle)
	{
		IATRACK_HANDLE hOverlayHandle = (IATRACK_HANDLE *)m_ulOverlayHandle;
		(void)IATrack_FreeHandle(hOverlayHandle);
		m_ulOverlayHandle = 0;
	}

	if (m_bStartPlaybackOverlayThread)
	{
		VOS_ThreadJoin(m_pStartPlaybackOverlayThread);
		if (NULL != m_pStartPlaybackOverlayThread)
		{
			VOS_free(m_pStartPlaybackOverlayThread);
			m_pStartPlaybackOverlayThread = NULL;
		}
		m_bStartPlaybackOverlayThread = false;
	}

	m_hWndHandle = NULL;
	m_ulPlayHandle = 0;
	m_ulRequestID = IA_INVALID_REQUEST_ID;
	m_ulQueryType = IA_INVALID_QUERY_TYPE;
	m_ulRuleType = 0;
	m_iSessionID = SDK_SESSIONID_INIT;
}//lint !e1740

IVS_INT32 CIAOverLayMgr::SetCameraID(const IVS_CHAR *szCameraID)
{
	CHECK_POINTER(szCameraID, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("CameraID: %s", szCameraID);

	if (!CToolsHelp::Strncpy(m_szCameraID, sizeof(m_szCameraID), szCameraID, IVS_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "SetCameraID", "Strncpy failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	return IVS_SUCCEED;
}

IVS_INT32 CIAOverLayMgr::SetNVRCode(const IVS_CHAR *szNVRCode)
{
	CHECK_POINTER(szNVRCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("NVRCode: %s", szNVRCode);
	if (!CToolsHelp::Strncpy(m_szNVRCode, sizeof(m_szNVRCode), szNVRCode, IVS_NVR_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Set NVRCode", "Strncpy failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	return IVS_SUCCEED;
}

IVS_VOID CIAOverLayMgr::SetTimeSpan(const IVS_TIME_SPAN &i_tmTimeSpan)
{
	IVS_DEBUG_TRACE("Set TimeSpan, StartTime: %s, EndTime: %s", i_tmTimeSpan.cStart, i_tmTimeSpan.cEnd);
	eSDK_MEMSET(&m_tmTimeSpan, 0x0, sizeof(m_tmTimeSpan));
	eSDK_MEMCPY(&m_tmTimeSpan, sizeof(m_tmTimeSpan), &i_tmTimeSpan, sizeof(i_tmTimeSpan));
	return;
}

const IVS_TIME_SPAN* CIAOverLayMgr::GetTimeSpan() const
{
	IVS_DEBUG_TRACE("");
	return &m_tmTimeSpan;
}

IVS_INT32 CIAOverLayMgr::EnableOverlay(ULONG ulRuleType, HWND hMsgWnd, ULONG ulQueryType)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(hMsgWnd, IVS_PARA_INVALID);
	if (IA_QUERY_TYPE_REALTIME != ulQueryType && IA_QUERY_TYPE_RECORD != ulQueryType)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "EnableOverlay", "IATrack_Init failed, QueryType: %lu invalid", ulQueryType);
	}
	
	SetWndHandle(hMsgWnd);
	SetQueryType(ulQueryType);
	SetRuleType(ulRuleType);

	if (!m_ulOverlayHandle)
	{
		//get IATrack handle
		IATRACK_HANDLE  h = IATrack_GetHandle();
		if (NULL == h)
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "EnableOverlay", "IATrack_GetHandle failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}

		//init IATrack
		LONG lRet = IATrack_Init(h, m_hWndHandle, IA_QUERY_TYPE_REALTIME);
		if (IVS_SUCCEED != lRet)
		{
			BP_RUN_LOG_ERR(lRet, "EnableOverlay", "IATrack_Init failed");
			IATrack_FreeHandle(h);
			return lRet;
		}

		m_ulOverlayHandle = (unsigned long)h ;
	}

	//get request param.
	IA_REQ_PARAM IAReqParam;
	eSDK_MEMSET(&IAReqParam, 0x0, sizeof(IAReqParam));
	IVS_INT32 iRet = GetReqParam(IAReqParam);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IA_INVALID_REQUEST_ID, "EnableOverlay", "GetReqParam Failed");
		(void)IATrack_FreeHandle((IATRACK_HANDLE)m_ulOverlayHandle);
		return IA_INVALID_REQUEST_ID;
	}

	// 获取用户管理类
	CHECK_POINTER(g_pNvs, IVS_NOT_INIT);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(m_iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	// 根据用户获取媒体基类
	CMediaBase* mediaBase = pUserMgr->GetRealPlayMgr().GetMediaBase(m_ulPlayHandle);
	if (NULL == mediaBase)
	{
		mediaBase = pUserMgr->GetPlaybackMgr().GetMediaBase(m_ulPlayHandle);
	}

	CIAMessageMgr &IAMessageMgr = CIAMessageMgr::Instance();
	m_ulRequestID = IAMessageMgr.TryRealStreamShare(this, IAReqParam);
	if (IA_INVALID_REQUEST_ID != m_ulRequestID)
	{
		BP_RUN_LOG_INF("EnableOverlay", "TryRealStreamShare success, RequestID: %lu", m_ulRequestID);
		// 设置画图回调
		if (NULL != mediaBase)
		{
			iRet = mediaBase->SetDrawCallBackEx(m_hWndHandle, OverlayProc, (void *)m_ulOverlayHandle);
		}
		else
		{
			BP_RUN_LOG_ERR(iRet, "EnableOverlay", "SetDrawCallBackEx failed");
		}

		return iRet;
	}
	m_ulRequestID = IAMessageMgr.AllocRequestID();
	IAReqParam.ulRequestID = m_ulRequestID;

	//start overlay
	//realplay
	if (IA_QUERY_TYPE_REALTIME == m_ulQueryType)
	{
		IVS_ULONG ulRet = IAMessageMgr.StartRealtimeOverlay(this, IAReqParam);
		if (IA_INVALID_REQUEST_ID == ulRet)
		{
			BP_RUN_LOG_ERR(IA_INVALID_REQUEST_ID, "EnableOverlay", "Start Realtime Overlay failed");
			IATrack_FreeHandle((IATRACK_HANDLE)m_ulOverlayHandle);
			return IA_INVALID_REQUEST_ID;
		}
	}
	//playback
	else
	{
		iRet = StartPlaybackOverlay(IAReqParam, NULL);
		if (IA_INVALID_REQUEST_ID == iRet)
		{
			BP_RUN_LOG_ERR(IA_INVALID_REQUEST_ID, "EnableOverlay", "Start Playback Overlay failed");
			IATrack_FreeHandle((IATRACK_HANDLE)m_ulOverlayHandle);
			return IA_INVALID_REQUEST_ID;
		}
	}

	// 设置画图回调
	if (NULL != mediaBase)
	{
		iRet = mediaBase->SetDrawCallBackEx(m_hWndHandle, OverlayProc, (void *)m_ulOverlayHandle);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(IA_INVALID_REQUEST_ID, "EnableOverlay", "Set Draw CallBack failed, return: %d", iRet);
			(void)IATrack_FreeHandle((IATRACK_HANDLE)m_ulOverlayHandle);
		}
	}
	else
	{
		BP_RUN_LOG_ERR(iRet, "EnableOverlay", "SetDrawCallBackEx failed");
		return iRet;
	}

	return iRet;
}

IVS_INT32 CIAOverLayMgr::DisableOverlay()
{
	IVS_DEBUG_TRACE("SessionID: %d, PlayHandle: %lu", m_iSessionID, m_ulPlayHandle);
	//join in the thread of sending request message
	m_bStartPlaybackOverlayThread = false;
	if (0 == m_ulOverlayHandle)
	{
		return IVS_SUCCEED;
	}

	CIAMessageMgr &IAMessageMgr = CIAMessageMgr::Instance();
	IVS_INT32 iRet = IAMessageMgr.StopOverlay(this, m_ulRequestID);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "DisableOverlay", "StopOverlay failed");
		return iRet;
	}

	//enable the draw callback function
	HWND hMsgWnd = GetWndHandle();
	if (NULL == hMsgWnd)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "DisableOverlay", "GetVideoWnd failed");
		return IVS_FAIL;
	}

	// 获取用户管理类
	CHECK_POINTER(g_pNvs, IVS_NOT_INIT);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(m_iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	// 根据用户获取媒体基类
	CMediaBase* mediaBase = pUserMgr->GetRealPlayMgr().GetMediaBase(m_ulPlayHandle);
	if (NULL == mediaBase)
	{
		mediaBase = pUserMgr->GetPlaybackMgr().GetMediaBase(m_ulPlayHandle);
	}

	if (NULL != mediaBase)
	{
		(void)mediaBase->SetDrawCallBackEx(hMsgWnd, NULL, NULL);
	}

	//free IATrack handle
	IATRACK_HANDLE h = (IATRACK_HANDLE *)m_ulOverlayHandle;
	(void)IATrack_FreeHandle(h);
	m_ulOverlayHandle = 0;

	return IVS_SUCCEED;
}


IVS_INT32 CIAOverLayMgr::InsertIALayoutMessage(const IA_LAYOUT_MESSAGE &LayoutMessage)
{
	IVS_DEBUG_TRACE("");
	if (0 == m_ulOverlayHandle)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "InsertIALayoutMessage", "OverlayHandle Invalid");
		return IVS_FAIL;
	}

	IATRACK_HANDLE h = (IATRACK_HANDLE *)m_ulOverlayHandle;
	return IATrack_InsertIALayoutMessage(h, &LayoutMessage);
}

IVS_INT32 CIAOverLayMgr::GetReqParam(IA_REQ_PARAM &IAReqParam)const
{
	IVS_DEBUG_TRACE("");

	IAReqParam.ulQueryType = m_ulQueryType;
	IAReqParam.ulRuleType = m_ulRuleType;
	CHECK_SUCCESS(CToolsHelp::Strncpy(IAReqParam.CameraID, sizeof(IAReqParam.CameraID), m_szCameraID, IVS_DEV_CODE_LEN));
	CHECK_SUCCESS(CToolsHelp::Strncpy(IAReqParam.NVRCode, sizeof(IAReqParam.NVRCode), m_szNVRCode, IVS_NVR_CODE_LEN));
	CHECK_SUCCESS(CToolsHelp::Strncpy(IAReqParam.StartTime, sizeof(IAReqParam.StartTime), m_tmTimeSpan.cStart, IVS_TIME_LEN));
	CHECK_SUCCESS(CToolsHelp::Strncpy(IAReqParam.StopTime, sizeof(IAReqParam.StopTime), m_tmTimeSpan.cEnd, IVS_TIME_LEN));

	return IVS_SUCCEED;
}

IVS_INT32 CIAOverLayMgr::StartPlaybackOverlay(const IA_REQ_PARAM &IAReqParam, IVS_CHAR *pTransID)
{
	IVS_DEBUG_TRACE("");

	//create thread to send request message for playback
	SEND_PLAYBACK_MSG_THREAD_PARAM *pThreadParam = IVS_NEW(pThreadParam);
	if (NULL == pThreadParam)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "StartPlaybackOverlay", "NEW fail!");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	pThreadParam->iSessionID = m_iSessionID;
	pThreadParam->pTransID = pTransID;
	pThreadParam->stIAReqParam = IAReqParam;
	pThreadParam->ulOverLayHandle = m_ulOverlayHandle;
	m_bStartPlaybackOverlayThread = true;
	IVS_ULONG ulRet = VOS_CreateThread((VOS_THREAD_FUNC)StartPlaybackOverlayThread, pThreadParam, &m_pStartPlaybackOverlayThread, VOS_DEFAULT_STACK_SIZE);
	if (VOS_OK != ulRet)
	{
		m_bStartPlaybackOverlayThread = false;
		IVS_DELETE(pThreadParam);
		BP_RUN_LOG_ERR(IA_INVALID_REQUEST_ID, "StartPlaybackOverlay", "create thread fail!");
		return IA_INVALID_REQUEST_ID;
	}

	//set realplay dealy time
	CIAMessageMgr &IAMessageMgr = CIAMessageMgr::Instance();
	IVS_INT32 iRet = IAMessageMgr.SetPlayDelayTime(this);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IA_INVALID_REQUEST_ID, "StartRealtimeOverlay", "SetPlayDelayTime failed, return: %d", iRet);
		m_bStartPlaybackOverlayThread = false;
		IVS_DELETE(pThreadParam);
		return IA_INVALID_REQUEST_ID;
	}

	//insert panel into map, used to insert parse overlay message
	iRet = IAMessageMgr.InsertIAObject(this, pThreadParam->stIAReqParam);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IA_INVALID_REQUEST_ID, "StartRealtimeOverlay", "InsertIAObject failed, return: %d", iRet);
		m_bStartPlaybackOverlayThread = false;
		IVS_DELETE(pThreadParam);
		return IA_INVALID_REQUEST_ID;
	}

	return static_cast<int>(IAReqParam.ulRequestID);
}

ULONG __SDK_CALL CIAOverLayMgr::StartPlaybackOverlayThread(void *lpvoid)
{
	IVS_DEBUG_TRACE("");

	CHECK_POINTER(lpvoid, NULL);
	SEND_PLAYBACK_MSG_THREAD_PARAM *pThreadParam = reinterpret_cast<SEND_PLAYBACK_MSG_THREAD_PARAM *>(lpvoid);
	CIAMessageMgr &IAMessageMgr = CIAMessageMgr::Instance();

	IVS_INT32 iRet;
	std::string strOverLayStartTime = pThreadParam->stIAReqParam.StartTime;	//开始轨迹叠加时间
	std::string strRecordEndTime = pThreadParam->stIAReqParam.StopTime;	//录像停止时间
	time_t tmRecordEndTime = CToolsHelp::UTCStr2Time(strRecordEndTime.c_str(), IVS_DATE_TIME_FORMAT);	//录像停止UTC时间

	while(m_bStartPlaybackOverlayThread)
	{
		//根据请求的轨迹叠加开始时间求结束时间
		std::string strReqStartTime = pThreadParam->stIAReqParam.StartTime;
		std::string strReqStopTime = pThreadParam->stIAReqParam.StopTime;
		time_t tmReqStartTime = CToolsHelp::UTCStr2Time(strReqStartTime.c_str(), IVS_DATE_TIME_FORMAT);
		time_t tmReqStopTime = tmReqStartTime + RECORD_OVERLAY_TIMESTEP;
		//如果求出的轨迹叠加结束时间大于录像停止时间，则用录像停止时间,并停止轨迹叠加线程，否则用求出的时间
		if (tmReqStopTime < tmRecordEndTime)
		{
			CToolsHelp::UTCTime2Str(tmReqStopTime, strReqStopTime, IVS_DATE_TIME_FORMAT);
		}
		else
		{
			strReqStopTime = strRecordEndTime;
			m_bStartPlaybackOverlayThread = false;
		}
		//将请求轨迹叠加开始和结束时间写入请求参数中
		eSDK_STRCPY(pThreadParam->stIAReqParam.StartTime, sizeof(pThreadParam->stIAReqParam.StartTime), strReqStartTime.c_str());
		eSDK_STRCPY(pThreadParam->stIAReqParam.StopTime, sizeof(pThreadParam->stIAReqParam.StopTime), strReqStopTime.c_str());

		//发送请求命令
		iRet = IAMessageMgr.SendenableMessage(pThreadParam->iSessionID, pThreadParam->stIAReqParam, pThreadParam->pTransID);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "StartPlaybackOverlayThread", "SendenableMessage fail!");
			//此处应该有事件上报
			break;
		}

		//sleep 10s
		VOS_Sleep(10 * 1000);
	}

	IVS_DELETE(pThreadParam);
	m_bStartPlaybackOverlayThread = false;
	VOS_pthread_exit(NULL);
	//IATrack_FreeHandle((IATRACK_HANDLE)pThreadParam->ulOverLayHandle);
	//此处应该有事件上报
	return 0;
}


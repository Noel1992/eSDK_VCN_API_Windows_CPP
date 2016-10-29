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

/*****************************************************************
 filename		:	IAOverLayMgr.h
 author			:	zwx211831
 created			:	2015/7/15
 description	:	智能轨迹叠加管理类;
 copyright		:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history			:	2015/7/15初始版本
*****************************************************************/

#ifndef __IA_OVERLAY_MGR_H__
#define __IA_OVERLAY_MGR_H__

#include "vos.h"
#include "SDKDef.h"
#include <Windows.h>
#include "IVS_IATrackAPI.h"

//param. for the thread of send playback request message
typedef struct
{
	IA_REQ_PARAM stIAReqParam;
	IVS_INT32 iSessionID;
	IVS_CHAR *pTransID;
	IVS_ULONG ulOverLayHandle;
}SEND_PLAYBACK_MSG_THREAD_PARAM;

class CIAOverLayMgr
{
public:
	CIAOverLayMgr();
	virtual ~CIAOverLayMgr();

private:
	//window handle of video
	HWND m_hWndHandle;
	//play handle of video
	IVS_ULONG m_ulPlayHandle;
	//handle of overlay
	IVS_ULONG m_ulOverlayHandle;
	//request ID of overlay
	IVS_ULONG m_ulRequestID;
	//query type of overlay, realplay or playback
	IVS_ULONG m_ulQueryType;
	//rule type of overlay. RuleType is IA_QD, see line from 621 to 624 in hwsdk.h
	IVS_ULONG m_ulRuleType;
	//User SessionID
	IVS_INT32 m_iSessionID;
	//Camera Code
	IVS_CHAR m_szCameraID[IVS_DEV_CODE_LEN + 1];
	//NVR Code
	IVS_CHAR m_szNVRCode[IVS_NVR_CODE_LEN + 1];
	//time span of playback
	IVS_TIME_SPAN m_tmTimeSpan;

public:
	const IVS_CHAR *GetCameraID() const {return m_szCameraID;}
	IVS_INT32 SetCameraID(const IVS_CHAR *szCameraID);

	const IVS_CHAR *GetNVRCode() const {return m_szNVRCode;}
	IVS_INT32 SetNVRCode(const IVS_CHAR *szNVRCode);

	IVS_VOID SetPlayHandle(IVS_ULONG ulPlayHandle) {m_ulPlayHandle = ulPlayHandle;}
	IVS_ULONG GetPlayHandle() const {return m_ulPlayHandle;}

	IVS_VOID SetWndHandle(HWND hWndHandle) {m_hWndHandle = hWndHandle;}
	HWND GetWndHandle(void) { return m_hWndHandle; }

	IVS_VOID SetSessionID(IVS_INT32 iSessionID) {m_iSessionID = iSessionID;}
	IVS_INT32 GetSessionID() const {return m_iSessionID;}

	IVS_VOID SetQueryType(IVS_ULONG ulQueryType) {m_ulQueryType = ulQueryType;}
	IVS_ULONG GetQueryType() const {return m_ulQueryType;}

	IVS_VOID SetRuleType(IVS_ULONG ulRuleType) {m_ulRuleType = ulRuleType;}
	IVS_ULONG GetRuleType() const {return m_ulRuleType;}

	IVS_VOID SetTimeSpan(const IVS_TIME_SPAN &i_tmTimeSpan);
	const IVS_TIME_SPAN *GetTimeSpan() const;

private:
	//Thread handle of send playback request message
	VOS_Thread *m_pStartPlaybackOverlayThread;
	//the thread is running or not
	static bool m_bStartPlaybackOverlayThread;

public:
	//enable overlay
	IVS_INT32 EnableOverlay(ULONG ulRuleType, HWND hMsgWnd, ULONG ulQueryType);
	//disable overlay
	IVS_INT32 DisableOverlay();
	//insert message of IA overlay
	IVS_INT32 InsertIALayoutMessage(const IA_LAYOUT_MESSAGE &LayoutMessage);

private:
	//get request param.
	IVS_INT32 GetReqParam(IA_REQ_PARAM &IAReqParam)const;
	//try share stream
	IVS_ULONG TryRealStreamShare(const IA_REQ_PARAM &IAReqParam);
	//start playback overlay
	IVS_INT32 StartPlaybackOverlay(const IA_REQ_PARAM &IAReqParam, IVS_CHAR *pTransID);
	//create thread to send request message for playback. send one message every 10s, each message request 60s's duration.
	static ULONG __SDK_CALL StartPlaybackOverlayThread(void *lpvoid);
};


#endif

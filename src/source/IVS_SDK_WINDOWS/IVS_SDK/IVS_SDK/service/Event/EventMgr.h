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
 filename    :    EventMgr.h
 author      :    s00191067
 created     :    2012/11/20
 description :    异常类;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/20 初始版本
*********************************************************************/
#ifndef EVENT_MGR_H
#define EVENT_MGR_H

// #include <queue>
#include "vos.h"
#include "NetSourceCommon.h"
#include "BaseJob.h"
#include "rtsp_client_api.h"
#include "IVS_Player.h"
#include "SDKDef.h"
#include "UserMgr.h"

typedef struct
{
	int iSessionID;
	int iEventType;
	int iBufSize;
	void* pEventData;
}EVENT_CALL_BACK_DATA;

class _BaseThread;
class CEventMgr
{
public:
    CEventMgr(void);

    ~CEventMgr(void);

    int SetEventCallBack( int iSessionID, EventCallBack fEventCallBack = NULL, void *pUserData = NULL );

    int Init();

    void UnInit();

    int OnEventCallBack(int iSessionID, int iEventType, void* pEventData, int iBufSize);

    PLAYER_EXCEPTION_CALLBACK GetPlayerExceptionCallBack() const;

    NET_EXCEPTION_CALLBACK GetNetExceptionCallBack() const;

    fExceptionCallBack GetRtspExceptionCallBack() const;

private:
	// 处理流消息
    void HandleStreamMsg(int iSessionID, int iEventType, void* pEventData, int iBufSize);
	// 处理实况流消息
	void HandleRealVideoStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// 处理录像流消息
	void HandlePlaybackStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// 处理智真录像流消息
	void HandleTPPlaybackStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// 处理备份录像流消息
	void HandleBackupRecordStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// 处理容灾录像流消息
	void HandleDisasteBackupRecordStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// 处理前端录像流消息
	void HandlePUPlaybackStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// 处理下载流消息
	void HandleDownloadStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// 处理音频广播流消息
	void HandleBroadcastStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// 处理音频对接流消息
	void HandleAudioCallStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);

	// 处理电视墙实况流消息
	void HandleTVWallRealVideoStreamMsg(CUserMgr *pUserMgr, STREAM_EXCEPTION_INFO& stExceptionInfo);
	//处理智能分析轨迹叠数据
	void HandleIntelligenceAnalysisEvent(const void *pEventBuf);

private:
    //因为有本地回事件的sessionid为-1，所以要加1;
    EventCallBack  m_EventCallBack[SDK_CLIENT_TOTAL + 1];
    void*          m_EventUserData[SDK_CLIENT_TOTAL + 1];
	VOS_Mutex*     m_pMutexEventCB;				// 事件回调锁;
    _BaseThread     *m_pEventCallbackThread;    //事件异常回调;

private:
	// 流消息处理方法
	typedef void (__thiscall CEventMgr::*FnProStreamMsg)(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr); 
	// 流消息处理方法表
	typedef struct
	{
		IVS_SERVICE_TYPE enServiceType;
		FnProStreamMsg fnProStreamMsg;
	}STREAM_MSG_PRO;
	// 流消息处理方法数组
	static STREAM_MSG_PRO m_proStreamMsg[];
	// 根据业务类型获取对应的流消息处理方法
	FnProStreamMsg GetFnProStreamMsg(IVS_SERVICE_TYPE enServiceType) const;

};
#endif //EVENT_MGR_H

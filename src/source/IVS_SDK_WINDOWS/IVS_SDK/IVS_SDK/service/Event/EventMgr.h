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
 description :    �쳣��;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/20 ��ʼ�汾
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
	// ��������Ϣ
    void HandleStreamMsg(int iSessionID, int iEventType, void* pEventData, int iBufSize);
	// ����ʵ������Ϣ
	void HandleRealVideoStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// ����¼������Ϣ
	void HandlePlaybackStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// ��������¼������Ϣ
	void HandleTPPlaybackStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// ������¼������Ϣ
	void HandleBackupRecordStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// ��������¼������Ϣ
	void HandleDisasteBackupRecordStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// ����ǰ��¼������Ϣ
	void HandlePUPlaybackStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// ������������Ϣ
	void HandleDownloadStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// ������Ƶ�㲥����Ϣ
	void HandleBroadcastStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);
	// ������Ƶ�Խ�����Ϣ
	void HandleAudioCallStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr);

	// �������ǽʵ������Ϣ
	void HandleTVWallRealVideoStreamMsg(CUserMgr *pUserMgr, STREAM_EXCEPTION_INFO& stExceptionInfo);
	//�������ܷ����켣������
	void HandleIntelligenceAnalysisEvent(const void *pEventBuf);

private:
    //��Ϊ�б��ػ��¼���sessionidΪ-1������Ҫ��1;
    EventCallBack  m_EventCallBack[SDK_CLIENT_TOTAL + 1];
    void*          m_EventUserData[SDK_CLIENT_TOTAL + 1];
	VOS_Mutex*     m_pMutexEventCB;				// �¼��ص���;
    _BaseThread     *m_pEventCallbackThread;    //�¼��쳣�ص�;

private:
	// ����Ϣ������
	typedef void (__thiscall CEventMgr::*FnProStreamMsg)(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr); 
	// ����Ϣ��������
	typedef struct
	{
		IVS_SERVICE_TYPE enServiceType;
		FnProStreamMsg fnProStreamMsg;
	}STREAM_MSG_PRO;
	// ����Ϣ����������
	static STREAM_MSG_PRO m_proStreamMsg[];
	// ����ҵ�����ͻ�ȡ��Ӧ������Ϣ������
	FnProStreamMsg GetFnProStreamMsg(IVS_SERVICE_TYPE enServiceType) const;

};
#endif //EVENT_MGR_H

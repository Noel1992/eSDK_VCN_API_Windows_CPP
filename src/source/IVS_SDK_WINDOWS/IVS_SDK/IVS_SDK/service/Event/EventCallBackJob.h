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
 filename    :    IVSEventQueue.h
 author      :    s00191067
 created     :    2012/11/20
 description :    事件队列，包括消息，异常事件等;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/20 初始版本
*********************************************************************/
#ifndef IVS_EVENT_QUEUE_H
#define IVS_EVENT_QUEUE_H
// #include <queue>
#include "EventMgr.h"
#include "BaseJob.h"
#include "vos.h"
#include "SDKDef.h"

class CEventCallBackJob : public BaseJob
{
public:
    virtual ~CEventCallBackJob(void);

    static CEventCallBackJob& instance()
    {
        static CEventCallBackJob s_eventCallBackJob;
        return s_eventCallBackJob;
    }

    virtual void MainLoop();

    virtual void kill();

    int PostEvent(int iSessionID, int iEventType, const void* pEventData, int iBufSize);

    void SetEventMgr(CEventMgr* pEventMgr)
    {
        m_pEventMgr = pEventMgr;
		m_bRun = true;
    }

private:
    CEventCallBackJob(void);
    EVENT_CALL_BACK_DATA* PopEvent();

private:
    std::queue<EVENT_CALL_BACK_DATA*>   m_eventQueue;
    VOS_Mutex*                          m_pMutexEventQueue;       // 队列锁;
    bool                                m_bRun;                   // 线程运行标志; 
    CEventMgr*                          m_pEventMgr;              // 事件管理类对象;
};
#endif //IVS_EVENT_QUEUE_H

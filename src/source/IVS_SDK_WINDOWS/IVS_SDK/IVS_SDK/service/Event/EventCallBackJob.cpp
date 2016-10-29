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

#include "EventCallBackJob.h"

#include "IVSCommon.h"
#include "ivs_error.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "LockGuard.h"

CEventCallBackJob::CEventCallBackJob(void)
    : m_bRun(true)
    , m_pEventMgr(NULL)
{
    m_pMutexEventQueue = VOS_CreateMutex();
}

CEventCallBackJob::~CEventCallBackJob(void)
{
    m_bRun = false;
    m_pEventMgr = NULL;
    
    try
    {
        if (NULL != m_pMutexEventQueue)
        {
            VOS_DestroyMutex(m_pMutexEventQueue);
            m_pMutexEventQueue = NULL;
        }
    }
    catch(...)
    {}//lint !e1775
}

void CEventCallBackJob::MainLoop()
{
    while (m_bRun)
    {
        EVENT_CALL_BACK_DATA* pstEvent = PopEvent();
        // 队列中无数据sleep100毫秒;
        if (NULL == pstEvent)
        {
            Sleep(100);
        }
        else
        {
            if (NULL == m_pEventMgr)
            {
                // 此处不能打日志;
                return;
            }

            (void)m_pEventMgr->OnEventCallBack(pstEvent->iSessionID, pstEvent->iEventType, pstEvent->pEventData, pstEvent->iBufSize);
			IVS_DELETE(pstEvent->pEventData, MUILI);
            IVS_DELETE(pstEvent);
        }
    }
}

void CEventCallBackJob::kill()
{
    //停止线程;
    m_bRun = false;
}

int CEventCallBackJob::PostEvent(int iSessionID, int iEventType, const void* pEventData, int iBufSize)
{
    CLockGuard lock(m_pMutexEventQueue);
    //构造事件对象
    EVENT_CALL_BACK_DATA* pstEventData = IVS_NEW(pstEventData);
    if (NULL == pstEventData)
    {
		BP_RUN_LOG_ERR(IVS_FAIL, "event data null", "NA");
        return IVS_FAIL;
    }

    eSDK_MEMSET(pstEventData, 0, sizeof(EVENT_CALL_BACK_DATA));
    pstEventData->iSessionID = iSessionID;
    pstEventData->iEventType = iEventType;

	IVS_CHAR* pEvent = IVS_NEW((IVS_CHAR* &)pEvent, (unsigned long)iBufSize);
	if (!CToolsHelp::Memcpy(pEvent, (unsigned int)iBufSize, pEventData, (unsigned int)iBufSize))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "memcpy fail", "NA");
		IVS_DELETE(pstEventData);
		IVS_DELETE(pEvent, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
    pstEventData->pEventData = (void*)pEvent;
    pstEventData->iBufSize = iBufSize;

    //插入队列前先检查下当前的长度,如果已达最大值，那删除第一个;
    if (m_eventQueue.size() >= MAX_EVENT_QUEUE_LEN)
    {
        EVENT_CALL_BACK_DATA* pstEventDataFront = m_eventQueue.front();//lint !e64 匹配
        IVS_DELETE(pstEventDataFront->pEventData, MUILI);
        IVS_DELETE(pstEventDataFront);
        m_eventQueue.pop();
        BP_DBG_LOG("event queue over max size[%u]", m_eventQueue.size());
    }

    //插入队列
    m_eventQueue.push(pstEventData);
	BP_RUN_LOG_INF("CEventCallBackJob PostEvent", "SessionID: %d, EventType: %d, BufSize: %d", 
		pstEventData->iSessionID, pstEventData->iEventType, pstEventData->iBufSize);
    return IVS_SUCCEED;
}

EVENT_CALL_BACK_DATA* CEventCallBackJob::PopEvent()
{
    CLockGuard lock(m_pMutexEventQueue);
    if (0 == m_eventQueue.size())
    {
        return NULL;
    }

    EVENT_CALL_BACK_DATA* pstEventData = m_eventQueue.front();//lint !e64 匹配
    //delete stEventData1.pEventData;
    m_eventQueue.pop();

	BP_RUN_LOG_INF("CEventCallBackJob PopEvent", "SessionID: %d, EventType: %d, BufSize: %d", 
		pstEventData->iSessionID, pstEventData->iEventType, pstEventData->iBufSize);

    return pstEventData;
}

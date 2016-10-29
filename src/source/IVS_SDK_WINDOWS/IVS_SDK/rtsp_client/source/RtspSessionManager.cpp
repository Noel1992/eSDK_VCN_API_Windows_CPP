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

/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspSessionManager.cpp
  作    者 : y00182336
  功能描述 : RTSP客户端通讯库
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#include "Rtsp_Log.h"
#include "RtspRetCode.h"
#include "RtspSessionManager.h"
#include "RtspReactorManager.h"
using namespace std;


CRtspSessionManagerTimer::CRtspSessionManagerTimer()
{//lint !e1928
    m_pRtspSessionListMutex = NULL;
    m_pRtspSessionList      = NULL;
    m_unThreadIndex         = (unsigned int)-1;//lint !e1924
}

CRtspSessionManagerTimer::~CRtspSessionManagerTimer()
{
    m_pRtspSessionListMutex = NULL;
    m_pRtspSessionList      = NULL;
}

// 定时器回调函数，用于定期发送检查连接状态
int CRtspSessionManagerTimer::handle_timeout(const ACE_Time_Value &, const void *)
{
    if ((NULL == m_pRtspSessionListMutex) || (NULL == m_pRtspSessionList))
    {
        return 0;
    }
    ACE_Guard<ACE_Thread_Mutex> locker(*m_pRtspSessionListMutex);
    if (m_pRtspSessionList->empty())
    {
        return 0;
    }

    // 检查连接状态消息
    RtspSessionIter iter = m_pRtspSessionList->begin();
    CRtspSession *pSession = NULL;
    while (iter != m_pRtspSessionList->end())
    {
        pSession = *iter;
        RtspSessionIter pIter = iter++;
        if (NULL == pSession)
        {
            // 从List中移除session并释放
            m_pRtspSessionList->erase(pIter);
            continue;
        }

        // 只处理同线程下的连接，避免多线程操作导致崩溃
        if ((RTSP_SESSION_STATUS_ABNORMAL == pSession->getStatus()) &&
            ((pSession->m_unSessionIndex % RTSP_THREAD_NUM) == m_unThreadIndex))
        {
			m_pRtspSessionList->erase(pIter);

			pSession->cancelRecvTimer();
			pSession->KillRtpTimer();
			(void)pSession->disconnectPeer();

            IVS_RUN_LOG_INF("release rtsp session [0x%p]", pSession);
            delete pSession;
            pSession = NULL;	
        }
    }
    return 0;
}//lint !e1788


/***************************************************************************
 函 数 名 : CRtspSessionManager
 功能描述 : 构造函数
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
CRtspSessionManager::CRtspSessionManager()
{
    m_fExceptionCallBack = NULL;
    m_pUser              = NULL;
    m_unSessionIndex     = 0;
    for(int i=0; i<RTSP_THREAD_NUM; i++)
    {
        m_pTimer[i]   = NULL;
        m_lTimerID[i] = -1;
    }
	
}


/***************************************************************************
 函 数 名 : ~CRtspSessionManager
 功能描述 : 析构函数
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
CRtspSessionManager::~CRtspSessionManager()
{
    m_fExceptionCallBack = NULL;
    m_pUser              = NULL;
}

/***************************************************************************
 函 数 名 : init
 功能描述 : 初始化连接管理器
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
int CRtspSessionManager::init()
{
    // 创建定时器
    try
    {
        for(unsigned int i=0; i<RTSP_THREAD_NUM; i++)
        {
            m_pTimer[i] = new CRtspSessionManagerTimer;
            m_pTimer[i]->setThreadIndex(i);
            m_pTimer[i]->setRtspSessionListMutex(&m_RtspSessionListMutex);
            m_pTimer[i]->setRtspSessionList(&m_RtspSessionList);
//             ACE_Time_Value tv(1, 0); //lint !e747
//             m_lTimerID[i] = CRtspReactorManager::instance()->RegistTimer((ACE_Event_Handler *)m_pTimer[i], i, tv, tv);
//             if (-1 == m_lTimerID[i])
//             {
//                 IVS_RUN_LOG_CRI("regist check timer fail.");
//                 freeAllTimer();
//                 return RET_FAIL;
//             }
        }
    }
    catch(...)
    {
        freeAllTimer();
        IVS_RUN_LOG_CRI("start check timer fail.");
        return RET_FAIL;
    }

    return RET_OK;
}

/***************************************************************************
 函 数 名 : freeAllTimer
 功能描述 : 停止定时器，删除资源
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspSessionManager::freeAllTimer()
{
    for(unsigned int i=0; i<RTSP_THREAD_NUM; i++)
    {
//         if (-1 != m_lTimerID[i])
//         {
//             (void)CRtspReactorManager::instance()->unRegistTimer(m_lTimerID[i], i);
//             m_lTimerID[i] = -1;
//         }
        if (NULL != m_pTimer[i])
        {
            delete m_pTimer[i];
            m_pTimer[i]  = NULL;
        }
    }

    return;
}

/***************************************************************************
 函 数 名 : closeManager
 功能描述 : 关闭连接管理器
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspSessionManager::closeManager()
{
    // 取消定时器，删除定时器资源
    freeAllTimer();

    ACE_Guard<ACE_Thread_Mutex> locker(m_RtspSessionListMutex);
    // 关闭所有连接，所有连接出队列
    if (m_RtspSessionList.empty())
    {
        IVS_RUN_LOG_INF("Session list already empty.");
        return;
    }
    RtspSessionIter iter = m_RtspSessionList.begin();
    CRtspSession *pSession = NULL;
    // 关闭通讯库时，中断
    while(iter != m_RtspSessionList.end())
    {
        pSession = *iter;
        RtspSessionIter pIter = iter++;
        if (NULL == pSession)
        {
            // 从List中移除session并释放
            m_RtspSessionList.erase(pIter);
            continue;
        }
        else
        {
            // 清除异常连接
            (void)pSession->disconnectPeer();
            IVS_RUN_LOG_INF("start release rtsp session [0x%p]", pSession);
            // 从List中移除session并释放
            m_RtspSessionList.erase(pIter);
            delete pSession;
            pSession = NULL;
        }
    }

    return;
}//lint !e1788



/***************************************************************************
 函 数 名 : createRtspSession
 功能描述 : 创建RTSP连接
 输入参数 : bBlocked                同步/异步标志位（true：同步；false：异步）
 输出参数 : 无
 返 回 值 : CRtspSession*           NULL-失败
***************************************************************************/
CRtspSession* CRtspSessionManager::createRtspSession(const bool bBlocked)
{
    CRtspSession* RtspSession = NULL;
    try
    {
        RtspSession = new CRtspSession;
    }
    catch(...)
    {
		IVS_RUN_LOG_CRI("new session throw exception.");
        return NULL;
    }

	int nRet = RtspSession->InitData();
	if (RET_OK != nRet)
	{
		delete RtspSession;
		RtspSession = NULL;
		IVS_RUN_LOG_CRI("init rtsp session data fail.");
		return NULL; //lint !e438
	}

	IVS_RUN_LOG_CRI("createRtspSession this = 0x%08p", RtspSession);

    // 设置同步标志位
    RtspSession->m_bBlocked = bBlocked;
    RtspSession->m_unSessionIndex = getSessionIndex();

    // 连接入队列
    nRet = pushRtspSession(RtspSession);
    if (RET_OK != nRet)
    {
        delete RtspSession;
        RtspSession = NULL;
        IVS_RUN_LOG_CRI("push rtsp session fail.");
    }
    return RtspSession;
}


/***************************************************************************
 函 数 名 : findSession
 功能描述 : 查找连接
 输入参数 : pSession                被查找的连接
 输出参数 : 无
 返 回 值 : true-成功               false-失败
***************************************************************************/
bool CRtspSessionManager::findSession(const CRtspSession *pSession)
{
    if (NULL == pSession)
    {
        return false;
    }
    
    // 在所有创建的连接队列中查找
    ACE_Guard<ACE_Thread_Mutex> locker(m_RtspSessionListMutex);
    return isSessionInList(pSession, m_RtspSessionList);
}//lint !e1788

bool CRtspSessionManager::deleteSession(const CRtspSession *pSession)
{
	if (NULL == pSession)
	{
		return false;
	}

	bool bRet = false;
	// 在所有创建的连接队列中查找
	ACE_Guard<ACE_Thread_Mutex> locker(m_RtspSessionListMutex);
	RtspSessionIter iter;
	for (iter = m_RtspSessionList.begin(); iter != m_RtspSessionList.end(); ++iter)
	{
		if (pSession == *iter)
		{
			m_RtspSessionList.erase(iter);
			bRet = true;
			break;
		}
	}
	return bRet;
}//lint !e1788

//*************************内部实现**************************//

/***************************************************************************
 函 数 名 : pushRtspSession
 功能描述 : RtspSession放入队列中
 输入参数 : pSession                被操作的连接
 输出参数 : 无
 返 回 值 : RET_OK-成功             NULL-失败
***************************************************************************/
int CRtspSessionManager::pushRtspSession(CRtspSession* pSession)
{
    try
    {
        ACE_Guard<ACE_Thread_Mutex>  locker(m_RtspSessionListMutex);
        m_RtspSessionList.push_back(pSession);
    }//lint !e1788
    catch (...)
    {
        return RET_FAIL;
    }
    return RET_OK;
}//lint !e429



/***************************************************************************
 函 数 名 : isSessionInList
 功能描述 : 检查连接是否存在
 输入参数 : pSession                被操作的连接
            SessionList             被操作的队列
 输出参数 : 无
 返 回 值 : true-存在               false-不存在
***************************************************************************/
bool CRtspSessionManager::isSessionInList(const CRtspSession *pSession, 
                                          RtspSessionList &SessionList) const
{
    RtspSessionIter iter;
    for (iter = SessionList.begin(); iter != SessionList.end(); ++iter)
    {
        if (pSession == *iter)
        {
            return true;
        }
    }
    return false;
}


unsigned int CRtspSessionManager::getSessionIndex()
{
    ACE_Guard<ACE_Thread_Mutex> locker (m_unSessionIndexMutex);
    return m_unSessionIndex++;
}//lint !e1788


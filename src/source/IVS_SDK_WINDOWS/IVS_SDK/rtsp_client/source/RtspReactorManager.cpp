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

//lint -e429
/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspReactorManager.cpp
  作    者 : y00182336
  功能描述 : RTSP Client反应器管理
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#include "Rtsp_Log.h"
#include "RtspRetCode.h"
#include "RtspReactorManager.h"
#include "RtspSessionManager.h"


CRtspReactorManager::CRtspReactorManager()
{//lint !e1928
    m_bRunFlag      = false;
    m_pReactor      = NULL;

    m_ThreadNum     = 0;
}

CRtspReactorManager::~CRtspReactorManager()
{
    m_bRunFlag    = false;
    if (NULL != m_pReactor)
    {
        try
        {
            for (unsigned int i = 0; i < RTSP_THREAD_NUM; i++)
            {
                if (NULL != m_pReactor[i])
                {
                    delete m_pReactor[i];
                    m_pReactor[i]  = NULL;
                }
            }
            delete[] m_pReactor;
            m_pReactor = NULL;
        }
        catch(...)
        {

        }

    }
    m_ThreadNum     = 0;
}


/***************************************************************************
 函 数 名 : openManager
 功能描述 : 打开反应器管理线程
 输入参数 : ThreadNum            反应器线程个数
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::openManager( )
{
    int nRet = createResources();
    if (RET_OK != nRet)
    {
        IVS_RUN_LOG_CRI("rtsp reactor manager open, fail to create resource nRet[%d].", nRet);
        return RET_FAIL;
    }

    // 标志位置位
    m_bRunFlag = true;
    nRet = createServiceThreads();
    if (RET_OK != nRet)
    {
        IVS_RUN_LOG_CRI("rtsp reactor manager open, fail to create service threads nRet[%d].", nRet);
        return RET_FAIL;
    }

    IVS_RUN_LOG_INF("rtsp reactor manager open success.");
    return RET_OK;
}


/***************************************************************************
 函 数 名 : closeManager
 功能描述 : 关闭反应器管理线程
 输入参数 : 无            
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::closeManager()
{
    IVS_RUN_LOG_INF("rtsp reactor manager closing.");
    m_bRunFlag  = false;
    m_ThreadNum = 0;
    (void)wait();
    IVS_RUN_LOG_INF("rtsp reactor manager closed.");
    return RET_OK;
}


/***************************************************************************
 函 数 名 : svc
 功能描述 : 反应器线程函数
 输入参数 : 无            
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::svc()
{
    IVS_RUN_LOG_INF("reactor manager thread start.");
    unsigned int unThreadIndex = getThreadIndex();
    if ((NULL == m_pReactor) || (NULL == m_pReactor[unThreadIndex]))
    {
        IVS_RUN_LOG_CRI("reactor manager thread exit, resource not created.");
        return RET_FAIL;
    }

    (void)m_pReactor[unThreadIndex]->owner(ACE_OS::thr_self());

    (void)m_pReactor[unThreadIndex]->restart(false);
    
    ACE_Time_Value delaytime;
    while (m_bRunFlag)
    {
		ACE_Time_Value  tv;
		if (CRtspSessionManager::instance() && 
			CRtspSessionManager::instance()->m_pTimer[unThreadIndex])
		{
			CRtspSessionManager::instance()->m_pTimer[unThreadIndex]->handle_timeout(tv, NULL);
		}
	
		delaytime.set((time_t)0, RTSP_MAX_TIME_PER_THREAD);
        if (m_pReactor[unThreadIndex]->handle_events(&delaytime) < 0)
        {
            continue;
        }
    }

    IVS_RUN_LOG_INF("rtsp reactor manager thread exit.");
    return RET_OK;
}



/***************************************************************************
 函 数 名 : RegistHandle
 功能描述 : 注册接收事件
 输入参数 : pHandle                事件回调类
            mask                事件触发类型
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::RegistHandle(CRtspSession *pHandle, unsigned long mask)
{
	IVS_RUN_LOG_CRI("in RegistHandle.");

    if ((NULL == pHandle) || (NULL == m_pReactor))
    {
		IVS_RUN_LOG_CRI("CRtspReactorManager::RegistHandle pHandle = NULL or NULL == m_pReactor.");
        return RET_FAIL;
    }
    unsigned int unThreadIndex = pHandle->m_unSessionIndex % RTSP_THREAD_NUM;
    if (NULL == m_pReactor[unThreadIndex])
    {
		IVS_RUN_LOG_CRI("CRtspReactorManager::RegistHandle m_pReactor[%u] = NULL.", unThreadIndex);
        return RET_FAIL;
    }

    int nRet = m_pReactor[unThreadIndex]->register_handler(pHandle->get_handle(),
                                                         (ACE_Event_Handler *)pHandle,
                                                         mask);
    if (RET_OK != nRet)
    {
        IVS_RUN_LOG_CRI("rtsp reactor manager, fail to register handler nRet[%d] unThreadIndex[%u] pHandle[0x%p].", nRet, unThreadIndex, pHandle);
        return RET_FAIL;
    }
    IVS_RUN_LOG_INF("rtsp reactor manager, regist handle unThreadIndex[%u] pHandle[0x%p].", unThreadIndex, pHandle);
    return RET_OK;
}


/***************************************************************************
 函 数 名 : unRegistHandle
 功能描述 : 取消接收事件
 输入参数 : pHandle                事件回调类
            mask                事件触发类型
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::unRegistHandle(const ACE_Event_Handler *pHandle, unsigned long mask) const
{
	IVS_RUN_LOG_INF("CRtspReactorManager::unRegistHandle Session[%lu] in.", pHandle);
    if ((NULL == pHandle) || (NULL == m_pReactor))
    {
        return RET_FAIL;
    }

    // 取得当前负载连接个数最轻的反应器
    ACE_Reactor *pReactor = pHandle->reactor();
    if (NULL == pReactor)
    {
        IVS_RUN_LOG_CRI("rtsp reactor manager, fail to unregist handle[%p].", pHandle);
        return RET_FAIL;
    }

    return pReactor->remove_handler(pHandle->get_handle(), mask);
}

/***************************************************************************
 函 数 名 : RegistTimer
 功能描述 : 注册定时器
 输入参数 : pHandle              事件应答句柄
           delta                超时时间
           interval             超时时间
 输出参数 : 无       
 返 回 值 : timer_id - 成功       -1 - 失败
***************************************************************************/
long CRtspReactorManager::RegistTimer(ACE_Event_Handler *pHandle, unsigned int unIndex,
                                      const ACE_Time_Value &delta,
                                      const ACE_Time_Value &interval) const
{
    unsigned int unThreadIndex = unIndex % RTSP_THREAD_NUM;
    if ((NULL == pHandle) || (NULL == m_pReactor) || (NULL == m_pReactor[unThreadIndex]))
    {
        return RET_FAIL;
    }

    return m_pReactor[unThreadIndex]->schedule_timer(pHandle, 0, delta, interval);
}


/***************************************************************************
 函 数 名 : unRegistTimer
 功能描述 : 取消接收定时器
 输入参数 : timer_id            定时器Id
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::unRegistTimer(long timer_id, unsigned int unIndex) const
{
    unsigned int unThreadIndex = unIndex % RTSP_THREAD_NUM;
    if ((-1 == timer_id) || (NULL == m_pReactor) || (NULL == m_pReactor[unThreadIndex]))
    {
        IVS_RUN_LOG_CRI("rtsp reactor manager unregist timer, fail to cancel timer timerId[%ld] m_pReactor[%p] unThreadIndex[%u].",
            timer_id, m_pReactor, unThreadIndex);
        return RET_FAIL;
    }

    int nRet = m_pReactor[unThreadIndex]->cancel_timer(timer_id);
    if (0 == nRet)
    {
        IVS_RUN_LOG_CRI("rtsp reactor manager cancel timer success, timerId[%ld].", timer_id);
        return RET_OK;
    }
    IVS_RUN_LOG_CRI("rtsp reactor manager unregist timer, fail to cancel timer timerId[%ld] nRet[%d].", timer_id, nRet);
    return RET_FAIL;
}

// 获取线程号
unsigned int CRtspReactorManager::getThreadIndex()
{
    ACE_Guard <ACE_Thread_Mutex> locker(m_ThreadNumMutex);
    return m_ThreadNum++;
}//lint !e1788

//*********************************内部实现********************************//

/***************************************************************************
 函 数 名 : createResources
 功能描述 : 创建反应器资源
 输入参数 : 无            
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::createResources()
{
    try
    {
        m_pReactor = new ACE_Reactor*[RTSP_THREAD_NUM];
    }
    catch(...)
    {
        IVS_RUN_LOG_CRI("rtsp reactor manager create resources, fail to create reactor array.");
        return RET_FAIL;
    }
    for(unsigned int i = 0; i < RTSP_THREAD_NUM; i++)
    {
        ACE_Reactor_Impl *pImpl = NULL;

        try
        {
#ifdef WIN32
            pImpl = new ACE_Select_Reactor(RTSP_MAX_EPOLL_SIZE, true);
#else
            pImpl = new ACE_Dev_Poll_Reactor(RTSP_MAX_EPOLL_SIZE, true);
#endif
        }
        catch(...)
        {
            IVS_RUN_LOG_CRI("rtsp reactor manager create resources, fail to create ace reactor[%u].", i);
            return RET_FAIL;
        }

        try
        {
            m_pReactor[i] = new ACE_Reactor(pImpl, true);
        }
        catch(...)
        {
            delete m_pReactor[i];
            m_pReactor[i] = NULL;
            IVS_RUN_LOG_CRI("rtsp reactor manager create resources, fail to create reactor[%u].", i);
            return RET_FAIL;
        }
    }
    return RET_OK; //lint !e429
}//lint !e429

/***************************************************************************
 函 数 名 : createResources
 功能描述 : 创建反应器线程
 输入参数 : 无            
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::createServiceThreads()
{
    size_t* pStackSize = NULL;
    pStackSize = new (size_t[RTSP_THREAD_NUM]);//lint !e838
    for(unsigned int i = 0; i < RTSP_THREAD_NUM; i++)
    {
        pStackSize[i] = DEFAULT_THREAD_STACK_SIZE;
    }
    m_ThreadNum = 0;
    int nRet = activate(THR_NEW_LWP  | THR_JOINABLE, //lint !e835
                        (int)RTSP_THREAD_NUM,
                        0,
                        ACE_DEFAULT_THREAD_PRIORITY,
                        -1,
                        0,
                        0,
                        0,
                        pStackSize,
                        0 );

    delete[] pStackSize;
    if (-1 == nRet)
    {
        IVS_RUN_LOG_CRI("rtsp reactor manager create service threads, fail to active nRet[%d] errno[%d].", nRet, errno);
        return RET_FAIL;
    }
    IVS_RUN_LOG_INF("rtsp reactor manager create service threads success.");
    return RET_OK;
}

//lint +e429


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
  文 件 名 : RtspSessionManager.h
  作    者 : y00182336
  功能描述 : RTSP客户端通讯库
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#ifndef __RTSPSESSIONMANAGER_H_
#define __RTSPSESSIONMANAGER_H_

#include <list>
#include "ace_header.h"
#include "RtspSession.h"
#include "rtsp_client_datatype.h"
#include "RtspReactorManager.h"

using namespace std;

// 定义RTSP连接队列
typedef std::list<CRtspSession*>             RtspSessionList;
typedef std::list<CRtspSession*>::iterator   RtspSessionIter;

class CRtspSessionManagerTimer : public ACE_Event_Handler
{
public:
    CRtspSessionManagerTimer();
    virtual ~CRtspSessionManagerTimer();

    // 定时器回调函数，用于定期检查连接是否需要释放
    int handle_timeout(const ACE_Time_Value &tv, const void *arg);

    // 保存线程ID
    void setThreadIndex(const unsigned int unIndex) {m_unThreadIndex = unIndex;};
    void setRtspSessionListMutex(ACE_Thread_Mutex* pRtspSessionListMutex) {m_pRtspSessionListMutex = pRtspSessionListMutex;};
    void setRtspSessionList(RtspSessionList* pRtspSessionList) {m_pRtspSessionList = pRtspSessionList;};
private:
    unsigned int                m_unThreadIndex;

    ACE_Thread_Mutex*           m_pRtspSessionListMutex;         // 线程锁
    RtspSessionList*            m_pRtspSessionList;              // 所有RTSP连接
};


class CRtspSessionManager
{
public:
    virtual ~CRtspSessionManager();

    static CRtspSessionManager* instance()
    {
        static CRtspSessionManager SessionManager;
        return &SessionManager;
    };

    //初始化，注册反应器
    int init();

    // 关闭连接管理器
    void closeManager();

    // 创建连接
    CRtspSession* createRtspSession(const bool bBlocked);

    // 定时器回调函数，用于定期发送Options消息
    int handle_timeout(const ACE_Time_Value &tv, const void *arg);
    
    // 检查连接是否存在
    bool findSession(const CRtspSession *pSession);

	bool deleteSession(const CRtspSession *pSession);
private:
    CRtspSessionManager();
    
    unsigned int getSessionIndex();

    // 检查队列中是否存在连接
    bool isSessionInList(const CRtspSession *pSession, 
                            RtspSessionList &SessionList) const;

    // 检查连接是否超时
    void checkSession();

    // 连接入队列
    int pushRtspSession(CRtspSession* pSession);
	
    void freeAllTimer();

public:

    fExceptionCallBack          m_fExceptionCallBack;           // 异常回调函数
    void*                       m_pUser;                        // 用户参数

private:

    ACE_Thread_Mutex            m_RtspSessionListMutex;         // 线程锁
    RtspSessionList             m_RtspSessionList;              // 所有RTSP连接

    ACE_Thread_Mutex            m_unSessionIndexMutex;          // 连接号线程锁
    unsigned int                m_unSessionIndex;               // 连接号
public:
    CRtspSessionManagerTimer*   m_pTimer[RTSP_THREAD_NUM];      // 定时器数组
    long                        m_lTimerID[RTSP_THREAD_NUM];    // 定时器ID数组
};

#endif //__RTSPSESSIONMANAGER_H_

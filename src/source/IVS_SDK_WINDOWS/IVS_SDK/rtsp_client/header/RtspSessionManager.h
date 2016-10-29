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
   ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� �� : RtspSessionManager.h
  ��    �� : y00182336
  �������� : RTSP�ͻ���ͨѶ��
  �޸���ʷ :
    1 ���� : 2011-10-12
      ���� : y00182336
      �޸� : ����

 ******************************************************************************/
#ifndef __RTSPSESSIONMANAGER_H_
#define __RTSPSESSIONMANAGER_H_

#include <list>
#include "ace_header.h"
#include "RtspSession.h"
#include "rtsp_client_datatype.h"
#include "RtspReactorManager.h"

using namespace std;

// ����RTSP���Ӷ���
typedef std::list<CRtspSession*>             RtspSessionList;
typedef std::list<CRtspSession*>::iterator   RtspSessionIter;

class CRtspSessionManagerTimer : public ACE_Event_Handler
{
public:
    CRtspSessionManagerTimer();
    virtual ~CRtspSessionManagerTimer();

    // ��ʱ���ص����������ڶ��ڼ�������Ƿ���Ҫ�ͷ�
    int handle_timeout(const ACE_Time_Value &tv, const void *arg);

    // �����߳�ID
    void setThreadIndex(const unsigned int unIndex) {m_unThreadIndex = unIndex;};
    void setRtspSessionListMutex(ACE_Thread_Mutex* pRtspSessionListMutex) {m_pRtspSessionListMutex = pRtspSessionListMutex;};
    void setRtspSessionList(RtspSessionList* pRtspSessionList) {m_pRtspSessionList = pRtspSessionList;};
private:
    unsigned int                m_unThreadIndex;

    ACE_Thread_Mutex*           m_pRtspSessionListMutex;         // �߳���
    RtspSessionList*            m_pRtspSessionList;              // ����RTSP����
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

    //��ʼ����ע�ᷴӦ��
    int init();

    // �ر����ӹ�����
    void closeManager();

    // ��������
    CRtspSession* createRtspSession(const bool bBlocked);

    // ��ʱ���ص����������ڶ��ڷ���Options��Ϣ
    int handle_timeout(const ACE_Time_Value &tv, const void *arg);
    
    // ��������Ƿ����
    bool findSession(const CRtspSession *pSession);

	bool deleteSession(const CRtspSession *pSession);
private:
    CRtspSessionManager();
    
    unsigned int getSessionIndex();

    // ���������Ƿ��������
    bool isSessionInList(const CRtspSession *pSession, 
                            RtspSessionList &SessionList) const;

    // ��������Ƿ�ʱ
    void checkSession();

    // ���������
    int pushRtspSession(CRtspSession* pSession);
	
    void freeAllTimer();

public:

    fExceptionCallBack          m_fExceptionCallBack;           // �쳣�ص�����
    void*                       m_pUser;                        // �û�����

private:

    ACE_Thread_Mutex            m_RtspSessionListMutex;         // �߳���
    RtspSessionList             m_RtspSessionList;              // ����RTSP����

    ACE_Thread_Mutex            m_unSessionIndexMutex;          // ���Ӻ��߳���
    unsigned int                m_unSessionIndex;               // ���Ӻ�
public:
    CRtspSessionManagerTimer*   m_pTimer[RTSP_THREAD_NUM];      // ��ʱ������
    long                        m_lTimerID[RTSP_THREAD_NUM];    // ��ʱ��ID����
};

#endif //__RTSPSESSIONMANAGER_H_

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
  �� �� �� : RtspSessionManager.cpp
  ��    �� : y00182336
  �������� : RTSP�ͻ���ͨѶ��
  �޸���ʷ :
    1 ���� : 2011-10-12
      ���� : y00182336
      �޸� : ����

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

// ��ʱ���ص����������ڶ��ڷ��ͼ������״̬
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

    // �������״̬��Ϣ
    RtspSessionIter iter = m_pRtspSessionList->begin();
    CRtspSession *pSession = NULL;
    while (iter != m_pRtspSessionList->end())
    {
        pSession = *iter;
        RtspSessionIter pIter = iter++;
        if (NULL == pSession)
        {
            // ��List���Ƴ�session���ͷ�
            m_pRtspSessionList->erase(pIter);
            continue;
        }

        // ֻ����ͬ�߳��µ����ӣ�������̲߳������±���
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
 �� �� �� : CRtspSessionManager
 �������� : ���캯��
 ������� : ��
 ������� : ��
 �� �� ֵ : ��
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
 �� �� �� : ~CRtspSessionManager
 �������� : ��������
 ������� : ��
 ������� : ��
 �� �� ֵ : ��
***************************************************************************/
CRtspSessionManager::~CRtspSessionManager()
{
    m_fExceptionCallBack = NULL;
    m_pUser              = NULL;
}

/***************************************************************************
 �� �� �� : init
 �������� : ��ʼ�����ӹ�����
 ������� : ��
 ������� : ��
 �� �� ֵ : ��
***************************************************************************/
int CRtspSessionManager::init()
{
    // ������ʱ��
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
 �� �� �� : freeAllTimer
 �������� : ֹͣ��ʱ����ɾ����Դ
 ������� : ��
 ������� : ��
 �� �� ֵ : ��
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
 �� �� �� : closeManager
 �������� : �ر����ӹ�����
 ������� : ��
 ������� : ��
 �� �� ֵ : ��
***************************************************************************/
void CRtspSessionManager::closeManager()
{
    // ȡ����ʱ����ɾ����ʱ����Դ
    freeAllTimer();

    ACE_Guard<ACE_Thread_Mutex> locker(m_RtspSessionListMutex);
    // �ر��������ӣ��������ӳ�����
    if (m_RtspSessionList.empty())
    {
        IVS_RUN_LOG_INF("Session list already empty.");
        return;
    }
    RtspSessionIter iter = m_RtspSessionList.begin();
    CRtspSession *pSession = NULL;
    // �ر�ͨѶ��ʱ���ж�
    while(iter != m_RtspSessionList.end())
    {
        pSession = *iter;
        RtspSessionIter pIter = iter++;
        if (NULL == pSession)
        {
            // ��List���Ƴ�session���ͷ�
            m_RtspSessionList.erase(pIter);
            continue;
        }
        else
        {
            // ����쳣����
            (void)pSession->disconnectPeer();
            IVS_RUN_LOG_INF("start release rtsp session [0x%p]", pSession);
            // ��List���Ƴ�session���ͷ�
            m_RtspSessionList.erase(pIter);
            delete pSession;
            pSession = NULL;
        }
    }

    return;
}//lint !e1788



/***************************************************************************
 �� �� �� : createRtspSession
 �������� : ����RTSP����
 ������� : bBlocked                ͬ��/�첽��־λ��true��ͬ����false���첽��
 ������� : ��
 �� �� ֵ : CRtspSession*           NULL-ʧ��
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

    // ����ͬ����־λ
    RtspSession->m_bBlocked = bBlocked;
    RtspSession->m_unSessionIndex = getSessionIndex();

    // ���������
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
 �� �� �� : findSession
 �������� : ��������
 ������� : pSession                �����ҵ�����
 ������� : ��
 �� �� ֵ : true-�ɹ�               false-ʧ��
***************************************************************************/
bool CRtspSessionManager::findSession(const CRtspSession *pSession)
{
    if (NULL == pSession)
    {
        return false;
    }
    
    // �����д��������Ӷ����в���
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
	// �����д��������Ӷ����в���
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

//*************************�ڲ�ʵ��**************************//

/***************************************************************************
 �� �� �� : pushRtspSession
 �������� : RtspSession���������
 ������� : pSession                ������������
 ������� : ��
 �� �� ֵ : RET_OK-�ɹ�             NULL-ʧ��
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
 �� �� �� : isSessionInList
 �������� : ��������Ƿ����
 ������� : pSession                ������������
            SessionList             �������Ķ���
 ������� : ��
 �� �� ֵ : true-����               false-������
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


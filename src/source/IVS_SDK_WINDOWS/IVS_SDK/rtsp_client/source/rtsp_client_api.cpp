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
  �� �� �� : RTSP_CLIENT_API.cpp
  ��    �� : y00182336
  �������� : RTSP�ͻ���ͨѶ��
  �޸���ʷ :
    1 ���� : 2011-10-12
      ���� : y00182336
      �޸� : ����

 ******************************************************************************/
#include "rtsp_client_api.h"
#include "RtspRetCode.h"
#include "Rtsp_Log_Manager.h"
#include "RtspReactorManager.h"
#include "RtspSessionManager.h"
#include "RtspSetupMessage.h"
#include "RtspPlayMessage.h"
#include "RtspPauseMessage.h"
#include "RtspTeardownMessage.h"
#include "RtspOptionsMessage.h"
#include "RtspSession.h"
#include "eSDK_Securec.h"

// ����ͷ��������־λ
static bool g_bIsFirst = true;         //lint !e956

/* ommment for win64
// Ϊjava�ṩ__stdcall��ʽ�µı���
#ifdef WIN32
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SetLog=_RTSP_CLIENT_SetLog@12")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_Init=_RTSP_CLIENT_Init@8")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_GetHandle=_RTSP_CLIENT_GetHandle@8")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SetExceptionCallBack=_RTSP_CLIENT_SetExceptionCallBack@8")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SetDataCallBack=_RTSP_CLIENT_SetDataCallBack@12")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SendSetupMsg=_RTSP_CLIENT_SendSetupMsg@28")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SendPlayMsg=_RTSP_CLIENT_SendPlayMsg@24")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SendPauseMsg=_RTSP_CLIENT_SendPauseMsg@12")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SendTeardownMsg=_RTSP_CLIENT_SendTeardownMsg@12")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_GetStatus=_RTSP_CLIENT_GetStatus@4")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_FreeHandle=_RTSP_CLIENT_FreeHandle@4")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_Cleanup=_RTSP_CLIENT_Cleanup@0")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SendOptionMsg=_RTSP_CLIENT_SendOptionMsg@20")
#endif
*/
/***************************************************************************
 �� �� �� : RTSP_CLIENT_SetLog
 �������� : ͨѶ����־�̳߳�ʼ�����������ó�ʼ������־����ӡ
 ������� : enLevel        ��־�ȼ�
            bDebugflag     ��ӡDEBUG��Ϣ��־
			pszPath       ��־�ļ�·��
 ������� : ��
 �� �� ֵ : RET_OK-�ɹ�     RET_FAIL-ʧ��
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SetLog(const unsigned int unLevel, const int iDebugflag, 
                               const char* pszPath)
{ //lint !e1784
    if (unLevel > (unsigned int)RTSP_LOG_LEVEL_DEBUG) //lint !e40
    {
        IVS_RUN_LOG_CRI("set rtsp client log, invalid log Level[%u], debug switch[%d].",
                        unLevel, iDebugflag); //lint !e730 !e40 !e119
        return RTSP_ERR_PARAMETER;            //lint !e40
    }

    if (NULL != pszPath)
    {
        if (NSS_LOG_FILE_PATH_LEN < strlen(pszPath))
        {
            IVS_RUN_LOG_CRI("set rtsp client log, file path too long[%d] Level[%u], debug switch[%d].",
                ACE_OS::strlen(pszPath), unLevel, iDebugflag); //lint !e730 !e40 !e119
            return RTSP_ERR_PARAMETER;                         //lint !e40
        }
        eSDK_STRNCPY(CRtsp_Log_Manager::instance().m_strLogRootPath, NSS_LOG_FILE_PATH_LEN + 1, pszPath, strlen(pszPath));
    }

    // ������־
    CRtsp_Log_Manager::instance().m_RefreshLevel = (RTSP_LOG_LEVEL_E) unLevel;//lint !e63 !e1013 !e40 !e10
    CRtsp_Log_Manager::instance().m_bDebugFlag = iDebugflag ? true : false;
    IVS_RUN_LOG_INF("rtsp client init log manager success.");
    return RET_OK;
}



/***************************************************************************
 �� �� �� : RTSP_CLIENT_Init
 �������� : ͨѶ���ʼ��
 ������� : ��
 ������� : ��
 �� �� ֵ : RET_OK-�ɹ�     RET_FAIL-ʧ��
***************************************************************************/
int RTSPAPI RTSP_CLIENT_Init(unsigned int unLevel, const char* pszPath) /*lint -esym(714,RTSP_CLIENT_Init)*/
{ //lint !e1784
    // �����ظ�����
    if (!g_bIsFirst)
    {
        return RET_OK;
    }
    g_bIsFirst = false;
//#ifdef WIN32
    ACE::init();
//#endif

    // �����־·��Ϊ�գ�ȡĬ��·��
    std::string strLogPath = RTSP_CLIENT_LOG_PATH;
    if (NULL != pszPath && 0 != strlen(pszPath))
    {
        strLogPath = pszPath;
        for(string::size_type pos(0); pos!=string::npos; pos += strlen("/")) 
        {
            if((pos = strLogPath.find("\\",pos)) != string::npos)  
            {
                strLogPath.replace(pos,strlen("\\"),"/"); 
            }
            else 
            {
                break;    
            }
        }    
    }//lint !e850
    // ����־
    int nRet = CRtsp_Log_Manager::instance().initLog(RTSP_CLIENT_SERVICE_NAME, 
                                                     strLogPath.c_str(), unLevel);
    if (RET_OK != nRet)
    {
        return RET_FAIL;
    }

	// �������ӹ���
	nRet= CRtspSessionManager::instance()->init();
	if (RET_OK != nRet)
	{
    	(void)CRtsp_Log_Manager::instance().endLog();
		return RET_FAIL;
	}

    // �򿪷�Ӧ������
    nRet = CRtspReactorManager::instance()->openManager();
    if (RET_OK != nRet)
    {
        (void)CRtsp_Log_Manager::instance().endLog();
        return RET_FAIL;
    }

    IVS_DBG_LOG("init rtsp client success.");
    return RET_OK;
}



/***************************************************************************
 �� �� �� : RTSP_CLIENT_GetHandle
 �������� : ��ȡ���Ӿ��
 ������� : bBlocked        ͬ��/�첽��ʽ
 ������� : nHandle         �������Ӿ��
 �� �� ֵ : RET_OK-�ɹ�     RET_FAIL-ʧ��
***************************************************************************/
int RTSPAPI RTSP_CLIENT_GetHandle(const bool bBlocked, /*lint -esym(714,RTSP_CLIENT_GetHandle)*/
                                  IVS_ULONG *nHandle)
{ //lint !e1784
    if (NULL == nHandle)
    {
        IVS_RUN_LOG_CRI("handle is NULL."); //lint !e119 !e40
        return RET_FAIL;
    }
    CRtspSession* pSession = CRtspSessionManager::instance()->createRtspSession(bBlocked);
    if (NULL == pSession)
    {
        IVS_RUN_LOG_CRI("get session handle fail.");//lint !e119 !e40
        return RET_FAIL;
    }
    *nHandle = (IVS_ULONG)pSession;
    
    IVS_DBG_LOG("get session handle success, mode[%d], handle[0x%p].", bBlocked, pSession); //lint !e730
    return RET_OK;
}


/***************************************************************************
 �� �� �� : RTSP_CLIENT_SetExceptionCallBack
 �������� : ���ûص�����
 ������� : cbExceptionCallBack          �ص�����
            pUser                        �û��������ص������з���
 ������� : ��
 �� �� ֵ : RET_OK-�ɹ�     RET_FAIL-ʧ��
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SetExceptionCallBack   /*lint -esym(714,RTSP_CLIENT_SetExceptionCallBack)*/
(fExceptionCallBack cbExceptionCallBack, void* pUser) //lint !e40 !e10
{ //lint !e1784 !e10
    // �������
    if (NULL == cbExceptionCallBack)
    {
        IVS_RUN_LOG_CRI("set Exception Call Back function fail, parameter abnormal.");
        return RTSP_ERR_PARAMETER;
    }

    CRtspSessionManager::instance()->m_fExceptionCallBack = cbExceptionCallBack;
    CRtspSessionManager::instance()->m_pUser = pUser;
    IVS_DBG_LOG("set Exception Call Back function success.");
    return RET_OK;
}


/***************************************************************************
 �� �� �� : RTSP_CLIENT_SetDataCallBack
 �������� : ����ý�����ص�����
 ������� :  nHandle                    ���Ӿ��
            cbDataCallBack              ý�����ص�����
            pUser                       �û��������ص������з���
 ������� : ��
 �� �� ֵ : RET_OK-�ɹ�     RET_FAIL-ʧ��
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SetDataCallBack /*lint -esym(714,RTSP_CLIENT_SetDataCallBack)*/
(IVS_ULONG nHandle, fDataCallBack cbDataCallBack, void* pUser) //lint !e601 !e40
{ //lint !e1784 !e745
    // �����Ч�Լ��
    if(0 == nHandle)
    {
        IVS_RUN_LOG_CRI("parameter invalid. 0 == nHandle");//lint !e119 !e40
        return RTSP_ERR_PARAMETER; //lint !e40
    }

	if(NULL == cbDataCallBack)
	{
		IVS_RUN_LOG_CRI("set data Call Back for NULL.");//lint !e119 !e40
	}

    // ��������Ƿ����
    CRtspSession* pSession = (CRtspSession*) (nHandle);//lint !e1924
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_CRI("set data Call Back function fail, session not exist, handle[0x%p].", pSession);//lint !e119 !e40
        return RTSP_ERR_NOT_EXIST; //lint !e40
    }

    // ���� modify by zwx211831�����ûص���Ҫ�͵��ûص�����ʹ��ͬһ����
	{
		ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_CallBackMutex);
		pSession->m_fDataCallBack = cbDataCallBack;  //lint !e63 !e1013 !e40
	}
    
    pSession->m_pUser = pUser;
    IVS_DBG_LOG("set data Call Back function success, handle[0x%p].", pSession);
    return RET_OK;
}//lint !e1788


static int sendSetupMessage(const bool bTcpFlag, CRtspSession* pSession, 
                           const char *sURL,
                           const INET_ADDR* pstServerAddr,        //lint !e10 !e808
                           const MEDIA_ADDR* pstLocalMediaAddr)   //lint !e49
{//lint !e49
    // ý����ͨ��TCP��ʽ���䣬�ûص��������ݳ�ȥ
    if ((bTcpFlag) && (NULL == pSession->m_fDataCallBack))   //lint !e1013 !e40 !e10
    {
        IVS_RUN_LOG_CRI("data call back funtion not exist.");//lint !e119 !e40
        return RTSP_ERR_PARAMETER; //lint !e40
    }

    // �������״̬
    if (RTSP_SESSION_STATUS_INIT != pSession->getStatus()) //lint !e1013 !e1055 !e40 !e10 !e746
    {
        IVS_RUN_LOG_CRI("send rtsp setup message fail, invalid status[%d], handle[%p].", 
                        pSession->getStatus(), pSession);//lint !e119 !e1013 !e40 !e10
        return RTSP_ERR_STAUTS_ABNORMAL; //lint !e40
    }

    // ����SETUP��Ϣ�࣬���յ���Ӧ���߳�ʱ�ͷ�
    CRtspSetupMessage* pRtspMsg = NULL;
    pRtspMsg = pSession->createRtspMessage((CRtspSetupMessage*)NULL); //lint !e838 !e64 !e1013 !e1055 !e40 !e10 !e746
    if (NULL == pRtspMsg)
    {
        IVS_RUN_LOG_CRI("create rtsp setup requst message fail.");//lint !e119 !e40
        return RET_FAIL;
    }

    // �������ӷ�ʽ
    pRtspMsg->m_bTcpFlag = bTcpFlag; //lint !e40
    // ����URL
    std::string strRtspUrl = "";
    strRtspUrl.append(sURL);         //lint !e1025 !e40
    pRtspMsg->setRtspUrl(strRtspUrl);
    // ���ô�������
    if (bTcpFlag) //lint !e40
    {
        // Setup��Ϣ����¼TCP���ʹ���ý������Ӳ����Ҫ
        pRtspMsg->setTransType(TRANS_TYPE_TCP);
        if (NULL != pstLocalMediaAddr) //lint !e40
        {
            pRtspMsg->setDestinationIp(pstLocalMediaAddr->unMediaIp);       //lint !e613 !e1013 !e40 !e10
            pRtspMsg->setClientPort(pstLocalMediaAddr->unMediaVideoPort,    //lint !e1013 !e40 !e10
                                    pstLocalMediaAddr->unMediaAudioPort);   //lint !e613 !e1013 !e40 !e10
        }
    }
    else
    {
        pRtspMsg->setTransType(TRANS_TYPE_UDP);
        // ����ý�������ؽ��յ�ַ�Ͷ˿�
        pRtspMsg->setDestinationIp(pstLocalMediaAddr->unMediaIp);       //lint !e613 !e1013 !e40 !e10
        pRtspMsg->setClientPort(pstLocalMediaAddr->unMediaVideoPort,    //lint !e1013 !e40 !e10
                                pstLocalMediaAddr->unMediaAudioPort);   //lint !e613 !e1013 !e40 !e10

    }


    // ��װRTSP SETUP��Ϣ
    std::string strRtspMsg = "";
    int nRet = pRtspMsg->encodeMessage(strRtspMsg);
    if (RET_OK != nRet)
    {
        pSession->destroyRtspMsg();  //lint !e534 !e1013 !e1055 !e40 !e10 !e746
        return RET_FAIL;
    }

    // ��������
    ACE_INET_Addr remoteAddr(pstServerAddr->unAddrPort, pstServerAddr->unAddrIp); //lint !e1013 !e40 !e10

    nRet = pSession->connectPeer(remoteAddr); //lint !e1013 !e1055 !e40 !e10 !e746
    if (RET_OK != nRet)
    {
        IVS_RUN_LOG_CRI("connect peer fail, remote address[%s:%d], handle[0x%p].", 
                        remoteAddr.get_host_addr(),
                        remoteAddr.get_port_number(),
                        pSession);//lint !e119 !e40
        (void)pSession->disconnectPeer(); //lint !e1013 !e1055 !e40 !e10 !e746
        return RTSP_ERR_DISCONNECT; //lint !e40
    }
    IVS_RUN_LOG_INF("connect peer success, remote address [%s:%d], handle[0x%p].", 
                    remoteAddr.get_host_addr(),
                    remoteAddr.get_port_number(),
                    pSession);

    // ����RTSP SETUP��Ϣ
    nRet = pSession->sendMessage(strRtspMsg.c_str(), strRtspMsg.length()); //lint !e1013 !e1055 !e40 !e10 !e746
    if (RET_OK != nRet)
    {
		IVS_RUN_LOG_INF("sendSetupMessage failed, remote address [%s:%d], handle[0x%p] nRet[%d].", 
						remoteAddr.get_host_addr(),
						remoteAddr.get_port_number(),
						pSession, nRet);
        return nRet;
    }

	IVS_RUN_LOG_INF("sendSetupMessage succeed, remote address [%s:%d], handle[0x%p].", 
					remoteAddr.get_host_addr(),
					remoteAddr.get_port_number(),
					pSession);
    return RET_OK;
}


static int handleSynchSetup(CRtspSession* pSession, const unsigned int unTimeout,
                            const bool /*bTcpFlag*/, MEDIA_ADDR *pstPeerMediaAddr) //lint !e601 !e40
{
    // �ȴ��źŴ��������������ڴ�
    ACE_Time_Value wait = ACE_OS::gettimeofday();
    wait.sec (wait.sec () + unTimeout);

    int nRet = pSession->m_pRtspSemaphore->acquire(&wait);
    // ���ճ�ʱ��ֱ�ӷ���
    if(0 != nRet)
    {
        ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_Mutex);
        IVS_RUN_LOG_ERR("receive rtsp setup response out of time, handle[0x%p].", pSession);
        pSession->destroyRtspMsg();
        return RTSP_ERR_TIMEOUT; //lint !e40
    }//lint !e1788

    // ��������ֱ�ӷ���
    if (!pSession->m_bRecvRetFlag)
    {
		ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_Mutex);
		int iRet = pSession->m_iRetCode;
        pSession->destroyRtspMsg();
		IVS_RUN_LOG_ERR("receive rtsp setup response decode error, handle[0x%p], RetCode[%d].", pSession, iRet);
        return iRet;
    }//lint !e1788

    // UDP���䷽ʽ�£���ȡý�������͵�ַ
    //if (false == bTcpFlag)
    const CRtspSetupMessage* pRtspMsg = dynamic_cast<const CRtspSetupMessage*>(pSession->m_pRtspMsg);
    if (NULL == pRtspMsg)
    {
		IVS_RUN_LOG_ERR("receive rtsp setup response msg is null, handle[0x%p].", pSession);
        return RET_FAIL;
    }
    if (0 != pRtspMsg->getServerIp())
    {
        pstPeerMediaAddr->unMediaIp        = pRtspMsg->getServerIp();           //lint !e613 !e63 !e1013 !e40 !e10
        pstPeerMediaAddr->unMediaVideoPort = pRtspMsg->getServerVideoPort();    //lint !e613 !e63 !e1013 !e40 !e10
        pstPeerMediaAddr->unMediaAudioPort = pRtspMsg->getServerAudioPort();    //lint !e613 !e63 !e1013 !e40 !e10
    }

    // �ͷ���Ϣ
    pSession->destroyRtspMsg();
    // �����ɹ�������״̬Ϊready
    pSession->setStatus(RTSP_SESSION_STATUS_READY); //lint !e40
    IVS_RUN_LOG_INF("send setup message success, handle[0x%p].", pSession);
    return RET_OK;
}//lint !e550 !e818

/***************************************************************************
 �� �� �� : RTSP_CLIENT_SendSetupMsg
 �������� : ����RTSP SETUP����
 ������� :  nHandle                     ���Ӿ��
            pstServerAddr               RTSP��������ַ
            sURL                        SETUP��Ϣ��װ�е�URL
            unTimeout                   SETUP��Ϣ��ʱʱ�䣨��λ��S��
            bTcpFlag                    �����Ƿ�ΪTCP��ʽ
            pstLocalMediaAddr           ͬ��ģʽ�£�ý�������ؽ��յ�ַ���첽ģʽ��Ϊ��
 ������� : pstPeerMediaAddr            ͬ��ģʽ�£�ý�������Ͷ˵�ַ���첽ģʽ��Ϊ��
 �� �� ֵ : RET_OK-�ɹ�     ����-ʧ��
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SendSetupMsg                                                  /*lint -esym(714,RTSP_CLIENT_SendSetupMsg)*/
                                    (IVS_ULONG nHandle , const INET_ADDR* pstServerAddr,    //lint !e808 !e10
                                    const char *sURL, unsigned int unTimeout, bool bTcpFlag,
                                    const MEDIA_ADDR* pstLocalMediaAddr, MEDIA_ADDR *pstPeerMediaAddr )  //lint !e49
{ //lint !e1784 !e49
    // �������
    if ((0 == nHandle) || (NULL == pstServerAddr) //lint !e40
        || (0 == unTimeout) || (NULL == sURL)     //lint !e40
        || (NULL == CRtspSessionManager::instance()->m_fExceptionCallBack)) //lint !e1013 !e40
    {
        IVS_RUN_LOG_ERR("send rtsp setup message fail, parameter invalid.");
        return RTSP_ERR_PARAMETER; //lint !e40
    }

    // UDP��ʽ�����ý���ַ
    if ((!bTcpFlag) && ((NULL == pstLocalMediaAddr) || (NULL == pstPeerMediaAddr))) //lint !e40
    {
        IVS_RUN_LOG_ERR("udp channel must have media addr.");
        return RTSP_ERR_PARAMETER; //lint !e40
    }

    // ��������Ƿ����
    CRtspSession* pSession = (CRtspSession*) (nHandle);    //lint !e40 !e1924
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_ERR("send rtsp setup message fail, session not exist, handle[0x%p].", pSession);
        return RTSP_ERR_NOT_EXIST; //lint !e40
    }

	// add by w00210470
	pSession->EnableRtpTimeoutCheck(NULL == pstLocalMediaAddr); //lint !e40

    // ����
    ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_SessionMutex);
    int nRet = sendSetupMessage(bTcpFlag, pSession, sURL, pstServerAddr, pstLocalMediaAddr);//lint !e119 !e40
    if (RET_OK != nRet)
    {
		IVS_RUN_LOG_ERR("sendSetupMessage fail 2, handle[0x%p].", pSession);
        return nRet;
    }

    // ����ͬ����ʽ���ȴ��ź���
    if (pSession->m_bBlocked)
    {
        nRet = handleSynchSetup(pSession, unTimeout, bTcpFlag, pstPeerMediaAddr); //lint !e40
        if (RET_OK != nRet)
        {
            IVS_RUN_LOG_INF("send setup message fail, synch mode handle[0x%p], RetCode[%d].", pSession, nRet);
        }
        return nRet;
    }
    else
    {
        // �첽��ʽ�£�ע�ᳬʱ��ʱ�����ڽ��յ���Ӧ��Ϣ��ʱ��ȡ����ʱ��
        nRet = pSession->setRecvTimer(unTimeout); //lint !e40
        if (RET_OK != nRet)
        {
            pSession->destroyRtspMsg();
            return RET_FAIL;
        }
    }

    IVS_RUN_LOG_INF("send setup message success, handle[0x%p].", pSession);
    return RET_OK;
}//lint !e1788


/***************************************************************************
 �� �� �� : RTSP_CLIENT_SendPlayMsg
 �������� : ����RTSP PLAY����
 ������� :  nHandle                    ���Ӿ��
            sURL                        PLAY��Ϣ��װ�е�URL
            dScale                      �ط�/���ز�������
            stRange                     �ط�/����ʱ���
            unTimeout                   PLAY��Ϣ��ʱʱ�䣨��λ��S��
 ������� : ��
 �� �� ֵ : RET_OK-�ɹ�     RET_FAIL-ʧ��
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SendPlayMsg(IVS_ULONG nHandle, const char *sURL, double dScale,  /*lint -esym(714,RTSP_CLIENT_SendPlayMsg)*/
                                    const MEDIA_RANGE_S* stRange, unsigned int unTimeout)  //lint !e49 !e10 !e808
{ //lint !e1784 !e49
    // �����Ч�Լ��
    if ((0 == nHandle) || (0 == unTimeout) || (NULL == sURL)) //lint !e40
    {
        IVS_RUN_LOG_CRI("send rtsp play message fail, parameter is invalid.");//lint !e119 !e40
        return RTSP_ERR_PARAMETER; //lint !e40
    }

    // ��������Ƿ����
    CRtspSession* pSession = (CRtspSession*) (nHandle);     //lint !e40 !e1924
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_CRI("send rtsp play message fail, session not exist, handle[0x%p].", pSession);//lint !e119 !e40
        return RTSP_ERR_NOT_EXIST; //lint !e40
    }

    // ����
    ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_SessionMutex);
    // �������״̬
    if (RTSP_SESSION_STATUS_INIT == pSession->getStatus()) //lint !e1013 !e40
    {
        IVS_RUN_LOG_CRI("send rtsp play message fail, invalid status[%d], handle[0x%p].",
                        pSession->getStatus(),
                        pSession);//lint !e119 !e1013 !e40
        return RTSP_ERR_STAUTS_ABNORMAL;//lint !e40
    }

    // ����PLAY��Ϣ�࣬ʹ�����ɾ��
    CRtspPlayMessage* pRtspMsg = NULL;
    pRtspMsg = pSession->createRtspMessage((CRtspPlayMessage*)NULL); //lint !e838
    if (NULL == pRtspMsg)
    {
        return RET_FAIL;
    }

    // ����RTSP URL
    std::string strRtspUrl = "";
    strRtspUrl.append(sURL);          //lint !e1025 !e40
    pRtspMsg->setRtspUrl(strRtspUrl);
    pRtspMsg->setRang(stRange); //lint !e40
    pRtspMsg->setScale(dScale); //lint !e40

    // RTSP PLAY��Ϣ��װ
    string strRtspMsg = "";
    int nRet = pRtspMsg->encodeMessage(strRtspMsg);
    if (RET_OK != nRet)
    {
        pSession->destroyRtspMsg();
        return RET_FAIL;
    }

    // ����RTSP PLAY��Ϣ
    nRet = pSession->sendMessage(strRtspMsg.c_str(), strRtspMsg.length());
    if (RET_OK != nRet)
    {
        return nRet;
    }
    // ����ͬ����ʽ
    if (pSession->m_bBlocked)
    {
        // �ȴ��źŴ��������������ڴ�
        ACE_Time_Value wait = ACE_OS::gettimeofday();
        wait.sec (wait.sec () + unTimeout); //lint !e40
        nRet = pSession->m_pRtspSemaphore->acquire(&wait);
        if (0 != nRet)
        {
            ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_Mutex); //lint !e578
            pSession->destroyRtspMsg();
            return RTSP_ERR_TIMEOUT; //lint !e40
        }//lint !e1788
        // ����ʧ�ܣ�ֱ�ӷ���
        if (!pSession->m_bRecvRetFlag)
        {
            nRet = pSession->m_iRetCode;
            return nRet;
        }
		// ���سɹ�������RTP���ݼ��
		pSession->SetupRtpTimer(); 
    }
    else
    {
        // �첽��ʽ�£�ע�ᳬʱ��ʱ��
        nRet = pSession->setRecvTimer(unTimeout); //lint !e40
        if (RET_OK != nRet)
        {
            pSession->destroyRtspMsg();
            return RET_FAIL;
        }
    }


    IVS_DBG_LOG("send play message success, handle[0x%p].", pSession);
    return RET_OK;
}//lint !e1788

/***************************************************************************
 �� �� �� : RTSP_CLIENT_SendOptionMsg
 �������� : ����RTSP OPTIONS����
 ������� :  nHandle                    ���Ӿ��
            sURL                        OPTIONS��Ϣ��װ�е�URL
            unTimeout                   OPTIONS��Ϣ��ʱʱ�䣨��λ��S��
 ������� : ��
 �� �� ֵ : RET_OK-�ɹ�     RET_FAIL-ʧ��
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SendOptionMsg(IVS_ULONG nHandle,const char *sURL,unsigned int unTimeout, char *strMsg, int &iLength) /*lint -esym(714,RTSP_CLIENT_SendOptionMsg)*/
{
    if ((0 == nHandle) || (NULL == sURL) || (0 == unTimeout))
    {
        IVS_RUN_LOG_CRI("send teardown message fail, parameter is invalid.");//lint !e119 !e40
        return RTSP_ERR_PARAMETER; //lint !e40
    }

    // ��������Ƿ����
    CRtspSession* pSession = (CRtspSession*)nHandle; //lint !e1924
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_CRI("handle is not a valid Session.");//lint !e119 !e40
        return RTSP_ERR_NOT_EXIST; //lint !e40
    }

    // ����
    ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_SessionMutex);

    // ��װoption��Ϣ
    CRtspOptionsMessage* pRtspMsg = NULL;
    pRtspMsg = pSession->createRtspMessage((CRtspOptionsMessage*)NULL); //lint !e838
    if (NULL == pRtspMsg)
    {
        IVS_RUN_LOG_CRI("create option message failed.");//lint !e119 !e40
        return RET_FAIL;
    }

    std::string strRtspMsg = "";
    pRtspMsg->setRtspUrl(sURL);
    int nRet = pRtspMsg->encodeMessage(strRtspMsg);
    if (RET_OK != nRet)
    {
        pSession->destroyRtspMsg();
        IVS_RUN_LOG_CRI("encode pause message fail.");//lint !e119 !e40
        return RET_FAIL;
    }

    // ����RTSP OPTION��Ϣ
    nRet = pSession->sendMessage(strRtspMsg.c_str(), strRtspMsg.length());
    if (RET_OK != nRet)
    {
        IVS_RUN_LOG_CRI("send pause message fail.");//lint !e119 !e40
        return RET_FAIL; 
    }

    // �����ͬ��ģʽ���ȴ����Ž��
    if (pSession->m_bBlocked)
    {
        // ����Pause��Ϣ�ĳ�ʱʱ��
        ACE_Time_Value wait = ACE_OS::gettimeofday();
        wait.sec(wait.sec() + unTimeout);
        nRet = pSession->m_pRtspSemaphore->acquire(&wait);
        if (-1 == nRet)
        {
            ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_Mutex); //lint !e578
            pSession->destroyRtspMsg();
            IVS_RUN_LOG_CRI("Sync mode:recv pause response timeout.");//lint !e119 !e40
            return RTSP_ERR_TIMEOUT; //lint !e40
        }//lint !e1788
        else if (0 != nRet)
        {
            IVS_RUN_LOG_CRI("Sync mode:recv pause response fail.");//lint !e119 !e40
            return RET_FAIL;
        }
        else
        {
        }

        if (!pSession->m_bRecvRetFlag)
        {
            return RET_FAIL;
        }
        else
        {
            if (NULL == strMsg)
            {
                return RET_FAIL;
            }
            iLength = (int)pSession->m_strTimeInfo.length();//lint !e1924
            eSDK_MEMCPY(strMsg, (unsigned int)iLength, pSession->m_strTimeInfo.c_str(),(unsigned int)iLength); //lint !e586
        }
    }
    else
    {   //�첽ģʽ��ע�ᳬʱ��ʱ��
        nRet = pSession->setRecvTimer(unTimeout);
        if (RET_OK != nRet)
        {
            pSession->destroyRtspMsg();
            IVS_RUN_LOG_CRI("Async mode:set pause recv timer failed.");//lint !e119 !e40
            return RET_FAIL;
        }
    }
    return RET_OK;
}//lint !e1788
/***************************************************************************
 �� �� �� : RTSP_CLIENT_SendPauseMsg
 �������� : ����RTSP PAUSE����
 ������� :  nHandle                     ���Ӿ��
             sURL                        PAUSE��Ϣ��װ�е�URL
             unTimeout                   PAUSE��Ϣ��ʱʱ�䣨��λ��S��
 ������� : ��
 �� �� ֵ : RET_OK-�ɹ�     RET_FAIL-ʧ��
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SendPauseMsg(IVS_ULONG nHandle, const char *sURL, unsigned int unTimeout) /*lint -esym(714,RTSP_CLIENT_SendPauseMsg)*/
{ //lint !e1784
    // �����Ч�Լ��
    if ((0 == nHandle) || (NULL == sURL) || (0 == unTimeout))
    {
        IVS_RUN_LOG_CRI("send pause message fail, parameter invalid.");//lint !e119 !e40
        return RTSP_ERR_PARAMETER; //lint !e40
    }

    // ��������Ƿ����
    CRtspSession* pSession = (CRtspSession*)nHandle;//lint !e1924
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_CRI("handle is not a valid Session.");//lint !e119 !e40
        return RTSP_ERR_NOT_EXIST; //lint !e40
    }

    // ����
    ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_SessionMutex);
    // ���Ự״̬������Ѿ���READY״̬��ֱ�ӷ���
    if (RTSP_SESSION_STATUS_READY == pSession->getStatus()) //lint !e1013 !e40
    {
        return RET_OK;
    } 
    else if (RTSP_SESSION_STATUS_PLAYING != pSession->getStatus()) //lint !e1013 !e40
    {
        IVS_RUN_LOG_CRI("send rtsp pause message fail, invalid status[%d], handle[0x%p].", 
                        pSession->getStatus(),
                        pSession);//lint !e119 !e1013 !e40
        // �������PLAYING״̬�����ش���
        return RTSP_ERR_STAUTS_ABNORMAL; //lint !e40
    }
    else
    {
    }

    // ��װpause��Ϣ
    CRtspPauseMessage* pRtspMsg = NULL;
    pRtspMsg = pSession->createRtspMessage((CRtspPauseMessage*)NULL); //lint !e838
    if (NULL == pRtspMsg)
    {
        IVS_RUN_LOG_CRI("create pause message failed.");//lint !e119 !e40
        return RET_FAIL;
    }

    pRtspMsg->setRtspUrl(sURL);
    std::string strRtspMsg = "";
    int nRet = pRtspMsg->encodeMessage(strRtspMsg);
    if (RET_OK != nRet)
    {
        pSession->destroyRtspMsg();
        IVS_RUN_LOG_CRI("encode pause message fail.");//lint !e119 !e40
        return RET_FAIL;
    }

    // ����RTSP Pause��Ϣ
    nRet = pSession->sendMessage(strRtspMsg.c_str(), strRtspMsg.length());
    if (RET_OK != nRet)
    {
        IVS_RUN_LOG_CRI("send pause message fail.");//lint !e119 !e40
        return RET_FAIL;
    }

    // �����ͬ��ģʽ���ȴ����Ž��
    if (pSession->m_bBlocked)
    {
        // ����Pause��Ϣ�ĳ�ʱʱ��
        ACE_Time_Value wait = ACE_OS::gettimeofday();
        wait.sec(wait.sec() + unTimeout);
        nRet = pSession->m_pRtspSemaphore->acquire(&wait);
        if (-1 == nRet)
        {
            ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_Mutex); //lint !e578
            pSession->destroyRtspMsg();
            IVS_RUN_LOG_CRI("Sync mode:recv pause response timeout.");//lint !e119 !e40
            return RTSP_ERR_TIMEOUT; //lint !e40
        }//lint !e1788
        else if (0 != nRet)
        {
            IVS_RUN_LOG_CRI("Sync mode:recv pause response fail.");//lint !e119 !e40
            return RET_FAIL;
        }
        else
        {
        }

        if (!pSession->m_bRecvRetFlag)
        {
            return RET_FAIL;
        }
    }
    else
    {   //�첽ģʽ��ע�ᳬʱ��ʱ��
        nRet = pSession->setRecvTimer(unTimeout);
        if (RET_OK != nRet)
        {
            pSession->destroyRtspMsg();
            IVS_RUN_LOG_CRI("Async mode:set pause recv timer failed.");//lint !e119 !e40
            return RET_FAIL;
        }
    }
    return RET_OK;
}//lint !e1788


/***************************************************************************
 �� �� �� : RTSP_CLIENT_SendTeardownMsg
 �������� : ����RTSP TEARDOWN����
 ������� :  nHandle                    ���Ӿ��
            sURL                        TEARDOWN��Ϣ��װ�е�URL
            unTimeout                   PAUSE��Ϣ��ʱʱ�䣨��λ��S��
 ������� : ��
 �� �� ֵ : RET_OK-�ɹ�     RET_FAIL-ʧ��
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SendTeardownMsg(IVS_ULONG nHandle, const char *sURL, unsigned int unTimeout) /*lint -esym(714,RTSP_CLIENT_SendTeardownMsg)*/
{ //lint !e1784
    // ��μ��
    if ((0 == nHandle) || (NULL == sURL) || (0 == unTimeout))
    {
        IVS_RUN_LOG_CRI("send teardown message fail, parameter is invalid.");//lint !e119 !e40
        return RTSP_ERR_PARAMETER;//lint !e40
    }

    // ���Ӳ���
    CRtspSession* pSession = (CRtspSession*)nHandle;//lint !e1924
    // ��������Ƿ����
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_CRI("handle is not a valid Session.");//lint !e119 !e40
        return RTSP_ERR_NOT_EXIST; //lint !e40
    }

    // ����
    ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_SessionMutex);
    // �����INIT״̬����������ֱ�ӷ��سɹ�
    if (RTSP_SESSION_STATUS_INIT == pSession->getStatus()) //lint !e1013 !e40
    {
        IVS_RUN_LOG_INF("session[%p] status has been init already, don't need to teardown.", nHandle);
        return RET_OK;
    }

    // ��װTeardown��Ϣ
    CRtspTeardownMessage* pRtspMsg = NULL;
    pRtspMsg = pSession->createRtspMessage((CRtspTeardownMessage*)NULL); //lint !e838
    if (NULL == pRtspMsg)
    {
        IVS_RUN_LOG_CRI("create teardown message fail.");//lint !e119 !e40
        return RET_FAIL;
    }

    pRtspMsg->setRtspUrl(sURL);
    std::string strRtspMsg = "";
    int nRet = pRtspMsg->encodeMessage(strRtspMsg);
    if (RET_OK != nRet)
    {
        pSession->destroyRtspMsg();
        IVS_RUN_LOG_CRI("encode teardown message fail.");//lint !e119 !e40
        return RET_FAIL;
    }
    // ������Ϣ
    nRet = pSession->sendMessage(strRtspMsg.c_str(), strRtspMsg.length());
    if (RET_OK != nRet)
    {
        IVS_RUN_LOG_CRI("send teardown message fail.");//lint !e119 !e40
        return RET_FAIL;
    }

    // ͬ��ģʽ������Ϣ
    if (pSession->m_bBlocked)
    {
        // ����Teardown��Ϣ�ĳ�ʱʱ��
        ACE_Time_Value wait = ACE_OS::gettimeofday();
        wait.sec(wait.sec() + unTimeout);
        nRet = pSession->m_pRtspSemaphore->acquire(&wait);
        if (-1 == nRet)
        {
            // �ǲ���Ӧ�ü���
            ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_Mutex); //lint !e578
            pSession->destroyRtspMsg();
            IVS_RUN_LOG_CRI("sync mode recv teardown response timeout, handle[0x%p].", pSession);//lint !e119 !e40

            return RTSP_ERR_TIMEOUT; //lint !e40
        }//lint !e1788
        if (!pSession->m_bRecvRetFlag)
        {
            return RET_FAIL;
        }
    }
    else
    {
        // �첽ģʽ�����ö�ʱ��
        nRet = pSession->setRecvTimer(unTimeout);
        if (RET_OK != nRet)
        {
            pSession->destroyRtspMsg();
            IVS_RUN_LOG_CRI("async mode set teardown recv timer fail.");//lint !e119 !e40
            return RET_FAIL;
        }
    }

    return RET_OK;
}//lint !e1788



/***************************************************************************
 �� �� �� : RTSP_CLIENT_GetStatus
 �������� : ��ȡ���ӵ�ǰ״̬
 ������� : nHandle                     ���Ӿ��
 ������� : ��
 �� �� ֵ : RTSP_SESSION_STATUS -����״̬
***************************************************************************/
RTSP_SESSION_STATUS RTSPAPI RTSP_CLIENT_GetStatus  /*lint !e129 -esym(714,RTSP_CLIENT_GetStatus)*/
(IVS_ULONG nHandle)
{ //lint !e1784 !e808
    // ��μ��
    if (0 == nHandle)
    {
        return RTSP_SESSION_STATUS_ABNORMAL; //lint !e40
    }

    // ��������Ƿ����
    CRtspSession* pSession = (CRtspSession*) (nHandle);//lint !e1924
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_CRI("rtsp session not exist.");//lint !e119 !e40
        return RTSP_SESSION_STATUS_ABNORMAL; //lint !e40
    }

    return pSession->getStatus(); //lint !e1013
}


/***************************************************************************
 �� �� �� : RTSP_CLIENT_FreeHandle
 �������� : �ͷ����Ӿ��
 ������� : nHandle                     ���Ӿ��
 ������� : ��
 �� �� ֵ : RET_OK-�ɹ�     RET_FAIL-ʧ��
***************************************************************************/
int RTSPAPI RTSP_CLIENT_FreeHandle(IVS_ULONG nHandle)/*lint -esym(714,RTSP_CLIENT_FreeHandle)*/
{ //lint !e1784
	IVS_RUN_LOG_INF("RTSP_CLIENT_FreeHandle [0x%p].", nHandle);

    // �����Ч�Լ��
    if (0 == nHandle)
    {
        return RTSP_ERR_PARAMETER; //lint !e40
    }

    // ��������Ƿ����
    CRtspSession* pSession = (CRtspSession*) (nHandle);//lint !e1924
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_CRI("rtsp session not exist.");//lint !e119 !e40
        return RTSP_ERR_NOT_EXIST; //lint !e40
    }
 
	/* 
	 *  connect ʧ�ܵ� rtsp session ���ᱻע�ᵽreactor��������
	 *  �������ɵ�����ɾ��
	 */
	if (pSession->IsConnectFail())
	{
		IVS_RUN_LOG_CRI("RTSP_CLIENT_FreeHandle: del session handle[0x%p].", pSession);//lint !e119 !e40
		CRtspSessionManager::instance()->deleteSession(pSession);
		delete pSession;
		return RET_OK;
	}

    // �ͷ�����
	IVS_RUN_LOG_CRI("begin Set Status abnormal state.");//lint !e119 !e40
    ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_SessionMutex);
    pSession->setStatus(RTSP_SESSION_STATUS_ABNORMAL); //lint !e40
	IVS_RUN_LOG_CRI("end Set Status abnormal state."); //lint !e119 !e40
    return RET_OK;
}//lint !e1788


/***************************************************************************
 �� �� �� : RTSP_CLIENT_Cleanup
 �������� : �ͷ�ͨѶ��
 ������� : ��
 ������� : ��
 �� �� ֵ : RET_OK-�ɹ�     RET_FAIL-ʧ��
***************************************************************************/
int RTSPAPI RTSP_CLIENT_Cleanup()
{ //lint !e1784
    if (g_bIsFirst)
    {
        return RET_OK;
    }
    g_bIsFirst = true;

    // �رշ�Ӧ��������
    if (RET_OK != CRtspReactorManager::instance()->closeManager())
    {
        return RET_FAIL;
    }

	// �ȹر�RTSP���ӹ�����
	CRtspSessionManager::instance()->closeManager();

    // �ر���־
    (void)CRtsp_Log_Manager::instance().endLog();

    return RET_OK;
}



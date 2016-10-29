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
	filename	: 	Download.cpp
	author		:	z00201790
	created		:	2012/12/06	
	description	:	����¼������ҵ����;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/06 ��ʼ�汾;
*********************************************************************/
#include <time.h>
#include "Download.h"
#include "IVS_Player.h"
#include "LockGuard.h"
#include "IVS_Trace.h"
#include "bp_os.h"
#include "UserMgr.h"


// #define SCALE_BACK_32           ((double)-32.0) //Ĭ����С����
// #define SCALE_DEFAULT           ((double)1.0)   //Ĭ������
// #define SCALE_FORWARD_4         ((double)4.0)   //�ı���ǰ��
// #define SCALE_FORWARD_RESERVED  ((double)256.0) //Ԥ�������ٶ�
// #define SCALE_FORWARD_UNLIMIT   ((double)255.0) //������
// #define SCALE_FORWARD_8_ALL     ((double)254.0) //8����ȫ��ǰ��
// #define SCALE_BACK_2_ALL        ((double)253.0) //2����ȫ������
// #define SCALE_BACK_1_ALL        ((double)252.0) //1����ȫ������
// #define SCALE_FORWARD_4_KEY	    ((double)251.0) //4����ǰ��ֻ����I֡


// Rtspý�����ݻص�
static void  __SDK_CALL g_fRtspDataCallBack(int /*handle*/, char *pBuffer, unsigned int ulBufSize, void *pUser)//lint !e528 
{
	if ((NULL == pBuffer)|| (0 == ulBufSize))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, 
			"Download g_fRtsp Data Call Back", "Input param invalid");
		return;
	}


	if (*pBuffer != '$')
	{
		BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_DATA_ERROR, 
			"Download g_fRtsp Data Call Back", "rtsp media data format error, do not have $.");
		return;
	}

	if (1 == *(pBuffer+1) || 3 == *(pBuffer+1))
	{
		BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_DATA_ERROR, 
			"Download g_fRtsp Data Call Back", "rtsp media data format error, do not 1 or 3.");
		return;
	}

	//��ȡ�ص������������;
	CMediaBase* pMediaBase = static_cast<CMediaBase*>(pUser);
	if (NULL == pMediaBase)
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"Download g_fRtsp Data Call Back",
			"transform pUser to MediaBase pointer failed.");
		return;
	}

	unsigned int uiPlayerChannel = pMediaBase->GetPlayerChannel();
	if (0 == uiPlayerChannel)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Download g_fRtsp Data Call Back", 
			 "pMediaBase's uiPlayerChannel = 0.");
		return;
	}

	pBuffer   += 4;
	ulBufSize -= 4;
	(void) IVS_PLAY_InputRtpData(uiPlayerChannel, pBuffer, ulBufSize); //ͨ��TCPЭ�����
} //lint !e818 !e528

static void __SDK_CALL g_fNetDataCallBack(char *pBuf, unsigned int uiSize, void *pUser)//lint !e818 !e830
{
	//��ȡ�ص������������;
	CMediaBase* pMediaBase = reinterpret_cast<CMediaBase*>(pUser);
	if (NULL == pMediaBase)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Download Net Data Call Back", "transform pUser to MediaBase pointer failed.");
		return;
	}

	unsigned int uiPlayerChannel = pMediaBase->GetPlayerChannel();
	if (0 == uiPlayerChannel)
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"Download Net Data Call Back", "pMediaBase's uiPlayerChannel = 0.");
		return;
	}

	(void)IVS_PLAY_InputRtpData(uiPlayerChannel, pBuf, uiSize);
}//lint !e818 !e528



CDownload::CDownload(void)
{
	m_uLastPercent  = 0; 
	m_uLastLeftTime = 0;
	m_ullLastInfoTick = 0;
	m_ullPastTime = 0;
    m_fSpeed = 1.0;                       
	m_enServiceType = SERVICE_TYPE_DOWNLOAD;
	m_bPaused   = false;
	eSDK_MEMSET(&m_stTimeSpanInter, 0, sizeof(m_stTimeSpanInter));
}


CDownload::~CDownload(void)
{
}


void CDownload::SetHandle(unsigned long /*ulHandle*/)
{

}//lint !e1762

unsigned long  CDownload::GetHandle()
{
	return 0;
}//lint !e1762


int CDownload::StartPlatformDownLoad(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const char* pCameraCode, const IVS_DOWNLOAD_PARAM *pDownloadParam)
{
	CHECK_POINTER(pDownloadParam,IVS_PARA_INVALID);

	//CLockGuard lock(m_pMediaMutex);
	IVS_MEDIA_PARA stMediaPara;
	eSDK_MEMSET(&stMediaPara, 0, sizeof(IVS_MEDIA_PARA));

	// ����Ĭ��ֵ;
	stMediaPara.ProtocolType   = PROTOCOL_RTP_OVER_TCP;
	stMediaPara.DecoderCompany = VENDOR_HUAWEI;
	stMediaPara.StreamType     = STREAM_TYPE_MAIN;
	stMediaPara.TransMode      = MEDIA_TRANS;
	stMediaPara.BroadCastType  = BROADCAST_UNICAST;
	stMediaPara.VideoDecType   = VIDEO_DEC_H264;
	stMediaPara.AudioDecType   = AUDIO_DEC_G711A;

	START_PLAY_PARAM stParam;
	stParam.enSourceType       = SOURCE_RTP;
	stParam.enServiceType      = SERVICE_TYPE_DOWNLOAD;/*SERVICE_TYPE_DOWNLOAD*/;/*SERVICE_TYPE_PLAYBACK*/;/*SERVICE_TYPE_DOWNLOAD FIX ME Ŀǰʹ�� SERVICE_TYPE_DOWNLOAD �޷���ȡ��URL*/; 
	if(NULL != pDomainCode)
	{
		stParam.enServiceType      = SERVICE_TYPE_BACKUPRECORD_DOWNLOAD;
	}
	stParam.fSpeed             = (float)pDownloadParam->uiDownloadSpeed; //fSpeed;
	stParam.cbRtspDataCallBack = g_fRtspDataCallBack;
	stParam.cbNetDataCallBack  = g_fNetDataCallBack;

	TIME_SPAN_INTERNAL stTimeSpanInter;
	stTimeSpanInter.iStartTime = (IVS_INT32)UTCStr2Time(pDownloadParam->stTimeSpan.cStart, IVS_DATE_TIME_FORMAT);
	stTimeSpanInter.iEndTime   = (IVS_INT32)UTCStr2Time(pDownloadParam->stTimeSpan.cEnd,   IVS_DATE_TIME_FORMAT);
	if (stTimeSpanInter.iStartTime >= stTimeSpanInter.iEndTime)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start Platform DownLoad", "time span invalid");
		return IVS_PARA_INVALID;
	}
	
	int iRet = StartDownLoad(pDomainCode,pNVRCode,pDownloadParam, &stMediaPara, pCameraCode, stTimeSpanInter, stParam, NULL);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Start Platform DownLoad", "Start DownLoad error");
		return iRet;
	}

	// FIX ME Ŀǰʹ�� SERVICE_TYPE_DOWNLOAD �޷���ȡ��URL
	stParam.enServiceType = SERVICE_TYPE_DOWNLOAD;
	SetServiceType(stParam.enServiceType);
	m_uLastLeftTime  = m_tmEnd - m_tmStart; //lint !e712

	return IVS_SUCCEED;
}


int CDownload::StopPlatformDownLoad()
{
	int iRet = CMediaBase::StopDownLoad();
	m_fSpeed = 0.0;
	RetSet();
	if (iRet != IVS_SUCCEED)
	{
		BP_RUN_LOG_ERR(iRet, "Stop Platform DownLoad", "Stop DownLoad failed.");
		iRet = IVS_FAIL;
	}
	return iRet;

}

// ����
int CDownload::StartDisasterRecoveryDownload(const char* pCameraCode, const IVS_DOWNLOAD_PARAM *pDownloadParam)
{
	CHECK_POINTER(pDownloadParam,IVS_PARA_INVALID);

    //CLockGuard lock(m_pMediaMutex);
    IVS_MEDIA_PARA stMediaPara;
    eSDK_MEMSET(&stMediaPara, 0, sizeof(IVS_MEDIA_PARA));

    // ����Ĭ��ֵ;
    stMediaPara.ProtocolType   = PROTOCOL_RTP_OVER_TCP;
    stMediaPara.DecoderCompany = VENDOR_HUAWEI;
    stMediaPara.StreamType     = STREAM_TYPE_MAIN;
    stMediaPara.TransMode      = MEDIA_TRANS;
    stMediaPara.BroadCastType  = BROADCAST_UNICAST;
    stMediaPara.VideoDecType   = VIDEO_DEC_H264;
    stMediaPara.AudioDecType   = AUDIO_DEC_G711A;

    START_PLAY_PARAM stParam;
    stParam.enSourceType       = SOURCE_RTP;
    // TODO ������ַ�������
    stParam.enServiceType      = SERVICE_TYPE_DISASTEBACKUP_DOWNLOAD;/*SERVICE_TYPE_DOWNLOAD*/;/*SERVICE_TYPE_PLAYBACK*/;/*SERVICE_TYPE_DOWNLOAD FIX ME Ŀǰʹ�� SERVICE_TYPE_DOWNLOAD �޷���ȡ��URL*/; 
    stParam.fSpeed             = (float)pDownloadParam->uiDownloadSpeed; //fSpeed;
    stParam.cbRtspDataCallBack = g_fRtspDataCallBack;
    stParam.cbNetDataCallBack  = g_fNetDataCallBack;

    TIME_SPAN_INTERNAL stTimeSpanInter;
    stTimeSpanInter.iStartTime = (IVS_INT32)UTCStr2Time(pDownloadParam->stTimeSpan.cStart, IVS_DATE_TIME_FORMAT);
    stTimeSpanInter.iEndTime   = (IVS_INT32)UTCStr2Time(pDownloadParam->stTimeSpan.cEnd,   IVS_DATE_TIME_FORMAT);
    if (stTimeSpanInter.iStartTime >= stTimeSpanInter.iEndTime)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start Platform DownLoad", "time span invalid");
        return IVS_PARA_INVALID;
    }

    // TODO
    IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
    if (NULL != this->m_pUserMgr)
    {
        // ��ȡ��·�ɣ���¼��־
        CDomainRouteMgr& domainRouteMgr = m_pUserMgr->GetDomainRouteMgr();

        string strLocalDomainCode;
        domainRouteMgr.GetLocalDomainCode(strLocalDomainCode);

        //this->m_pUserMgr->GetDomainCode(strLocalDomainCode);
        if (!CToolsHelp::Memcpy(chDomaCode,IVS_DOMAIN_CODE_LEN,strLocalDomainCode.c_str(),strLocalDomainCode.length()))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Query Record List", "memcpy failed");
            return IVS_ALLOC_MEMORY_ERROR;
        }
       // eSDK_MEMSET(chDomaCode,'\0',IVS_DOMAIN_CODE_LEN + 1);
        IVS_DEBUG_TRACE(" record method disaster recovery set domain is local[%s]. ",strLocalDomainCode.c_str());
    }

    int iRet = StartDownLoad(chDomaCode,NULL,pDownloadParam, &stMediaPara, pCameraCode, stTimeSpanInter, stParam, NULL);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start Platform DownLoad", "Start DownLoad error");
        return iRet;
    }

    // FIX ME Ŀǰʹ�� SERVICE_TYPE_DOWNLOAD �޷���ȡ��URL

    SetServiceType(stParam.enServiceType);
    m_uLastLeftTime  = m_tmEnd - m_tmStart; //lint !e712

    return IVS_SUCCEED;
}

// ����
int CDownload::StopDisasterRecoveryDownload()
{
    int iRet = CMediaBase::StopDownLoad();
    m_fSpeed = 0.0;
    RetSet();
    if (iRet != IVS_SUCCEED)
    {
        BP_RUN_LOG_ERR(iRet, "Stop Platform DownLoad", "Stop DownLoad failed.");
        iRet = IVS_FAIL;
    }
    return iRet;

}

int  CDownload::DownLoadPause()
{
	CLockGuard lock(m_pMediaMutex);
	int iRet = IVS_FAIL;
	if (RTSP_SESSION_STATUS_PLAYING == RTSP_CLIENT_GetStatus(m_iRtspHandle))
	{
		iRet = RTSP_CLIENT_SendPauseMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
		if (0 != iRet)
		{
			BP_RUN_LOG_ERR(iRet,"Play Back Pause", "RTSP client send pause play fail.");
			return IVS_PLAYER_RET_RTSP_SEND_PAUSE_FAIL;
		}
	}

	(void)IVS_PLAY_PausePlay(m_ulPlayerChannel);
	m_bPaused = true;
	return IVS_SUCCEED;
}//lint !e1762

int  CDownload::DownLoadResume()
{
	CLockGuard lock(m_pMediaMutex);

	m_bStepPlay = false;
	int iRet = IVS_FAIL;
	if (RTSP_SESSION_STATUS_READY == RTSP_CLIENT_GetStatus(m_iRtspHandle))
	{
		MEDIA_RANGE_S stRange;
		eSDK_MEMSET(&stRange, 0x0, sizeof(MEDIA_RANGE_S));
		stRange.enRangeType = RANGE_TYPE_UTC;
		stRange.MediaBeginOffset = OFFSET_CUR;
		iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, m_stMediaRsp.szURL, (double)m_fSpeed, &stRange, RTSP_CLIENT_TIMEOUT);
		if (0 != iRet)
		{
			BP_RUN_LOG_ERR(iRet,"Play Back Resume", "IVS pause play fail.");
			return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
		}
	}
	m_bPaused = false;
	//m_ullLastInfoTick = 0; // reset
	return IVS_SUCCEED;
}//lint !e1762

int CDownload::SetDownLoadSpeed(float fSpeed)
{
	CLockGuard lock(m_pMediaMutex);
	int iRet = IVS_FAIL;
	RTSP_SESSION_STATUS state = RTSP_CLIENT_GetStatus(m_iRtspHandle);

	if (RTSP_SESSION_STATUS_PLAYING == state ||
		RTSP_SESSION_STATUS_READY   == state)
	{
		MEDIA_RANGE_S stRange;
		eSDK_MEMSET(&stRange, 0x0, sizeof(MEDIA_RANGE_S));
		stRange.enRangeType = RANGE_TYPE_UTC;
		stRange.MediaBeginOffset = OFFSET_CUR;
		m_fSpeed = fSpeed;
		iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, m_stMediaRsp.szURL, (double)m_fSpeed, &stRange, RTSP_CLIENT_TIMEOUT);
		if (0 != iRet)
		{
			BP_RUN_LOG_ERR(iRet,"Play Back Resume", "IVS pause play fail.");
			return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
		}
	}
	return IVS_SUCCEED;
}//lint !e1762

int CDownload::GetDownLoadInfo(IVS_DOWNLOAD_INFO* pInfo)
{
	CHECK_POINTER(pInfo,IVS_PARA_INVALID);

	unsigned long long DownloadSize  = 0;
	unsigned int DownloadSpeed = 0;
    unsigned int RecordTime    = 0;

	int result = IVS_PLAY_GetRecordStatis(m_ulPlayerChannel,
		  &RecordTime, &DownloadSpeed, &DownloadSize);

	if (IVS_SUCCEED != result)
	{
		BP_RUN_LOG_ERR(result, "Get DownLoad Info", "Fail Error=%d", result);
		return result;
	}
	
	pInfo->uiDownloadStatus = m_bPaused ? 0 : 1;
	pInfo->uiDownloadSize   = DownloadSize / 1024;
	pInfo->uiDownloadSpeed  = DownloadSpeed / 1024;
	pInfo->uiProgress       = RecordTime  * 100 /(((m_tmEnd - m_tmStart) - 1) * 1000); //lint !e647 !e732
	pInfo->uiProgress       = pInfo->uiProgress > 100 ? 100 : pInfo->uiProgress;
	if((0 == pInfo->uiTimeLeft && pInfo->uiProgress > 90) || (0 == pInfo->uiDownloadSpeed && pInfo->uiProgress > 90)) // Fixed: the uiProgress can not reach 100, always at 99 when download done
	{
		pInfo->uiProgress = 100;
	}
	
	if (m_fSpeed < 255.0 && m_fSpeed >= 1.0)
	{
		unsigned int extra = (100 - pInfo->uiProgress) * (m_tmEnd - m_tmStart) / 100; //lint !e712
	 	pInfo->uiTimeLeft = extra / m_fSpeed;   //lint !e524 !e732
	}
	else
	{
		ACE_Time_Value tv = BP_OS::gettimeofday();
		unsigned long long now = tv.sec() * 1000 + tv.usec() / 1000;//lint !e732 

		if (m_ullLastInfoTick)
		{
			unsigned long long diffMS = now - m_ullLastInfoTick; //lint !e712
			
			if (!m_bPaused)
			{
				m_ullPastTime += diffMS;
			}
			
			if (0 == pInfo->uiProgress)
			{
				pInfo->uiTimeLeft = 0;
				BP_DBG_LOG("IVS Play RecordStatis: CurrentProgress is zero, cannot calculate left time");
			}
			else
			{
				if (pInfo->uiProgress == m_uLastPercent )
				{
					pInfo->uiTimeLeft = m_uLastLeftTime;
				}
				else
				{
					pInfo->uiTimeLeft = (unsigned int)((100 - pInfo->uiProgress) * m_ullPastTime)/(pInfo->uiProgress*1000);//lint !e647
				}

				BP_DBG_LOG("IVS Play RecordStatis: CurrentProgress[%u] LastPercent[%u] PastTime[%llu] TimeLeft[%u]", pInfo->uiProgress, m_uLastPercent, m_ullPastTime, pInfo->uiTimeLeft);
			}
		}
		else
		{
			m_ullPastTime = 0;
			pInfo->uiTimeLeft = 0;
			BP_DBG_LOG("IVS Play RecordStatis: FirstTime, cannot calculate left time");
		}

		m_ullLastInfoTick =  now;
		m_uLastPercent = pInfo->uiProgress;
	}

	m_uLastLeftTime = pInfo->uiTimeLeft;

	BP_DBG_LOG("IVS Play RecordStatis:RecordTime[%u] DownloadSpeed[%u] DownloadSize[%llu] DownloadInfo: Status[%u] Size[%llu] Speed[%u] Progress[%u] TimeLeft[%u]",
		RecordTime, DownloadSpeed, DownloadSize, pInfo->uiDownloadStatus, pInfo->uiDownloadSize, pInfo->uiDownloadSpeed, pInfo->uiProgress, pInfo->uiTimeLeft);
	return result;
}


int CDownload::StartPUDownLoad(const char* pCameraCode, 
	      const IVS_DOWNLOAD_PARAM* pDownloadParam, const char *pFileName)
{
	CHECK_POINTER(pDownloadParam,IVS_PARA_INVALID);

	CLockGuard lock(m_pMediaMutex);

	IVS_MEDIA_PARA stMediaPara;
	eSDK_MEMSET(&stMediaPara, 0, sizeof(IVS_MEDIA_PARA));
	stMediaPara.ProtocolType   = PROTOCOL_RTP_OVER_TCP;
	stMediaPara.DecoderCompany = VENDOR_HUAWEI;
	stMediaPara.StreamType     = STREAM_TYPE_MAIN;
	stMediaPara.TransMode      = MEDIA_TRANS;
	stMediaPara.BroadCastType  = BROADCAST_UNICAST;
	stMediaPara.VideoDecType   = VIDEO_DEC_H264;
	stMediaPara.AudioDecType   = AUDIO_DEC_G711A;

    START_PLAY_PARAM stParam;
	stParam.enSourceType       = SOURCE_RTP;
	stParam.enServiceType      = SERVICE_TYPE_PU_DOWNLOAD;/*SERVICE_TYPE_DOWNLOAD �޷���ȡ��URL */
	stParam.fSpeed             = pDownloadParam->uiDownloadSpeed;
	stParam.cbRtspDataCallBack = g_fRtspDataCallBack;
	stParam.cbNetDataCallBack  = g_fNetDataCallBack;

	TIME_SPAN_INTERNAL stTimeSpanInter;
	stTimeSpanInter.iStartTime = (IVS_INT32)UTCStr2Time(pDownloadParam->stTimeSpan.cStart, IVS_DATE_TIME_FORMAT);
	stTimeSpanInter.iEndTime   = (IVS_INT32)UTCStr2Time(pDownloadParam->stTimeSpan.cEnd,   IVS_DATE_TIME_FORMAT);
	if (stTimeSpanInter.iStartTime >= stTimeSpanInter.iEndTime)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start Platform DownLoad", "time span invalid");
		return IVS_PARA_INVALID;
	}

	//��ʼ�ط�;
	int iRet = StartDownLoad(NULL,NULL,pDownloadParam, &stMediaPara, pCameraCode, stTimeSpanInter, stParam, pFileName);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Start PU Play Back", "Start play error.");
		return iRet;
	}

	// FIX ME Ŀǰʹ�� SERVICE_TYPE_DOWNLOAD �޷���ȡ��URL
	stParam.enServiceType = SERVICE_TYPE_PU_DOWNLOAD;
	SetServiceType(stParam.enServiceType);
	m_uLastLeftTime     = m_tmEnd - m_tmStart; //lint !e712

	return 0;
}


int CDownload::StopPUDownLoad()
{
	int iRet = CMediaBase::StopDownLoad();
    m_fSpeed = 0.0;
    RetSet();
	if (iRet != IVS_SUCCEED)
	{
		BP_RUN_LOG_ERR(iRet, "Stop PU DownLoad", "StopDownLoad failed.");
		iRet = IVS_FAIL;
	}
	return iRet;
}


/*
 * StartDownLoad
 * ��������
 */
int CDownload::StartDownLoad(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_DOWNLOAD_PARAM *pParam,const IVS_MEDIA_PARA* pstMediaPara, const IVS_CHAR* pCameraCode, 
	    const TIME_SPAN_INTERNAL& stTimeSpanInter, const START_PLAY_PARAM& stParam,	const char* pFileName)
{
    if (NULL == pCameraCode || strlen(pCameraCode) > (CODE_LEN-1) || 0 == strlen(pCameraCode))
	{
		BP_RUN_LOG_ERR(IVS_SMU_CAMERA_CODE_NOT_EXIST, "Start DownLoad", "camera code invalid");
		return IVS_PARA_INVALID;
	}
	
	IVS_DEBUG_TRACE("");

    bool bRet = CToolsHelp::Strncpy(m_szCameraID, sizeof(m_szCameraID), pCameraCode, strlen(pCameraCode));  
	if (!bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Start DownLoad", "Strncpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	int iRet = SetMediaParam(pstMediaPara);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Start DownLoad", "set media param error");
		return iRet;
	}

	//��SCU��ȡý��URL���������ݱ����ڻ���.
	iRet = DoGetUrlDownLoad(pDomainCode,pNVRCode,stParam.enServiceType, stTimeSpanInter, pFileName);
	if (IVS_SUCCEED != iRet) 
	{
		BP_RUN_LOG_ERR(iRet,"Start download", "get url play back error, stream default.");
		return iRet;
	}

	//add by w00210470 ������������ʱ���ֹͣʱ��
	m_tmStart = stTimeSpanInter.iStartTime;
	m_tmEnd   = stTimeSpanInter.iEndTime;

	// ���û�����������;
    iRet = CMediaBase::StartDownload(stParam, pCameraCode, pParam);
	if (iRet != IVS_SUCCEED)
	{
		m_fSpeed = 0.0;
		BP_RUN_LOG_ERR(iRet,"Start download", "call base class failed.");
		return iRet;
	}

	m_fSpeed = stParam.fSpeed;
	return IVS_SUCCEED;
}

// ��ȡdownload url
int CDownload::DoGetUrlDownLoad(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,IVS_SERVICE_TYPE ServiceType, 
	    const TIME_SPAN_INTERNAL& stTimeSpanInter, const char *pFileName)
{
	IVS_DEBUG_TRACE("");

	//������Ϣ;
	eSDK_MEMSET(&m_stUrlMediaInfo, 0, sizeof(ST_URL_MEDIA_INFO));

	//У�����
	if (NULL != pFileName)
	{
	     (void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szFileName, FILE_NAME_LEN,
			  pFileName, strlen(pFileName) + 1);
	}

	//TODO : szDstPort��ʵ�����ã���һ���̶�ֵ�����Э������szDstPP��νṹ��δ����
	m_stUrlMediaInfo.ServiceType = ServiceType;
	//std::string strCameraDomainCode = m_szCameraID;
	std::string strCameraCode;
	std::string strDomainCode;
	int iRet = CToolsHelp::SplitCameraDomainCode(m_szCameraID, strCameraCode, strDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Do Get Url DownLoad", "SplitCameraCode failed, RetCode=%d.", iRet);
		return iRet;
	}

	if((NULL != pNVRCode) && (0 != strlen(pNVRCode)))
	{
		(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szNVRCode, sizeof(m_stUrlMediaInfo.szNVRCode),pNVRCode, IVS_NVR_CODE_LEN);//lint !e534
	}
	else
	{
		eSDK_MEMSET(&m_stUrlMediaInfo.szNVRCode,0,IVS_NVR_CODE_LEN);//lint !e545
		m_pUserMgr->GetRecordList().GetRecordNVRInfo(m_stUrlMediaInfo.szSrcCode,stTimeSpanInter.iStartTime,stTimeSpanInter.iEndTime,m_stUrlMediaInfo.szNVRCode);
	}
	if((NULL != pDomainCode) && (0 != strlen(pDomainCode)))
	{
		(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.cDomainCode,IVS_DOMAIN_CODE_LEN+1,  pDomainCode, strlen(pDomainCode));//lint !e534
	}
	else
	{
		/*
		//�����(w90004666) 2015-09-22 17:05 ����֪���ط�����ֻ֧�����е�ֱ��ģʽ����ʱ��֧����ת�������ȼ��ͣ�CU&SDK�����Ҫ�����£�����ǻط����صĻ���
		//��GetURL��Ϣ����IPC�����򣬶�����ת����
		CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR); //593
		CDeviceMgr& deviceMgr = m_pUserMgr->GetDeviceMgr();
		//���������
		m_stUrlMediaInfo.IsProgressionTransmit = false;
		std::string strCurDomainCode;
		m_pUserMgr->GetDomainCode(strCurDomainCode);
		if ((strCurDomainCode != strDomainCode)  &&
			(m_pUserMgr->IsSupportProgressionTransmit()) &&
			(!deviceMgr.IsShadowDev(m_szCameraID)))
		{
			std::string strProgressionDomainCode;
			m_pUserMgr->ProgressionTransmitDomainCode(strProgressionDomainCode);

			//���豸�򱣴�����
			(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.cCameraDomain,IVS_DOMAIN_CODE_LEN+1,  strDomainCode.c_str(), strDomainCode.size());//lint !e534

			//�����������Ϊת�����
			strDomainCode = strProgressionDomainCode;
			m_stUrlMediaInfo.IsProgressionTransmit = true;  
		}
		*/
		(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.cDomainCode,IVS_DOMAIN_CODE_LEN+1,  strDomainCode.c_str(), strDomainCode.size());//lint !e534
	}

	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szSrcCode, sizeof(m_stUrlMediaInfo.szSrcCode), strCameraCode.c_str(), strCameraCode.size());//lint !e534
    (void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstCode,sizeof(m_stUrlMediaInfo.szDstCode), "01",strlen("01"));//lint !e534 
	m_stUrlMediaInfo.MediaPara.StreamType = m_stuMediaParam.StreamType;
	m_stUrlMediaInfo.MediaPara.ProtocolType = m_stuMediaParam.ProtocolType;
	m_stUrlMediaInfo.PackProtocolType = PACK_PROTOCOL_ES;
	m_stUrlMediaInfo.MediaPara.BroadCastType = m_stuMediaParam.BroadCastType;
    (void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstIP,sizeof(m_stUrlMediaInfo.szDstIP), "",strlen(""));//lint !e534
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstPort,sizeof(m_stUrlMediaInfo.szDstPort), "35016",strlen("35016"));//lint !e534
	m_stUrlMediaInfo.MediaPara.VideoDecType = m_stuMediaParam.VideoDecType;
	m_stUrlMediaInfo.MediaPara.AudioDecType = m_stuMediaParam.AudioDecType;
	m_stUrlMediaInfo.MediaPara.TransMode = m_stuMediaParam.TransMode;

	std::string strTime;
	UTCTime2Str((time_t)stTimeSpanInter.iStartTime, strTime, IVS_DATE_TIME_NET_FORMAT);
	strTime.append("-");
	std::string strEndTime;
	UTCTime2Str((time_t)stTimeSpanInter.iEndTime, strEndTime, IVS_DATE_TIME_NET_FORMAT);   
	strTime.append(strEndTime);
	if (TIME_SPAN_LEN <= strTime.size())
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url Play Back", "Parse time span from rsp url failed. time string over max length. strTime=%s.", strTime.c_str());
		return iRet;
	}
	(void)CToolsHelp::Memcpy(m_stUrlMediaInfo.szTimeSpan,TIME_SPAN_LEN, strTime.c_str(), strTime.length());
	//��ȡý��URL
	iRet = GetURL(CToolsHelp::GetSCCHead().c_str(), m_pUserMgr, m_stUrlMediaInfo, m_stMediaRsp, m_bAssociatedAudio);//lint !e838
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url DownLoad", "platform play back get URL failed!");
		return iRet;
	}

	//����URL,�õ�ServiceType��������鲥ʵ����ֱ��������ôЭ������תΪudp
	std::string strServiceType;
	iRet = ParseURL(m_stMediaRsp.szURL, URL_SERVICE_TYPE_FLAG, strServiceType);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url DownLoad", "parse service type from url failed, URL=%s",
			m_stMediaRsp.szURL);
		return iRet;
	}
	int iServiceType = CToolsHelp::StrToInt(strServiceType);

	std::string strMediaTransMode;
	iRet = ParseURL(m_stMediaRsp.szURL, URL_MEDIA_TRANS_MODE, strMediaTransMode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url DownLoad", "parse media trans mode failed from url");
		return iRet;
	}
	int iMediaTransMode = CToolsHelp::StrToInt(strMediaTransMode);

	std::string strBroadcastType;
	iRet = ParseURL(m_stMediaRsp.szURL, URL_BROADCAST_TYPE, strBroadcastType);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url DownLoad", "parse broadcast type failed from url");
		return iRet;
	}
	int iBroadcastType = CToolsHelp::StrToInt(strBroadcastType);

	std::string strTimeSpan;
	iRet = ParseURL(m_stMediaRsp.szURL, URL_TIME_SPAN, strTimeSpan);
	if (IVS_SUCCEED != iRet || strTimeSpan.empty())
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url DownLoad", "parse timespan failed from url");
		return iRet;
	}

	unsigned int uiPos = strTimeSpan.find("-");
	if (std::string::npos == uiPos || uiPos >= strTimeSpan.length())
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url DownLoad", "parse timespan failed from url,find spliter failed.");
		return iRet;
	}
	std::string strStartTime = strTimeSpan.substr(0, uiPos);
	if (std::string::npos == (uiPos+1) || (uiPos+1) >= strTimeSpan.length())
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url DownLoad", "parse timespan failed from url,find spliter failed.");
		return iRet;
	}
	strEndTime = strTimeSpan.substr(uiPos+1, strlen(IVS_DATE_TIME_NET_FORMAT));

	m_stTimeSpanInter.iStartTime = (IVS_INT32)UTCStr2Time(strStartTime.c_str(), IVS_DATE_TIME_NET_FORMAT);
	m_stTimeSpanInter.iEndTime = (IVS_INT32)UTCStr2Time(strEndTime.c_str(), IVS_DATE_TIME_NET_FORMAT);

	BP_RUN_LOG_INF("Do Get Url DownLoad", "strStartTime=%s strEndTime=%s iStartTime=%d iEndTime=%d.", strStartTime.c_str(), strEndTime.c_str(), m_stTimeSpanInter.iStartTime, m_stTimeSpanInter.iEndTime);

	m_enServiceType = (IVS_SERVICE_TYPE)iServiceType;
	m_stuMediaParam.TransMode = (IVS_MEDIA_TRANS_MODE)iMediaTransMode;
	m_stuMediaParam.BroadCastType = (IVS_BROADCAST_TYPE)iBroadcastType;

	return IVS_SUCCEED;
}


int CDownload::GetDownloadRtspURL(const IVS_CHAR* pDomainCode,
	const IVS_CHAR* pNVRCode,
	const IVS_CHAR* pCameraCode, 
	const IVS_URL_MEDIA_PARAM *pURLMediaParam, 
	IVS_CHAR *pRtspURL, 
	IVS_UINT32 uiBufferSize)
{
	IVS_DEBUG_TRACE("");

	//У�����
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pURLMediaParam, IVS_PARA_INVALID);

	BP_RUN_LOG_INF("Enter Get Rtsp URL", "CameraDomainCode = %s, StreamType = %u, ProtocolType = %u, ClientType: %d, PackProtocolType: %d.",
		pCameraCode, pURLMediaParam->StreamType, pURLMediaParam->ProtocolType, pURLMediaParam->iClientType, pURLMediaParam->PackProtocolType);

	//������Ϣ;
	eSDK_MEMSET(&m_stUrlMediaInfo, 0, sizeof(ST_URL_MEDIA_INFO));

	m_stUrlMediaInfo.ServiceType = SERVICE_TYPE_DOWNLOAD;

	std::string strCameraDomainCode = pCameraCode;
	std::string strCameraCode;
	std::string strDomainCode;
	IVS_INT32 iRet = CToolsHelp::SplitCameraDomainCode(strCameraDomainCode, strCameraCode, strDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"GetDownloadRtspURL", "CToolsHelp::SplitCameraCode failed, RetCode=%d.", iRet);
		return iRet;
	}
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szSrcCode, sizeof(m_stUrlMediaInfo.szSrcCode), strCameraCode.c_str(), strCameraCode.size());//lint !e534
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstCode,sizeof(m_stUrlMediaInfo.szDstCode), "01",strlen("01"));

	if(NULL == pDomainCode || 0 == strlen(pDomainCode))
	{
		(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.cDomainCode, sizeof(m_stUrlMediaInfo.cDomainCode), strDomainCode.c_str(), strDomainCode.length());
	}
	else
	{
		(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.cDomainCode,IVS_DOMAIN_CODE_LEN+1,  pDomainCode, strlen(pDomainCode));
	}

	IVS_MEDIA_PARA stMediaPara;
	eSDK_MEMSET(&stMediaPara, 0, sizeof(IVS_MEDIA_PARA));
	stMediaPara.AudioDecType = (IVS_AUDIO_DEC_TYPE)pURLMediaParam->AudioDecType;
	stMediaPara.BroadCastType = (IVS_BROADCAST_TYPE)pURLMediaParam->BroadCastType;
	stMediaPara.ProtocolType = (IVS_PROTOCOL_TYPE)pURLMediaParam->ProtocolType;
	stMediaPara.StreamType = (IVS_STREAM_TYPE)pURLMediaParam->StreamType;
	stMediaPara.TransMode = (IVS_MEDIA_TRANS_MODE)pURLMediaParam->TransMode;
	stMediaPara.VideoDecType = (IVS_VIDEO_DEC_TYPE)pURLMediaParam->VideoDecType;
	iRet = SetMediaParam(&stMediaPara);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"GetDownloadRtspURL", "set media param error");
		return iRet;
	}

	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.cDomainCode,IVS_DOMAIN_CODE_LEN+1,  strDomainCode.c_str(), strDomainCode.size());
	BP_RUN_LOG_INF("GetDownloadRtspURL", "CameraCode[%s]DomainCode[%s].", m_stUrlMediaInfo.szSrcCode,  m_stUrlMediaInfo.cDomainCode);

	m_stUrlMediaInfo.MediaPara.AudioDecType = m_stuMediaParam.AudioDecType;
	m_stUrlMediaInfo.MediaPara.BroadCastType = m_stuMediaParam.BroadCastType;
	m_stUrlMediaInfo.MediaPara.ProtocolType = m_stuMediaParam.ProtocolType;
	m_stUrlMediaInfo.MediaPara.StreamType = m_stuMediaParam.StreamType;
	m_stUrlMediaInfo.MediaPara.TransMode = m_stuMediaParam.TransMode;
	m_stUrlMediaInfo.MediaPara.VideoDecType = m_stuMediaParam.VideoDecType;
	m_stUrlMediaInfo.PackProtocolType = (IVS_PACK_PROTOCOL_TYPE)pURLMediaParam->PackProtocolType;
	m_stUrlMediaInfo.iClientType = pURLMediaParam->iClientType;
	 //client_type=1ʱ,PassMTU=0����֧�ֻ�ȡNAT��ַ
	if(1 == m_stUrlMediaInfo.iClientType)
	{
		m_stUrlMediaInfo.IsPassMTU = false;
	}
	
	// ��ѡ
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstIP, sizeof(m_stUrlMediaInfo.szDstIP), "",strlen(""));//lint !e534
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstPort, sizeof(m_stUrlMediaInfo.szDstPort), "35016",strlen("35016"));

	std::string strTime;
	UTCStr2Span(strTime, pURLMediaParam->stTimeSpan.cStart, pURLMediaParam->stTimeSpan.cEnd);
	if (TIME_SPAN_LEN <= strTime.size())
	{
		BP_RUN_LOG_ERR(iRet,"GetDownloadRtspURL", "Parse time span from rsp url failed. time string over max length. strTime=%s.", strTime.c_str());
		return iRet;
	}
	(void)CToolsHelp::Memcpy(m_stUrlMediaInfo.szTimeSpan,TIME_SPAN_LEN, strTime.c_str(), strTime.length());

	if((NULL != pNVRCode) && (0 != strlen(pNVRCode)))
	{
		(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szNVRCode, sizeof(m_stUrlMediaInfo.szNVRCode),pNVRCode, IVS_NVR_CODE_LEN);
	}
	else
	{
		eSDK_MEMSET(m_stUrlMediaInfo.szNVRCode,0,IVS_NVR_CODE_LEN);
		TIME_SPAN_INTERNAL stTimeSpanInter;
		stTimeSpanInter.iStartTime = (IVS_INT32)UTCStr2Time(pURLMediaParam->stTimeSpan.cStart, IVS_DATE_TIME_FORMAT);
		stTimeSpanInter.iEndTime   = (IVS_INT32)UTCStr2Time(pURLMediaParam->stTimeSpan.cEnd,   IVS_DATE_TIME_FORMAT);
		if (stTimeSpanInter.iStartTime >= stTimeSpanInter.iEndTime)
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, "GetDownloadRtspURL", "time span invalid");
			return IVS_PARA_INVALID;
		}
		m_pUserMgr->GetRecordList().GetRecordNVRInfo(m_stUrlMediaInfo.szSrcCode,stTimeSpanInter.iStartTime,stTimeSpanInter.iEndTime,m_stUrlMediaInfo.szNVRCode);
	}

	BP_RUN_LOG_INF("GetDownloadRtspURL", "CameraCode[%s]NVRCode[%s].", m_stUrlMediaInfo.szSrcCode, m_stUrlMediaInfo.szNVRCode);

	iRet = GetURL(CToolsHelp::GetSCCHead().c_str(), m_pUserMgr, m_stUrlMediaInfo, m_stMediaRsp, m_bAssociatedAudio, true);

	std::string strURL = GetUrlBuffer();
	if (strURL.empty())
	{
		iRet = IVS_URL_INVALID;
		BP_RUN_LOG_ERR(iRet, "GetDownloadRtspURL", "URL is NULL");
		return iRet;
	}
	if (strURL.length() >= uiBufferSize)
	{
		iRet = IVS_PARA_INVALID;
		BP_RUN_LOG_ERR(iRet, "GetDownloadRtspURL", "Para Invalid, Buffer is too small");
		return iRet;
	}

	eSDK_STRCPY(pRtspURL, uiBufferSize, strURL.c_str());
	BP_RUN_LOG_INF("GetDownloadRtspURL success", "");

	return iRet;
}








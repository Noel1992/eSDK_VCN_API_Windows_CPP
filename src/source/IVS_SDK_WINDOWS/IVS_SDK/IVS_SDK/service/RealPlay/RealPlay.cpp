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

#include "RealPlay.h"
#include "IVS_Player.h"
#include "LockGuard.h"
#include "IVS_Trace.h"
#include "AutoResume.h"
#include "UserMgr.h"

//TODO 异常类需要剥离出去，因为rtspclient的异常回调函数只有一个、网络收流的异常函数也只有一个。
// 考虑是否整改rtsp和网络收流，让每个通道绑定一个异常回调函数;



// Rtsp媒体数据回调
static void  __SDK_CALL g_fRtspDataCallBack(int /*handle*/, 
                                            char *pBuffer,
                                            unsigned int ulBufSize,
                                            void *pUser)//lint !e528 
{
    //入参有效性检查
    if ((NULL == pBuffer) 
        || (0 == ulBufSize)
        || (NULL == pUser))
    {
        return;
    }


    if (*pBuffer != '$')
    {
        return;
    }
    if (1 == *(pBuffer+1) || 3 == *(pBuffer+1))
    {
        return;
    }
	
	if (0 != *(pBuffer+1) && 2 != *(pBuffer+1))
    {
        return;
    }


    //获取回调出来的类对象;
    const CMediaBase* pMediaBase = (CMediaBase*)pUser; //lint !e1924
    unsigned int uiPlayerChannel = pMediaBase->GetPlayerChannel();
    if (0 == uiPlayerChannel)
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"g_fRtsp Data Call Back", "pMediaBase's uiPlayerChannel = 0.");
        return;
    }

    pBuffer   += 4;
    ulBufSize -= 4;
    IVS_PLAY_InputRtpData(uiPlayerChannel, pBuffer, ulBufSize);
    return;
} //lint !e818

static void __SDK_CALL g_fNetDataCallBack( char *pBuf, unsigned int uiSize, void *pUser)//lint !e818 !e830
{
    //获取回调出来的类对象;
    const CMediaBase* pMediaBase = reinterpret_cast<CMediaBase*>(pUser);
    if (NULL == pMediaBase)
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"g_fNet Data Call Back", "transform pUser to MediaBase pointer failed.");
        return;
    }
    unsigned int uiPlayerChannel = pMediaBase->GetPlayerChannel();
    if (0 == uiPlayerChannel)
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"g_fNet Data Call Back", "pMediaBase's uiPlayerChannel = 0.");
        return;
    }

    (void)IVS_PLAY_InputRtpData(uiPlayerChannel, pBuf, uiSize);
    return;
}//lint !e818

// 裸码流回调;
static IVS_VOID CALLBACK g_fReassmCallBack(IVS_RAW_FRAME_INFO* pstRawFrameInfo,
                                           IVS_CHAR*           pFrame,
                                           IVS_UINT32          uFrameSize,
                                           REC_PACKET_INFO*    /*pstRecPacketInfo*/,
                                           IVS_VOID*       pUser)
{
    CRealPlay* pRealPlay = static_cast<CRealPlay*>(pUser);
    if (NULL == pRealPlay)
    {
        return;
    }

    pRealPlay->DoRawFrameCallBack(pstRawFrameInfo, pFrame, uFrameSize);
}

// 解码回调，返回解码后数据;
static IVS_VOID CALLBACK g_fDecoderCallBack(IVS_INT32   iStreamType,
                                            IVS_VOID*   pFrameInfo,
                                            IVS_VOID*   pUser)
{
    CRealPlay* pRealPlay = static_cast<CRealPlay*>(pUser);
    if (NULL == pRealPlay)
    {
        return;
    }

    pRealPlay->DoFrameCallBack(iStreamType, pFrameInfo);
}

CRealPlay::CRealPlay(void)
{//lint !e1927 !e1926 !e1928
    m_pRealPlayRawCBMutex = VOS_CreateMutex();
    m_pRealPlayFrameDecoderCBMutex = VOS_CreateMutex();

    CMediaBase::RetSet();
    RetSet();//lint !e1506

	m_RefCounter.counter = 0;
	m_nStatus = STATUS_READY;
}//lint !e1566


CRealPlay::~CRealPlay(void)
{
    try
    {
        VOS_DestroyMutex(m_pRealPlayRawCBMutex);
        m_pRealPlayRawCBMutex = NULL;
        VOS_DestroyMutex(m_pRealPlayFrameDecoderCBMutex);
        m_pRealPlayFrameDecoderCBMutex = NULL;
	    RetSet(); //lint !e1506已经实现了虚函数;
    }
    catch(...)
    {

    }
}//lint !e1579


void CRealPlay::RetSet()
{
	IVS_DEBUG_TRACE("");
	CMediaBase::RetSet();
	m_WndList.clear();
    m_fRealPlayCallBackRaw = NULL;
    m_pRealPlayUserDataRaw = NULL;
    m_fRealPlayCallBackFrame = NULL;
    m_pRealPlayUserDataFrame = NULL;
    eSDK_MEMSET(&m_CameraPlayInfo, 0, sizeof(CAMERA_PLAY_INFO));
}

int CRealPlay::ReStartRealPlay(const IVS_MEDIA_PARA* pMediaPara, const IVS_CHAR* pCameraCode)
{
	IVS_DEBUG_TRACE("");

    CLockGuard lock(m_pMediaMutex);

    /*
	 * EvenetMgr 机制可能在主线程stop之后，
	 * 将CRealPlay对象插入重连队列
	 * 这里判断主线程是否已经关闭本通道
	 */
	if (STATUS_FINISHED == m_nStatus || STATUS_PLAYING == m_nStatus)
	{
		return IVS_PARA_INVALID;
	}

	IVS_PLAY_ResetData(m_ulPlayerChannel); 
	IVS_PLAY_SetResetFlag(m_ulPlayerChannel, false);

	START_PLAY_PARAM stParam;
	stParam.enServiceType = SERVICE_TYPE_REALVIDEO;
	stParam.cbRtspDataCallBack = g_fRtspDataCallBack;  //lint !e63 !e1013 !e40
	stParam.cbNetDataCallBack = g_fNetDataCallBack;
	//开始实况;
	int iRet = ReStartStream(pMediaPara, pCameraCode, stParam);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"ReStart Real Play", "Start play error.");
	}
	return iRet;
}//lint !e1788

int CRealPlay::StartRealPlay(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode,HWND hWnd)
{
	IVS_DEBUG_TRACE("");

    CLockGuard lock(m_pMediaMutex);
    START_PLAY_PARAM stParam;
    stParam.enServiceType = SERVICE_TYPE_REALVIDEO;
    stParam.cbRtspDataCallBack = g_fRtspDataCallBack;  //lint !e63 !e1013 !e40
    stParam.cbNetDataCallBack = g_fNetDataCallBack;
	//开始实况;
	int iRet = StartPlay(pRealplayPara, pCameraCode, stParam);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start Real Play", "Start play error.");
        return iRet;
    }

    iRet = IVS_PLAY_SetPlayWindow(m_ulPlayerChannel, hWnd);
    // 将窗口插入到链表第一个;
    if (IVS_SUCCEED == iRet)
    {
		// 保存实况参数，用于恢复, add by zwb
		eSDK_MEMSET(&m_CameraPlayInfo, 0, sizeof(CAMERA_PLAY_INFO));
		bool bRet = CToolsHelp::Memcpy((IVS_CHAR*)&m_CameraPlayInfo,IVS_DEV_CODE_LEN, pCameraCode, strlen(pCameraCode));//lint !e1924
		if(false == bRet)
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Start Real Play", "Memcpy error.");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		bRet = CToolsHelp::Memcpy((IVS_CHAR*)&m_CameraPlayInfo + IVS_DEV_CODE_LEN, //lint !e1924
                                  sizeof(IVS_MEDIA_PARA)+IVS_DEV_CODE_LEN,
                                  &m_stuMediaParam, sizeof(IVS_MEDIA_PARA));

		if(false == bRet)
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Start Real Play", "Memcpy error.");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		m_CameraPlayInfo.uPlayHandle = (ULONG)this; //lint !e1924
		m_CameraPlayInfo.pUserData = m_pUserMgr; 
		m_CameraPlayInfo.pPlayObj  = this;
    }

	return iRet;
}

int CRealPlay::ReStartStream(const IVS_MEDIA_PARA* pMediaPara, const IVS_CHAR* pCameraCode, const START_PLAY_PARAM& stParam)
{
	//校验参数
	if (NULL == pCameraCode || strlen(pCameraCode) > (CODE_LEN-1) || strlen(pCameraCode) == 0)
	{
		BP_RUN_LOG_ERR(IVS_SMU_CAMERA_CODE_NOT_EXIST,"ReStart Stream", "camera code invalid");
		return IVS_PARA_INVALID;
	}
	IVS_DEBUG_TRACE("");

	//设置参数	
	bool bRet = CToolsHelp::Strncpy(m_szCameraID, sizeof(m_szCameraID), pCameraCode, strlen(pCameraCode));//lint !e534
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"ReStart Stream", "Strncpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	int iRet = SetMediaParam(pMediaPara);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"StartRealPlay", "set media param error");
		return iRet;
	}

	//从SCU获取媒体URL，解析数据保存在基类.
	iRet = DoGetUrlRealPlay();
	if (IVS_SUCCEED != iRet) 
	{
		BP_RUN_LOG_ERR(iRet,"ReStart Stream", "get url real play error, stream default");
		return iRet;
	}

	// 调用基类启动播放;
	iRet = CMediaBase::ReStartStream(stParam);
	if (iRet != IVS_SUCCEED)
	{
		BP_RUN_LOG_ERR(iRet,"ReStart Stream", "call base class failed.");
		return IVS_FAIL;
	}

	m_nStatus = STATUS_PLAYING;

	return iRet;
}

int CRealPlay::StartPlay(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, const START_PLAY_PARAM& stParam)
{
    //校验参数
    if (NULL == pCameraCode || strlen(pCameraCode) > (CODE_LEN-1) || strlen(pCameraCode) == 0)
    {
        BP_RUN_LOG_ERR(IVS_SMU_CAMERA_CODE_NOT_EXIST,"Start RealPlay", "camera code invalid");
        return IVS_PARA_INVALID;
    }
	CHECK_POINTER(pRealplayPara, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

    //设置参数
    bool bRet = CToolsHelp::Strncpy(m_szCameraID, sizeof(m_szCameraID), pCameraCode, strlen(pCameraCode));//lint !e534
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Start RealPlay", "Strncpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    IVS_MEDIA_PARA stMediaPara;
    eSDK_MEMSET(&stMediaPara, 0, sizeof(IVS_MEDIA_PARA));
    stMediaPara.ProtocolType = (IVS_PROTOCOL_TYPE)pRealplayPara->uiProtocolType; //lint !e1924
    stMediaPara.StreamType = (IVS_STREAM_TYPE)pRealplayPara->uiStreamType;       //lint !e1924
    stMediaPara.TransMode = (IVS_MEDIA_TRANS_MODE)pRealplayPara->bDirectFirst;   //lint !e1924
    stMediaPara.BroadCastType = (IVS_BROADCAST_TYPE)pRealplayPara->bMultiCast;   //lint !e1924
    stMediaPara.VideoDecType = VIDEO_DEC_H264;
    stMediaPara.AudioDecType = AUDIO_DEC_G711U;
    int iRet = SetMediaParam(&stMediaPara);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start RealPlay", "set media param error");
        return iRet;
    }

    //从SCU获取媒体URL，解析数据保存在基类.
    iRet = DoGetUrlRealPlay();
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start RealPlay", "get url real play error, stream default");
        return iRet;
    }

    pRealplayPara->uiStreamType = m_stuMediaParam.StreamType;

    // 调用基类启动播放;
    iRet = CMediaBase::StartPlay(stParam);
    if (iRet != IVS_SUCCEED)
    {
        BP_RUN_LOG_ERR(iRet,"Start RealPlay", "call base class failed.");
        return iRet;
    }
	// 关闭实况的流控事件
	try
	{
		IVS_PLAY_StopNotifyBuffEvent(m_ulPlayerChannel);
	}
	catch (...)
	{//lint !e1775
		BP_RUN_LOG_INF("StopNotifyBuffEvent", " exception"); 
	}
	
	BP_RUN_LOG_INF("Start RealPlay", "cameraCode=%s, playHandle=0x%p, playChannel=0x%p, rtspHandle=0x%p, netChannel=0x%p", pCameraCode, m_ulHandle, m_ulPlayerChannel, m_iRtspHandle, m_ulNetSourceChannel);
	m_nStatus = STATUS_PLAYING;
	
	return iRet;
}

int CRealPlay::DoGetUrlRealPlay()
{
	IVS_DEBUG_TRACE("");
	CDeviceMgr& deviceMgr = m_pUserMgr->GetDeviceMgr();

    //请求消息;
    eSDK_MEMSET(&m_stUrlMediaInfo, 0, sizeof(ST_URL_MEDIA_INFO));

    //TODO : szDstPort无实际作用，传一个固定值；打包协议类型szDstPP入参结构体未定义
    m_stUrlMediaInfo.ServiceType = SERVICE_TYPE_REALVIDEO;

    std::string strCameraDomainCode = m_szCameraID;
    std::string strCameraCode;
    std::string strDomainCode;
    int iRet = CToolsHelp::SplitCameraDomainCode(m_szCameraID, strCameraCode, strDomainCode);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url RealPlay", "CToolsHelp::SplitCameraCode failed, RetCode=%d.", iRet);
        return iRet;
    }

	//本域域编码
	m_stUrlMediaInfo.IsProgressionTransmit = false;
	std::string strCurDomainCode;
	m_pUserMgr->GetDomainCode(strCurDomainCode);
	if ((strCurDomainCode != strDomainCode)  &&
		(m_pUserMgr->IsSupportProgressionTransmit()) &&
		(!deviceMgr.IsShadowDev(m_szCameraID)))
	{
		std::string strProgressionDomainCode;
		m_pUserMgr->ProgressionTransmitDomainCode(strProgressionDomainCode);

		//将设备域保存起来
		(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.cCameraDomain,IVS_DOMAIN_CODE_LEN+1,  strDomainCode.c_str(), strDomainCode.size());//lint !e534

		//将域编码设置为转发域的
		strDomainCode = strProgressionDomainCode;
		m_stUrlMediaInfo.IsProgressionTransmit = true;  
	}
	
    (void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szSrcCode, sizeof(m_stUrlMediaInfo.szSrcCode), strCameraCode.c_str(), strCameraCode.size());//lint !e534
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.cDomainCode,IVS_DOMAIN_CODE_LEN+1,  strDomainCode.c_str(), strDomainCode.size());//lint !e534
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstCode,sizeof(m_stUrlMediaInfo.szDstCode), "01",strlen("01"));//lint !e534
	m_stUrlMediaInfo.MediaPara.StreamType = m_stuMediaParam.StreamType;
    m_stUrlMediaInfo.MediaPara.ProtocolType = m_stuMediaParam.ProtocolType;
	m_stUrlMediaInfo.MediaPara.VideoDecType = m_stuMediaParam.VideoDecType;
	m_stUrlMediaInfo.MediaPara.AudioDecType = m_stuMediaParam.AudioDecType;
	m_stUrlMediaInfo.MediaPara.TransMode = m_stuMediaParam.TransMode;
    m_stUrlMediaInfo.PackProtocolType = PACK_PROTOCOL_ES;
    m_stUrlMediaInfo.MediaPara.BroadCastType = m_stuMediaParam.BroadCastType;
    (void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstIP,sizeof(m_stUrlMediaInfo.szDstIP), "",strlen(""));//lint !e534
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstPort,sizeof(m_stUrlMediaInfo.szDstPort), "35016",strlen("35016"));//lint !e534

    //获取媒体URL
    iRet = GetURL(CToolsHelp::GetSCCHead().c_str(), m_pUserMgr, m_stUrlMediaInfo, m_stMediaRsp, m_bAssociatedAudio);//lint !e838
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url RealPlay", "real play SCC get URL failed!");
        return iRet;
    }

	//保存NVRCode;
	bool bRet = CToolsHelp::Memcpy(m_szNvrCode, sizeof(m_szNvrCode), m_stUrlMediaInfo.szNVRCode, strlen(m_stUrlMediaInfo.szNVRCode));  
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Start Play Back", "eSDK_MEMCPY error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	iRet = ParseIPAddr(m_stMediaRsp.szURL,m_strMuIpAddr);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url RealPlay", "parse IP Addr failed from url: %s",
			m_stMediaRsp.szURL);
		return iRet;
	}
    //解析URL,得到ServiceType，如果是组播实况（直连），那么协议类型转为udp
    std::string strServiceType;
    iRet = ParseURL(m_stMediaRsp.szURL, URL_SERVICE_TYPE_FLAG, strServiceType);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url RealPlay", "parse service type failed from url: %s",
            m_stMediaRsp.szURL);
        return iRet;
    }
    int iServiceType = CToolsHelp::StrToInt(strServiceType);

    std::string strMediaTransMode;
    iRet = ParseURL(m_stMediaRsp.szURL, URL_MEDIA_TRANS_MODE, strMediaTransMode);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url RealPlay", "parse media trans mode failed from url");
        return iRet;
    }
    int iMediaTransMode = CToolsHelp::StrToInt(strMediaTransMode);

    std::string strBroadcastType;
    iRet = ParseURL(m_stMediaRsp.szURL, URL_BROADCAST_TYPE, strBroadcastType);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url RealPlay", "parse broadcast type failed from url");
        return iRet;
    }
    int iBroadcastType = CToolsHelp::StrToInt(strBroadcastType);
    
    m_enServiceType = (IVS_SERVICE_TYPE)iServiceType;                   //lint !e1924
    m_stuMediaParam.TransMode = (IVS_MEDIA_TRANS_MODE)iMediaTransMode;  //lint !e1924
    m_stuMediaParam.BroadCastType = (IVS_BROADCAST_TYPE)iBroadcastType; //lint !e1924

    // 直连组播实况，需要将协议类型转换为UDP;
	if (SERVICE_TYPE_REALVIDEO == m_enServiceType 
        && MEDIA_DIRECT == m_stuMediaParam.TransMode
        && BROADCAST_MULTICAST == m_stuMediaParam.BroadCastType)
	{
		m_stuMediaParam.ProtocolType = PROTOCOL_RTP_OVER_UDP;
	}

    // add for 流复用 [12/17/2012 z90003203]
    std::string strStreamType;
    iRet = ParseURL(m_stMediaRsp.szURL, URL_STREAM_TYPE, strStreamType);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url RealPlay", "parse stream type failed from url");
        return iRet;
    }
    int iStreamType = CToolsHelp::StrToInt(strStreamType);
    m_stuMediaParam.StreamType = (IVS_STREAM_TYPE)iStreamType; //lint !e1924

    if (STREAM_TYPE_UNSPECIFIED == m_stuMediaParam.StreamType)
    {
        m_stuMediaParam.StreamType = STREAM_TYPE_MAIN;
    }
    // end add
	return iRet;
}//lint !e529

//停止实况
int CRealPlay::StopRealPlay()
{
	IVS_DEBUG_TRACE("");

    CLockGuard lock(m_pMediaMutex);

    int iRet = CMediaBase::StopPlay();

	//重置参数
	RetSet();

	m_nStatus = STATUS_FINISHED;

    if (iRet != IVS_SUCCEED)
    {
        iRet = IVS_FAIL;
    }
	return iRet;
}//lint !e1788

int CRealPlay::AddPlayWindow(HWND hWnd)
{
	return IVS_PLAY_SetPlayWindow(m_ulPlayerChannel, hWnd);
}

int CRealPlay::DelPlayWindow(HWND hWnd)
{
	return IVS_PLAY_DelPlayWindow(m_ulPlayerChannel, hWnd);
}

int CRealPlay::AddPlayWindowPartial(HWND hWnd, void* pPartial)
{
	return IVS_PLAY_AddPlayWindowPartial(m_ulPlayerChannel, hWnd, (LAYOUT_DRAW_PARTIAL*)pPartial); //lint !e1924
}

int CRealPlay::UpdateWindowPartial(HWND hWnd, void* pPartial)
{
	return IVS_PLAY_UpdateWindowPartial(m_ulPlayerChannel, hWnd, (LAYOUT_DRAW_PARTIAL*)pPartial); //lint !e1924
}

//判断窗口句柄是否存在;
bool CRealPlay::IsExistHWnd(const HWND& hwnd)
{
    bool bRet = false;
    WndListIter iter = m_WndList.begin();
    WndListIter iterEnd = m_WndList.end();
    for (; iter != iterEnd; iter++)
    {
        if (hwnd == *iter)
        {
            bRet = true;
            break;
        }
    }
    return bRet;
}

//删除WndList中的Wnd;
int CRealPlay::DelHWndfromList(const HWND& hwnd)
{
    int bRet = IVS_FAIL;
    WndListIter iter = m_WndList.begin();
    WndListIter iterEnd = m_WndList.end();
    for (; iter != iterEnd; iter++)
    {
        if (hwnd == *iter)
        {
            m_WndList.erase(iter);
            bRet = IVS_SUCCEED;
            break;
        }
    }
    return bRet;
}


//获取CameraCode
char* CRealPlay::GetCameraCode()
{
	return m_szCameraID;//lint !e1536
}//lint !e1762

void CRealPlay::SetHandle(unsigned long ulHandle)
{
	m_ulHandle = ulHandle;
}//lint !e1762

unsigned long CRealPlay::GetHandle()
{
	return m_ulHandle;
}//lint !e1762

//开始实况(裸码流)
int CRealPlay::StartRealPlayCBRaw(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, RealPlayCallBackRaw fRealPlayCallBackRaw, void* pUserData )
{
	IVS_DEBUG_TRACE("");
    CLockGuard lock(m_pMediaMutex);
    START_PLAY_PARAM stParam;
    stParam.cbRtspDataCallBack = g_fRtspDataCallBack;  //lint !e63 !e1013 !e40
    stParam.cbNetDataCallBack = g_fNetDataCallBack;
    int iRet = StartPlay(pRealplayPara, pCameraCode, stParam);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start RealPlay CBRaw", "start play failed.");
        return iRet;
    }
	
    m_iStartType = PLAY_TYPE_CB;
    (void)VOS_MutexLock(m_pRealPlayRawCBMutex);
    m_fRealPlayCallBackRaw = fRealPlayCallBackRaw;
    m_pRealPlayUserDataRaw = pUserData;
    (void)VOS_MutexUnlock(m_pRealPlayRawCBMutex);
    iRet = IVS_PLAY_SetRawFrameCallBack(m_ulPlayerChannel, g_fReassmCallBack, this);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start RealPlay CBRaw", "Set Raw Frame CallBack failed.");
        return iRet;
    }

    // 保存实况参数，用于恢复, add by zwb
    eSDK_MEMSET(&m_CameraPlayInfo, 0, sizeof(CAMERA_PLAY_INFO));
    bool bRet = CToolsHelp::Memcpy((IVS_CHAR*)&m_CameraPlayInfo, IVS_DEV_CODE_LEN, pCameraCode, strlen(pCameraCode));
    if(false == bRet)
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Start Real Play", "Memcpy error.");
        return IVS_ALLOC_MEMORY_ERROR;
    }

    bRet = CToolsHelp::Memcpy((IVS_CHAR*)&m_CameraPlayInfo + IVS_DEV_CODE_LEN, //lint !e1924
                               sizeof(IVS_MEDIA_PARA)+IVS_DEV_CODE_LEN,
                               &m_stuMediaParam, sizeof(IVS_MEDIA_PARA));

    if(false == bRet)
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Start Real Play", "Memcpy error.");
        return IVS_ALLOC_MEMORY_ERROR;
    }
    m_CameraPlayInfo.uPlayHandle = (ULONG)this; //lint !e1924
    m_CameraPlayInfo.pUserData = m_pUserMgr;
	m_CameraPlayInfo.pPlayObj  = this;

	return iRet; 
}//lint !e1788

void CRealPlay::DoRawFrameCallBack(IVS_RAW_FRAME_INFO* pstRawFrameInfo, IVS_CHAR* pFrame, IVS_UINT32 uFrameSize)
{
    (void)VOS_MutexLock(m_pRealPlayRawCBMutex);
    if (NULL != m_fRealPlayCallBackRaw)
    {
        m_fRealPlayCallBackRaw(m_ulHandle, pstRawFrameInfo, pFrame, uFrameSize, m_pRealPlayUserDataRaw);
    }
    (void)VOS_MutexUnlock(m_pRealPlayRawCBMutex);
}

void CRealPlay::DoFrameCallBack(IVS_INT32 iStreamType, IVS_VOID* pFrameInfo)
{
    (void)VOS_MutexLock(m_pRealPlayFrameDecoderCBMutex);
    if (NULL != m_fRealPlayCallBackFrame)
    {
        m_fRealPlayCallBackFrame(m_ulHandle, (unsigned int)iStreamType, pFrameInfo, m_pRealPlayUserDataFrame); //lint !e1924
    }
    (void)VOS_MutexUnlock(m_pRealPlayFrameDecoderCBMutex);
}

//开始实况(YUV流)
int CRealPlay::StartRealPlayCBFrame(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, RealPlayCallBackFrame fRealPlayCallBackFrame, void* pUserData )
{
	IVS_DEBUG_TRACE("");
    CLockGuard lock(m_pMediaMutex);
    START_PLAY_PARAM stPlayParam;
    stPlayParam.cbRtspDataCallBack = g_fRtspDataCallBack;  //lint !e63 !e1013 !e40
    stPlayParam.cbNetDataCallBack = g_fNetDataCallBack;
    int iRet = StartPlay(pRealplayPara, pCameraCode, stPlayParam);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start RealPlay CB Frame", "start play failed.");
        return iRet;
    }

    m_iStartType = PLAY_TYPE_YUV;
    (void)VOS_MutexLock(m_pRealPlayFrameDecoderCBMutex);
    m_fRealPlayCallBackFrame = fRealPlayCallBackFrame;
    m_pRealPlayUserDataFrame = pUserData;
    (void)VOS_MutexUnlock(m_pRealPlayFrameDecoderCBMutex);
    iRet = IVS_PLAY_SetFrameCallBack(m_ulPlayerChannel, g_fDecoderCallBack, this);
	return iRet;
}

int CRealPlay::ReuseStream(HWND hWnd)
{
	IVS_DEBUG_TRACE("");
    LAYOUT_DRAW_NORMAL Layout;
    eSDK_MEMSET(&Layout, 0, sizeof(LAYOUT_DRAW_NORMAL));
    int iRet = IVS_PLAY_AddPlayWindow(m_ulPlayerChannel, hWnd, DRAW_NORMAL, &Layout);
    // 如果复用成功，窗口插入到list最后;
    if (IVS_SUCCEED == iRet)
    {
        m_WndList.push_back(hWnd);
    }
    return iRet;
}

bool CRealPlay::IsExistStream(const char* pszCameraID, IVS_STREAM_TYPE eStreamType) const
{
    if (NULL == pszCameraID)
    {
        return false;
    }

    std::string strCameraID = pszCameraID;
    if ((0 == strCameraID.compare(m_szCameraID))
        && ((STREAM_TYPE_UNSPECIFIED == eStreamType)
        || (eStreamType == m_stuMediaParam.StreamType)))
    {
        return true;
    }

    return false;
}

// 播放随路语音
int CRealPlay::PlaySound()
{
    return CMediaBase::PlaySound();
}

//停止播放随路语音
int CRealPlay::StopSound()
{
    return CMediaBase::StopSound();
}

//设置音量
int CRealPlay::SetVolume(unsigned int uiVolumeValue)
{
	IVS_DEBUG_TRACE("");
    return CMediaBase::SetVolume(uiVolumeValue);
}

//获取音量
int CRealPlay::GetVolume(unsigned int* puiVolumeValue)
{
	IVS_DEBUG_TRACE("");
    return CMediaBase::GetVolume(puiVolumeValue);
}

// 获取播放信息
void CRealPlay::GetCameraPlayInfo(CAMERA_PLAY_INFO& cameraPlayInfo)const
{
	eSDK_MEMSET(&cameraPlayInfo, 0, sizeof(CAMERA_PLAY_INFO));
	bool bRet = CToolsHelp::Memcpy(&cameraPlayInfo, sizeof(CAMERA_PLAY_INFO), &m_CameraPlayInfo, sizeof(CAMERA_PLAY_INFO));
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Camera PlayInfo", "Memcpy error.");
	}
}

// 获取播放窗口
int CRealPlay::GetPlayWindows(HWND *pBuf, int size)
{
	IVS_DEBUG_TRACE("");
     WndListIter iter = m_WndList.begin();
	 WndListIter end  = m_WndList.end();
	 int count = 0;
	 for(; iter != end && count < size; iter++)
	 {
		 pBuf[count++] = *iter;
	 }
	 return count;
}

int CRealPlay::StopStream()
{
	IVS_DEBUG_TRACE("");
	CLockGuard lock(m_pMediaMutex);

	if (STATUS_FINISHED == m_nStatus)
	{
		return IVS_SUCCEED;
	}

	(void)CMediaBase::StopStream();

	m_nStatus = STATUS_STOPPED;

	return IVS_SUCCEED;
}//lint !e1788

long CRealPlay::GetRef()
{
	return atomic_inc(&m_RefCounter);
}

long CRealPlay::PutRef()
{
	return atomic_dec(&m_RefCounter);
}
	
IVS_INT32 CRealPlay::GetRealPlayRtspURL(const IVS_CHAR* pCameraCode, 
	const IVS_URL_MEDIA_PARAM *pURLMediaParam, 
	IVS_CHAR *pRtspURL, 
	IVS_UINT32 uiBufferSize)
{
	IVS_DEBUG_TRACE("");
	CDeviceMgr& deviceMgr = m_pUserMgr->GetDeviceMgr();

	//校验参数
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pURLMediaParam, IVS_PARA_INVALID);

	BP_RUN_LOG_INF("Enter Get Rtsp URL", "CameraDomainCode = %s, StreamType = %u, ProtocolType = %u, ClientType: %d, PackProtocolType: %d.",
		pCameraCode, pURLMediaParam->StreamType, pURLMediaParam->ProtocolType, pURLMediaParam->iClientType, pURLMediaParam->PackProtocolType);

	//请求消息;
	eSDK_MEMSET(&m_stUrlMediaInfo, 0, sizeof(ST_URL_MEDIA_INFO));

	m_stUrlMediaInfo.ServiceType = SERVICE_TYPE_REALVIDEO;

	std::string strCameraDomainCode = pCameraCode;
	std::string strCameraCode;
	std::string strDomainCode;
	IVS_INT32 iRet = CToolsHelp::SplitCameraDomainCode(strCameraDomainCode, strCameraCode, strDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"GetRealPlayRtspURL", "CToolsHelp::SplitCameraCode failed, RetCode=%d.", iRet);
		return iRet;
	}

	///*	//平台暂不支持第三方工具播放具有转发域功能的URL，暂时注释
	//本域域编码
	m_stUrlMediaInfo.IsProgressionTransmit = false;
	std::string strCurDomainCode;
	m_pUserMgr->GetDomainCode(strCurDomainCode);
	if ((strCurDomainCode != strDomainCode)  &&
		(m_pUserMgr->IsSupportProgressionTransmit()) &&
		(!deviceMgr.IsShadowDev(m_szCameraID)))
	{
		std::string strProgressionDomainCode;
		m_pUserMgr->ProgressionTransmitDomainCode(strProgressionDomainCode);

		//将设备域保存起来
		(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.cCameraDomain,IVS_DOMAIN_CODE_LEN+1,  strDomainCode.c_str(), strDomainCode.size());//lint !e534

		//将域编码设置为转发域的
		strDomainCode = strProgressionDomainCode;
		m_stUrlMediaInfo.IsProgressionTransmit = true;  
	}
	//*/
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szSrcCode, sizeof(m_stUrlMediaInfo.szSrcCode), strCameraCode.c_str(), strCameraCode.size());//lint !e534
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstCode,sizeof(m_stUrlMediaInfo.szDstCode), "01",strlen("01"));

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
		BP_RUN_LOG_ERR(iRet,"GetRealPlayRtspURL", "set media param error");
		return iRet;
	}

	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.cDomainCode,IVS_DOMAIN_CODE_LEN+1,  strDomainCode.c_str(), strlen(strDomainCode.c_str()));

	m_stUrlMediaInfo.MediaPara.AudioDecType = m_stuMediaParam.AudioDecType;
	m_stUrlMediaInfo.MediaPara.BroadCastType = m_stuMediaParam.BroadCastType;
	m_stUrlMediaInfo.MediaPara.ProtocolType = m_stuMediaParam.ProtocolType;
	m_stUrlMediaInfo.MediaPara.StreamType = m_stuMediaParam.StreamType;
	m_stUrlMediaInfo.MediaPara.TransMode = m_stuMediaParam.TransMode;
	m_stUrlMediaInfo.MediaPara.VideoDecType = m_stuMediaParam.VideoDecType;
	m_stUrlMediaInfo.PackProtocolType = (IVS_PACK_PROTOCOL_TYPE)pURLMediaParam->PackProtocolType;
	m_stUrlMediaInfo.iClientType = pURLMediaParam->iClientType;
	//client_type=1时,PassMTU=0才能支持获取NAT地址
	if(1 == m_stUrlMediaInfo.iClientType)
	{
		m_stUrlMediaInfo.IsPassMTU = false; 
	}

	// 可选
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstIP, sizeof(m_stUrlMediaInfo.szDstIP), "",strlen(""));//lint !e534
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstPort, sizeof(m_stUrlMediaInfo.szDstPort), "35016",strlen("35016"));

	iRet = GetURL(CToolsHelp::GetSCCHead().c_str(), m_pUserMgr, m_stUrlMediaInfo, m_stMediaRsp, m_bAssociatedAudio);

	std::string strURL = GetUrlBuffer();
	if (strURL.empty())
	{
		iRet = IVS_URL_INVALID;
		BP_RUN_LOG_ERR(iRet, "GetRealPlayRtspURL", "URL is NULL");
		return iRet;
	}
	if (strURL.length() >= uiBufferSize)
	{
		iRet = IVS_PARA_INVALID;
		BP_RUN_LOG_ERR(iRet, "GetRealPlayRtspURL", "Para Invalid, Buffer is too small");
		return iRet;
	}

	eSDK_STRCPY(pRtspURL, uiBufferSize, strURL.c_str());
	BP_RUN_LOG_INF("GetRealPlayRtspURL success", "");

	return iRet;
}








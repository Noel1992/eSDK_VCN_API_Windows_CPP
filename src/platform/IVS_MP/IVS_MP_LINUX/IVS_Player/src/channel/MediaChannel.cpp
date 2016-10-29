#include "MediaChannel.h"
#include "RenderAPI.h"
#include "DecoderAPI.h"
#include "ReassembleAPI.h"
#ifdef WIN32
#include "RecorderAPI.h"
#include "Mp4FileSource.h"
#include "AviFileSource.h"
#endif
#include "_BaseThread.h"
#include "IVSCommon.h"
#include "ivs_log.h"
#include "RtpDataType.h"
#ifdef WIN32
#include "Capture.h"
#include <windows.h>
#endif
#include <stdlib.h>
#include <stdio.h>

#define UPPER_FRAME 8
#define LOWER_FRAME 3
#define UPPER_TIME  500
#define LOWER_TIME  200

#define RENDER_FRAMES_LOW  10 // 单步播放渲染模块缓存下限
#define RENDER_FRAMES_HIGH 20 // 单步播放渲染模块缓存上限
#define RENDER_FRAMES_STEP 5

#define  NO_NEED_ENCRYPTED (-1)  //不加密标志

BOOL CMediaChannel::m_bHardwareDec = FALSE;

// 拼帧回调，返回拼帧后未解码数据;
static IVS_VOID __SDK_CALLBACK ReassmCallBack(IVS_RAW_FRAME_INFO* pstRawFrameInfo,
                                        IVS_CHAR*           pFrame,
                                        IVS_UINT32          uFrameSize,
                                        REC_PACKET_INFO*    /*pstRecPacketInfo*/,
                                        IVS_VOID*       pUser)
{
    CMediaChannel* pMediaChannel = static_cast <CMediaChannel*>(pUser);

    if (NULL == pMediaChannel)
    {
        IVS_LOG(IVS_LOG_ERR, "ReassmCallBack", "Call back param error, pUser cannot transform to CMediaChannel");
        return;
    }

    if (SERVICE_TYPE_AUDIO_CALL == pMediaChannel->GetPlayMode())
    {
        (void)pMediaChannel->ProcessRawAudioFrame(pstRawFrameInfo, pFrame, uFrameSize);
        return;
    }

    (void)pMediaChannel->ProcessRawFrame(pstRawFrameInfo, pFrame, uFrameSize);
	//pMediaChannel->InsertRecordFrame(pstRawFrameInfo, pFrame, uFrameSize);
}

// 解码回调，返回解码后数据;
static IVS_VOID __SDK_CALLBACK DecoderCallBack(IVS_INT32   iStreamType,
                                         IVS_VOID*   pFrameInfo,
                                         IVS_VOID*   pUser)
{
    CMediaChannel* pMediaChannel = static_cast <CMediaChannel*>(pUser);

    if (NULL == pMediaChannel)
    {
        IVS_LOG(IVS_LOG_ERR, "DecoderCallBack", "Call back param error, pUser cannot transform to CMediaChannel");
        return;
    }

    (void)pMediaChannel->ProcessFrame(iStreamType, pFrameInfo);
}
#ifdef WIN32
/*
 * FileSourceCallBack
 * 文件Source数据回调函数
 */
static IVS_VOID __SDK_CALLBACK FileSourceCallBack(MEDIA_FRAME_INFO *pInfo, IVS_VOID *pUser)
{
	CMediaChannel* pMediaChannel = (CMediaChannel*)pUser;
	if (NULL == pMediaChannel)
	{
		IVS_LOG(IVS_LOG_ERR, "FileSourceCallBack", "Channel = NULL");
		return;
	}

	(void)pMediaChannel->ProcessFileFrame(pInfo);
}
#endif

/*
 *  PlayEndCallBack
 *  播放结束回调
 */
static IVS_VOID __SDK_CALLBACK PlayEndCallBack(IVS_VOID *pUser)
{
	CMediaChannel* pMediaChannel = (CMediaChannel*)pUser;
	if (NULL == pMediaChannel)
	{
		IVS_LOG(IVS_LOG_ERR, "PlayEndCallBack", "Channel = NULL");
		return;
	}
    (void)pMediaChannel->ProcessPlayEnd();
}



CMediaChannel::CMediaChannel(void)
{
    Init();
}

CMediaChannel::~CMediaChannel(void)
{
    try
    {
        UnInit();
    }
    catch (...)
    {
        IVS_LOG(IVS_LOG_DEBUG, "~CMediaChannel", "Destructor throw exception."); //lint !e1551
    }
}

void CMediaChannel::Init()
{
    m_enVideoDecoderType = VIDEO_DEC_H264;
    m_enAudioDecoderType = AUDIO_DEC_G711U;
    m_enServiceMode = SERVICE_TYPE_REALVIDEO;
    m_enSourceType = SOURCE_RTP;
    m_handleReassemble = NULL;
    m_handleDecoder = NULL;
    m_handleRender = NULL;
	m_handleRecorder = NULL;
	m_bRecordFlag    = false;

	m_bStreamInputControl = false;
 
	m_pSource = NULL;
    m_pSourceThread = NULL;
    m_bNeedInputData2DecoderFlag = false;
    m_bNeedInputData2RenderFlag = false;
    m_bShowStatis = false;
    m_cbReassmCB = NULL;
    m_cbDecoderCB = NULL;
    m_pReassmUserData = NULL;
    m_pDecoderUserData = NULL;
    m_cbExceptionCB = NULL;
    m_pExceptionUserData = NULL;
    m_bStreamTypeFlay = false;

	m_tmDownLoadBeg = 0;
	m_tmDownLoadEnd = 0; 
	m_bRepFinish    = false;

	m_pSecretKey = NULL;
	m_ulKeyLen = 0;
	m_iDecType = NO_DEC_TYPE;

    m_cbEventCB = NULL;
    m_pEventUserData = NULL;

	m_dSpeed = 0.0;

	m_bBackPlayStep = false;

    m_bIsVideoDecoderTypeChange = false;
    memset(&m_VideoAttr, 0x0, sizeof(m_VideoAttr));
    memset(&m_AudioAttr, 0x0, sizeof(m_AudioAttr));
}

void CMediaChannel::UnInit()
{
    IVS_LOG(IVS_LOG_DEBUG, "In CMediaChannel::UnInit", "");
    m_enVideoDecoderType = VIDEO_DEC_H264;
    m_enAudioDecoderType = AUDIO_DEC_G711U;
    m_enServiceMode = SERVICE_TYPE_REALVIDEO;
    m_enSourceType = SOURCE_RTP;

    m_cbReassmCB = NULL;
    m_cbDecoderCB = NULL;
    m_cbExceptionCB = NULL;
    m_pReassmUserData = NULL;
    m_pDecoderUserData = NULL;
    m_pExceptionUserData = NULL;

    m_cbEventCB = NULL;
    m_pEventUserData = NULL;

    m_bNeedInputData2DecoderFlag = false;
    m_bNeedInputData2RenderFlag = false;
    m_bShowStatis = false;
    m_bStreamTypeFlay = false;
	m_dSpeed = 0.0;

    if (NULL != m_handleReassemble)
    {
        IVS_LOG(IVS_LOG_DEBUG, "CMediaChannel::UnInit begin Reass_FreeHandle", "");
        Reass_FreeHandle(m_handleReassemble);
        m_handleReassemble = NULL;
        IVS_LOG(IVS_LOG_DEBUG, "CMediaChannel::UnInit end Reass_FreeHandle", "");
    }

    if (NULL != m_handleDecoder)
    {
        Decoder_FreeHandle(m_handleDecoder);
        m_handleDecoder = NULL;
    }

    if (NULL != m_handleRender)
    {
        //Render_FreeHandle(m_handleRender);
        m_handleRender = NULL;
    }

	if (NULL != m_handleRecorder)
	{
		//Recorder_FreeHandle(m_handleRecorder);
		m_handleRecorder = NULL;
	}

    // 先释放线程对象，在线程
    if (NULL != m_pSourceThread)
    {
        IVS_DELETE(m_pSourceThread);
        m_pSourceThread = NULL;
    }
#ifdef WIN32
    if (NULL != m_pSource)
    {
        IVS_DELETE(m_pSource);
        m_pSource = NULL;
    }
#endif
	IVS_DELETE(m_pSecretKey,MUILI);
    IVS_LOG(IVS_LOG_DEBUG, "Out CMediaChannel::UnInit", "");
}

int CMediaChannel::OpenStream(const OPEN_STREAM_PARAM& stOpenStreamParam)
{
    int iRet = IVS_SUCCEED;

    // 存储数据
    m_enServiceMode = stOpenStreamParam.enServiceType;
    m_enSourceType = stOpenStreamParam.enSourceType;

#if 0
    // 非下载业务，初始化渲染、解码模块;
    if (SERVICE_TYPE_DOWNLOAD != m_enServiceMode)
    {
        // 获取渲染句柄
        //m_handleRender = Render_GetHandle();

        // 设置异常回调函数;
        //Render_SetExceptionCallback(m_handleRender, stOpenStreamParam.fExceptionCallback, stOpenStreamParam.pUser);

		if (SERVICE_TYPE_REALVIDEO == m_enServiceMode)
		{
			//Render_SetStreamMode(m_handleRender, STREAM_REALTIME);
		}
		else
		{
			//Render_SetStreamMode(m_handleRender, STREAM_REMOTEFILE);
		}

        // 语音对讲和广播不需要初始化解码模块
        if (SERVICE_TYPE_AUDIO_CALL != m_enServiceMode
            && SERVICE_TYPE_AUDIO_BROADCAST != m_enServiceMode)
        {
            // 获取解码句柄
            m_handleDecoder = Decoder_GetHandle();

            VIDEO_PARAM stVideoParam;
            memset(&stVideoParam, 0, sizeof(stVideoParam));
            stVideoParam.VideoFormat = m_enVideoDecoderType;
            stVideoParam.HardwareDec = CMediaChannel::m_bHardwareDec;

            AUDIO_PARAM stAudioParam;
            memset(&stAudioParam, 0, sizeof(stAudioParam));
            stAudioParam.AudioFormat = AUDIO_DEC_G711U;
            stAudioParam.BitRate = 128000;
            stAudioParam.Channels = 1;
            stAudioParam.SampleRate = 8000;

            iRet = Decoder_OpenStream(m_handleDecoder, 
                                      &stVideoParam, 
                                      &stAudioParam, 
                                      stOpenStreamParam.uiLowerLimit, 
                                      stOpenStreamParam.uiUpperLimit);

            // 设置异常回调函数;
            iRet += Decoder_SetExceptionCallback(m_handleDecoder, stOpenStreamParam.fExceptionCallback, stOpenStreamParam.pUser);

            // 设置事件回调函数;
            iRet += Decoder_SetEventCallback(m_handleDecoder, stOpenStreamParam.fEventCallback, stOpenStreamParam.pUser);

            // 设置解码回调;
            iRet += Decoder_SetDecoderCB(m_handleDecoder, DecoderCallBack, this);
        }

		m_cbExceptionCB = stOpenStreamParam.fExceptionCallback;
		m_pExceptionUserData = stOpenStreamParam.pUser;

        m_cbEventCB = stOpenStreamParam.fEventCallback;
        m_pEventUserData = stOpenStreamParam.pUser;

#if 0
		ShowPlayStatis(true);
#endif 
    }
#endif 	
    return iRet;
}

int CMediaChannel::CloseStream()
{
#if 0
	if (NULL == m_handleDecoder)
    {
        return IVS_SUCCEED;
    }

    // 渲染模块调用StopPlay即停止线程,此处仅调用解码库;
    int iRet = Decoder_CloseStream(m_handleDecoder);
    return iRet;
#endif 
    return IVS_SUCCEED;
}

/*
 * StartPlay
 * 启动播放
 */
int CMediaChannel::StartPlay()
{
#if 0
	int iRet = IVS_FAIL;//Render_StartPlayVideo(m_handleRender);
    if (IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Media Channel StartPlay", "Render Fail");
		return iRet;
    }

    if (SERVICE_TYPE_AUDIO_BROADCAST == m_enServiceMode
        || SERVICE_TYPE_AUDIO_CALL == m_enServiceMode)
    {
        //iRet = Render_StartPlayAudio(m_handleRender);
        if (IVS_SUCCEED != iRet)
        {
            IVS_LOG(IVS_LOG_ERR, "Media Channel StartPlay", "Render Fail");
            return iRet;
        }
    }
    else
    {
        iRet = Decoder_Start(m_handleDecoder);
        if (IVS_SUCCEED != iRet)
        {
            IVS_LOG(IVS_LOG_ERR, "Media Channel StartPlay", "Decoder Fail");
            //Render_StopPlay(m_handleRender);
            return iRet;
        }
    }

	if (SOURCE_FILE == m_enSourceType)
	{
        iRet = IVS_FAIL;
		IVS_ASSERT(NULL != m_pSource);
    	if (NULL != m_pSource)
		{
			iRet = m_pSource->Start();
		}
		
		if (IVS_SUCCEED != iRet)
		{
			IVS_LOG(IVS_LOG_ERR, "Media Channel StartPlay", "File Source Fail");
			Decoder_Stop(m_handleDecoder);
			//Render_StopPlay(m_handleRender);
            return iRet;
		}
	}
	
	return IVS_SUCCEED;
#endif 
    return IVS_SUCCEED;	
}

/*
 * StopPlay
 * 停止播放
 */
int CMediaChannel::StopPlay()
{
#if 0
	if (SOURCE_FILE == m_enSourceType)
	{
		IVS_ASSERT(NULL != m_pSource);
		if (NULL != m_pSource)
		{
			m_pSource->Stop();
		}
	}

	if (NULL != m_handleReassemble)
	{
		(void)Reass_ReSetReassemble(m_handleReassemble);
	}

    if (NULL != m_handleDecoder)
    {
        (void)Decoder_Stop(m_handleDecoder);
    }
    
    //(void)Render_StopPlay(m_handleRender);
   	return IVS_SUCCEED;
#endif 
     return IVS_SUCCEED;
}

/*
 * PausePlay
 * 暂停播放
 */
int CMediaChannel::PausePlay()
{
#if 0
	if (SOURCE_FILE == m_enSourceType)
	{
		IVS_ASSERT(NULL != m_pSource);
		if (NULL != m_pSource)
		{
			m_pSource->Pause();
		}
	}

	if (NULL != m_handleRecorder)
	{
		//Recorder_Split(m_handleRecorder);
	}
	
	if (NULL != m_handleRender && NULL != m_handleDecoder)
	{
		Decoder_Stop(m_handleDecoder);
		//Render_PausePlay(m_handleRender);
		return IVS_SUCCEED;
	}
	return IVS_FAIL;
#endif 
    return IVS_SUCCEED; 
}

/*
 * ResumePlay
 * 恢复播放
 */
int CMediaChannel::ResumePlay()
{
#if 0
	if (SOURCE_FILE == m_enSourceType)
	{
		IVS_ASSERT(NULL != m_pSource);
		if (NULL != m_pSource)
		{
			m_pSource->Resume();
		}
	}
		
	if (NULL != m_handleRender && NULL != m_handleDecoder)
	{
		//Render_ResumePlay(m_handleRender);
		Decoder_Start(m_handleDecoder);
		return IVS_SUCCEED;
	}
	return IVS_FAIL;
#endif 
    return IVS_SUCCEED;
}


int CMediaChannel::PlaySound()
{
    int iRet = IVS_FAIL;
    if (NULL != m_handleRender)
    {
        //iRet = Render_StartPlayAudio(m_handleRender);
    }
    return iRet;
}

int CMediaChannel::StopSound()
{
    int iRet = IVS_FAIL;
    if (NULL != m_handleRender)
    {
        //iRet = Render_StopPlayAudio(m_handleRender);
    }
    return iRet;
}

/*
 * StepForward
 * 单步前进播放
 */
int CMediaChannel::StepForward(unsigned int &PicInPlayer)
{
#if 0
	IVS_ASSERT(m_handleDecoder);
	IVS_ASSERT(m_handleRender);

	if (m_handleReassemble)
	{
		Reass_SetPlayBackMode(m_handleReassemble, FALSE);
       /*
		* DTS2013060707094
		* 倒放转正放 强制从I帧开始接收，否则花屏
		*/
		if (m_bBackPlayStep)
		{
			Reass_ReSetReassemble(m_handleReassemble);
		}
	}

	if (m_pSource)
	{
		if (m_bBackPlayStep)
		{
			unsigned long long tick = 0;
			//(void)Render_GetCurrentFrameTick(m_handleRender, &tick);
			m_pSource->SetPlayTime((unsigned int)(tick / 1000));
		}
		
		m_pSource->SetPlaySpeed(1.0);
		m_pSource->Resume();
	}

	if (m_handleDecoder && m_handleRender)
	{
		// 渲染模块播放一帧
		unsigned int PicInRender  = 0;
		unsigned int PicInDecoder = 0;
		
		/*
		 * 播放方向发生改变，清除End标记
		 */
		if (m_bBackPlayStep)
		{
			Decoder_MarkStreamEnd(m_handleDecoder, FALSE);
			m_bBackPlayStep = false;
		}

		//Render_StepForward(m_handleRender, &PicInRender);
		Decoder_GetSourceBufferRemain(m_handleDecoder, PicInDecoder);
		
		// TODO 驱动解码模块解一定帧数
		if (PicInRender < RENDER_FRAMES_LOW)
		{
			Decoder_StepFrames(m_handleDecoder, RENDER_FRAMES_STEP);
		}
		else if (PicInRender > RENDER_FRAMES_HIGH) 
		{
			Decoder_Stop(m_handleDecoder);
		}

		PicInPlayer = PicInRender + PicInDecoder;
		return IVS_SUCCEED;
	}
    return IVS_FAIL;
#endif 	
    return IVS_SUCCEED;	
}

/*
 * StepBackward
 * 渲染模块播放一帧
 * 倒放单步回退，不对解码器缓存作控制
 */
int CMediaChannel::StepBackward(unsigned int &PicInPlayer)
{
#if 0
	IVS_ASSERT(m_handleDecoder);
	IVS_ASSERT(m_handleRender);

	if (m_handleReassemble)
	{
		Reass_SetPlayBackMode(m_handleReassemble, TRUE);
	}

	if (m_pSource)
	{
		if (!m_bBackPlayStep)
		{
			unsigned long long tick = 0;
			//(void)Render_GetCurrentFrameTick(m_handleRender, &tick);
			m_pSource->SetPlayTime((unsigned int)(tick / 1000));
		}

    	m_pSource->SetPlaySpeed(-1.0);
		m_pSource->Resume();
	}
	
	if (m_handleDecoder && m_handleRender)
	{
		/*
		 * 播放方向发生改变，清除End标记
		 */
		if (!m_bBackPlayStep)
		{
			Decoder_MarkStreamEnd(m_handleDecoder, FALSE);
			m_bBackPlayStep = true;
		}
		
		Decoder_Start(m_handleDecoder);
		unsigned int PicInRender  = 0;
		//Render_StepBackward(m_handleRender, &PicInRender);
		PicInPlayer = PicInRender;
		return IVS_SUCCEED;
	}

	return IVS_SUCCEED;
#endif 
    return IVS_SUCCEED;
}//lint !e1762 !e1764


//设置音量
int CMediaChannel::SetVolume(unsigned int uiVolumeValue)
{
    int iRet = IVS_FAIL;
    if (NULL != m_handleRender)
    {
        //iRet = Render_SetAudioVolume(m_handleRender, uiVolumeValue);
    }
    return iRet;
}

//获取音量
int CMediaChannel::GetVolume(unsigned int* puiVolumeValue)
{
    int iRet = IVS_FAIL;
    if (NULL != m_handleRender)
    {
        //iRet = Render_GetAudioVolume(m_handleRender, puiVolumeValue);
    }
    return iRet;
}

// 设置裸码流回调;
int CMediaChannel::SetRawFrameCallBack(REASSM_CALLBACK cbReassmCallBack, void *pUser)
{
    if (NULL == cbReassmCallBack)
    {
        return IVS_PARA_INVALID;
    }

    m_ReassmCBMutex.Lock();
    m_cbReassmCB = cbReassmCallBack;
    m_pReassmUserData = pUser;
    m_ReassmCBMutex.UnLock();
    return IVS_SUCCEED;
}

// 设置帧数据回调;
int CMediaChannel::SetFrameCallBack(DECODER_CALLBACK cbDecoderCB, void *pUser)
{
    if (NULL == cbDecoderCB)
    {
        return IVS_PARA_INVALID;
    }

    m_DecoderCBMutex.Lock();
    m_cbDecoderCB = cbDecoderCB;
    m_pDecoderUserData = pUser;
    m_DecoderCBMutex.UnLock();

    // 设置了解码后数据回调，需要将数据送解码模块;
    m_bNeedInputData2DecoderFlag = true;
    return IVS_SUCCEED;
}

#ifdef WIN32
// 设置播放窗口;
int CMediaChannel::SetPlayWindow(HWND hWnd)
{
    if (NULL == hWnd)
    {
        return IVS_PARA_INVALID;
    }

    int iRet = IVS_FAIL;//Render_SetPlayWindow(m_handleRender, hWnd);
    if (IVS_SUCCEED == iRet)
    {
        // 设置了解码后数据回调，需要将数据送解码模块;
        m_bNeedInputData2DecoderFlag = true;
        m_bNeedInputData2RenderFlag = true;
    }
    return iRet;
}

// 增加播放窗口;
int CMediaChannel::AddPlayWindow(HWND hWnd, int nType, void *pLayout)
{
    if (NULL == hWnd)
    {
        return IVS_PARA_INVALID;
    }

    int iRet = Render_AddPlayWindow(m_handleRender, hWnd, nType, pLayout);
    if (IVS_SUCCEED == iRet)
    {
        // 设置了解码后数据回调，需要将数据送解码模块;
        m_bNeedInputData2DecoderFlag = true;
        m_bNeedInputData2RenderFlag = true;
    }
    return iRet;
}

// 更新播放窗口布局
int CMediaChannel::UpdateWindowLayout(HWND hWnd, int nType, void *pLayout)
{
	if (NULL == hWnd)
	{
		return IVS_PARA_INVALID;
	}

	if (NULL != m_handleRender)
	{
		return Render_UpdateWindowLayout(m_handleRender, hWnd, nType, pLayout);
	}
	return IVS_FAIL;
}


// 删除播放窗口;
int CMediaChannel::DelPlayWindow(HWND hWnd)
{
    int iRet = Render_DelPlayWindow(m_handleRender, hWnd);
    return iRet;
}
#endif
// 设置窗口播放比例
int CMediaChannel::SetDisplayAspectRatio(unsigned int uiScale)
{
    int iFS = (DISPLAY_SCALE_FULL == uiScale)? 1 : 0;
	return IVS_SUCCEED;//Render_SetAspectRatio(m_handleRender, iFS, 0, 0);
}


int CMediaChannel::InputRtpData(char *pszBuf, unsigned int uiSize)
{
	/*
	 *  语音 广播/ 对讲模式，直接创建组帧模块
	 *  add by w00210470
	 */
	
	if (!m_bStreamTypeFlay)
	{
		if (SERVICE_TYPE_AUDIO_BROADCAST == m_enServiceMode
			|| SERVICE_TYPE_AUDIO_CALL == m_enServiceMode)
		{
			SetReassembleHandl();
		}
	}

	if (!m_bStreamTypeFlay)
	{
		return IVS_SUCCEED;
	}

    //如果存在编码格式切换,则先清除掉原先的拼帧句柄，再创建新的句柄，再去input数据;
    if (NULL != m_handleReassemble)
    {
        if (m_bIsVideoDecoderTypeChange)
        {
            IVS_LOG(IVS_LOG_INFO, "InputRtpData", " VideoType is Change, Current VideoType[%d]", (int)m_enVideoDecoderType);
            int iRes = Reass_StopVideoDecryptData(m_handleReassemble);
            if (IVS_SUCCEED != iRes)
            {
                IVS_LOG(IVS_LOG_ERR, "InputRtpData", "StopVideoDecryptData failed, RetValue[%d]", iRes);
            }

            iRes = Reass_ReSetReassemble(m_handleReassemble);
            if (IVS_SUCCEED != iRes)
            {
                IVS_LOG(IVS_LOG_ERR, "InputRtpData", "ReSetReassemble failed, RetValue[%d]", iRes);
            }

            Reass_FreeHandle(m_handleReassemble);
            m_handleReassemble = NULL;

            m_bIsVideoDecoderTypeChange = false;

            /* 创建组帧模块 */
            IVS_HANDLE h = Reass_GetHandle(m_enVideoDecoderType);
            if (NULL == h)
            {
                IVS_LOG(IVS_LOG_ERR, "InputRtpData", "Get Reassemble module fail");
                return IVS_FAIL;
            }

            if (m_dSpeed < 0)
            {
                Reass_SetPlayBackMode(h, TRUE);
            }

            (void)Reass_SetMediaAttr(h, &m_VideoAttr, &m_AudioAttr);
            (void)Reass_SetExceptionCallBack(h, m_cbExceptionCB, m_pExceptionUserData);
            (void)Reass_SetCompleteCallback(h, ReassmCallBack, this);
            (void)Reass_SetDecryptDataKeyAndMode(h,m_pSecretKey, m_ulKeyLen, m_iDecType);

            m_handleReassemble = h;
            m_bStreamTypeFlay  = true;
        }
    }
    else
    {
        /* 创建组帧模块 */
        IVS_HANDLE h = Reass_GetHandle(m_enVideoDecoderType);
        if (NULL == h)
        {
            IVS_LOG(IVS_LOG_ERR, "InputRtpData", "Get Reassemble module fail");
            return IVS_FAIL;
        }

        if (m_dSpeed < 0)
        {
            Reass_SetPlayBackMode(h, TRUE);
        }

        (void)Reass_SetMediaAttr(h, &m_VideoAttr, &m_AudioAttr);
        (void)Reass_SetExceptionCallBack(h, m_cbExceptionCB, m_pExceptionUserData);
        (void)Reass_SetCompleteCallback(h, ReassmCallBack, this);
        (void)Reass_SetDecryptDataKeyAndMode(h,m_pSecretKey, m_ulKeyLen, m_iDecType);

        m_handleReassemble = h;
        m_bStreamTypeFlay  = true;
    }
	
	int iRet = Reass_InsertPacket(m_handleReassemble, pszBuf, uiSize);

	if (m_handleDecoder && NULL == m_cbReassmCB)
	{
        unsigned int framesInDec = 0;
	    Decoder_GetSourceBufferRemain(m_handleDecoder, framesInDec);

	    if (framesInDec >= 80)
	    {
		    return IVS_PLAYER_RET_DECODER_BUFFER_FULL;
	    }
    }
	return iRet;
}
   

void CMediaChannel::PreDecodeRtp(char *pszBuf, unsigned int uiSize)
{
	if (NULL == pszBuf)
	{
		return ;
	}
	unsigned int uiHeadSize = sizeof(FIXED_HEADER);
	if (uiHeadSize > uiSize)
	{
		IVS_LOG(IVS_LOG_ERR,"PreDecodeRtp","Rtp Packet too short");
		return ;
	}

    if (SERVICE_TYPE_AUDIO_BROADCAST == m_enServiceMode
        || SERVICE_TYPE_AUDIO_CALL == m_enServiceMode)
    {
        SetReassembleHandl();
        return;
    }

	FIXED_HEADER* pRtpHdr = (FIXED_HEADER*)pszBuf; //lint !e826

	if (PAY_LOAD_TYPE_MJPEG == pRtpHdr->payload || 24 == pRtpHdr->payload)
	{
		m_enVideoDecoderType = VIDEO_DEC_MJPEG;
		SetReassembleHandl();
	}
	else
	{
		m_enVideoDecoderType = VIDEO_DEC_H264;
		SetReassembleHandl();
	}
}

void CMediaChannel::SetReassembleHandl()
{
	// rtp包方式需要创建拼帧模块;
	if (SOURCE_RTP == m_enSourceType)
	{
		// 根据视频解码类型获取拼帧句柄;
		m_handleReassemble = Reass_GetHandle(m_enVideoDecoderType);
		if (NULL == m_handleReassemble)
		{
			return ;
		}

		if (m_dSpeed < 0)
		{
			Reass_SetPlayBackMode(m_handleReassemble, TRUE);
		}
		
		// 设置异常回调函数;
		Reass_SetExceptionCallBack(m_handleReassemble, m_cbExceptionCB, m_pExceptionUserData);
		// 设置拼帧回调;
		Reass_SetCompleteCallback(m_handleReassemble, ReassmCallBack, this);
		//如果密钥存在的话，设置密钥
		(void)Reass_SetDecryptDataKeyAndMode(m_handleReassemble, m_pSecretKey, m_ulKeyLen, m_iDecType);
		m_bStreamTypeFlay = true;
	}
}

#ifdef WIN32
/*
 * OpenFile
 * 打开文件
 */
int CMediaChannel::OpenFile(const char *pFileName, PLAYER_EXCEPTION_CALLBACK Callback, void *pUser)
{
    if (NULL == pFileName || '\0' == pFileName[0])
    {
        IVS_LOG(IVS_LOG_ERR, "Player Open File", "FileName Invalid");
        return IVS_PARA_INVALID;
    }
    
	if (_access(pFileName, 0))
	{
		IVS_LOG(IVS_LOG_ERR, "Player Open File", "file %s access error", pFileName);
		return IVS_PLAYER_RET_FILE_ACCESS_ERROR;
	}

	FILE_TYPE FileType = FILE_TYPE_MP4;
	try 
	{
        switch (FileType)
	    {
	    case FILE_TYPE_AVI:
		    m_pSource = new CAviFileSource;
		    break;

	    case FILE_TYPE_MP4:
	    default:
		    m_pSource = new CMp4FileSource;
       	    break;
	   }//lint !e525
    }
	catch (...)
	{
		IVS_LOG(IVS_LOG_ERR, "Open File Create Source", "Out of Mem");
		return IVS_FAIL;
    }

    if (IVS_SUCCEED != m_pSource->Init(pFileName))
	{
		delete(m_pSource);
		m_pSource = NULL;
		return IVS_FAIL;
	}

	m_pSource->SetFrameDataCallBack(FileSourceCallBack, (this));
	m_pSource->SetExceptionCallBack(Callback, pUser);

    //m_handleRender = Render_GetHandle();
	//Render_SetExceptionCallback(m_handleRender, Callback, pUser);

	m_handleDecoder = Decoder_GetHandle();
	VIDEO_PARAM stVideoParam;
    memset(&stVideoParam, 0, sizeof(stVideoParam));
    stVideoParam.VideoFormat = m_enVideoDecoderType;
    stVideoParam.HardwareDec = FALSE;

    AUDIO_PARAM stAudioParam;
    memset(&stAudioParam, 0, sizeof(stAudioParam));
    stAudioParam.AudioFormat = AUDIO_DEC_G711U;
    stAudioParam.BitRate     = 128000;
    stAudioParam.Channels    = 1;
    stAudioParam.SampleRate  = 8000;

	Decoder_OpenStream(m_handleDecoder, &stVideoParam, &stAudioParam, 0, 100, 10, 0);
    Decoder_SetExceptionCallback(m_handleDecoder, Callback, pUser);
    Decoder_SetDecoderCB(m_handleDecoder, DecoderCallBack, this);
	//Render_SetStreamMode(m_handleRender, STREAM_LOCALFILE);
	//Render_SetPlayDelay(m_handleRender, 50);
	//Render_SetPlayEndCallBack(m_handleRender, PlayEndCallBack, this);
		
    m_cbExceptionCB      = Callback;
	m_pExceptionUserData = pUser;

	m_enSourceType  = SOURCE_FILE;
	m_enServiceMode = SERVICE_TYPE_INVALID;

#if 0
	ShowPlayStatis(true);
#endif 
	return IVS_SUCCEED;
}
#endif
/*
 * ProcessPlayEnd
 * 处理播放结束
 */
int CMediaChannel::ProcessPlayEnd()
{
	if (!m_cbExceptionCB)
	{
        return IVS_FAIL;
	}
		
	if (m_pSource)
	{
		m_cbExceptionCB(0, IVS_EVENT_LOCAL_PLAYBACK_FAILED, NULL, m_pExceptionUserData);
	}

	return IVS_SUCCEED;
}

#ifdef WIN32
/*
 * ProcessFileFrame
 * 处理文件source数据
 */
int CMediaChannel::ProcessFileFrame(MEDIA_FRAME_INFO *pInfo)
{
	IVS_ASSERT(m_handleDecoder && m_handleRender);
	if (NULL == m_handleDecoder || NULL == m_handleRender ||
		NULL == m_pSource || NULL == pInfo)
	{
		return IVS_FAIL;
	}

	if (pInfo->bEOS)
	{
		//(void)Decoder_SendVideoFrame(m_handleDecoder, NULL, NULL, 0);
		(void)Decoder_MarkStreamEnd(m_handleDecoder, TRUE);
		return IVS_SUCCEED;
	}

    if (pInfo->bVideo)
	{
		IVS_RAW_FRAME_INFO RawInfo;
		memset(&RawInfo, 0, sizeof(RawInfo));
		RawInfo.uiFrameType  = pInfo->Vinfo.uFrameType;
		RawInfo.uiStreamType = pInfo->Vinfo.uPayloadType;
		RawInfo.uiHeight     = pInfo->Vinfo.uHeight;
		RawInfo.uiWidth      = pInfo->Vinfo.uWidth;
		RawInfo.ullTimeTick  = pInfo->Vinfo.ullTimeTick;
		RawInfo.uiTimeStamp  = pInfo->Vinfo.uTimeStamp;
		RawInfo.uiGopSequence = pInfo->Vinfo.uGopSequence;
		RawInfo.uiWaterMarkValue = 0;

		int iRet = Decoder_SendVideoFrame(m_handleDecoder, &RawInfo, pInfo->Vinfo.pBuffer, pInfo->Vinfo.uSize);
	    if (IVS_PLAYER_RET_DECODER_BUFFER_FULL == iRet)
		{
			while (m_pSource->IsRuning() && IVS_PLAYER_RET_DECODER_BUFFER_FULL == iRet)
			{
#ifdef WIN32
				Sleep(10);
#else
				VOS_Sleep(10);
#endif
			    iRet = Decoder_SendVideoFrame(m_handleDecoder, &RawInfo, pInfo->Vinfo.pBuffer, pInfo->Vinfo.uSize);
			}
		}
	}
	else 
	{
		IVS_AUDIO_FRAME  AudioFrame;
		memset(&AudioFrame, 0, sizeof(AudioFrame));
		AudioFrame.pFrame        = pInfo->Ainfo.pBuffer;
		AudioFrame.uiFrameSize   = pInfo->Ainfo.uSize;
		AudioFrame.uiAudioFormat = (unsigned int)pInfo->Ainfo.uPayloadType;
		AudioFrame.uiBits        = pInfo->Ainfo.uBits;
		AudioFrame.uiChannels    = pInfo->Ainfo.uChannels;
		AudioFrame.uiSampleRate  = pInfo->Ainfo.ulSR;
		AudioFrame.uiTimeStamp   = pInfo->Ainfo.uTimeStamp;
		AudioFrame.ullTimeTick   = pInfo->Ainfo.ullTimeTick;

		int iRet = Render_InputAudioData(m_handleRender, &AudioFrame);
		if (IVS_SUCCEED != iRet)
		{
			while (m_pSource->IsRuning() && IVS_SUCCEED != iRet)
			{
#ifdef WIN32
				Sleep(50);
#else
				VOS_Sleep(50);
#endif
				iRet = Render_InputAudioData(m_handleRender, &AudioFrame);
			}
		}
	}
	return IVS_SUCCEED;
}//lint !e818
#endif

int CMediaChannel::ProcessRawFrame(IVS_RAW_FRAME_INFO* pstRawFrameInfo, char* pszFrame, unsigned int uiFrameSize)
{
	if (NULL == m_handleDecoder)
	{
		//return IVS_SUCCEED;
	}
	
	if (NULL == pstRawFrameInfo)
    {
        IVS_LOG(IVS_LOG_ERR, "ProcessRawFrame", "Input param error, pstRawFrameInfo = NULL");
        return IVS_PARA_INVALID;
    }

    // 播发裸码流数据;
    m_ReassmCBMutex.Lock();
    if (NULL != m_cbReassmCB)
    {
        m_cbReassmCB(pstRawFrameInfo, pszFrame, uiFrameSize, NULL, m_pReassmUserData);
        m_ReassmCBMutex.UnLock();
        return IVS_SUCCEED;
    }
    m_ReassmCBMutex.UnLock();

#if 0
    int iRet = IVS_FAIL;
    IVS_PAYLOAD_TYPE enStreamType = (IVS_PAYLOAD_TYPE)pstRawFrameInfo->uiStreamType;
	if (PAY_LOAD_TYPE_H264 == enStreamType || 
		PAY_LOAD_TYPE_MJPEG == enStreamType || 
		PAY_LOAD_TYPE_H265 == enStreamType || 
		PAY_LOAD_TYPE_SVAC == enStreamType)
    {
        iRet = Decoder_SendVideoFrame(m_handleDecoder, pstRawFrameInfo, pszFrame, uiFrameSize);

        // 如果缓冲区满，等待10ms后重试一次 [1/21/2013 z90003203]
        if (IVS_PLAYER_RET_DECODER_BUFFER_FULL == iRet)
        {
#ifdef WIN32
            Sleep(10);
#else
			VOS_Sleep(10);
#endif
            iRet = Decoder_SendVideoFrame(m_handleDecoder, pstRawFrameInfo, pszFrame, uiFrameSize);
        }
    }
    else
    {
		// 服务为回放类型的，只在1倍速下播放声音，其它倍速不播放;
		if (0.1 < abs(m_dSpeed - 1.0) && 
			(SERVICE_TYPE_PLAYBACK == m_enServiceMode 
			|| SERVICE_TYPE_PU_PLAYBACK == m_enServiceMode
			|| SERVICE_TYPE_BACKUPRECORD_PLAYBACK == m_enServiceMode
			|| SERVICE_TYPE_DISASTEBACKUP_PLAYBACK == m_enServiceMode))
		{
			IVS_LOG(IVS_LOG_DEBUG, "CMediaChannel::ProcessRawFrame",
					"ServiceType[%d] Speed is[%f], do not send audio frame to decoder.", m_enServiceMode, m_dSpeed);
			return IVS_SUCCEED;
		}

		// 音频数据可以直接送渲染;
		if (PAY_LOAD_TYPE_PCMU == enStreamType || PAY_LOAD_TYPE_PCMA == enStreamType)
		{
			iRet = ProcessRawAudioFrame(pstRawFrameInfo, pszFrame, uiFrameSize);
		}
		else
		{
			iRet = Decoder_SendAudioFrame(m_handleDecoder, pstRawFrameInfo, pszFrame, uiFrameSize);

			// 如果缓冲区满，等待10ms后重试一次 [1/21/2013 z90003203]
			if (IVS_PLAYER_RET_DECODER_BUFFER_FULL == iRet)
			{
#ifdef WIN32
				Sleep(10);
#else
				VOS_Sleep(10);
#endif
				iRet = Decoder_SendAudioFrame(m_handleDecoder, pstRawFrameInfo, pszFrame, uiFrameSize);
			}
		}

    }


    return iRet;
#endif 

    return IVS_SUCCEED;

}

int CMediaChannel::ProcessRawAudioFrame(IVS_RAW_FRAME_INFO* pstRawFrameInfo,
                                        char* pszFrame,
                                        unsigned int uiFrameSize)
{
    if (NULL == m_handleRender)
    {
        return IVS_SUCCEED;
    }

    if (NULL == pstRawFrameInfo)
    {
        IVS_LOG(IVS_LOG_ERR, "ProcessRawFrame", "Input param error, pstRawFrameInfo = NULL");
        return IVS_PARA_INVALID;
    }

    int iRet = IVS_FAIL;
    IVS_PAYLOAD_TYPE enStreamType = (IVS_PAYLOAD_TYPE)pstRawFrameInfo->uiStreamType;
    if (PAY_LOAD_TYPE_PCM == enStreamType
        || PAY_LOAD_TYPE_PCMU == enStreamType
        || PAY_LOAD_TYPE_PCMA == enStreamType)
    {
        IVS_AUDIO_FRAME stAudioFrame;
        memset(&stAudioFrame, 0, sizeof(stAudioFrame));

        stAudioFrame.pFrame = (IVS_UCHAR*)pszFrame;
        stAudioFrame.uiFrameSize = uiFrameSize;
        stAudioFrame.uiSampleRate = 8000;
        stAudioFrame.uiChannels = 1;
        stAudioFrame.uiBits = 8;
        stAudioFrame.uiTimeStamp = pstRawFrameInfo->uiTimeStamp;
        stAudioFrame.ullTimeTick = pstRawFrameInfo->ullTimeTick;
        stAudioFrame.uiAudioFormat = pstRawFrameInfo->uiStreamType;

        //iRet = Render_InputAudioData(m_handleRender, &stAudioFrame);
    }
    else
    {
        // 不支持
        IVS_LOG(IVS_LOG_ERR, "Process Raw Frame",
            "Unsupport audio format %d.", pstRawFrameInfo->uiStreamType);
    }

    return iRet;
}//lint !e818

// 处理解码后的帧数据;
int CMediaChannel::ProcessFrame(int iStreamType, void* pFrameInfo)
{
	// 播发裸码流数据;
	m_DecoderCBMutex.Lock();
	if (NULL != m_cbDecoderCB)
	{
		m_cbDecoderCB(iStreamType, pFrameInfo, m_pDecoderUserData);
		m_DecoderCBMutex.UnLock();
		return IVS_SUCCEED;
	}
	m_DecoderCBMutex.UnLock();

	int iRet = IVS_FAIL;
    if (MEDIA_STREAM_VIDEO == iStreamType)
    {
        IVS_VIDEO_FRAME* pstVideoFrame = static_cast<IVS_VIDEO_FRAME*>(pFrameInfo);
        if (NULL != pstVideoFrame)
        {
            //iRet = Render_InputVideoData(m_handleRender, pstVideoFrame);

			// 如果插入数据到渲染模块失败，sleep一把解码线程，暂停解码
#if 0    
			if (IVS_FAIL == iRet)
            {
                IVS_LOG(IVS_LOG_DEBUG, "CMediaChannel::ProcessFrame",
                        "Input video data to render buffer failed, sleep decoder thread.");
                Sleep(1000);
            }
#endif 
			while (IVS_FAIL == iRet && 
				   !Decoder_IsCancel(m_handleDecoder) && 
				   SERVICE_TYPE_REALVIDEO != m_enServiceMode)
			{
#ifdef WIN32
				Sleep(50);
#else
				VOS_Sleep(50);
#endif
				//iRet = Render_InputVideoData(m_handleRender, pstVideoFrame);
     		}
        }
        else
        {
			IVS_LOG(IVS_LOG_DEBUG, "CMediaChannel::ProcessFrame", "End of Stream");
			//(void)Render_InputVideoData(m_handleRender, NULL);
        }
    }
    else if(MEDIA_STREAM_AUDIO == iStreamType)
    {
        IVS_AUDIO_FRAME* pstAudioFrame = static_cast<IVS_AUDIO_FRAME*>(pFrameInfo);
        if (NULL != pstAudioFrame)
        {
            //iRet = Render_InputAudioData(m_handleRender, pstAudioFrame);
        }
        else
        {
            IVS_LOG(IVS_LOG_DEBUG, "CMediaChannel::ProcessFrame",
                    "Call back param error, pFrameInfo cannot transform to AUDIO_FRAME");
        }
    }
    else
    {
        IVS_LOG(IVS_LOG_ERR, "CMediaChannel::ProcessFrame",
                "StreamType error, iStreamType=%d", iStreamType);
    }


    return iRet;
}
//获取视频质量参数
int CMediaChannel::GetMediaInfo(IVS_MEDIA_INFO* pstMediaInfo)
{
	if (NULL == pstMediaInfo)
	{
		return IVS_FAIL;
	}
	if ((NULL == m_handleReassemble && NULL == m_pSource) ||
		 NULL == m_handleRender)
	{
		return IVS_FAIL;
	}
	//创建从渲染获取视频参数的对象
	MEDIA_INFO* mediaInfo = IVS_NEW(mediaInfo);

	if (NULL == mediaInfo)
	{
		return IVS_FAIL;
	}

	if (IVS_SUCCEED != IVS_FAIL)//Render_GetMediaInfo(m_handleRender, mediaInfo))
    {
        IVS_DELETE(mediaInfo);
        return IVS_FAIL;
    }
	pstMediaInfo->uiVideoCodec = m_enVideoDecoderType;
	pstMediaInfo->uiAudioStatus =(unsigned int) mediaInfo->AudioStatus;
	pstMediaInfo->uiVideoHeight = mediaInfo->Height;
	pstMediaInfo->uiVideoWidth = mediaInfo->Width;
	
    if (NULL != m_handleReassemble)
	{
		int iRet = IVS_FAIL;
		iRet = Reass_GetVideoCodeRate(m_handleReassemble, &pstMediaInfo->uiVideoBitrate,
			&pstMediaInfo->uiVideoAverageBitrate);
		iRet += Reass_GetAudioCodeRate(m_handleReassemble, &pstMediaInfo->uiAudioBitRate,
			&pstMediaInfo->uiAudioAverageBitRate);
		iRet += Reass_GetLostPacketRate(m_handleReassemble, &pstMediaInfo->fLossTolerance);
		iRet += Reass_GetFPS(m_handleReassemble,&pstMediaInfo->uiVideoFrameRate);
		if(IVS_SUCCEED != iRet)
		{
			IVS_DELETE(mediaInfo);
			return IVS_FAIL;
		}
	}
	else if (NULL != m_pSource)
	{
		unsigned int w = 0, h = 0;
        m_pSource->GetMediaInfo(pstMediaInfo->uiVideoFrameRate, w, h);
	}

	pstMediaInfo->uiVideoBitrate /= 1000;
	pstMediaInfo->uiVideoAverageBitrate /= 1000;
	pstMediaInfo->uiAudioBitRate /= 1000;
	pstMediaInfo->uiAudioAverageBitRate /= 1000;

    IVS_DELETE(mediaInfo);
	return	IVS_SUCCEED;
}

//设置开启/停止校验水印
int CMediaChannel::SetWaterMarkState(BOOL bIsStart)
{
    IVS_LOG(IVS_LOG_DEBUG, "","SetWaterMarkState bIsStart is :%d", bIsStart);
    if ((NULL == m_handleDecoder) || (NULL == m_handleReassemble))
    {
        return IVS_FAIL;
    }

    int iRet = Reass_SetWaterMarkState(m_handleReassemble, bIsStart);
    iRet += Decoder_SetWaterMarkState(m_handleDecoder, bIsStart);
    if (IVS_SUCCEED != iRet)
    {
        iRet = IVS_FAIL;
    }
    return iRet;
}

int CMediaChannel::GetPlayedTime(unsigned long long *pTime)
{
    if (NULL == m_handleRender)
    {
        return IVS_FAIL;
    }
    return IVS_FAIL;//Render_GetPlayedTime(m_handleRender, pTime);
}

int CMediaChannel::GetCurrentFrameTick(unsigned long long *pTick)
{
	if (NULL == m_handleRender)
    {
        return IVS_FAIL;
    }
    return IVS_FAIL;//Render_GetCurrentFrameTick(m_handleRender, pTick);
}

int CMediaChannel::SetPlayTime(time_t tm)
{
	if (m_pSource)
	{
		m_pSource->SetPlayTime((unsigned int)tm);
	}

	if (NULL == m_handleRender)
	{
		return IVS_FAIL;
	}
	return IVS_FAIL;//Render_SetPlayTime(m_handleRender, tm);
}

int CMediaChannel::SetDownLoadTimeSpan(time_t beg, time_t end, bool bRepFin)
{
	IVS_ASSERT(m_enServiceMode == SERVICE_TYPE_DOWNLOAD);
	if (SERVICE_TYPE_DOWNLOAD != m_enServiceMode)
	{
		return IVS_FAIL;
	}
	m_tmDownLoadBeg = beg;
	m_tmDownLoadEnd = end;
	m_bRepFinish    = bRepFin;
	return IVS_SUCCEED;
}

int CMediaChannel::SetPlayQualityMode(int iPlayQualityMode)
{
	if (NULL == m_handleRender || NULL == m_handleReassemble)
	{
		return IVS_FAIL;
	}

	int iRet = IVS_FAIL;
	if (PLAY_QUALITY_DEFINITION == iPlayQualityMode)
	{
		iRet = Reass_SetRawFrameLimit(m_handleReassemble,UPPER_FRAME);
		//iRet += Render_SetPlayDelay(m_handleRender,UPPER_TIME);
	}
	if (PLAY_QUALITY_FLUENCY == iPlayQualityMode)
	{
		iRet = Reass_SetRawFrameLimit(m_handleReassemble,LOWER_FRAME);
		//iRet += Render_SetPlayDelay(m_handleRender,LOWER_TIME);
	}

    if (IVS_SUCCEED != iRet)
    {
        iRet = IVS_FAIL;
    }
	return iRet;
}
#ifdef WIN32
void __SDK_CALLBACK CMediaChannel::DrawStatsCallBack(HDC hDc, RECT *rc, IVS_UINT64 FrameID, IVS_VOID *pUser)
{
	CMediaChannel *pChannel = (CMediaChannel *)pUser;
	IVS_ASSERT(NULL != pChannel);
	if (NULL == pChannel)
	{
		return;
	}

	TEXTMETRIC tm;
	memset(&tm, 0, sizeof(tm));
	(void)GetTextMetrics(hDc, &tm);
	int iHeight = tm.tmHeight + tm.tmExternalLeading;

	int oldmode = SetBkMode(hDc, TRANSPARENT);
	COLORREF oldcolor = SetTextColor(hDc, RGB(255, 0, 0));

	char buf[256] = {0};
	int  textlen = 0;
	int  yPos = 0;

	// 输出拼帧模块统计参数
	CODE_RATE_INFO  ReassInfo;
	memset(&ReassInfo, 0, sizeof(ReassInfo));
	Reass_GetStatisInfo(pChannel->m_handleReassemble, &ReassInfo);
	float lostPacket = 0.0;
	Reass_GetLostPacketRate(pChannel->m_handleReassemble,&lostPacket);
	
	textlen = _snprintf_s(buf, 255, "Audio:%ubps lostPacket:%f", 
				ReassInfo.uAudioCodeRate, lostPacket); //lint !e838
    textlen = (textlen < 0) ? 0: textlen;
	TextOut(hDc, 0, (yPos += iHeight), buf, textlen);

	textlen = _snprintf_s(buf, 255, "Video:%ubps Avg:%ubps", 
				ReassInfo.uVideoCodeRate, ReassInfo.uAvgVideoCodeRate);
    textlen = (textlen < 0) ? 0: textlen;
	TextOut(hDc, 0, (yPos += iHeight), buf, textlen);

    textlen = _snprintf_s(buf, 255, "FrameList:%lu MemBlock:%lu", 
				ReassInfo.uFrameListSize, ReassInfo.uUsedMemBlockNum);
    textlen = (textlen < 0) ? 0: textlen;
	TextOut(hDc, 0, (yPos += iHeight), buf, textlen);

    // 输出渲染模块统计参数	
	RENDER_STATIS RenderInfo;
	memset(&RenderInfo, 0, sizeof(RENDER_STATIS));
	//Render_GetStatisInfo(pChannel->m_handleRender, &RenderInfo);

	textlen = _snprintf_s(buf, 255, "Input:%lu Drop:%lu Rate:%lu", 
		       RenderInfo.InFrames, RenderInfo.DropFrames, RenderInfo.InFrameRate);

    textlen = (textlen < 0) ? 0: textlen;
	TextOut(hDc, 0, (yPos += iHeight), buf, textlen);

	textlen = _snprintf_s(buf, 255, "OutPut:%lu Skip:%lu Rate:%.3f", 
				RenderInfo.OutFrames, RenderInfo.SkipFrames, RenderInfo.OutFrameRate);

    textlen = (textlen < 0) ? 0: textlen;
	TextOut(hDc, 0, (yPos += iHeight), buf, textlen);

	textlen = _snprintf_s(buf, 255, "VQueueSize:%lu AQueueSize:%lu", 
				RenderInfo.VListSz, RenderInfo.AListSz);

    textlen = (textlen < 0) ? 0: textlen;
	TextOut(hDc, 0, (yPos += iHeight), buf, textlen);

	textlen = _snprintf_s(buf, 255, "TimeTick:%lu", RenderInfo.uLastRtpStamp);

    textlen = (textlen < 0) ? 0: textlen;
	TextOut(hDc, 0, (yPos += iHeight), buf, textlen);

	textlen = _snprintf_s(buf, 255, "PlayedTime:%llu", RenderInfo.PlayedTime);

    textlen = (textlen < 0) ? 0: textlen;
	TextOut(hDc, 0, (yPos += iHeight), buf, textlen);

	textlen = _snprintf_s(buf, 255, "FrameTick:%llu", RenderInfo.FrameTick);

    textlen = (textlen < 0) ? 0: textlen;
	TextOut(hDc, 0, (yPos += iHeight), buf, textlen);

	unsigned int  FramesInDecoder = 0;
	Decoder_GetSourceBufferRemain(pChannel->m_handleDecoder, FramesInDecoder);
	
	textlen = _snprintf_s(buf, 255, "FramesInDec:%u", FramesInDecoder);

    textlen = (textlen < 0) ? 0: textlen;
	TextOut(hDc, 0, (yPos += iHeight), buf, textlen);

	SetTextColor(hDc, oldcolor);
	SetBkMode(hDc, oldmode);
}//lint !e715 !e818 !e954 无须const

int CMediaChannel::ShowPlayStatis(bool enable)
{
	if (NULL == m_handleRender)
	{
		return IVS_FAIL;
	}
		
	if (enable)
	{
		if (!m_bShowStatis)
		{
			//Render_SetDrawCallBack(m_handleRender, DrawStatsCallBack, this);
			m_bShowStatis = true;
		}
	}
	else 
	{
		if (m_bShowStatis)
		{
			//Render_SetDrawCallBack(m_handleRender, NULL, NULL);
			m_bShowStatis = false;
		}
	}
	return IVS_SUCCEED;
}
#endif
void CMediaChannel::SetResetFlag(bool bIsReset)
{
	Reass_SetResetFlag(m_handleReassemble, bIsReset);
	//(void)Render_SetResetFlag(m_handleRender, bIsReset);
}
void CMediaChannel::ResetData()

{
    (void)Reass_ReSetReassemble(m_handleReassemble);
    (void)Decoder_ResetBuffer(m_handleDecoder);
    //(void)Render_ResetBuffer(m_handleRender);
}

int CMediaChannel::StartRecord(const char* pszCamera, const char*pCamName, const char *pName, const IVS_LOCAL_RECORD_PARAM* pParm, 
	    PLAYER_EXCEPTION_CALLBACK fExceptionCallback, void* pUser, bool bAssociatedAudio)
{
    if (m_bRecordFlag)
	{
		return IVS_SUCCEED;
	}
	
	CAutoLock lock(m_RecorderMutex);
	//m_handleRecorder = Recorder_GetHandle();
	if (NULL == m_handleRecorder)
	{
        IVS_LOG(IVS_LOG_ERR, "Start Record", "Recorder_GetHandle Fail");
		return IVS_ALLOC_MEMORY_ERROR;
    }
	
	// 设置回调

	int result = IVS_FAIL;

	if (SERVICE_TYPE_DOWNLOAD == m_enServiceMode)
	{
		DOWNLOAD_PARAM DLParam = { m_tmDownLoadBeg, m_tmDownLoadEnd, m_bRepFinish };
		//result = Recorder_Init(m_handleRecorder, pszCamera, pCamName, pName, pParm, bAssociatedAudio, &DLParam);
	}
	else
	{
		//result = Recorder_Init(m_handleRecorder, pszCamera, pCamName, pName, pParm, bAssociatedAudio, NULL);
	}
	
	if (IVS_SUCCEED != result)
	{
		 IVS_LOG(IVS_LOG_ERR, "Start Record", "Recorder Init Fail Err:%d", result);
         //Recorder_FreeHandle(m_handleRecorder);
		 m_handleRecorder = NULL;
		 return result;
	}

	//Recorder_SetExceptionCallback(m_handleRecorder, fExceptionCallback, pUser);
	//result = Recorder_Start(m_handleRecorder);
	if (IVS_SUCCEED != result)
	{
	    IVS_LOG(IVS_LOG_ERR, "Start Record", "Recorder Start Fail Err:%d", result);
		//Recorder_FreeHandle(m_handleRecorder);
		m_handleRecorder = NULL;
		return result;
	}
	m_bRecordFlag = true;
	return IVS_SUCCEED;
}

void CMediaChannel::StopRecord()
{
	if (!m_bRecordFlag)
	{
		return;
	}
	m_bRecordFlag = false;
	CAutoLock lock(m_RecorderMutex);
	//Recorder_Stop(m_handleRecorder);
	//Recorder_FreeHandle(m_handleRecorder);
	m_handleRecorder = NULL;
}

int CMediaChannel::GetRecordStatis(unsigned int *pTime, unsigned int *pSpeed, IVS_UINT64 *pTotalSz)
{
	if (!m_bRecordFlag)
	{
		return IVS_FAIL;
	}
	
	CAutoLock lock(m_RecorderMutex);
	if (m_handleRecorder)
	{
        //Recorder_GetStatis(m_handleRecorder, pTime, pSpeed, pTotalSz);
	}
	return IVS_SUCCEED;
}

int  CMediaChannel::StopDecryptData()
{
	if (NULL != m_handleReassemble)
	{
		int iRet = Reass_StopVideoDecryptData(m_handleReassemble);
		if (IVS_SUCCEED != iRet)
		{
			IVS_LOG(IVS_LOG_ERR, "Stop Decrypt Data", "StopVideoDecryptData failed, RetValue[%d]", iRet);
		}

		iRet = Reass_ReSetReassemble(m_handleReassemble);
		if (IVS_SUCCEED != iRet)
		{
			IVS_LOG(IVS_LOG_ERR, "Stop Decrypt Data", "ReSetReassemble failed, RetValue[%d]", iRet);
		}

		Reass_FreeHandle(m_handleReassemble);
		m_handleReassemble = NULL;
		IVS_LOG(IVS_LOG_INFO, "Stop Decrypt Data", "Succeed.");
		return IVS_SUCCEED;
	}
	IVS_LOG(IVS_LOG_ERR, "Stop Decrypt Data", "Stop Decrypt Data Error");
	return IVS_FAIL;
}

int CMediaChannel::SetSecretKey(int iDecType, const char* pSecretKey)
{
    if (NO_NEED_ENCRYPTED == iDecType)
    {
        IVS_LOG(IVS_LOG_DEBUG, "Set SecretKey", "No Need Key, iDecType:%d", iDecType);
        //表示不需要加密
        m_iDecType = iDecType;
        m_pSecretKey = NULL;
        m_ulKeyLen = 0;
        if (NULL != m_handleReassemble)
        {
            (void)Reass_SetDecryptDataKeyAndMode(m_handleReassemble, m_pSecretKey, m_ulKeyLen, iDecType);
        }
        return IVS_SUCCEED;
    }
    else
    {
	    if (NULL == pSecretKey)
	    {
			IVS_LOG(IVS_LOG_ERR, "Media Channel SetSecretKey failed", "SecretKey is NULL.");
		    return IVS_FAIL;
	    }
	    //密钥长度
	    unsigned long ulKeyLen = 0;
	    switch(iDecType)
	    {
		    //XOR加密的时候固定密钥长度为16
	    case IPC_ENCRYPT_SIMPLE:
	    case IPC_ENCRYPT_SIMPLE_HEADER:
		    ulKeyLen = 16 + 1;//加一是为了和Aes256加密时候的密钥设置一致，后面会统一做减一处理
		    break;
	    case IPC_ENCRYPT_AES256:
	    case IPC_ENCRYPT_AES256_PARTIAL:
		    ulKeyLen = strlen(pSecretKey) + 1;
		    break;
	    default:
		    IVS_LOG(IVS_LOG_ERR, "Set SecretKey", "SecretKey Type Error: %d",iDecType);
		    return IVS_FAIL;
	    }
	    IVS_LOG(IVS_LOG_INFO, "Set SecretKey", "SecretKey Len: %d",ulKeyLen);

	    if (NULL != m_pSecretKey)
	    {
		    IVS_DELETE(m_pSecretKey,MUILI);
		    m_pSecretKey = NULL;
	    }
	    m_pSecretKey = IVS_NEW(m_pSecretKey,ulKeyLen);
	    if (NULL == m_pSecretKey)
	    {
			IVS_LOG(IVS_LOG_ERR, "Media Channel SetSecretKey failed", "NEW m_pSecretKey failed.");
		    return IVS_FAIL;
	    }
	    memset(m_pSecretKey,0,ulKeyLen);
	    m_ulKeyLen = ulKeyLen -1;
	    m_iDecType = iDecType;
	    memcpy(m_pSecretKey, pSecretKey, m_ulKeyLen);
	    if (NULL != m_handleReassemble)
	    {
		    (void)Reass_SetDecryptDataKeyAndMode(m_handleReassemble, m_pSecretKey, m_ulKeyLen, iDecType);
	    }
	    return IVS_SUCCEED;
    }
}

void CMediaChannel::InsertRecordFrame(const IVS_RAW_FRAME_INFO* pstRawFrameInfo, 
	                    const char* pFrame, unsigned int uiFrameSize)
{
	if (!m_bRecordFlag)
	{
		return;
	}
	CAutoLock lock(m_RecorderMutex);
	if (NULL != m_handleRecorder)
	{
		//if (IVS_SUCCEED == Recorder_InputFrame(m_handleRecorder, pFrame, uiFrameSize, 
		//	(int)pstRawFrameInfo->uiStreamType, pstRawFrameInfo->uiTimeStamp))
		{
			return;
		}
		
		// 下载模式
		if (SERVICE_TYPE_DOWNLOAD == m_enServiceMode)
		{
			while (1) //lint !e716
			{
#ifdef WIN32
				Sleep(200);
#else
				VOS_Sleep(200);
#endif
				int result = IVS_SUCCEED;/*Recorder_InputFrame(m_handleRecorder, pFrame, uiFrameSize, 
					         (int)pstRawFrameInfo->uiStreamType, pstRawFrameInfo->uiTimeStamp);*/
				if (IVS_SUCCEED == result)
				{
					break;
				}
			}
		}
	}
}
#ifdef WIN32
// 本地抓拍单张图片
int CMediaChannel::LocalSnapshot(const char *pSavePath, IVS_ULONG ulSnapshotFormat, const IVS_RECT *Rct)
{
	//IVS_RECT *Rct 可以为空
	if (NULL == pSavePath)
	{
		IVS_LOG(IVS_LOG_ERR, "Local Snapshot", "Save Path is NULL");
		return IVS_PARA_INVALID;
	}
	if (MAX_PATH_LENTH < strlen(pSavePath))
	{
		IVS_LOG(IVS_LOG_ERR, "Local Snapshot", "Save Path too Long");
		return IVS_DIR_ERROR;
	}
	if (NULL == m_handleRender)
	{
		IVS_LOG(IVS_LOG_ERR, "Local Snapshot", "RenderHandle is NULL");
		return IVS_FAIL;
	}
	unsigned long ulWidth  = 0;
	unsigned long ulHeight = 0;
	void *pBits = NULL;
	BYTE *pOutPutBits = NULL;
	char cFilePath[MAX_PATH_LENTH + 1] = {0};
	strncpy_s(cFilePath, sizeof(cFilePath), pSavePath, _TRUNCATE);

	// 检查文件名与磁盘空间
	int iRet = CCapture::CheckDirectoryAndSpace(cFilePath, ulSnapshotFormat);
	if (IVS_SUCCEED != iRet)
	{
		IVS_LOG(IVS_LOG_ERR, "Local Snapshot", "Check Directory And Space Failed");
		return iRet;
	}
	// 获取图像DIB32
	//iRet = Render_GetImageDIB32(m_handleRender, &ulWidth, &ulHeight, &pBits);
	if (IVS_SUCCEED != iRet)
	{
		IVS_LOG(IVS_LOG_ERR, "Local Snapshot", "Get Image DIB Failed");
		return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
	}
	if (NULL != Rct)// 局部放大抓拍处理
	{
		if (NULL == IVS_NEW(pOutPutBits, ulWidth * 4 * ulHeight))
		{
			IVS_LOG(IVS_LOG_ERR, "Local Snapshot", "No Enough Memory");
			//Render_PutImageDIB32(m_handleRender, pBits);
			return IVS_ALLOC_MEMORY_ERROR;
		}
		iRet = CCapture::StretchBitmap(ulWidth, ulHeight, *Rct, pBits, pOutPutBits);
		if (IVS_SUCCEED != iRet)
		{
			IVS_LOG(IVS_LOG_ERR, "Local Snapshot", "Stretch Bitmap Failed");
			IVS_DELETE(pOutPutBits, MUILI);
			//Render_PutImageDIB32(m_handleRender, pBits);
			return iRet;
		}
	} 
    else
    {
		pOutPutBits = (BYTE *)pBits;
	}
	
	// 生成图片文件
	switch (ulSnapshotFormat)
	{
	case CAPTURE_FORMAT_JPG: // JPG类型
		iRet = CCapture::CreateJPGFile(cFilePath, ulWidth, ulHeight, pOutPutBits);
		break;
	case CAPTURE_FORMAT_BMP: // BMP类型
		iRet = CCapture::CreateBMPFile(cFilePath, ulWidth, ulHeight, pOutPutBits);
		break;
	default:
		IVS_LOG(IVS_LOG_ERR, "Local Snapshot", "Snapshot Format Invalid");
		iRet = IVS_FILE_FORMAT_ERROR;
	}
	
	
	if (pOutPutBits != pBits)
	{
		IVS_DELETE(pOutPutBits);
	}
	//Render_PutImageDIB32(m_handleRender, pBits);

	return iRet;

}

int CMediaChannel::SetDrawCallBack(DRAW_CALLBACK  callback, void *pUser)
{
	return IVS_FAIL;//Render_SetDrawCallBack(m_handleRender, callback, pUser);
}

int CMediaChannel::SetDrawCallBackEx(HWND hWnd, DRAW_CALLBACK callback, void *pUser)
{
	return IVS_FAIL;//Render_SetDrawCallBackEx(m_handleRender, hWnd, callback, pUser);
}
#endif
void CMediaChannel::SetPlaySpeed(double speed)
{
	if (NULL != m_handleReassemble)
	{
		if (speed < 0)
		{
			Reass_SetPlayBackMode(m_handleReassemble, TRUE);
		}
		else
		{
			Reass_SetPlayBackMode(m_handleReassemble, FALSE);
			/*
			 * DTS2013060707094
			 * 倒放转正放 强制从I帧开始接收，否则花屏
			 */
			if (m_bBackPlayStep)
			{
				Reass_ReSetReassemble(m_handleReassemble);
			}
		}
	}

	/*
	 * 速度方向发生变化，清除解码器流结束标记
	 * add by w00210470
	 */
	if (NULL != m_handleDecoder)
	{
		bool bBackPlayStep = (speed < 0);
		if ((int)m_bBackPlayStep != (int)bBackPlayStep)
		{
			IVS_LOG(IVS_LOG_NOTICE, "Set PlaySpeed",
				   "Direction Change %f %f", m_dSpeed, speed);
			(void)Decoder_MarkStreamEnd(m_handleDecoder, FALSE);
		}
	}

	if (m_pSource)
	{
		m_pSource->SetPlaySpeed(speed);
		m_pSource->Resume();
	}
	//Render_SetPlaySpeed(m_handleRender, speed);

	m_dSpeed    = speed;
	m_bBackPlayStep = (speed < 0);
}

int CMediaChannel::SetDelayFrameNum(unsigned int uDelayFrameNum)
{
    if (NULL == m_handleDecoder)
    {
        return IVS_FAIL;
    }
    return Decoder_SetDelayFrameNum(m_handleDecoder, uDelayFrameNum);
}

int CMediaChannel::GetSourceBufferRemain(IVS_UINT32 &uBufferCount)
{
    if (NULL == m_handleDecoder)
    {
        return IVS_FAIL;
    }

	unsigned int FramesInRender = 0;

	// 正向播放，或负向I帧(小于-2倍)
	if (NULL != m_handleRender && (m_dSpeed > 0 || m_dSpeed <= -2) && (!m_bBackPlayStep))
	{
		//FramesInRender = Render_GetBufFrameNum(m_handleRender, &FramesInRender);
		FramesInRender = (FramesInRender <= 2) ? 0 : FramesInRender;
    }

	unsigned int FramesInDecoder = 0;
	Decoder_GetSourceBufferRemain(m_handleDecoder, FramesInDecoder);

	uBufferCount = FramesInDecoder + FramesInRender;

    return IVS_SUCCEED;
}

int CMediaChannel::SetFileTimeRange(unsigned int beg, unsigned int end)
{
	if (NULL == m_pSource)
	{ 
		return IVS_FAIL;
	}
	return m_pSource->SetPlayRange(beg, end);
}

void CMediaChannel::StopNotifyBuffEvent()
{
    if (NULL == m_handleDecoder)
    {
        return;
    }
    Decoder_StopNotifyBuffEvent(m_handleDecoder);
}


/*
 * SetMeidaAttribute
 * 设置媒体属性
 * add by w00210470
 */
int CMediaChannel::SetMeidaAttribute(const MEDIA_ATTR *pVideoAttr, const MEDIA_ATTR *pAudioAttr)
{
    if (NULL == pVideoAttr || NULL == pAudioAttr)
    {
        return IVS_FAIL;
    }

    if (SERVICE_TYPE_AUDIO_BROADCAST == m_enServiceMode	|| 
        SERVICE_TYPE_AUDIO_CALL == m_enServiceMode)
    {
        return IVS_SUCCEED;
    }

    //已经存在拼帧句柄,如果编码格式是否有变化则设置标志位m_bIsVideoDecoderTypeChange;
    if (NULL != m_handleReassemble)
    {
        if (m_enVideoDecoderType == pVideoAttr->type)
        {
            IVS_LOG(IVS_LOG_INFO, "SetMeidaAttribute", " VideoType is same, VideoType[%u]", pVideoAttr->type);
        }
        else
        {
            m_bIsVideoDecoderTypeChange = true;
            IVS_LOG(IVS_LOG_INFO, "SetMeidaAttribute", " VideoType is Change, VideoType[%u]", pVideoAttr->type);
        }

        m_enVideoDecoderType = (IVS_VIDEO_DEC_TYPE)pVideoAttr->type;

        memcpy(&m_VideoAttr, pVideoAttr, sizeof(MEDIA_ATTR));
        memcpy(&m_AudioAttr, pAudioAttr, sizeof(MEDIA_ATTR));

        return IVS_SUCCEED;
    }

    IVS_ASSERT(VIDEO_DEC_H264  == pVideoAttr->type || 
        VIDEO_DEC_MJPEG == pVideoAttr->type  || 
		VIDEO_DEC_H265 == pVideoAttr->type ||
		VIDEO_DEC_SVAC == pVideoAttr->type);

    m_enVideoDecoderType = (IVS_VIDEO_DEC_TYPE)pVideoAttr->type;


    IVS_LOG(IVS_LOG_NOTICE, "Set MediaAttribute", "V:%d A:%d Video Codec %d", 
        (int)pVideoAttr->rtpmap, (int)pAudioAttr->rtpmap, m_enVideoDecoderType);

	/*m_AudioSampleRate */
	m_AudioAttr.samplerate = pAudioAttr->samplerate;
	IVS_LOG(IVS_LOG_INFO,"Set Meida Attribute","SetMeidaAttribute m_AudioSampleRate[%d]",m_AudioAttr.samplerate);

    /* 创建组帧模块 */
    IVS_HANDLE h = Reass_GetHandle(m_enVideoDecoderType);
    if (NULL == h)
    {
        IVS_LOG(IVS_LOG_ERR, "Set Media Attrbute", "Get Reass module fail");
        return IVS_FAIL;
    }

    if (m_dSpeed < 0)
    {
        Reass_SetPlayBackMode(h, TRUE);
    }

    (void)Reass_SetMediaAttr(h, pVideoAttr, pAudioAttr);
    (void)Reass_SetExceptionCallBack(h, m_cbExceptionCB, m_pExceptionUserData);
    (void)Reass_SetCompleteCallback(h, ReassmCallBack, this);
    (void)Reass_SetDecryptDataKeyAndMode(h,m_pSecretKey, m_ulKeyLen, m_iDecType);

    m_handleReassemble = h;
    m_bStreamTypeFlay  = true;

    IVS_LOG(IVS_LOG_NOTICE, "Set MediaAttribute", "V:%d A:%d", 
        (int)pVideoAttr->rtpmap, (int)pAudioAttr->rtpmap);

    return IVS_SUCCEED;
}

int CMediaChannel::SetWaitForVertiBlank(int bEnable)
{
    if (m_handleRender)
    {
        return IVS_SUCCEED;//Render_SetWaitForVertiBlank(m_handleRender, bEnable); 
    }
    return IVS_FAIL;
}


void CMediaChannel::ReSetFlowContrlFlag()
{
    if (NULL == m_handleDecoder)
    {
        return;
    }
    Decoder_ReSetFlowContrlFlag(m_handleDecoder);
}


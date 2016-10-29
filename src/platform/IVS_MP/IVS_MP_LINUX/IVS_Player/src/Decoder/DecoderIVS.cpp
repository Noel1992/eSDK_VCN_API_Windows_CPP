/*******************************************************************************
//  版权所有    2012 华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  DecoderIVS.cpp
//  文件说明:   IVS解码器实现
//  版    本:   eIVS V100R001C30
//  作    者:   周启明/z90003203
//  创建日期:   2012/08/30
//
//  修改标识：
//  修改说明：
*******************************************************************************/

#include "DecoderIVS.h"
#ifdef WIN32
#include "HisiDecoder.h"
#include "IntelDecoder.h"
#include "HisiAudioDecoder.h"
#include "NvdiaDecoder.h"
#endif
#include "IVSCommon.h"
#include "AutoMutex.h"
#include "LockGuard.h"

#define MAX_AUDIO_BUFFER_SIZE (4096)
#define DEFAULT_BUFFER_WARNING_TIMES (1)
//#define VIDEO_BUF_EX_COUNT	  (20)

//#define DECODER_FRAME_SIZE  (512000)


CDecoderIVS::CDecoderIVS(void)
: m_pVideoDecoder(NULL)
, m_pAudioDecoder(NULL)
, m_DecoderMode(DECODE_MODE_NONE)
, m_pVideoBuffer(NULL)
, m_pAudioBuffer(NULL)
, m_pDecoderThread(NULL)
, m_state(BaseJob::created)
, m_bRunning(false)
, m_uDelayFrameNum(0)
, m_uStepFrameNum(0)
, m_uBufferCount(0)
, m_uExBufCount (VIDEO_BUF_EX_COUNT)
, m_bNotifyBuffEvent(true)
, m_bEOF(false)
, m_bEofNotified (false)
{
    m_bFullFlag = FALSE;
    m_iFullWarning = DEFAULT_BUFFER_WARNING_TIMES;
    m_iEmptyWarning = DEFAULT_BUFFER_WARNING_TIMES;
    m_uLowerLimit = 10;
    m_uUpperLimit = 80;

    m_hWnd = NULL;
    m_bIsCheckWaterMark = false;
}

CDecoderIVS::~CDecoderIVS(void) throw()
{
    try
    {
        (void)CloseStream();//lint !e1506

         IVS_DELETE(m_pDecoderThread);

         IVS_DELETE(m_pVideoDecoder);
         IVS_DELETE(m_pAudioDecoder);

         IVS_DELETE(m_pVideoBuffer);
         IVS_DELETE(m_pAudioBuffer);

    }
    catch (...)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder IVS",
            "ReleaseDecoder throw exception."); //lint !e1551
        return;
    }
	m_hWnd = NULL;
}

int CDecoderIVS::InitAVDecoder(VIDEO_PARAM* pVideoParam,
                               AUDIO_PARAM* pAudioParam)
{
    if (NULL == pVideoParam)
    {
        IVS_LOG(IVS_LOG_ERR, "Init AVDecoder", "Input param is not invalid.");
        return IVS_PARA_INVALID;
    }

    IVS_LOG(IVS_LOG_DEBUG, "InitAVDecoder", "VideoFormat %d HardwareDec %d",
            pVideoParam->VideoFormat, pVideoParam->HardwareDec);

    IVS_DELETE(m_pVideoDecoder);
#ifdef WIN32
    // 硬解初始化
    if (IVS_TRUE == pVideoParam->HardwareDec)
    {
        // 增加获取负载能力的方法
        if (CNvidiaDecoder::HasHwDecAbility()
            && (!CNvidiaDecoder::CheckIsOverLoad())
            &&(VIDEO_DEC_H264 == pVideoParam->VideoFormat))
        {
            m_DecoderMode = DECODE_HW_NVDIA;
            if (NULL == IVS_NEW((CNvidiaDecoder *&)m_pVideoDecoder))
            {
                IVS_LOG(IVS_LOG_ERR, "Init AVDecoder", "Create IntelDecoder failed.");
            }
        }
        else if (CIntelDecoder::HasHwDecAbility())
        {
            m_DecoderMode = DECODE_HW_INTEL;
            if (NULL == IVS_NEW((CIntelDecoder *&)m_pVideoDecoder))
            {
                IVS_LOG(IVS_LOG_ERR, "Init AVDecoder", "Create IntelDecoder failed.");
            }
        }
        else
        {
            IVS_LOG(IVS_LOG_WARNING, "Get hard ware decode fail", "current computer unsurpport.");
        }
    }

    // 如果硬解失败，转到软解
    if (NULL == m_pVideoDecoder)
    {
        m_DecoderMode = DECODE_SW_HICILICON;

        if (NULL == IVS_NEW((CHisiDecoder *&)m_pVideoDecoder))
        {
            IVS_LOG(IVS_LOG_ERR, "Init AVDecoder", "Create HisiDecoder failed.");
            return IVS_PLAYER_RET_CREATE_DECODER_ERROR;
        }
    }

    // 初始化音频解码库
    if (pAudioParam != NULL)
    {
        IVS_LOG(IVS_LOG_DEBUG, "InitAVDecoder",
                "AudioFormat %d BitRate %u Channels %u SampleRate %u",
                pAudioParam->AudioFormat, pAudioParam->BitRate,
                pAudioParam->Channels, pAudioParam->SampleRate);

        IVS_DELETE(m_pAudioDecoder);
        if (NULL == IVS_NEW((CHisiAudioDecoder *&)m_pAudioDecoder))
        {
            IVS_LOG(IVS_LOG_ERR, "Init AVDecoder", "Create HisiAudioDecoder failed.");
            return IVS_PLAYER_RET_CREATE_DECODER_ERROR;
        }
        (void)m_pAudioDecoder->InitDecoder(pAudioParam);
    }
	return m_pVideoDecoder->InitDecoder(pVideoParam);
#else
	return IVS_SUCCEED;
#endif

    
}

void CDecoderIVS::ReleaseDecoder()
{
    (void)CloseStream();
}

int CDecoderIVS::Start(HWND hWnd)
{
    if (BaseJob::running == m_state)
	{
		m_uStepFrameNum = 0;
		return IVS_SUCCEED;
	}

    //AutoMutex _mutex(m_hMutex);
	CLockGuard lock(m_hMutex);
    m_state = BaseJob::running;
	m_uStepFrameNum = 0;

    int iRet = IVS_SUCCEED;
    if (NULL != m_pVideoDecoder)
    {
        iRet = m_pVideoDecoder->Start(hWnd);
    }

    if (NULL != m_pAudioDecoder)
    {
        iRet += m_pAudioDecoder->Start(hWnd);
    }

    m_hWnd = hWnd;
    return iRet;
}

void CDecoderIVS::Stop()
{
	m_state = BaseJob::created;
    //AutoMutex _mutex(m_hMutex);
	CLockGuard lock(m_hMutex);
   
    if (NULL != m_pVideoDecoder)
    {
        m_pVideoDecoder->Stop();
    }

    if (NULL != m_pAudioDecoder)
    {
        m_pAudioDecoder->Stop();
    }

    return;
}

/*****************************************************************************
函数名称：CDecoderIVS::SetWaterMarkState
功能描述：设置水印校验开始/停止
输入参数：bIsCheckWaterMark 开始为true,停止为false
输出参数：无
返 回 值：无
*****************************************************************************/
void CDecoderIVS::SetWaterMarkState(BOOL bIsCheckWaterMark)
{
    if (m_pVideoDecoder != NULL)
    {
        m_pVideoDecoder->SetWaterMarkState(bIsCheckWaterMark);

        if (FALSE == bIsCheckWaterMark)
        {
            m_bIsCheckWaterMark = false;
        }
        else
        {
            m_bIsCheckWaterMark = true;
        }
    }

    return;
}

//设置异常上报回调函数
void CDecoderIVS::SetExceptionCallback(PLAYER_EXCEPTION_CALLBACK pExceptionCallback, void* pUserParam)
{
    if (m_pVideoDecoder != NULL)
    {
        m_pVideoDecoder->SetExceptionCallback(pExceptionCallback, pUserParam);
        CBaseDecoder::SetExceptionCallback(pExceptionCallback, pUserParam);
    }

    return;
}

VIDEO_DECODE_MODE CDecoderIVS::GetDecoderType() const
{
    return m_DecoderMode;
}

int CDecoderIVS::GetVideoResolution(int &iWidth, int &iHeight)
{
    if (NULL == m_pVideoDecoder)
    {
        return IVS_FAIL;
    }

    return m_pVideoDecoder->GetVideoResolution(iWidth, iHeight);
}

int CDecoderIVS::OpenStream(VIDEO_PARAM* pVideoParam,
                            AUDIO_PARAM* pAudioParam,
                            unsigned int uLowerLimit,
                            unsigned int uUpperLimit,
							IVS_UINT32 uBufferCount,
							IVS_UINT32 uExBufCount)
{
    int iRet = InitAVDecoder(pVideoParam, pAudioParam);

    if (IVS_SUCCEED != iRet
        || NULL == m_pVideoDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Open Stream", "InitAVDecoder fail with %d.", iRet);
        return iRet;
    }

	// 音频解码器初始化，失败只记录日志，不返回失败 [11/18/2012 z90003203]
	if (m_pAudioDecoder != NULL)
	{
		if (NULL == IVS_NEW(m_pAudioBuffer))
		{
			IVS_LOG(IVS_LOG_ERR, "CDecoderIVS::OpenStream()",
				"Open Stream: Create audio buffer fail.");
			m_pAudioDecoder->ReleaseDecoder();
			IVS_DELETE(m_pAudioBuffer);
			//             return PLAYER_RET_ALLOC_MEM_ERROR;
		}

		if (!m_pAudioBuffer->Initialise(uBufferCount, MAX_AUDIO_BUFFER_SIZE))
		{
			IVS_LOG(IVS_LOG_ERR, "Open Stream",
				"Initialise audio buffer BufferCount %u MAX_AUDIO_BUFFER_SIZE.");

			m_pAudioDecoder->ReleaseDecoder();
			IVS_DELETE(m_pAudioBuffer);
			//             return IVS_PLAYER_RET_ALLOC_MEM_ERROR;
		}
	}

	m_bRunning = true;

	try
	{
		m_pDecoderThread = new _BaseThread((BaseJob* )this, 0);
	}
	catch(...)
	{
		IVS_DELETE(m_pDecoderThread);
		IVS_DELETE(m_pVideoBuffer);
		IVS_DELETE(m_pAudioBuffer);
		m_pVideoDecoder->ReleaseDecoder();
		IVS_LOG(IVS_LOG_ERR, "Open Stream",
				"Create BaseThread throw exception.");
		return IVS_PLAYER_RET_ALLOC_MEM_ERROR;
	}

	m_uLowerLimit = uLowerLimit;
	m_uUpperLimit = uUpperLimit;
	m_uBufferCount = uBufferCount;
	m_uExBufCount  = uExBufCount;
    return iRet;
}

int CDecoderIVS::InitBufMgr(IVS_UINT32 uBufferSize, IVS_UINT32 uBufferCount)
{
	int iRet = IVS_SUCCEED;
	//AutoMutex _mutex(m_hMutex);
	CLockGuard lock(m_hMutex);
	IVS_DELETE(m_pVideoBuffer);
	if (NULL == IVS_NEW(m_pVideoBuffer))
	{
		IVS_LOG(IVS_LOG_ERR, "Open Stream", "Create video buffer fail.");
		m_pVideoDecoder->ReleaseDecoder();    //lint !e613

		if (m_pAudioDecoder != NULL)
		{
			m_pAudioDecoder->ReleaseDecoder();
		}

		return IVS_PLAYER_RET_ALLOC_MEM_ERROR;
	}

	if (!m_pVideoBuffer->Initialise(uBufferCount, uBufferSize))
	{
		IVS_LOG(IVS_LOG_ERR, "InitBufMgr",
				"Initialise video buffer BufferCount %u BufferSize %u.",
				uBufferSize, uBufferCount);
		m_pVideoDecoder->ReleaseDecoder();   //lint !e613

		if (m_pAudioDecoder != NULL)
		{
			m_pAudioDecoder->ReleaseDecoder();
		}

		IVS_DELETE(m_pVideoBuffer);
		return IVS_PLAYER_RET_ALLOC_MEM_ERROR;
	}

	return iRet;
}

int CDecoderIVS::CloseStream()
{
    m_bRunning = false;

    IVS_DELETE(m_pDecoderThread);

    if (m_pVideoDecoder != NULL)
    {
        m_pVideoDecoder->ReleaseDecoder();
        IVS_DELETE(m_pVideoDecoder);
    }

    if (m_pAudioDecoder != NULL)
    {
        m_pAudioDecoder->ReleaseDecoder();
        IVS_DELETE(m_pAudioDecoder);
    }


    m_DecoderMode = DECODE_MODE_NONE;

    IVS_DELETE(m_pVideoBuffer);
    IVS_DELETE(m_pAudioBuffer);

    return IVS_SUCCEED;
}

int CDecoderIVS::CheckBufMgrbyFBL(IVS_UINT32 uiWidth, IVS_UINT32 uiHeight, unsigned int uFrameSize)
{
	int iRet = IVS_SUCCEED;

	// 检查是否分配，如果未分配，根据分辨率分配buf。如果已经分配，检查已有的buf是否满足该分辨率，如果不满足，清空buf重新分配;

	if (NULL == m_pVideoBuffer)
	{
		unsigned int uInitBufSize = 0;
		unsigned int uDefaultSize = 0;
		iRet = GetDefaultSizebyFBL(uiWidth, uiHeight, uDefaultSize);
		if (iRet == IVS_SUCCEED && uDefaultSize > uFrameSize)
		{
			uInitBufSize = uDefaultSize;
		}
		else
		{
			unsigned int uHdrLen = sizeof(IVS_RAW_FRAME_INFO) + sizeof(unsigned int);
			uInitBufSize = (unsigned int)((uFrameSize + uHdrLen) * 1.5);
		}
		iRet = InitBufMgr(uInitBufSize, m_uBufferCount + m_uExBufCount/*VIDEO_BUF_EX_COUNT*/);
		IVS_LOG(IVS_LOG_DEBUG, "CheckBufMgrbyFBL InitBuf", "[ret:%d],m_pVideoBuffer = NULL, Use BufSize[%u].", iRet, uInitBufSize);
		return iRet;
	}

    //当实际长度大于原先分配的缓存长度时，需要重新分配缓存 edit by w00172146
	unsigned int uBffLen = m_pVideoBuffer->GetBufferLength();
	if (uBffLen < uFrameSize)
	{
        unsigned int uInitBufSize = (unsigned int)(uFrameSize * 1.5);

        IVS_LOG(IVS_LOG_ERR, "CheckBufMgrbyFBL2", "Video Buffer TotalCount[%u] Full Count[%u] Size[%u].", m_uBufferCount + m_uExBufCount, m_pVideoBuffer->GetFullBufCount(), uInitBufSize);
        CBufferManager* pVideoBuffer = RebuildBufMgr(uInitBufSize, m_uBufferCount + m_uExBufCount/*VIDEO_BUF_EX_COUNT*/);
        if (NULL == pVideoBuffer)
        {
            IVS_LOG(IVS_LOG_ERR, "CheckBufMgrbyFBL", "Rebuild Buf Mgr is Fail.");
            return IVS_PLAYER_RET_ALLOC_MEM_ERROR;
        }

        //AutoMutex _mutex(m_hMutex);
		CLockGuard lock(m_hMutex);
        //如果缓存中还有数据，需要先取出重新InsertFrame
        while (m_pVideoBuffer->GetFullBufCount() > 0)
        {
            BYTE* pTmpBuf = m_pVideoBuffer->GetFullBuf(FALSE);
            if (NULL == pTmpBuf)
            {
                IVS_LOG(IVS_LOG_DEBUG, "CheckBufMgrbyFBL", " Get Full Buf Fail.");
                break;
            }
            IVS_RAW_FRAME_INFO* pFrameInfo = reinterpret_cast<IVS_RAW_FRAME_INFO *>(pTmpBuf);//lint !e826
            unsigned int *pFrameSize = (unsigned int *)(pTmpBuf + sizeof(IVS_RAW_FRAME_INFO));//lint !e826
            BYTE* pFrame = pTmpBuf + (sizeof(IVS_RAW_FRAME_INFO) + sizeof(unsigned int));

            (void)InsertFrame(pVideoBuffer, pFrameInfo, (char*)pFrame, *pFrameSize);

			// 这块内存从VideoBuffer中弹出后未释放;
			delete [] pTmpBuf;               // deallocate it
			pTmpBuf = NULL;
        }

        //删除原先缓存，重新指向新分配的缓存
        IVS_DELETE(m_pVideoBuffer);
        m_pVideoBuffer = pVideoBuffer;
	
		//iRet = InitBufMgr(uInitBufSize, m_uBufferCount);
		IVS_LOG(iRet, "CheckBufMgrbyFBL InitBuf", "Buf len[%u] less than this Frame size[%u], Use BufSize[%u].", 
                uBffLen, uFrameSize, uInitBufSize);	   
    }

	return iRet;
}

int CDecoderIVS::GetDefaultSizebyFBL(IVS_UINT32	uiWidth, IVS_UINT32	uiHeight, unsigned int& uDefaultSize)
{
	int iRet = IVS_SUCCEED;
	IVS_LOG(IVS_LOG_DEBUG, "GetDefaultSizebyFBL", "width[%u] height[%u].", uiWidth, uiHeight);
	if (uiWidth > 3000 || uiHeight > 3000)
	{
		IVS_LOG(IVS_LOG_ERR, "GetDefaultSizebyFBL", "Stream width[%u] or height[%u] error.", uiWidth, uiHeight);
		return IVS_FAIL;
	}

	if (uiWidth < 352)
	{
		IVS_LOG(IVS_LOG_ERR, "GetDefaultSizebyFBL", "Stream width[%u] less than 352, use 352 instead.", uiWidth);
		uiWidth = 352;
	}
	if (uiHeight < 288)
	{
		IVS_LOG(IVS_LOG_ERR, "GetDefaultSizebyFBL", "Stream height[%u] less than 288, use 288 instead.", uiHeight);
		uiHeight = 288;
	}
	
	unsigned long ulSize = uiWidth * uiHeight;
	if (ulSize <= 360*300)//兼容352*288
	{
		uDefaultSize = 32000;
	}
	else if ((ulSize > 360*300) &&(ulSize <= 720*600))//704*576
	{
		uDefaultSize = 64000;
	}
	else if ((ulSize > 720*600) &&(ulSize <= 1300*750))//1280*720
	{
		uDefaultSize = 128000;
	}
	else if ((ulSize > 1300*750) &&(ulSize <= 1950*1100))//1920*1080
	{
		uDefaultSize = 256000;
	}
	else if (ulSize > 1950*1100)//大于1920*1080
	{
		uDefaultSize = 512000;
	}
	else
	{
		// 这种情况不存在;保险起见;
		IVS_LOG(IVS_LOG_ERR, "GetDefaultSizebyFBL", "ulSize[%u] error.", ulSize);
		return IVS_FAIL;
	}

	return iRet;
}//lint !e1762

int CDecoderIVS::SendVideoFrame(IVS_RAW_FRAME_INFO* pRawFrameInfo,
                                char *pFrame,
                                unsigned int uFrameSize)
{
    if (NULL == pRawFrameInfo
        || NULL == pFrame
        || 0 == uFrameSize)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Frame",
            "Decoder frame info cannot be NULL.");
        return IVS_PARA_INVALID;
    }

	// 根据码流分辨率调整buf大小;
	int iRet = CheckBufMgrbyFBL(pRawFrameInfo->uiWidth, pRawFrameInfo->uiHeight, uFrameSize);
    if (iRet != IVS_SUCCEED || NULL == m_pVideoBuffer)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Frame",
            "Decoder buffer is not initialized.");
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

	// 解决当视频缓冲区不足的情况下，I帧也被丢掉的情况
	unsigned int uFullBufCount = m_pVideoBuffer->GetFullBufCount();
	unsigned int uBufCount = m_pVideoBuffer->GetBufferCount();
	if (0 == uBufCount)
	{
		IVS_LOG(IVS_LOG_ERR, "Send Video Frame",
			"Decoder buffer Size do not set, call openstream first.");
		return IVS_PLAYER_RET_ORDER_ERROR;
	}

	// 如果预订空间已经用完99，下面的20帧空间只插入I帧，P帧全部丢弃;
	if ( uFullBufCount + 1 >= DECODER_FRAME_COUNT)
	{
		IVS_UINT32 uiFrameType = pRawFrameInfo->uiFrameType;
		if (H264_IDR_NALU_TYPE == uiFrameType)
		{
			iRet = InsertFrame(m_pVideoBuffer, pRawFrameInfo, pFrame, uFrameSize);
			IVS_LOG(IVS_LOG_DEBUG, "Send Video Frame", "video buffer size[%d] video frame type[%u] is idr, insert it.", uFullBufCount, uiFrameType);
		}
		else
		{
			IVS_LOG(IVS_LOG_ERR, "Send Video Frame",
					"video buffer size[%d] video frame type[%u] is not idr, drop it.", uFullBufCount, uiFrameType);
		}
	}
	else
	{
		iRet = InsertFrame(m_pVideoBuffer, pRawFrameInfo, pFrame, uFrameSize);
	}
	// end
    
	if (uBufCount == m_uExBufCount)
	{
		IVS_LOG(IVS_LOG_ERR, "Send Video Frame", "Video buffer total count equal exbufcount[%u]", uBufCount);
		return IVS_FAIL;
	}

	unsigned int uRate = (uFullBufCount * 100)/(uBufCount - m_uExBufCount/*VIDEO_BUF_EX_COUNT*/);//uBufCount > VIDEO_BUF_EX_COUNT，肯定大于0;
	
	// 如果帧数超过极限值,达到99、100帧，存在丢帧，打印日志记录;
	if ((98 < uRate) || (uRate >= (m_uUpperLimit * 1.2)))
	{
		// 如果已经超过100上限，说明已经开始丢弃p帧数据，打印日志记录;
		if (uFullBufCount > DECODER_FRAME_COUNT - 1)
		{
			IVS_LOG(IVS_LOG_ERR, "Send Video Frame", "Buffer Rate count[%u] Full warning flag[%d] Full count[%u] over 100, drop p frame.", uRate, m_iFullWarning, uFullBufCount);
		}
		else
		{
			IVS_LOG(IVS_LOG_DEBUG, "Send Video Frame", "upper limit Buffer Rate count[%u] Full warning flag[%d].", uRate, m_iFullWarning);
		}
	}

	//统计如果当前帧数据已经超过上限的1.2倍，或者超过95
    if ((uRate == m_uUpperLimit || uRate >= (m_uUpperLimit * 1.2) || uRate >= 95) && m_iFullWarning > 0 && m_pfunEventCallback)
    {
        m_bFullFlag = TRUE;
        int* iFullFlag = &m_bFullFlag;
		if (m_bNotifyBuffEvent)
		{
			IVS_LOG(IVS_LOG_INFO,"Send Video Frame","notify upper limit[%u] FullWarningFlag[%d].", uRate, m_iFullWarning);
			m_pfunEventCallback(0, IVS_EVENT_PLAYER_BUFFER, (IVS_VOID*)iFullFlag, m_pEventUserParam);
		}
		else
		{
			IVS_LOG(IVS_LOG_INFO,"Send Video Frame","upper limit m_bNotifyBuffEvent = false, upper limit[%u] FullWarningFlag[%d].", uRate, m_iFullWarning);
		}
        
        m_iFullWarning--;
    }
    else if (uRate < m_uUpperLimit)
    {
        m_iFullWarning = DEFAULT_BUFFER_WARNING_TIMES;
    }
	else
	{

	}

    return iRet;
}

int CDecoderIVS::SendAudioFrame(IVS_RAW_FRAME_INFO* pRawFrameInfo,
                                char *pFrame,
                                unsigned int uFrameSize)
{
//     AutoMutex _mutex(m_hMutex);
    return InsertFrame(m_pAudioBuffer, pRawFrameInfo, pFrame, uFrameSize);
}

int CDecoderIVS::InsertFrame(CBufferManager* pBufferManager,
                             const IVS_RAW_FRAME_INFO* pRawFrameInfo,
                             char *pFrame,
                             unsigned int uFrameSize) const
{
    if (NULL == pBufferManager)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Frame",
                "Decoder buffer is not initialized.");
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    if (NULL == pRawFrameInfo
        || NULL == pFrame
        || 0 == uFrameSize)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Frame",
                "Decoder frame info cannot be NULL.");
        return IVS_PARA_INVALID;
    }

    // 避免阻塞线程，将此接口有阻塞改为非阻塞 [1/17/2013 z90003203]
    BYTE* pBuffer = pBufferManager->GetEmptyBuf(FALSE);

    if (NULL == pBuffer)
    {
        IVS_LOG(IVS_LOG_WARNING, "Insert Frame",
                "Decoder buffer if full.");

        // 缓冲区满
        return IVS_PLAYER_RET_DECODER_BUFFER_FULL;
    }

    IVS_LOG(IVS_LOG_DEBUG, "InsertFrame",
        "FrameInfo %u %u %u %I64u %u %u %u %u pFrame 0x%x uFrameSize %u",
        pRawFrameInfo->uiStreamType, pRawFrameInfo->uiFrameType,
        pRawFrameInfo->uiTimeStamp, pRawFrameInfo->ullTimeTick,
        pRawFrameInfo->uiWaterMarkValue, pRawFrameInfo->uiWidth,
        pRawFrameInfo->uiHeight, pRawFrameInfo->uiGopSequence,
        pFrame, uFrameSize);

    unsigned int uBufferLen = pBufferManager->GetBufferLength();
    unsigned int uHdrLen = sizeof(IVS_RAW_FRAME_INFO) + sizeof(unsigned int);

    if ((uBufferLen > uHdrLen) && (uBufferLen - uHdrLen >= uFrameSize))
    {
#ifdef WIN32
        ::memcpy_s(pBuffer, uBufferLen, pRawFrameInfo, sizeof(IVS_RAW_FRAME_INFO));
#else
		memcpy(pBuffer,pRawFrameInfo,sizeof(IVS_RAW_FRAME_INFO));
#endif

        unsigned int *pFrameSize = (unsigned int *)(pBuffer + sizeof(IVS_RAW_FRAME_INFO));//lint !e826
        *pFrameSize = uFrameSize;
#ifdef WIN32
        ::memcpy_s(pBuffer + uHdrLen,
                   uBufferLen - uHdrLen,
                   pFrame,
                   uFrameSize);
#else
		memcpy(pBuffer + uHdrLen,pFrame,uFrameSize);
#endif

        (void)pBufferManager->InsertFullBuf(pBuffer);
    }
    else // uBufferLen <= pRawFrameInfo->Size
    {
		IVS_LOG(IVS_LOG_ERR, "Insert Frame",
			"Frame size(%u) is larger than buffer length(%u).",
			pFrame, uFrameSize);
		(void)pBufferManager->InsertEmptyBuf(pBuffer);
    }

    return IVS_SUCCEED;
}

int CDecoderIVS::GetFrameOut(CBaseDecoder* pDecoder,
                             CBufferManager* pBufferManager)
{
    if (NULL == pBufferManager
        || NULL == pDecoder)
    {
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    BYTE* pTmpBuf = pBufferManager->GetFullBuf(FALSE);

    if(NULL == pTmpBuf)
    {
//         IVS_LOG(IVS_LOG_ERR, "CDecoderIVS::GetVideoFrameOut()",
//             "m_pVideoBuffer->GetFullBuf() fail.");
        //Sleep(1);
        return IVS_SUCCEED;
    }

    IVS_RAW_FRAME_INFO* pFrameInfo = reinterpret_cast<IVS_RAW_FRAME_INFO *>(pTmpBuf);//lint !e826
    unsigned int *pFrameSize = (unsigned int *)(pTmpBuf + sizeof(IVS_RAW_FRAME_INFO));//lint !e826
    BYTE* pFrame = pTmpBuf + (sizeof(IVS_RAW_FRAME_INFO) + sizeof(unsigned int));

    if (PAY_LOAD_TYPE_H264 == pFrameInfo->uiStreamType || 
		PAY_LOAD_TYPE_MJPEG == pFrameInfo->uiStreamType || 
		PAY_LOAD_TYPE_H265 == pFrameInfo->uiStreamType ||
		PAY_LOAD_TYPE_SVAC == pFrameInfo->uiStreamType)
    {
        if ((PAY_LOAD_TYPE_MJPEG == pFrameInfo->uiStreamType)
            &&((DECODE_HW_NVDIA == m_DecoderMode) || (DECODE_HW_INTEL == m_DecoderMode)))
        {
            m_pVideoDecoder->Stop();          //lint !e613
            m_pVideoDecoder->ReleaseDecoder();  //lint !e613
            IVS_DELETE(m_pVideoDecoder);
            m_pVideoDecoder = NULL;

            m_DecoderMode = DECODE_SW_HICILICON;
#ifdef WIN32
            if (NULL == IVS_NEW((CHisiDecoder *&)m_pVideoDecoder))
#else
			if (NULL == m_pVideoDecoder)//IVS_NEW((CBaseDecoder *&)m_pVideoDecoder))
#endif
            {
                IVS_LOG(IVS_LOG_ERR, "Init AVDecoder", "Create HisiDecoder failed.");
                return IVS_PLAYER_RET_CREATE_DECODER_ERROR;
            }

            VIDEO_PARAM stVideoParam;
            stVideoParam.HardwareDec = 0;
            stVideoParam.VideoFormat = VIDEO_DEC_MJPEG;
            int iRet = m_pVideoDecoder->InitDecoder(&stVideoParam);
            if (IVS_SUCCEED != iRet)
            {
                IVS_DELETE(m_pVideoDecoder);
                m_pVideoDecoder = NULL;
                return iRet;
            }

            m_pVideoDecoder->SetDecoderCB(m_DecoderCB, m_pDecUserParam);
            m_pVideoDecoder->SetExceptionCallback(m_pfunExceptionCallback, m_pExcepUserParam);
            m_pVideoDecoder->SetWaterMarkState(m_bIsCheckWaterMark);
            (void)m_pVideoDecoder->Start(m_hWnd);
            pDecoder = m_pVideoDecoder;
        }
        (void)pDecoder->SendVideoFrame(pFrameInfo, (char *)pFrame, *pFrameSize);
        
        if (m_uStepFrameNum > 0)
        {
            m_uStepFrameNum--;

            // 解码完成后恢复暂停状态
            if (0 == m_uStepFrameNum)
            {
                //Stop();
            }
        }
    }
    else if (PAY_LOAD_TYPE_PCMU == pFrameInfo->uiStreamType
             || PAY_LOAD_TYPE_PCMA == pFrameInfo->uiStreamType
             || PAY_LOAD_TYPE_G723 == pFrameInfo->uiStreamType)
    {
        (void)pDecoder->SendAudioFrame(pFrameInfo, (char *)pFrame, *pFrameSize);
    }
    else
    {
        IVS_LOG(IVS_LOG_ERR, "Get Video Frame Out",
            "Unexpected StreamType %d.", pFrameInfo->uiStreamType);
    }

    (void)pBufferManager->InsertEmptyBuf(pTmpBuf);
    return IVS_SUCCEED;
}

/************************************************************
Func Name    :MainLoop
Date Created :2012-11-6
Author       :周启明 z90003203
Description  :TASK主循环，完成从缓冲区取数据送解码库
Input        :
Output       :
Return       :
Caution      :
************************************************************/
void CDecoderIVS::MainLoop()
{
    while(m_bRunning)
    {
        //AutoMutex _mutex(m_hMutex);
		CLockGuard lock(m_hMutex);

        // modify for 暂停时可以设置单步解码帧数
        if(BaseJob::created == m_state
            && 0 == m_uStepFrameNum)
        {
            //Sleep(10);
			VOS_Sleep(10);
            continue;
        }
        else if(BaseJob::finished == m_state)
        {
            break;
        }
        else
        {
            if(!this->AVDecodeProc())
            {
                m_state = BaseJob::finished;
            }
        }
    }
}

/************************************************************
Func Name    :MainLoop
Date Created :2012-11-6
Author       :周启明 z90003203
Description  :TASK主循环，完成从缓冲区取数据送解码库
Input        :
Output       :
Return       :
Caution      :
************************************************************/
void CDecoderIVS::kill()
{
	m_state = BaseJob::created;
    //AutoMutex _mutex(m_hMutex);
	CLockGuard lock(m_hMutex);
    Stop();
    m_state = BaseJob::finished;
	m_bRunning = false;
//     (void)CloseStream();
}

void CDecoderIVS::SetDelayFrameNum(unsigned int uDelayFrameNum)
{
    m_uDelayFrameNum = uDelayFrameNum;
}

int CDecoderIVS::StepFrames(unsigned int uStepFrameNum)
{
    // 只有在暂停状态下才能设置单步解码帧数
    if (BaseJob::created != m_state)
    {
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    // 设置完成后要设置解码库为解码状态
    (void)Start(NULL);

	m_uStepFrameNum = uStepFrameNum;

    return IVS_SUCCEED;
}

bool CDecoderIVS::AVDecodeProc()
{
    if (m_pVideoBuffer != 0)
    {
        unsigned int uFullBufCount = m_pVideoBuffer->GetFullBufCount();
        unsigned int uBufCount = m_pVideoBuffer->GetBufferCount();
        if (0 == uBufCount)
        {
            IVS_LOG(IVS_LOG_ERR, "AVDecodeProc",
                    "Decoder buffer do not set, call openstream first.");
            return false;
        }
        unsigned int uRate = (uFullBufCount * 100)/uBufCount;
        if (uRate <= m_uLowerLimit && m_iEmptyWarning > 0 && m_pfunEventCallback)
        {
            m_bFullFlag = FALSE;
            int* iFullFlag = &m_bFullFlag;
			if (m_bNotifyBuffEvent)
			{
				IVS_LOG(IVS_LOG_INFO,"AVDecodeProc","notify lower limit[%u] EmptyWarningFlag[%d].", uRate, m_iEmptyWarning);
				m_pfunEventCallback(0, IVS_EVENT_PLAYER_BUFFER, (IVS_VOID*)iFullFlag, m_pEventUserParam);
			}
			else
			{
				IVS_LOG(IVS_LOG_INFO,"AVDecodeProc","m_bNotifyBuffEvent = false, lower limit[%u] FullWarningFlag[%d].", uRate, m_iEmptyWarning);
			}
            m_iEmptyWarning--;
        }
        else if (uRate > m_uLowerLimit)
        {
            m_iEmptyWarning = DEFAULT_BUFFER_WARNING_TIMES;
        }

        if (m_uDelayFrameNum >= uFullBufCount)
        {
            //Sleep(1);
            return true;
        }
    }

    (void)GetFrameOut(m_pVideoDecoder, m_pVideoBuffer);
    (void)GetFrameOut(m_pAudioDecoder, m_pAudioBuffer);

	/*
	 * add by w00210470, 最后一帧后，发送一个空帧，标识文件结束
	 */
	if (NULL != m_pVideoDecoder && NULL != m_pVideoBuffer && 0 == m_pVideoBuffer->GetFullBufCount() && m_bEOF)
	{
		if (!m_bEofNotified)
		{
			(void)m_pVideoDecoder->SendVideoFrame(NULL, NULL, 0);
			m_bEofNotified = true;
		}
	}
    return true;
}

void CDecoderIVS::SetDecoderCB(DECODER_CALLBACK fnDecoderCB, void* pUserParam)
{
	if (NULL != m_pVideoDecoder)
	{
		m_pVideoDecoder->SetDecoderCB(fnDecoderCB, pUserParam);
        CBaseDecoder::SetDecoderCB(fnDecoderCB, pUserParam);
	}

    if (NULL != m_pAudioDecoder)
    {
        m_pAudioDecoder->SetDecoderCB(fnDecoderCB, pUserParam);
    }
}

int CDecoderIVS::GetSourceBufferRemain(unsigned int &uBufferCount) const
{
    if (NULL == m_pVideoBuffer)
    {
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    uBufferCount = m_pVideoBuffer->GetFullBufCount();
    return IVS_SUCCEED;
}

int CDecoderIVS::ResetBuffer()
{
    if (NULL == m_pVideoBuffer)
    {
        return IVS_FAIL;
    }

    IVS_LOG(IVS_LOG_INFO, "Reset Buffer",
        "Begin reset video buffer 0x%x.", m_pVideoBuffer);

    m_pVideoBuffer->Stop();

    int iRet = ResetFrameBuffer(m_pVideoBuffer);

    IVS_LOG(IVS_LOG_INFO, "Reset Buffer",
        "End reset video buffer and begin reset audio buffer 0x%x.", m_pAudioBuffer);

    if (m_pAudioBuffer != NULL)
    {
        m_pAudioBuffer->Stop();
        iRet += ResetFrameBuffer(m_pAudioBuffer);
        m_pAudioBuffer->Start();
    }

    m_pVideoBuffer->Start();

    IVS_LOG(IVS_LOG_INFO, "Reset Buffer",
        "End reset audio buffer with %d.", iRet);

    return iRet;
}

int CDecoderIVS::ResetFrameBuffer(CBufferManager* pBufferManager) const
{
    if (NULL == pBufferManager)
    {
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    pBufferManager->ResetBuffer();

    return IVS_SUCCEED;
}

void CDecoderIVS::StopNotifyBuffEvent()
{
	m_bNotifyBuffEvent = false;
}

void CDecoderIVS::SetEoS(bool bIsEOS)
{
	if (bIsEOS)
	{
		m_bEOF = true;
	}
	else
	{
		m_bEOF = false;
		m_bEofNotified = false;
	}
}


CBufferManager* CDecoderIVS::RebuildBufMgr(IVS_UINT32 uBufferSize, IVS_UINT32 uBufferCount) const
{
    //AutoMutex _mutex(m_hMutex);
    CBufferManager* pVideoBuffer = NULL;
    if (NULL == IVS_NEW(pVideoBuffer))
    {
        IVS_LOG(IVS_LOG_ERR, "Rebuild Buf Mgr", "Create video buffer fail.");
        return NULL;
    }

    if (!pVideoBuffer->Initialise(uBufferCount, uBufferSize))
    {
        IVS_LOG(IVS_LOG_ERR, "Rebuild Buf Mgr", "Initialise video buffer BufferCount %u BufferSize %u.",
                uBufferCount, uBufferSize);

        IVS_DELETE(pVideoBuffer);
        return NULL;
    }

    return pVideoBuffer;
}

void CDecoderIVS::ReSetFlowContrlFlag()
{
    m_iFullWarning = DEFAULT_BUFFER_WARNING_TIMES;
}

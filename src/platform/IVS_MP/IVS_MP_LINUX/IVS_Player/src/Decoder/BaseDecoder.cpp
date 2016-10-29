#include "BaseDecoder.h"
#include "AutoMutex.h"


CBaseDecoder::CBaseDecoder(void)
{
    m_uStreamType = (unsigned int)PAY_LOAD_NO_TYPE;
    memset(&m_stVideoParam, 0, sizeof(m_stVideoParam));
    memset(&m_stAudioParam, 0, sizeof(m_stAudioParam));

    m_pfunExceptionCallback  = NULL;
    m_pExcepUserParam = NULL;

    m_DecoderCB = NULL;
    m_pDecUserParam = NULL;

	/*
    m_hDecodeMutex = ::CreateMutex(NULL, FALSE, NULL);
	m_hDecoderCBMutex = ::CreateMutex(NULL, FALSE, NULL);
	m_hExceptionMutex = ::CreateMutex(NULL, FALSE, NULL);
	m_hEventMutex = ::CreateMutex(NULL, FALSE, NULL);
	*/
	m_hDecodeMutex = VOS_CreateMutex();
	m_hDecoderCBMutex = VOS_CreateMutex();
	m_hExceptionMutex = VOS_CreateMutex();
	m_hEventMutex = VOS_CreateMutex();


    m_bDecoding = false;

    m_pfunEventCallback = NULL;
    m_pEventUserParam = NULL;
}


CBaseDecoder::~CBaseDecoder(void) throw()
{
    if (NULL != m_hDecodeMutex)
    {
        //::CloseHandle(m_hDecodeMutex);
		VOS_DestroyMutex(m_hDecodeMutex);
        m_hDecodeMutex = NULL;
    }

    if (NULL != m_hDecoderCBMutex)
    {
        //::CloseHandle(m_hDecoderCBMutex);
		VOS_DestroyMutex(m_hDecoderCBMutex);
        m_hDecoderCBMutex = NULL;
    }

    if (NULL != m_hExceptionMutex)
    {
        //::CloseHandle(m_hExceptionMutex);
		VOS_DestroyMutex(m_hExceptionMutex);
        m_hExceptionMutex = NULL;
    }

    if (NULL != m_hEventMutex)
    {
        //::CloseHandle(m_hEventMutex);
		VOS_DestroyMutex(m_hEventMutex);
        m_hEventMutex = NULL;
    }

    m_pfunExceptionCallback  = NULL;
    m_pExcepUserParam = NULL;

    m_pfunEventCallback = NULL;
    m_pEventUserParam = NULL;

    m_DecoderCB = NULL;
    m_pDecUserParam = NULL;

}

void CBaseDecoder::SetWaterMarkState(BOOL bIsCheckWaterMark)
{
    return;
}//lint !e715

int CBaseDecoder::InitDecoder(VIDEO_PARAM *pVideoParam)
{
    return IVS_PLAYER_RET_UNSUPPORT_OPERATION;
}//lint !e715

int CBaseDecoder::InitDecoder(AUDIO_PARAM *pAudioParam)
{
    return IVS_PLAYER_RET_UNSUPPORT_OPERATION;
}//lint !e715

void CBaseDecoder::ReleaseDecoder()
{
    return;
}

int CBaseDecoder::OpenStream(VIDEO_PARAM *pVideoParam,
                             AUDIO_PARAM *pAudioParam,
                             unsigned int uBufferSize,
                             unsigned int uBufferCount)
{
    return IVS_PLAYER_RET_UNSUPPORT_OPERATION;
}//lint !e715

int CBaseDecoder::CloseStream()
{
    return IVS_PLAYER_RET_UNSUPPORT_OPERATION;
}

int CBaseDecoder::Start(HWND hWnd)
{
    return IVS_PLAYER_RET_UNSUPPORT_OPERATION;
}//lint !e715

void CBaseDecoder::Stop()
{
    return;
}

//设置异常上报回调函数
void CBaseDecoder::SetExceptionCallback(PLAYER_EXCEPTION_CALLBACK pExceptionCallback, void *pUserParam)
{
    AutoMutex _mutex(m_hExceptionMutex);
    m_pfunExceptionCallback = pExceptionCallback;
    m_pExcepUserParam = pUserParam;
}

//设置事件上报回调函数
void CBaseDecoder::SetEventCallback(PLAYER_EVENT_CALLBACK pEventCallback, void *pUserParam)
{
    AutoMutex _mutex(m_hEventMutex);
    m_pfunEventCallback = pEventCallback;
    m_pEventUserParam = pUserParam;
}

int CBaseDecoder::GetVideoResolution(int &iWidth, int &iHeight)
{
    return IVS_PLAYER_RET_UNSUPPORT_OPERATION;
}//lint !e715

void CBaseDecoder::SetDecoderCB(DECODER_CALLBACK fnDecoderCB, void *pUserParam)
{
    AutoMutex _mutex(m_hDecoderCBMutex);
    m_DecoderCB = fnDecoderCB;
    m_pDecUserParam = pUserParam;
}




#pragma once

#include "IVSPlayerDataType.h"
#include "ivs_log.h"
#include "ivs_error.h"
#include "vos.h"

#define  LOAD_RATIO 70

class CBaseDecoder
{
public:
    CBaseDecoder(void);
    virtual ~CBaseDecoder(void) throw();

    virtual int InitDecoder(VIDEO_PARAM *pVideoParam);

    virtual int InitDecoder(AUDIO_PARAM *pAudioParam);

    virtual void ReleaseDecoder();

    virtual int Start(HWND hWnd);

    virtual void Stop();

    virtual void SetWaterMarkState(BOOL bIsCheckWaterMark);

    virtual void SetExceptionCallback(PLAYER_EXCEPTION_CALLBACK pExceptionCallback, void *pUserParam);

    virtual void SetEventCallback(PLAYER_EVENT_CALLBACK pEventCallback, void *pUserParam);

    virtual int GetVideoResolution(int &iWidth, int &iHeight);

    virtual int OpenStream(VIDEO_PARAM *pVideoParam,
                           AUDIO_PARAM *pAudioParam,
                           unsigned int uBufferSize,
                           unsigned int uBufferCount);

    virtual int CloseStream();

    virtual int SendVideoFrame(IVS_RAW_FRAME_INFO *pRawFrameInfo,
                               char *pFrame,
                               unsigned int uFrameSize) = 0;

    virtual int SendAudioFrame(IVS_RAW_FRAME_INFO *pRawFrameInfo,
                               char *pFrame,
                               unsigned int uFrameSize) = 0;

    virtual void SetDecoderCB(DECODER_CALLBACK fnDecoderCB, void *pUserParam);

protected:
    VIDEO_PARAM         m_stVideoParam;
    AUDIO_PARAM         m_stAudioParam;

    unsigned int m_uStreamType;

    PLAYER_EXCEPTION_CALLBACK m_pfunExceptionCallback;  //异常上报回调函数
    void *m_pExcepUserParam;

    PLAYER_EVENT_CALLBACK m_pfunEventCallback;  //事件上报回调函数
    void *m_pEventUserParam;

    DECODER_CALLBACK m_DecoderCB;
    void *m_pDecUserParam;

	/*
    HANDLE m_hDecodeMutex;
	HANDLE m_hDecoderCBMutex;
	HANDLE m_hExceptionMutex;
	HANDLE m_hEventMutex;
	*/
	VOS_Mutex* m_hDecodeMutex;
	VOS_Mutex* m_hDecoderCBMutex;
	VOS_Mutex* m_hExceptionMutex;
	VOS_Mutex* m_hEventMutex;



    bool m_bDecoding;
};


/*******************************************************************************
//  版权所有    2012 华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  DecoderIVS.h
//  文件说明:   IVS解码器实现
//  版    本:   eIVS V100R001C30
//  作    者:   周启明/z90003203
//  创建日期:   2012/08/30
//
//  修改标识：
//  修改说明：
*******************************************************************************/

#pragma once
#include "BaseDecoder.h"
#ifdef WIN32
#include "BufferManager\BufferManager.h"
#else
#include "BufferManager.h"
#endif
#include "_BaseThread.h"

class CDecoderIVS :
    public CBaseDecoder, public BaseJob
{
public:
    CDecoderIVS(void);
    virtual ~CDecoderIVS(void) throw();

    virtual int InitAVDecoder(VIDEO_PARAM* pVideoParam,
                              AUDIO_PARAM* pAudioParam);

    virtual void ReleaseDecoder();

    virtual int Start(HWND hWnd);
    virtual void Stop();

    virtual void SetWaterMarkState(BOOL bIsCheckWaterMark);

    //设置异常上报回调函数
    virtual void SetExceptionCallback(PLAYER_EXCEPTION_CALLBACK pExceptionCallback, void* pUserParam);

    virtual int GetVideoResolution(int &iWidth, int &iHeight);

    virtual int OpenStream(VIDEO_PARAM* pVideoParam,
                           AUDIO_PARAM* pAudioParam,
                           unsigned int uLowerLimit,
                           unsigned int uUpperLimit,
						   IVS_UINT32 uBufferCount,
						   IVS_UINT32 uExBufCount); //lint !e1411

    virtual int CloseStream();

    virtual int SendVideoFrame(IVS_RAW_FRAME_INFO* pRawFrameInfo,
                               char *pFrame,
                               unsigned int uFrameSize);

    virtual int SendAudioFrame(IVS_RAW_FRAME_INFO* pRawFrameInfo,
                               char *pFrame,
                               unsigned int uFrameSize);

	virtual void SetDecoderCB(DECODER_CALLBACK fnDecoderCB, void* pUserParam);

    virtual void MainLoop();

    virtual void kill();

public:
    VIDEO_DECODE_MODE GetDecoderType() const;

    int GetSourceBufferRemain(unsigned int &uBufferCount) const;

    int ResetBuffer();

    void SetDelayFrameNum(unsigned int uDelayFrameNum);

    int StepFrames(unsigned int uStepFrameNum);

	// add by w00210470
	bool IsCancel() const { return m_state != BaseJob::running; } 

	void StopNotifyBuffEvent();

	void SetEoS(bool bIsEOS);

    void ReSetFlowContrlFlag();
private:
    bool AVDecodeProc();

    int InsertFrame(CBufferManager* pBufferManager,
                    const IVS_RAW_FRAME_INFO* pRawFrameInfo,
                    char *pFrame,
                    unsigned int uFrameSize) const;

    int GetFrameOut(CBaseDecoder* pDecoder, CBufferManager* pBufferManager);

    int ResetFrameBuffer(CBufferManager* pBufferManager) const;

	int InitBufMgr(IVS_UINT32 uBufferSize, IVS_UINT32 uBufferCount);
    CBufferManager* RebuildBufMgr(IVS_UINT32 uBufferSize, IVS_UINT32 uBufferCount) const;

	int CheckBufMgrbyFBL(IVS_UINT32	uiWidth, IVS_UINT32	uiHeight, unsigned int uFrameSize);

	int GetDefaultSizebyFBL(IVS_UINT32	uiWidth, IVS_UINT32	uiHeight, unsigned int& uDefaultSize);
private:
    CBaseDecoder*   m_pVideoDecoder;    // 视频解码器
    CBaseDecoder*   m_pAudioDecoder;    // 音频解码器
    VIDEO_DECODE_MODE     m_DecoderMode;
// 	unsigned int m_ulDisplayAspectRatio;
    CBufferManager* m_pVideoBuffer;
    CBufferManager* m_pAudioBuffer;

    _BaseThread* m_pDecoderThread;

    JOB_STATE m_state;

    bool m_bRunning;

    unsigned int m_uDelayFrameNum;      // 延时解码的帧数
    unsigned int m_uStepFrameNum;       // 单步解码帧数，配合单帧回放使用

    BOOL m_bFullFlag;
    int m_iFullWarning;
    int m_iEmptyWarning;

    unsigned int m_uLowerLimit;         // 媒体缓冲区下限
    unsigned int m_uUpperLimit;         // 媒体缓冲区上限

    HWND m_hWnd;
    bool m_bIsCheckWaterMark;

	IVS_UINT32 m_uBufferCount;			// 解码库缓存的帧数;
	IVS_UINT32 m_uExBufCount;           // 解码库附加缓存数

	bool m_bNotifyBuffEvent;			// 是否通知buff事件;

	bool m_bEOF;
	bool m_bEofNotified;
};


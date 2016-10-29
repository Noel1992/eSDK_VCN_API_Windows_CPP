/*******************************************************************************
//  ��Ȩ����    2012 ��Ϊ�������޹�˾
//  �� �� ����  IVS_Player
//  �� �� ����  DecoderIVS.h
//  �ļ�˵��:   IVS������ʵ��
//  ��    ��:   eIVS V100R001C30
//  ��    ��:   ������/z90003203
//  ��������:   2012/08/30
//
//  �޸ı�ʶ��
//  �޸�˵����
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

    //�����쳣�ϱ��ص�����
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
    CBaseDecoder*   m_pVideoDecoder;    // ��Ƶ������
    CBaseDecoder*   m_pAudioDecoder;    // ��Ƶ������
    VIDEO_DECODE_MODE     m_DecoderMode;
// 	unsigned int m_ulDisplayAspectRatio;
    CBufferManager* m_pVideoBuffer;
    CBufferManager* m_pAudioBuffer;

    _BaseThread* m_pDecoderThread;

    JOB_STATE m_state;

    bool m_bRunning;

    unsigned int m_uDelayFrameNum;      // ��ʱ�����֡��
    unsigned int m_uStepFrameNum;       // ��������֡������ϵ�֡�ط�ʹ��

    BOOL m_bFullFlag;
    int m_iFullWarning;
    int m_iEmptyWarning;

    unsigned int m_uLowerLimit;         // ý�建��������
    unsigned int m_uUpperLimit;         // ý�建��������

    HWND m_hWnd;
    bool m_bIsCheckWaterMark;

	IVS_UINT32 m_uBufferCount;			// ����⻺���֡��;
	IVS_UINT32 m_uExBufCount;           // ����⸽�ӻ�����

	bool m_bNotifyBuffEvent;			// �Ƿ�֪ͨbuff�¼�;

	bool m_bEOF;
	bool m_bEofNotified;
};


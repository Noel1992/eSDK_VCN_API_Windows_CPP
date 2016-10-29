/********************************************************************
filename    :    MediaChannel.h
author      :    s00191067
created     :    2012/11/07
description :    ý��ͨ����
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/07 ��ʼ�汾
*********************************************************************/
#ifndef MEDIA_CHANNELH
#define MEDIA_CHANNELH

#include "IVSPlayerDataType.h"
#include "FileSource.h"

#ifdef WIN32
#include "..\..\inc\common\Lock.h"
#else
#include "Lock.h"
#endif 

typedef struct OpenStreamParam
{
    IVS_SERVICE_TYPE            enServiceType;
    SOURCE_TYPE                 enSourceType;
    PLAYER_EXCEPTION_CALLBACK   fExceptionCallback;
    PLAYER_EVENT_CALLBACK       fEventCallback;
    void*                       pUser;
    IVS_UINT32                  uiLowerLimit;
    IVS_UINT32                  uiUpperLimit;
}OPEN_STREAM_PARAM;

class CSource;
class _BaseThread;
class CMediaChannel
{
public:
    CMediaChannel(void);
    ~CMediaChannel(void);

    // ��ʼ��
    void Init();

    // �������;
    void UnInit();

    // ������Ƶ��������;
    void SetVideoDecodeType(IVS_VIDEO_DEC_TYPE enVideoDecoderType){ m_enVideoDecoderType = enVideoDecoderType; };

    // ��ȡ��Ƶ��������;
    void GetVideoDecodeType(IVS_VIDEO_DEC_TYPE& enVideoDecoderType)const { enVideoDecoderType = m_enVideoDecoderType;};

    // ������Ƶ��������;
    void SetAudioDecodeType(IVS_AUDIO_DEC_TYPE enAudioDecoderType){ m_enAudioDecoderType = enAudioDecoderType; };

    // ��ȡ��Ƶ��������;
    void GetAudioDecodeType(IVS_AUDIO_DEC_TYPE& enAudioDecoderType)const { enAudioDecoderType = m_enAudioDecoderType;};

    // ��ȡ����ģʽ;
    IVS_SERVICE_TYPE GetPlayMode()const { return m_enServiceMode; };

	//������Ƶ��������ģʽ
	int SetPlayQualityMode(int iPlayQualityMode);

    // ��ȡ������;
    SOURCE_TYPE GetSourceType()const { return m_enSourceType; };

    // ����;
    int OpenStream(const OPEN_STREAM_PARAM& stOpenStreamParam);

    // �ر���;
    int CloseStream();

    // ��������;
    int StartPlay();

    // ֹͣ����;
    int StopPlay();

	// ��ͣ����
	int PausePlay();

	// �ָ�����
	int ResumePlay();

    // ������Ƶ����
    int PlaySound();

    // ֹͣ��Ƶ����
    int StopSound();

	// ����ǰ������
	int StepForward(unsigned int &PicInPlayer);

	// �������˲���
	int StepBackward(unsigned int &PicInPlayer);

	// ��������
    int SetVolume(unsigned int uiVolumeValue);

    // ��ȡ����
    int GetVolume(unsigned int* puiVolumeValue);

    // �����������ص�;
    int SetRawFrameCallBack(REASSM_CALLBACK cbReassmCallBack, void *pUser);

    // ����֡���ݻص�;
    int SetFrameCallBack(DECODER_CALLBACK cbDecoderCB, void *pUser);
#ifdef WIN32
    // ���ò��Ŵ���;
    int SetPlayWindow(HWND hWnd);
    // ���Ӳ��Ŵ���;
    int AddPlayWindow(HWND hWnd, int nType, void *pLayout);
	int UpdateWindowLayout(HWND hWnd, int nType, void *pLayout);
    // ɾ�����Ŵ���;
    int DelPlayWindow(HWND hWnd);
#endif

	// ���ò��ű���;
	int SetDisplayAspectRatio(unsigned int uiScale);

    // ����Rtp����;
    int InputRtpData(char *pszBuf, unsigned int uiSize);

#ifdef WIN32
    // ���ļ�;
    int OpenFile(const char* pszFileName, PLAYER_EXCEPTION_CALLBACK fExceptionCallback, void* pUser);
#endif

	// �����ļ�����ʱ�䷶Χ
	int SetFileTimeRange(unsigned int beg, unsigned int end);
    
    // ����δ��������;
    int ProcessRawFrame(IVS_RAW_FRAME_INFO* pstRawFrameInfo, char* pszFrame, unsigned int uiFrameSize);

    int ProcessRawAudioFrame(IVS_RAW_FRAME_INFO* pstRawFrameInfo,
                             char* pszFrame, unsigned int uiFrameSize);

	// �����ļ�source����
#ifdef WIN32
	int ProcessFileFrame(MEDIA_FRAME_INFO *info); 
#endif

    // ���������֡����;
    int ProcessFrame(int iStreamType, void* pFrameInfo);

	// �����Ž���
	int ProcessPlayEnd();

	//��ȡý����Ϣ
	int GetMediaInfo(IVS_MEDIA_INFO* pstMediaInfo);

	//���ÿ���/ֹͣУ��ˮӡ
	int SetWaterMarkState(BOOL bIsStart);

	//��ȡ��ǰ����ʱ��
	int GetPlayedTime(unsigned long long *pTime);

	int GetCurrentFrameTick(unsigned long long *pTick);

	int SetPlayTime(time_t tm);

	int SetDownLoadTimeSpan(time_t beg, time_t end, bool bRepFin);

	//���Ŵ�������ʾͳ����Ϣ
#ifdef WIN32
	int ShowPlayStatis(bool enable);
#endif

    // ��ո�ģ�黺������ݣ�����ƴ֡���ݺͽ�����YUV��������;
    void ResetData();

	// ������ջ����־λ
	void SetResetFlag(bool bIsReset);

	// ��������¼��
	int StartRecord(const char* pCamCode, const char*pCamName, const char *pName, const IVS_LOCAL_RECORD_PARAM* pParm, 
		        PLAYER_EXCEPTION_CALLBACK fExceptionCallback, void* pUser, bool bAssociatedAudio);

	// ֹͣ����¼��
	void StopRecord();

    // ��ȡ�ѱ���¼���ļ�ʱ��
	int GetRecordStatis(unsigned int *pTime, unsigned int *pSpeed, IVS_UINT64 *pTotalSz);

	// ���ò����ٶ�
    void SetPlaySpeed(double speed);

	int  SetSecretKey(int iDecType, const char* pSecretKey);

	int  StopDecryptData();
	
	// ���û�ͼ�ص�
#ifdef WIN32
	int  SetDrawCallBack(DRAW_CALLBACK  callback, void *pUser);
	int  SetDrawCallBackEx(HWND hWnd, DRAW_CALLBACK callback, void *pUser);
#endif

	// ����¼������
	void InsertRecordFrame(const IVS_RAW_FRAME_INFO* pstRawFrameInfo,const  char* pszFrame, unsigned int uiFrameSize);
#ifdef WIN32
	// ����ץ�ĵ���ͼƬ
	int LocalSnapshot(const char *pSavePath, IVS_ULONG ulSnapshotFormat, const IVS_RECT *Rct);
#endif

    // ������ʱ�����֡��
    int SetDelayFrameNum(unsigned int uDelayFrameNum);

    // ��ȡδ����֡������ʣ���֡�� 
    int GetSourceBufferRemain(IVS_UINT32 &uBufferCount);

	void StopNotifyBuffEvent();

	int  SetMeidaAttribute(const MEDIA_ATTR *pVideoAttr, const MEDIA_ATTR *pAudioAttr);

	int  SetWaitForVertiBlank(int bEnable);

    void ReSetFlowContrlFlag();

protected:
#ifdef WIN32
	static void __SDK_CALLBACK DrawStatsCallBack(HDC hDc, RECT *rc, IVS_UINT64 frameID, IVS_VOID *pUser);
#endif

private:
	//Ԥ��RTP������ȡ��Ƶ������
	void  PreDecodeRtp(char *pszBuf, unsigned int uiSize);
	void  SetReassembleHandl();


public:
    static BOOL             m_bHardwareDec;         // ����Ӳ���־;

private:
	bool                    m_bBackPlayStep;

private:

	time_t                  m_tmDownLoadBeg;
	time_t                  m_tmDownLoadEnd;
	bool                    m_bRepFinish;  


private:
    IVS_VIDEO_DEC_TYPE      m_enVideoDecoderType;   // ��Ƶ��������;
    IVS_AUDIO_DEC_TYPE      m_enAudioDecoderType;   // ��Ƶ��������;
    IVS_SERVICE_TYPE        m_enServiceMode;        // ����ģʽ;
    SOURCE_TYPE             m_enSourceType;         // ��������;RTP��Frame

	bool                    m_bStreamInputControl;  // �������ر��

    IVS_HANDLE              m_handleReassemble;     // ƴ֡����;
    IVS_HANDLE              m_handleDecoder;        // �������;
    IVS_HANDLE              m_handleRender;         // ��Ⱦ����;

	IVS_HANDLE              m_handleRecorder;       // ����¼����
    bool                    m_bRecordFlag;          // �Ƿ�ʼ����¼��
	CIVSMutex               m_RecorderMutex;        //lint !e601 // ¼����������

    CFileSource*            m_pSource;              // ����Դ;
    _BaseThread*            m_pSourceThread;        // �����߳�;

    bool                    m_bNeedInputData2DecoderFlag;    // ��Ҫ���������뵽����ģ��;
    bool                    m_bNeedInputData2RenderFlag;     // ��Ҫ������������Ⱦģ��;
    bool                    m_bShowStatis;          // ��ʾͳ�����ݵı�־;
	bool                    m_bStreamTypeFlay;      // ��ȡ��Ƶ����ʽ�ɹ�

    REASSM_CALLBACK         m_cbReassmCB;           // �������ص�����;
    DECODER_CALLBACK        m_cbDecoderCB;          // �������ݻص�����;
    void*                   m_pReassmUserData;      // �������ص��û�����;
    void*                   m_pDecoderUserData;     // ����ص��û�����;
    CIVSMutex               m_ReassmCBMutex; //lint !e38 !e148 !e601   // �������ص���;
    CIVSMutex               m_DecoderCBMutex;//lint !e38 !e148 !e601   // �������ص���;
    PLAYER_EXCEPTION_CALLBACK m_cbExceptionCB;      // �쳣�ص�����ָ��;
    void*                   m_pExceptionUserData;   // �쳣�ص��û�����;
    CIVSMutex               m_ExceptionCBMutex;//lint !e38 !e148 !e601  // �쳣�ص���;

    PLAYER_EVENT_CALLBACK   m_cbEventCB;            // �¼��ص�����ָ��;
    void*                   m_pEventUserData;       // �¼��ص��û�����;
	double					m_dSpeed;				// �����ٶ�;
private:
	char* m_pSecretKey;								//��Ƶ������Կ
	unsigned long m_ulKeyLen;						//��Կ����
	int m_iDecType;									//��������

    bool  m_bIsVideoDecoderTypeChange;              //�����ʽ�Ƿ��л�
    //rtp ý������
    MEDIA_ATTR   m_VideoAttr;
    MEDIA_ATTR   m_AudioAttr;
};
#endif //MEDIA_CHANNELH

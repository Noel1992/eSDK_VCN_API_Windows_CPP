/********************************************************************
filename    :    MediaChannel.h
author      :    s00191067
created     :    2012/11/07
description :    媒体通道类
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/07 初始版本
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

    // 初始化
    void Init();

    // 清空数据;
    void UnInit();

    // 设置视频解码类型;
    void SetVideoDecodeType(IVS_VIDEO_DEC_TYPE enVideoDecoderType){ m_enVideoDecoderType = enVideoDecoderType; };

    // 获取视频解码类型;
    void GetVideoDecodeType(IVS_VIDEO_DEC_TYPE& enVideoDecoderType)const { enVideoDecoderType = m_enVideoDecoderType;};

    // 设置音频解码类型;
    void SetAudioDecodeType(IVS_AUDIO_DEC_TYPE enAudioDecoderType){ m_enAudioDecoderType = enAudioDecoderType; };

    // 获取音频解码类型;
    void GetAudioDecodeType(IVS_AUDIO_DEC_TYPE& enAudioDecoderType)const { enAudioDecoderType = m_enAudioDecoderType;};

    // 获取播放模式;
    IVS_SERVICE_TYPE GetPlayMode()const { return m_enServiceMode; };

	//设置视频播放质量模式
	int SetPlayQualityMode(int iPlayQualityMode);

    // 获取流类型;
    SOURCE_TYPE GetSourceType()const { return m_enSourceType; };

    // 打开流;
    int OpenStream(const OPEN_STREAM_PARAM& stOpenStreamParam);

    // 关闭流;
    int CloseStream();

    // 启动播放;
    int StartPlay();

    // 停止播放;
    int StopPlay();

	// 暂停播放
	int PausePlay();

	// 恢复播放
	int ResumePlay();

    // 启动音频播放
    int PlaySound();

    // 停止音频播放
    int StopSound();

	// 单步前进播放
	int StepForward(unsigned int &PicInPlayer);

	// 单步后退播放
	int StepBackward(unsigned int &PicInPlayer);

	// 设置音量
    int SetVolume(unsigned int uiVolumeValue);

    // 获取音量
    int GetVolume(unsigned int* puiVolumeValue);

    // 设置裸码流回调;
    int SetRawFrameCallBack(REASSM_CALLBACK cbReassmCallBack, void *pUser);

    // 设置帧数据回调;
    int SetFrameCallBack(DECODER_CALLBACK cbDecoderCB, void *pUser);
#ifdef WIN32
    // 设置播放窗口;
    int SetPlayWindow(HWND hWnd);
    // 增加播放窗口;
    int AddPlayWindow(HWND hWnd, int nType, void *pLayout);
	int UpdateWindowLayout(HWND hWnd, int nType, void *pLayout);
    // 删除播放窗口;
    int DelPlayWindow(HWND hWnd);
#endif

	// 设置播放比例;
	int SetDisplayAspectRatio(unsigned int uiScale);

    // 插入Rtp数据;
    int InputRtpData(char *pszBuf, unsigned int uiSize);

#ifdef WIN32
    // 打开文件;
    int OpenFile(const char* pszFileName, PLAYER_EXCEPTION_CALLBACK fExceptionCallback, void* pUser);
#endif

	// 设置文件播放时间范围
	int SetFileTimeRange(unsigned int beg, unsigned int end);
    
    // 处理未解码数据;
    int ProcessRawFrame(IVS_RAW_FRAME_INFO* pstRawFrameInfo, char* pszFrame, unsigned int uiFrameSize);

    int ProcessRawAudioFrame(IVS_RAW_FRAME_INFO* pstRawFrameInfo,
                             char* pszFrame, unsigned int uiFrameSize);

	// 处理文件source数据
#ifdef WIN32
	int ProcessFileFrame(MEDIA_FRAME_INFO *info); 
#endif

    // 处理解码后的帧数据;
    int ProcessFrame(int iStreamType, void* pFrameInfo);

	// 处理播放结束
	int ProcessPlayEnd();

	//获取媒体信息
	int GetMediaInfo(IVS_MEDIA_INFO* pstMediaInfo);

	//设置开启/停止校验水印
	int SetWaterMarkState(BOOL bIsStart);

	//获取当前播放时间
	int GetPlayedTime(unsigned long long *pTime);

	int GetCurrentFrameTick(unsigned long long *pTick);

	int SetPlayTime(time_t tm);

	int SetDownLoadTimeSpan(time_t beg, time_t end, bool bRepFin);

	//播放窗口上显示统计信息
#ifdef WIN32
	int ShowPlayStatis(bool enable);
#endif

    // 清空各模块缓存的数据，包括拼帧数据和解码后的YUV缓存数据;
    void ResetData();

	// 设置清空缓存标志位
	void SetResetFlag(bool bIsReset);

	// 启动本地录像
	int StartRecord(const char* pCamCode, const char*pCamName, const char *pName, const IVS_LOCAL_RECORD_PARAM* pParm, 
		        PLAYER_EXCEPTION_CALLBACK fExceptionCallback, void* pUser, bool bAssociatedAudio);

	// 停止本地录像
	void StopRecord();

    // 获取已保存录像文件时间
	int GetRecordStatis(unsigned int *pTime, unsigned int *pSpeed, IVS_UINT64 *pTotalSz);

	// 设置播放速度
    void SetPlaySpeed(double speed);

	int  SetSecretKey(int iDecType, const char* pSecretKey);

	int  StopDecryptData();
	
	// 设置绘图回调
#ifdef WIN32
	int  SetDrawCallBack(DRAW_CALLBACK  callback, void *pUser);
	int  SetDrawCallBackEx(HWND hWnd, DRAW_CALLBACK callback, void *pUser);
#endif

	// 处理录像数据
	void InsertRecordFrame(const IVS_RAW_FRAME_INFO* pstRawFrameInfo,const  char* pszFrame, unsigned int uiFrameSize);
#ifdef WIN32
	// 本地抓拍单张图片
	int LocalSnapshot(const char *pSavePath, IVS_ULONG ulSnapshotFormat, const IVS_RECT *Rct);
#endif

    // 设置延时解码的帧数
    int SetDelayFrameNum(unsigned int uDelayFrameNum);

    // 获取未解码帧缓冲区剩余的帧数 
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
	//预解RTP包，获取视频包类型
	void  PreDecodeRtp(char *pszBuf, unsigned int uiSize);
	void  SetReassembleHandl();


public:
    static BOOL             m_bHardwareDec;         // 启动硬解标志;

private:
	bool                    m_bBackPlayStep;

private:

	time_t                  m_tmDownLoadBeg;
	time_t                  m_tmDownLoadEnd;
	bool                    m_bRepFinish;  


private:
    IVS_VIDEO_DEC_TYPE      m_enVideoDecoderType;   // 视频解码类型;
    IVS_AUDIO_DEC_TYPE      m_enAudioDecoderType;   // 音频解码类型;
    IVS_SERVICE_TYPE        m_enServiceMode;        // 播放模式;
    SOURCE_TYPE             m_enSourceType;         // 收流类型;RTP或Frame

	bool                    m_bStreamInputControl;  // 输入流控标记

    IVS_HANDLE              m_handleReassemble;     // 拼帧库句柄;
    IVS_HANDLE              m_handleDecoder;        // 解码库句柄;
    IVS_HANDLE              m_handleRender;         // 渲染库句柄;

	IVS_HANDLE              m_handleRecorder;       // 本地录像句柄
    bool                    m_bRecordFlag;          // 是否开始本地录像
	CIVSMutex               m_RecorderMutex;        //lint !e601 // 录像句柄互斥锁

    CFileSource*            m_pSource;              // 数据源;
    _BaseThread*            m_pSourceThread;        // 收流线程;

    bool                    m_bNeedInputData2DecoderFlag;    // 需要将数据送入到解码模块;
    bool                    m_bNeedInputData2RenderFlag;     // 需要将数据送入渲染模块;
    bool                    m_bShowStatis;          // 显示统计数据的标志;
	bool                    m_bStreamTypeFlay;      // 获取视频流格式成功

    REASSM_CALLBACK         m_cbReassmCB;           // 裸码流回调函数;
    DECODER_CALLBACK        m_cbDecoderCB;          // 解码数据回调函数;
    void*                   m_pReassmUserData;      // 裸码流回调用户数据;
    void*                   m_pDecoderUserData;     // 解码回调用户数据;
    CIVSMutex               m_ReassmCBMutex; //lint !e38 !e148 !e601   // 裸码流回调锁;
    CIVSMutex               m_DecoderCBMutex;//lint !e38 !e148 !e601   // 解码流回调锁;
    PLAYER_EXCEPTION_CALLBACK m_cbExceptionCB;      // 异常回调函数指针;
    void*                   m_pExceptionUserData;   // 异常回调用户数据;
    CIVSMutex               m_ExceptionCBMutex;//lint !e38 !e148 !e601  // 异常回调锁;

    PLAYER_EVENT_CALLBACK   m_cbEventCB;            // 事件回调函数指针;
    void*                   m_pEventUserData;       // 事件回调用户数据;
	double					m_dSpeed;				// 播放速度;
private:
	char* m_pSecretKey;								//视频加密密钥
	unsigned long m_ulKeyLen;						//密钥长度
	int m_iDecType;									//加密类型

    bool  m_bIsVideoDecoderTypeChange;              //编码格式是否切换
    //rtp 媒体属性
    MEDIA_ATTR   m_VideoAttr;
    MEDIA_ATTR   m_AudioAttr;
};
#endif //MEDIA_CHANNELH

/********************************************************************
filename    :    FileSource.h
author      :    w00210470
created     :    2012/11/07
description :    文件流基类
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2013/03/20 初始版本
*********************************************************************/
#ifndef __FILE_SOURCE_H__
#define __FILE_SOURCE_H__

#ifdef WIN32
#include <Windows.h>
#endif
#include "vos.h"

#include <time.h>
#include <string>
#include "IVSPlayerDataType.h"

enum {
	SOURCE_STOPPED = 0,
	SOURCE_READING = 1,
    SOURCE_PAUSED  = 2
};

typedef struct 
{
    unsigned int  uPayloadType;      // 负载类型
	char *        pBuffer;          // 数据缓存 
	unsigned int  uSize;             // 数据帧大小
    unsigned int  uFrameType;		// 帧数据类型，SPS、PPS、IDR、P（视频数据有效）
	unsigned int  uTimeStamp;		// 时间戳
	unsigned long long ullTimeTick;	// 绝对时间戳
	IVS_UINT32	  uWidth;			// 视频宽
	IVS_UINT32	  uHeight;			// 视频高
	IVS_UINT32	  uGopSequence;	// GOP序列
} VIDEO_FRAME;

typedef struct {
    int            uPayloadType;
	unsigned char* pBuffer;    
	unsigned int   uSize;
	unsigned long  ulSR; 
	unsigned int   uChannels;
    unsigned int   uBits;
	unsigned int   uTimeStamp;
	unsigned long long ullTimeTick;
} AUDIO_FRAME;

typedef struct {
    int bVideo;
	int bEOS;
    union {
		VIDEO_FRAME Vinfo;
		AUDIO_FRAME Ainfo;
	};
} MEDIA_FRAME_INFO;

typedef IVS_VOID (__SDK_CALLBACK *FILE_DATA_CALLBACK) (MEDIA_FRAME_INFO *info, IVS_VOID *pUser);

class CFileSource
{
public:
	CFileSource(void);
	virtual ~CFileSource(void);
	
public:
	int  Init(const char *filename, bool bCycle = false);
	int  Start();
	void Stop();
	int  Pause();
	int  Resume();

	void SetFrameDataCallBack(FILE_DATA_CALLBACK callback, void *pUser);
	void SetExceptionCallBack(PLAYER_EXCEPTION_CALLBACK callback, void *pUser);
	bool IsRuning() const { return m_bRunFlag; } 

	void  SetPlayTimeBase(time_t base) { m_TmBase = base; }
	virtual int SetPlayTime(unsigned int offset) = 0;
	virtual int SetPlaySpeed(double fSpeed) = 0;
	virtual int SetPlayRange(unsigned int beg, unsigned int end) = 0;
	virtual int GetMediaInfo(unsigned int &fps, unsigned int &w, unsigned int &h) { return IVS_SUCCEED; };


    void SetDstAudioPayLoadType(unsigned int uDstAudioPayLoadType)
    {
        m_uDstAudioPayLoadType = uDstAudioPayLoadType;
    }

    unsigned int GetDstAudioPayLoadType() const
    {
        return m_uDstAudioPayLoadType;
    }

	bool IsCycle() const { return m_bCycle; } 

protected:
	virtual int  InitInternal()  = 0;
	virtual int  GetNextFrame(MEDIA_FRAME_INFO &info) = 0;
    virtual bool SourceStarted() const { return m_bStarted ; }

private:
	static unsigned int __SDK_CALL ReadThreadProc(void* lpParameter);

    virtual void ExcuteFrameDataCallBack(MEDIA_FRAME_INFO &info);
    virtual void ExcuteExceptionCallBack(long ExceptionCode);


protected:	
	time_t m_TmBase;
#ifdef WIN32
	char   m_file[MAX_PATH + 1];
#else
	char m_file[PATH_MAX+1];
#endif

    //文件名称，包含绝对路径
//     std::string m_strFileName;

    //是否循环
    bool m_bCycle;

    FILE_DATA_CALLBACK m_pfFrameCB;
    void  * m_pUserFrame;
    PLAYER_EXCEPTION_CALLBACK m_pfExceptionCB;
    void  * m_pUserException;

    VOICE_EXCEPTION_DATA m_stAudioExceptionData;

private:
	bool    m_bInited;
	bool    m_bStarted;
	int     m_nState;
	VOS_Thread* m_hReadThread;
	VOS_Thread* m_pThread;
	bool    m_bRunFlag;

    unsigned int m_uDstAudioPayLoadType;
};

#endif //__FILE_SOURCE_H__

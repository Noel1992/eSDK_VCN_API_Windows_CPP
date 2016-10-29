/*******************************************************************************
//  版权所有    2012 华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  RenderAPI.h
//  文件说明：  音视频渲染模块接口封装
//  版    本：  IVS V100R001C02
//  作    者：  w00210470
//  创建日期:   2012/10/31
//  修改标识：
//  修改说明：
*******************************************************************************/
#ifndef __IVS_PLAYER_RENDER_H__
#define __IVS_PLAYER_RENDER_H__

#include "IVSPlayerDataType.h"

/*****************************************************************************
函数名称：Render_LibInit
功能描述：初始化渲染库
输入参数：NA  
输出参数：NA
返 回 值：int
 *****************************************************************************/
int Render_LibInit();

/*****************************************************************************
 函数名称：Render_LibClean
 功能描述：清理渲染库
 输入参数：NA  
 输出参数：NA
 返 回 值：void
 *****************************************************************************/
void Render_LibClean();

/*****************************************************************************
 函数名称：Render_GetHandle
 功能描述：获取渲染句柄
 输入参数：NA  
 输出参数：NA
 返 回 值：IVS_HANDLE
 *****************************************************************************/
IVS_HANDLE Render_GetHandle();

/*****************************************************************************
 函数名称：Render_FreeHandle
 功能描述：释放渲染句柄
 输入参数：NA  
 输出参数：NA
 返 回 值：int
 *****************************************************************************/
int Render_FreeHandle(IVS_HANDLE h);

/*****************************************************************************
 函数名称：Render_StartPlay
 功能描述：启动渲染通道音视频播放
 输入参数：
	@IVS_HANDLE： 渲染通道句柄
 输出参数：NA
 返 回 值：void
 *****************************************************************************/
int Render_StartPlay(IVS_HANDLE h);

/*****************************************************************************
 函数名称：Render_StopPlay
 功能描述：停止渲染通道音视频播放
 输入参数：
    @IVS_HANDLE： 渲染通道句柄
 输出参数：NA
 返 回 值：void
 *****************************************************************************/
int Render_StopPlay(IVS_HANDLE h);

/*****************************************************************************
 函数名称：Render_PausePlay
 功能描述：暂停通道播放
 输入参数：
    @IVS_HANDLE： 渲染通道句柄
 输出参数：NA
 返 回 值：void
 *****************************************************************************/
int Render_PausePlay(IVS_HANDLE h);

/*****************************************************************************
 函数名称：Render_ResumePlay
 功能描述：恢复通道播放
 输入参数：
    @IVS_HANDLE： 渲染通道句柄
 输出参数：NA
 返 回 值：void
 *****************************************************************************/
int Render_ResumePlay(IVS_HANDLE h);

/*****************************************************************************
 函数名称：Render_StartPlayAudio
 功能描述：启动渲染通道音频播放
 输入参数：
        @IVS_HANDLE： 渲染通道句柄
 输出参数：NA
 返 回 值：void
 *****************************************************************************/
int Render_StartPlayAudio(IVS_HANDLE h);

/*****************************************************************************
 函数名称：Render_StopPlayAudio
 功能描述：停止渲染通道音频播放
 输入参数：
        @IVS_HANDLE： 渲染通道句柄
 输出参数：NA
 返 回 值：void
 *****************************************************************************/
int Render_StopPlayAudio(IVS_HANDLE h);

/*****************************************************************************
 函数名称：Render_StartPlayVideo
 功能描述：启动渲染通道视频播放
 输入参数：
        @IVS_HANDLE： 渲染通道句柄
 输出参数：NA
 返 回 值：void
 *****************************************************************************/
int Render_StartPlayVideo(IVS_HANDLE h);

/*****************************************************************************
 函数名称：Render_StopPlayVideo
 功能描述：停止渲染通道视频播放
 输入参数：
        @IVS_HANDLE： 渲染通道句柄
 输出参数：NA
 返 回 值：void
 *****************************************************************************/
int Render_StopPlayVideo(IVS_HANDLE h);

/*****************************************************************************
 函数名称：Render_Reset
 功能描述：重置渲染通道
 输入参数：
     @IVS_HANDLE： 渲染通道句柄
 输出参数：NA
 返 回 值：void
 *****************************************************************************/
int Render_Reset(IVS_HANDLE h);

/*****************************************************************************
 函数名称：Render_SetpForward/Render_StepBackward
 功能描述：单步前进
 输入参数：
	 @IVS_HANDLE： 渲染通道句柄
 输出参数：
     @pLeftPic: 队列剩余帧数量
 返 回 值：void
 *****************************************************************************/
int Render_StepForward(IVS_HANDLE h, unsigned int *pLeftPic);
int Render_StepBackward(IVS_HANDLE h, unsigned int *pLeftPic);

/*****************************************************************************
 函数名称：Render_SetVideoSampleRate
 功能描述：设置通道视频采样率参数
 输入参数：
      @IVS_HANDLE： 渲染通道句柄
	  @hz：            采样率
 输出参数：NA
 返 回 值：void
*****************************************************************************/
int Render_SetVideoSampleRate(IVS_HANDLE h, unsigned int hz);

/*****************************************************************************
 函数名称：Render_SetAudioSampleRate
 功能描述：设置通道音频采样率参数
 输入参数：
     @IVS_HANDLE： 渲染通道句柄
	 @hz：            采样率
 输出参数：NA
 返 回 值：void
*****************************************************************************/
int Render_SetAudioSampleRate(IVS_HANDLE h, unsigned int hz);

/*****************************************************************************
 函数名称：Render_SetMediaRefTime
 功能描述：设置基准时间戳， 用于音视频RTP时间戳同步
 输入参数：
     @IVS_HANDLE： 渲染通道句柄
	 @uAudioTS：      音频基准时间戳（RTP）
	 @uAudioNTP：     音频基准时间戳所对NTP时间
	 @uVideoTS：      视频基准时间戳（RTP）
	 @uVideoNTP：     视频基准时间戳对应NTP时间
 输出参数：NA
 返 回 值：void
*****************************************************************************/
int Render_SetMediaRefTime(IVS_HANDLE h, unsigned int uAudioTS, unsigned int uAudioNTP,
	   unsigned int uVideoTS, unsigned int uVideoNTP);

/*****************************************************************************
 函数名称：Render_InputVideoData
 功能描述：输入视频数据
 输入参数：
      @IVS_HANDLE： 渲染通道句柄
	  @pVideoFrame：    视频帧信息指针
 输出参数：NA
 返 回 值：void
*****************************************************************************/
int Render_InputVideoData(IVS_HANDLE h, const IVS_VIDEO_FRAME *pVideoFrame);

/*****************************************************************************
 函数名称：Render_InputAudioData
 功能描述：输入音频数据
 输入参数：
      @IVS_HANDLE： 渲染通道句柄
	  @pAudioFrame：   音频帧信息指针
 输出参数：NA
 返 回 值：void
****************************************************************************/
int Render_InputAudioData(IVS_HANDLE h, const IVS_AUDIO_FRAME *pAudioFrame);

/*****************************************************************************
 函数名称：Render_SetPlayEndCallBack
 功能描述：设置播放结束回调
 输入参数：
      @IVS_HANDLE： 渲染通道句柄
	  @cbFunc：        回调函数指针
	  @pUser：         用户数据
 输出参数：NA
 返 回 值：void
****************************************************************************/
int Render_SetPlayEndCallBack(IVS_HANDLE h, PLAYEND_CALLBACK cbFunc, void *pUser);

/*****************************************************************************
 函数名称：Render_SetDisplayCallback
 功能描述：设置播放回调
 输入参数：
      @IVS_HANDLE： 渲染通道句柄
	  @cbFunc：        回调函数指针
	  @pUser：         用户数据
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_SetDisplayCallback(IVS_HANDLE h, DISPLAY_CALLBACK cbFunc, void *pUser);

/*****************************************************************************
 函数名称：Render_SetDisplayCallback
 功能描述：设置播放回调
 输入参数：
      @IVS_HANDLE： 渲染通道句柄
	  @cbFunc：        回调函数指针
	  @pUser:          用户数据
 输出参数：NA
 返 回 值：int
****************************************************************************/
#ifdef WIN32
int Render_SetDrawCallBack(IVS_HANDLE h, DRAW_CALLBACK cbFunc, void *pUser);
int Render_SetDrawCallBackEx(IVS_HANDLE h, HWND hWnd, DRAW_CALLBACK callback, void *pUser);
#endif

/*****************************************************************************
 函数名称：Render_SetPlayDelay
 功能描述：设置播放延时
 输入参数：
      @IVS_HANDLE： 渲染通道句柄
	  @uDelayMs：   播放延时，单位毫秒
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_SetPlayDelay(IVS_HANDLE h, unsigned int uDelayMs);

/*****************************************************************************
 函数名称：Render_CaptureImage
 功能描述：捕获图片
 输入参数：
      @IVS_HANDLE： 渲染通道句柄
	  @nType：         保存文件类型, BMP 或 JPEG
	  @pFileName：     图片文件名称
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_CaptureImage(IVS_HANDLE h, int nType, const char *pFileName);

/*****************************************************************************
 函数名称：Render_GetMediaInfo
 功能描述：获取媒体信息
 输入参数：
      @IVS_HANDLE： 渲染通道句柄
	  @pInfo：         媒体信息缓存结构体
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_GetMediaInfo(const IVS_HANDLE h, MEDIA_INFO *pInfo); 

/*****************************************************************************
 函数名称：Render_GetPlayTime
 功能描述：获取播放时间（单位ms）
 输入参数：
      @IVS_HANDLE： 渲染通道句柄
	  @pTime：      时间缓存
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_GetPlayedTime(const IVS_HANDLE h, unsigned long long *pTime);

/*****************************************************************************
 函数名称：Render_GetCurrentFrameTick
 功能描述：获取当前帧时间戳（单位ms）
 输入参数：
      @IVS_HANDLE： 渲染通道句柄
	  @pTime：      媒体信息缓存结构体
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_GetCurrentFrameTick(const IVS_HANDLE h, unsigned long long *pTick);
int Render_SetPlayTime(IVS_HANDLE h, time_t tm);

/*****************************************************************************
 函数名称：Render_SetBufFrameNum
 功能描述：设置播放缓存帧数量
 输入参数：
      @IVS_HANDLE： 渲染通道句柄
	  @nFrames：       缓存帧数量
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_SetBufFrameNum(IVS_HANDLE h, unsigned int nFrames);

/*****************************************************************************
 函数名称：Render_GetBufFrameNum
 功能描述：获取缓存帧数量
 输入参数：
      @IVS_HANDLE： 渲染通道句柄
	  @pFrames：       缓存帧数量
 输出参数：NA
 返 回 值：unsigned int
****************************************************************************/
unsigned int Render_GetBufFrameNum(const IVS_HANDLE h);

#ifdef WIN32
/*****************************************************************************
 函数名称：Render_SetPlayWindow
 功能描述：设置播放窗口句柄
 输入参数：
      @IVS_HANDLE： 渲染通道句柄
	  @hWnd：          渲染窗口
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_SetPlayWindow(IVS_HANDLE h, HWND hWnd);

/*****************************************************************************
 函数名称：Render_AddPlayWindow
 功能描述：增加播放窗口（流复用）
 输入参数：
	@IVS_HANDLE： 渲染通道句柄
	@hWnd：          窗口句柄
	@nType：         渲染布局类型
	@pLayout：       窗口布局参数
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_AddPlayWindow(IVS_HANDLE h, HWND hWnd, int nType, void *pLayout);

/*****************************************************************************
 函数名称：Render_UpdateWindowLayout
 功能描述：修改窗口播放布局
 输入参数：
	@IVS_HANDLE： 渲染通道句柄
	@hWnd：       窗口句柄
	@nType：      渲染布局类型
	@pLayout：    窗口布局参数
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_UpdateWindowLayout(IVS_HANDLE h, HWND hWnd, int nType, void *pLayout);


/*****************************************************************************
 函数名称：Render_DelPlayWindow
 功能描述：删除播放窗口（流复用）
 输入参数：
	@IVS_HANDLE： 渲染通道句柄
	@hWnd：          窗口句柄
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_DelPlayWindow(IVS_HANDLE h, HWND hWnd);

#endif

/*****************************************************************************
 函数名称：Render_SetVideoDrawerType
 功能描述：设置视频渲染类型
 输入参数：
	@IVS_HANDLE : 渲染通道句柄
	@nType： 类型
	@lData： 类型参数
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_SetVideoDrawerType(IVS_HANDLE h, int nType, long lData);

/*****************************************************************************
 函数名称：Render_SetOverlayMode
 功能描述：设置Overlay模式参数
 输入参数：
	@IVS_HANDLE : 渲染通道句柄
	@bEnable    : 类型
	@ColorKey   : 类型参数
 输出参数：NA
 返 回 值：int
****************************************************************************/
#ifdef WIN32
int Render_SetOverlayMode(IVS_HANDLE h, int bEnable, COLORREF ColorKey);

/*****************************************************************************
 函数名称：Render_SetWaitForVertiBlank
 功能描述：使能垂直同步
 输入参数：
	@bEnable： 
 输出参数：NA
 返 回 值：NA
****************************************************************************/
int Render_SetWaitForVertiBlank(IVS_HANDLE h, int bEnable);

/*****************************************************************************
 函数名称：Render_SetPlayMode
 功能描述：设置播放模式
 输入参数：
	@IVS_HANDLE： 渲染通道句柄
	@mode：          模式
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_SetPlayMode(IVS_HANDLE h, unsigned int mode);


int Render_SetStreamMode(IVS_HANDLE h, int mode);

/*****************************************************************************
 函数名称：Render_SetAspectRatio
 功能描述：设置播放窗口比例
 输入参数：
	@IVS_HANDLE： 渲染通道句柄
	@nWidth ：       宽
	@nHeight：       高
 输出参数：NA
 返 回 值：int
****************************************************************************/
int	Render_SetAspectRatio(IVS_HANDLE h, int bFS, unsigned int nHeight, unsigned int nWidth);

/*****************************************************************************
 函数名称：Render_GetAspectRatio
 功能描述：获取播放窗口比例
 输入参数：
	@IVS_HANDLE： 渲染通道句柄
	@pWidth ：       宽
	@pHeight：       高
 输出参数：NA
 返 回 值：int
****************************************************************************/
int	Render_GetAspectRatio(IVS_HANDLE h, unsigned int *pHeight, unsigned int *pWidth, int *pFull);


/*****************************************************************************
 函数名称：Render_SyncAudio
 功能描述：设置音频同步时延
 输入参数：
	@IVS_HANDLE： 渲染通道句柄
	@nWidth ：       宽
	@nHeight：       高
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_SyncAudio(IVS_HANDLE h, int ms);

/*****************************************************************************
 函数名称：Render_SetAudioVolume
 功能描述：设置播放音量
 输入参数：
	@IVS_HANDLE： 渲染通道句柄
	@nVolume ：      音量值
 输出参数：NA
 返 回 值：int
****************************************************************************/
int	Render_SetAudioVolume(IVS_HANDLE h, unsigned int nVolume);

/*****************************************************************************
 函数名称：Render_GetAudioVolume
 功能描述：获取播放音量
 输入参数：
	@IVS_HANDLE： 渲染通道句柄
	@pVolume：       音量值
 输出参数：NA
 返 回 值：int
****************************************************************************/
int	Render_GetAudioVolume(const IVS_HANDLE h, unsigned int* pVolume);

/*****************************************************************************
 函数名称：Render_SetPlaySpeed
 功能描述：设置播放速度
 输入参数：
 @IVS_HANDLE： 渲染通道句柄
 @Speed：         播放速度
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_SetPlaySpeed(IVS_HANDLE h, double Speed);

/*****************************************************************************
 函数名称：Render_GetPlaySpeed
 功能描述：获取播放速度
 输入参数：
 @IVS_HANDLE： 渲染通道句柄
 @pSpeed：        播放速度指针
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_GetPlaySpeed(const IVS_HANDLE h, double *pSpeed);

/*****************************************************************************
 函数名称：Render_SetPlayMode
 功能描述：设置播放模式
 输入参数：
     @IVS_HANDLE： 渲染通道句柄
     @mode：          播放模式
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_SetPlayMode(IVS_HANDLE h, RENDER_MODE mode);

/*****************************************************************************
 函数名称：Render_GetLastError
 功能描述：获取播放通道错误信息
 输入参数：
     @IVS_HANDLE： 渲染通道句柄
     @pError：        错误码缓存指针
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_GetLastError(const IVS_HANDLE h, unsigned long *pError);

/*****************************************************************************
 函数名称：Render_SetExceptionCallback
 功能描述：获取播放通道错误信息
 输入参数：
     @IVS_HANDLE： 渲染通道句柄
     @cb：         回调函数
	 @pUserData：  用户数据
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_SetExceptionCallback(IVS_HANDLE h, PLAYER_EXCEPTION_CALLBACK cb, void *pUserData);

/*****************************************************************************
 函数名称：Render_GetStatisInfo
 功能描述：获取播放通道统计信息
 输入参数：
     @IVS_HANDLE： 渲染通道句柄
     @pStatisInfo：统计信息缓存
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_GetStatisInfo(const IVS_HANDLE h, RENDER_STATIS *pStatisInfo);

/*****************************************************************************
 函数名称：Render_GetStatisInfo
 功能描述：清空缓存数据;
 输入参数：
     @IVS_HANDLE： 渲染通道句柄
 输出参数：NA
 返 回 值：int
****************************************************************************/
int Render_ResetBuffer(IVS_HANDLE h);

int Render_SetResetFlag(IVS_HANDLE h, bool bIsReset);

/*****************************************************************************
 函数名称：Render_GetImageDIB32
 功能描述：抓图;
 输入参数：
     @IVS_HANDLE： 渲染通道句柄
	 @pWidth:      宽度值指针
	 @pHeight:     高度值指针
	 @ppBits:      位图数据地址指针
 输出参数：NA
 返 回 值：int
****************************************************************************/
int  Render_GetImageDIB32(IVS_HANDLE h, unsigned long *pWidth, unsigned long *pHeight, void **ppBits);
void Render_PutImageDIB32(const IVS_HANDLE h, void *pBits);

#endif

#endif  /* end of __IVS_PLAYER_RENDER_H__ */






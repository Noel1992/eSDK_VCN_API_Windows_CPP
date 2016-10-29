/********************************************************************
filename    : DecoderAPI.h
author      : z90003203
created     : 2012/11/23
description : 音视频解码模块接口封装
copyright   : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     : 2012/10/31 初始版本
*********************************************************************/

#ifndef __DECODER_API__
#define __DECODER_API__

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include "IVSPlayerDataType.h"


/*****************************************************************************
函数名称：Decoder_GetHandle
功能描述：创建解码器，成功返回解码器句柄
输入参数：NA  
输出参数：NA
返 回 值：成功：解码器句柄 失败：NULL
*****************************************************************************/
IVS_HANDLE Decoder_GetHandle();


/*****************************************************************************
函数名称：Decoder_FreeHandle
功能描述：释放指定解码器资源
输入参数：hDecoder Decoder_GetHandle中创建的解码器句柄
输出参数：NA
返 回 值：IVS_VOID
*****************************************************************************/
IVS_VOID Decoder_FreeHandle(IVS_HANDLE hDecoder);


/*****************************************************************************
函数名称：Decoder_OpenStream
功能描述：打开流
输入参数：hDecoder     解码器句柄
          pVideoParam  视频参数
          pAudioParam  音频参数
          uBufferSize  一帧的缓冲大小
          uBufferCount 帧缓冲个数
输出参数：NA
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_OpenStream(IVS_HANDLE hDecoder,
                             VIDEO_PARAM *pVideoParam,
                             AUDIO_PARAM *pAudioParam,
                             IVS_UINT32 uLowerLimit,
                             IVS_UINT32 uUpperLimit,
                             IVS_UINT32 uBufferCount = DECODER_FRAME_COUNT,
							 IVS_UINT32 uExBufCount  = VIDEO_BUF_EX_COUNT);


/*****************************************************************************
函数名称：Decoder_CloseStream
功能描述：关闭流
输入参数：hDecoder     解码器句柄
输出参数：NA
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_CloseStream(IVS_HANDLE hDecoder);


/*****************************************************************************
函数名称：Decoder_SetDecoderCB
功能描述：设置解码后数据回调函数
输入参数：hDecoder     解码器句柄
          fnDecoderCB  YUV回调函数
          pUserParam   用户参数
输出参数：NA
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_SetDecoderCB(IVS_HANDLE hDecoder, DECODER_CALLBACK fnDecoderCB, void *pUserParam);


/*****************************************************************************
函数名称：Decoder_Start
功能描述：开始解码
输入参数：hDecoder     解码器句柄
          hWnd         窗口句柄，此版本中暂不支持显示解码内部渲染
输出参数：NA
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_Start(IVS_HANDLE hDecoder, HWND hWnd = NULL);


/*****************************************************************************
函数名称：Decoder_Stop
功能描述：停止解码
输入参数：hDecoder     解码器句柄  
输出参数：NA
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_Stop(IVS_HANDLE hDecoder);


/*****************************************************************************
函数名称：Decoder_SendVideoFrame
功能描述：送一帧视频帧到解码库
输入参数：hDecoder     解码器句柄
          pRawFrameInfo  未解码的视频帧信息
          pFrame        帧地址
          uFrameSize    帧长度
输出参数：NA
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_SendVideoFrame(IVS_HANDLE hDecoder,
                                 IVS_RAW_FRAME_INFO *pRawFrameInfo,
                                 IVS_CHAR* pFrame,
                                 IVS_UINT32 uFrameSize);


/*****************************************************************************
函数名称：Decoder_SendAudioFrame
功能描述：送一帧音频帧到解码库
输入参数：hDecoder     解码器句柄
          pRawFrameInfo  未解码的音频帧信息
          pFrame        帧地址
          uFrameSize    帧长度
输出参数：NA
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_SendAudioFrame(IVS_HANDLE hDecoder,
                                 IVS_RAW_FRAME_INFO *pRawFrameInfo,
                                 IVS_CHAR* pFrame,
                                 IVS_UINT32 uFrameSize);



/*****************************************************************************
函数名称：Decoder_SetExceptionCallback
功能描述：设置解码库事件回调函数
输入参数：hDecoder     解码器句柄
          fnDecExceptionCB 解码事件回调函数
          pUserParam   用户参数
输出参数：NA
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_SetExceptionCallback(IVS_HANDLE hDecoder, PLAYER_EXCEPTION_CALLBACK fnDecExceptionCB, void *pUserParam);


/*****************************************************************************
函数名称：Decoder_GetSourceBufferRemain
功能描述：获取未解码帧缓冲区剩余的帧数
输入参数：hDecoder     解码器句柄
输出参数：iBufferCount 帧缓冲个数
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_GetSourceBufferRemain(IVS_HANDLE hDecoder, IVS_UINT32 &uBufferCount);


/*****************************************************************************
函数名称：Decoder_SetWaterMarkState
功能描述：设置水印校验开始/停止
输入参数：bIsCheckWaterMark 开始为true,停止为false
输出参数：无
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_SetWaterMarkState(IVS_HANDLE hDecoder, IVS_BOOL bIsCheckWaterMark);

/*****************************************************************************
函数名称：Decoder_ResetBuffer
功能描述：清空缓存的数据;
输入参数：hDecoder     解码器句柄
输出参数：无
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_ResetBuffer(IVS_HANDLE hDecoder);

/*****************************************************************************
函数名称：Decoder_SetDelayFrameNum
功能描述：设置延时解码的帧数;
输入参数：hDecoder       解码器句柄
          uDelayFrameNum 延时解码的帧数
输出参数：无
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_SetDelayFrameNum(IVS_HANDLE hDecoder, IVS_UINT32 uDelayFrameNum);

/*****************************************************************************
函数名称：Decoder_StepFrames
功能描述：设置单步解码的帧数;
输入参数：hDecoder       解码器句柄
          uStepFrameNum 单步解码的帧数
输出参数：无
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_StepFrames(IVS_HANDLE hDecoder, IVS_UINT32 uStepFrameNum);

/*****************************************************************************
函数名称：Decoder_SetEventCallback
功能描述：设置解码库事件回调函数
输入参数：hDecoder     解码器句柄
          fnDecEventCB 解码事件回调函数
          pUserParam   用户参数
输出参数：NA
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_SetEventCallback(IVS_HANDLE hDecoder, PLAYER_EVENT_CALLBACK fnDecEventCB, void *pUserParam);


/*
 * Decoder_MarkStreamEnd
 * 解码机标记流结束
 */
IVS_INT32 Decoder_MarkStreamEnd(IVS_HANDLE hDecoder, IVS_BOOL bIsEnd);



/*
 * add by w00210470; test
 */
IVS_BOOL Decoder_IsCancel(IVS_HANDLE hDecoder);

IVS_VOID Decoder_StopNotifyBuffEvent(IVS_HANDLE hDecoder);

IVS_VOID Decoder_ReSetFlowContrlFlag(IVS_HANDLE hDecoder);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // __DECODER_API__

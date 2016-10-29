/********************************************************************
filename    : DecoderAPI.cpp
author      : z90003203
created     : 2012/11/23
description : 音视频解码模块接口封装
copyright   : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     : 2012/10/31 初始版本
*********************************************************************/

#include "DecoderAPI.h"
#include "DecodeHandler.h"
#include "IVSPlayerDataType.h"
#include "IVSCommon.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus 

/*****************************************************************************
函数名称：Decoder_GetHandle
功能描述：创建解码器，成功返回解码器句柄
输入参数：NA  
输出参数：NA
返 回 值：IVS_HANDLE
*****************************************************************************/
IVS_HANDLE Decoder_GetHandle()
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_GetHandle", "Enter");
    DecodeHandler* pDecodeHandler = NULL;

    try
    {
        pDecodeHandler = new DecodeHandler();
    }
    catch (...)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder GetHandle",
            "Decoder Create new DecodeHandler throw exception!");
		pDecodeHandler = NULL;
    }

    if(NULL == pDecodeHandler)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder GetHandle",
                "Decoder Create pDecodeHandler error!");

        return NULL;
    }

    int iRet = pDecodeHandler->CreateDecoder();
    if(IVS_SUCCEED != iRet)
    {
        IVS_DELETE(pDecodeHandler);

        IVS_LOG(IVS_LOG_ERR, "Decoder GetHandle",
                "Decoder Create CreateDecoder error!");

        return NULL;
    }

    IVS_HANDLE phDecodeHandler = reinterpret_cast<IVS_HANDLE>(pDecodeHandler);
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_GetHandle", "Leave Handle 0x%x", phDecodeHandler);

    return phDecodeHandler;
}


/*****************************************************************************
函数名称：Decoder_FreeHandle
功能描述：释放指定解码器资源
输入参数：hDecoder Decoder_GetHandle中创建的解码器句柄
输出参数：NA
返 回 值：IVS_INT32
*****************************************************************************/
IVS_VOID Decoder_FreeHandle(IVS_HANDLE hDecoder)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_FreeHandle", "Enter Handle 0x%x", hDecoder);
    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder FreeHandle",
                "Decoder Release hDecoder error!");
        return;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int nRet = pDecodeHandler->Stop();
    if(IVS_SUCCEED != nRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder FreeHandle", "pobjDecodeHandler Stop error!");
    }

    nRet = pDecodeHandler->ReleaseDecoder();
    if(IVS_SUCCEED != nRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder FreeHandle", "pobjDecodeHandler ReleaseDecoder error!");
    }

    IVS_DELETE(pDecodeHandler);
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_FreeHandle", "Leave");
}


/*****************************************************************************
函数名称：Decoder_OpenStream
功能描述：打开流
输入参数：hDecoder     解码器句柄
          pVideoParam  视频参数
          pAudioParam  音频参数
          iBufferSize  一帧的缓冲大小
          iBufferCount 帧缓冲个数
输出参数：NA
返 回 值：IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_OpenStream(IVS_HANDLE hDecoder,
                             VIDEO_PARAM* pVideoParam,
                             AUDIO_PARAM* pAudioParam,
                             IVS_UINT32 uLowerLimit,
                             IVS_UINT32 uUpperLimit,
							 IVS_UINT32 uBufferCount,
							 IVS_UINT32 uExBufCount)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_OpenStream",
			"Enter Handle 0x%x VideoParam 0x%x AudioParam 0x%x Limit %u %u BufferCount %u",
			hDecoder, pVideoParam, pAudioParam, uLowerLimit, uUpperLimit, uBufferCount);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder OpenStream",
				"Decoder OpenStream hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    IVS_INT32 iRet = pDecodeHandler->OpenStream(pVideoParam,
                                                pAudioParam,
                                                uLowerLimit,
                                                uUpperLimit,
												uBufferCount, 
												uExBufCount);

    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder OpenStream",
            "Decoder OpenStream OpenStream error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_OpenStream", "Leave");
    return IVS_SUCCEED;
}


/*****************************************************************************
函数名称：Decoder_CloseStream
功能描述：关闭流
输入参数：hDecoder     解码器句柄
输出参数：NA
返 回 值：IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_CloseStream(IVS_HANDLE hDecoder)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_CloseStream", "Enter Handle 0x%x", hDecoder);
    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder CloseStream",
            "Decoder CloseStream hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    IVS_INT32 iRet = pDecodeHandler->CloseStream();
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder CloseStream",
            "Decoder CloseStream CloseStream error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_CloseStream", "Leave");
    return IVS_SUCCEED;
}


/*****************************************************************************
函数名称：Decoder_SetDecoderCB
功能描述：设置解码后数据回调函数
输入参数：hDecoder     解码器句柄
          fnDecoderCB  YUV回调函数
          pUserParam   用户参数
输出参数：NA
返 回 值：IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_SetDecoderCB(IVS_HANDLE hDecoder,
                               DECODER_CALLBACK fnDecoderCB,
                               void* pUserParam)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetDecoderCB",
        "Enter Handle 0x%x DECODER_CALLBACK 0x%x UserParam 0x%x",
        hDecoder, fnDecoderCB, pUserParam);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetDecoderCB",
            "Decoder SetDecoderCB hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    IVS_INT32 iRet = pDecodeHandler->SetDecoderCB(fnDecoderCB, pUserParam);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetDecoderCB",
            "Decoder SetDecoderCB SetDecoderCB error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetDecoderCB", "Leave");
    return IVS_SUCCEED;
}



/*****************************************************************************
 函数名称：Decoder_Start
 功能描述：开始解码
 输入参数：hDecoder     解码器句柄  
 输出参数：NA
 返 回 值：IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_Start(IVS_HANDLE hDecoder, HWND hWnd)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_Start", "Enter Handle 0x%x HWND 0x%x",
            hDecoder, hWnd);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder Start",
            "Decoder Start hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    IVS_INT32 iRet = pDecodeHandler->Start(hWnd);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder Start",
            "Decoder Start Start error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_Start", "Leave");
    return IVS_SUCCEED;
}


/*****************************************************************************
函数名称：Decoder_Stop
功能描述：停止解码
输入参数：hDecoder     解码器句柄  
输出参数：NA
返 回 值：IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_Stop(IVS_HANDLE hDecoder)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_Stop", "Enter Handle 0x%x", hDecoder);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder Stop",
            "Decoder Stop hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    IVS_INT32 iRet = pDecodeHandler->Stop();
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder Stop",
            "Decoder Stop Stop error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_Stop", "Leave");
    return IVS_SUCCEED;
}


/*****************************************************************************
函数名称：Decoder_SendVideoFrame
功能描述：送一帧视频帧到解码库
输入参数：hDecoder     解码器句柄
          pRawFrameInfo  未解码的帧信息
          pFrame        帧地址
          uFrameSize    帧长度
输出参数：NA
返 回 值：IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_SendVideoFrame(IVS_HANDLE hDecoder,
                                 IVS_RAW_FRAME_INFO* pRawFrameInfo,
                                 IVS_CHAR* pFrame,
                                 IVS_UINT32 uFrameSize)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SendVideoFrame",
            "Enter Handle 0x%x pRawFrameInfo 0x%x pFrame 0x%x uFrameSize %u",
            hDecoder, pRawFrameInfo, pFrame, uFrameSize);

    if(NULL == hDecoder
       || NULL == pFrame
       || 0 == uFrameSize)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SendVideoFrame",
            "Decoder SendVideoFrame hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    IVS_INT32 iRet = pDecodeHandler->SendVideoFrame(pRawFrameInfo,
                                                    pFrame,
                                                    uFrameSize);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SendVideoFrame",
            "Decoder SendVideoFrame SendVideoFrame error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SendVideoFrame", "Leave");
    return IVS_SUCCEED;
}


/*****************************************************************************
函数名称：Decoder_SendAudioFrame
功能描述：送一帧音频帧到解码库
输入参数：hDecoder      解码器句柄
          pRawFrameInfo 未解码的帧信息
          pFrame        帧地址
          uFrameSize    帧长度
输出参数：NA
返 回 值：IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_SendAudioFrame(IVS_HANDLE hDecoder,
                                 IVS_RAW_FRAME_INFO* pRawFrameInfo,
                                 IVS_CHAR* pFrame,
                                 IVS_UINT32 uFrameSize)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SendAudioFrame",
        "Enter Handle 0x%x pRawFrameInfo 0x%x pFrame 0x%x uFrameSize %u",
        hDecoder, pRawFrameInfo, pFrame, uFrameSize);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SendAudioFrame",
            "Decoder SendAudioFrame hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    IVS_INT32 iRet = pDecodeHandler->SendAudioFrame(pRawFrameInfo,
                                                    pFrame,
                                                    uFrameSize);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SendAudioFrame",
            "Decoder SendAudioFrame SendAudioFrame error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SendAudioFrame", "Leave");
    return IVS_SUCCEED;
}


/*****************************************************************************
函数名称：Decoder_SetDecEventCB
功能描述：设置解码库事件回调函数
输入参数：hDecoder     解码器句柄
          fnDecEventCB 解码事件回调函数
          pUserParam   用户参数
输出参数：NA
返 回 值：IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_SetExceptionCallback(IVS_HANDLE hDecoder, PLAYER_EXCEPTION_CALLBACK fnDecExceptionCB, void* pUserParam)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetExceptionCallback",
            "Enter Handle 0x%x PLAYER_EXCEPTION_CALLBACK 0x%x UserParam 0x%x",
            hDecoder, fnDecExceptionCB, pUserParam);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetExceptionCallback",
            "Decoder SetExceptionCallback hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int iRet = pDecodeHandler->SetExceptionCallback(fnDecExceptionCB, pUserParam);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetExceptionCallback",
            "Decoder SetExceptionCallback SetExceptionCallback error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetExceptionCallback", "Leave");
    return IVS_SUCCEED;
}


/*****************************************************************************
函数名称：Decoder_GetSourceBufferRemain
功能描述：获取未解码帧缓冲区剩余的帧数
输入参数：hDecoder     解码器句柄
输出参数：uBufferCount 帧缓冲个数
返 回 值：IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_GetSourceBufferRemain(IVS_HANDLE hDecoder, IVS_UINT32 &uBufferCount)
{
    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder GetSourceBufferRemain",
            "Decoder GetSourceBufferRemain hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int iRet = pDecodeHandler->GetSourceBufferRemain(uBufferCount);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder GetSourceBufferRemain",
            "Decoder GetSourceBufferRemain GetSourceBufferRemain error!");
        return iRet;
    }

    return IVS_SUCCEED;
}//lint !e954

/*****************************************************************************
函数名称：Decoder_SetWaterMarkState
功能描述：设置水印校验开始/停止
输入参数：bIsCheckWaterMark 开始为true,停止为false
输出参数：无
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_SetWaterMarkState(IVS_HANDLE hDecoder, IVS_BOOL bIsCheckWaterMark)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetWaterMarkState",
            "Enter Handle 0x%x WaterMark Flag %d", hDecoder, bIsCheckWaterMark);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetWaterMarkState",
            "Decoder SetWaterMarkState hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int iRet = pDecodeHandler->SetWaterMarkState(bIsCheckWaterMark);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetWaterMarkState",
            "Decoder SetWaterMarkState SetWaterMarkState error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetWaterMarkState", "Leave");
    return IVS_SUCCEED;
}

/*****************************************************************************
函数名称：Decoder_ResetBuffer
功能描述：清空缓存的数据;
输入参数：hDecoder     解码器句柄
输出参数：无
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_ResetBuffer(IVS_HANDLE hDecoder)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_ResetBuffer", "Enter Handle 0x%x", hDecoder);
    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder ResetBuffer",
            "Decoder ResetBuffer hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int iRet = pDecodeHandler->ResetBuffer();
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder ResetBuffer",
            "Decoder ResetBuffer ResetBuffer error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_ResetBuffer", "Leave");
    return IVS_SUCCEED;
}

/*****************************************************************************
函数名称：Decoder_SetDelayFrameNum
功能描述：设置延时解码的帧数;
输入参数：hDecoder       解码器句柄
          uDelayFrameNum 延时解码的帧数
输出参数：无
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_SetDelayFrameNum(IVS_HANDLE hDecoder, IVS_UINT32 uDelayFrameNum)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetDelayFrameNum", "Enter Handle 0x%x", hDecoder);
    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetDelayFrameNum",
            "Decoder SetDelayFrameNum hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int iRet = pDecodeHandler->SetDelayFrameNum(uDelayFrameNum);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetDelayFrameNum",
            "Decoder SetDelayFrameNum SetDelayFrameNum error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetDelayFrameNum", "Leave");
    return IVS_SUCCEED;
}

/*****************************************************************************
函数名称：Decoder_StepFrames
功能描述：设置单步解码的帧数;
输入参数：hDecoder       解码器句柄
          uStepFrameNum 单步解码的帧数
输出参数：无
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
IVS_INT32 Decoder_StepFrames(IVS_HANDLE hDecoder, IVS_UINT32 uStepFrameNum)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_StepFrames", "Enter Handle 0x%x", hDecoder);
    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder StepFrames",
            "Decoder StepFrames hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int iRet = pDecodeHandler->StepFrames(uStepFrameNum);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder StepFrames",
            "Decoder StepFrames StepFrames error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_StepFrames", "Leave");
    return IVS_SUCCEED;
}

/*****************************************************************************
函数名称：Decoder_SetDecEventCB
功能描述：设置解码库事件回调函数
输入参数：hDecoder     解码器句柄
          fnDecEventCB 解码事件回调函数
          pUserParam   用户参数
输出参数：NA
返 回 值：IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_SetEventCallback(IVS_HANDLE hDecoder, PLAYER_EVENT_CALLBACK fnDecEventCB, void* pUserParam)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetEventCallback",
            "Enter Handle 0x%x PLAYER_EVENT_CALLBACK 0x%x UserParam 0x%x",
            hDecoder, fnDecEventCB, pUserParam);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetEventCallback",
            "Decoder SetEventCallback hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int iRet = pDecodeHandler->SetEventCallback(fnDecEventCB, pUserParam);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetEventCallback",
            "Decoder SetEventCallback SetEventCallback error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetEventCallback", "Leave");
    return IVS_SUCCEED;
}

IVS_INT32 Decoder_MarkStreamEnd(IVS_HANDLE hDecoder, IVS_BOOL bIsEnd)
{
	if(NULL == hDecoder)
	{
		IVS_LOG(IVS_LOG_ERR, "Decoder MarkStreamEnd", "hDecoder = NULL");
		return IVS_PARA_INVALID;
	}

	DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);
	return pDecodeHandler->MarkEoS(!!bIsEnd);
}

IVS_BOOL Decoder_IsCancel(IVS_HANDLE hDecoder)
{
    if (NULL == hDecoder)
	{
        return IVS_TRUE;
	}

	DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);
	return  (int)pDecodeHandler->IsCancel();
}

IVS_VOID Decoder_StopNotifyBuffEvent(IVS_HANDLE hDecoder)
{
	if (NULL == hDecoder)
	{
		return;  // IVS_TRUE;
	}

	DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);
	pDecodeHandler->StopNotifyBuffEvent();
}

IVS_VOID Decoder_ReSetFlowContrlFlag(IVS_HANDLE hDecoder)
{
    if (NULL == hDecoder)
    {
        return;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);
    pDecodeHandler->ReSetFlowContrlFlag();
}

#ifdef __cplusplus
}
#endif // __cplusplus


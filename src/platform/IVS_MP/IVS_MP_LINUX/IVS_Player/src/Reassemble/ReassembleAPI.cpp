/********************************************************************
filename    : ReassembleAPI.h
author      : c00206592
created     : 2012/11/17
description : 组帧方法API头文件
copyright   : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     : 2012/11/17 初始版本
 *********************************************************************/

#include "PacketReassemble.h"
#include "H264Reassemble.h"
#include "MjpegReassemble.h"
#include "HevcReassemble.h"
#include "ReassembleAPI.h"
#include "SVACReassemble.h"
#include "IVSCommon.h"
#ifdef WIN32
#include "..\..\inc\common\log\ivs_log.h"
#else
#include "ivs_log.h"
#endif 

#define CHECK_HANDLE_NULL(p, f)  do{ if(!(p)) { return IVS_FAIL; }}while (0)  //lint !e1960                                

/*****************************************************************************
 函数名称：Reass_GetHandle
 功能描述：获取渲染句柄
 输入参数：nDecoderType 编码类型(VIDEO_DEC_TYPE)
 输出参数：NA
 返 回 值：RENDER_HANDLE
*****************************************************************************/
IVS_HANDLE Reass_GetHandle(IVS_INT32 iDecoderType)
{
    IVS_TRACE();

    CPacketReassemble* reassHandle = NULL;
    switch (iDecoderType)
    {
    case VIDEO_DEC_H264:
        (void)IVS_NEW((CH264Reassemble * &)reassHandle);
        IVS_LOG(IVS_LOG_DEBUG, "Reass_GetHandle", "Init H264Reassemble");
        break;

    case VIDEO_DEC_MJPEG:
        (void)IVS_NEW((CMjpegReassemble * &)reassHandle);
        IVS_LOG(IVS_LOG_DEBUG, "Reass_GetHandle", "Init MjpegReassemble");
        break;

	case VIDEO_DEC_H265:
		(void)IVS_NEW((CHevcReassemble * &)reassHandle);
		IVS_LOG(IVS_LOG_DEBUG, "Reass_GetHandle", "Init HevcReassemble");
		break;

	case VIDEO_DEC_SVAC:
		(void)IVS_NEW((CSvacReassemble * &)reassHandle);
		IVS_LOG(IVS_LOG_DEBUG, "Reass_GetHandle", "Init SVACReassemble");
		break;

    default:
        break;
    }

    if (NULL == reassHandle)
    {
        IVS_LOG(IVS_LOG_ERR, "Reass_GetHandle",
                "PacketReassemble initial assemble error, Create assemble failed.");
        return NULL;
    }

    if (IVS_FAIL == reassHandle->Init())
    {
        IVS_DELETE(reassHandle);
        reassHandle = NULL;
    }

    return reassHandle;
}//lint !e1788

/*****************************************************************************
 函数名称：Render_FreeHandle
 功能描述：释放渲染句柄
 输入参数：IVS_HANDLE 句柄
 输出参数：NA
 返 回 值：void
*****************************************************************************/
void Reass_FreeHandle(IVS_HANDLE h)
{
    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;
    IVS_DELETE(packeReassemble);
}

/*****************************************************************************
 函数名称：Reass_InsertPacket
 功能描述：插入RTP包
 输入参数：
 IVS_HANDLE：     渲染通道句柄
 pBuf：           数据流
 ulBufLen：       数据长度
 输出参数：NA
 返 回 值：void
*****************************************************************************/
int Reass_InsertPacket(IVS_HANDLE h, IVS_CHAR* pBuf, IVS_UINT32 ulBufLen)
{
    CHECK_HANDLE_NULL(h, "Reass_InsertPacket");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    return packeReassemble->InsertPacket(pBuf, ulBufLen);
}

/*****************************************************************************
 函数名称：Reass_GetVideoCodeRate
 功能描述：获取视频码率数据
 输入参数：
 IVS_HANDLE     ： 渲染通道句柄
 uVideoCodeRate :  实时码率
 uAvgVideoCodeRate：平均码率
 输出参数：NA
 返 回 值：
 IVS_SUCCEED   ：  成功
 IVS_FAIL      ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int Reass_GetVideoCodeRate(IVS_HANDLE h, IVS_UINT32* uVideoCodeRate, IVS_UINT32* uAvgVideoCodeRate)
{
    CHECK_HANDLE_NULL(h, "Reass_GetVideoCodeRate");

    CHECK_HANDLE_NULL(uVideoCodeRate, "Reass_GetVideoCodeRate");

    CHECK_HANDLE_NULL(uAvgVideoCodeRate, "Reass_GetVideoCodeRate");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    *uVideoCodeRate = packeReassemble->GetVideoCodeRate();

    *uAvgVideoCodeRate = packeReassemble->GetAvgVideoCodeRate();

    return IVS_SUCCEED;
}

/*****************************************************************************
 函数名称：Reass_GetAudioCodeRate
 功能描述：获取音频码率数据
 输入参数：
 IVS_HANDLE： 渲染通道句柄
 uAudioCodeRate:  实时码率
 uAvgAudioCodeRate :平均码率
 输出参数：NA
 返 回 值：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int Reass_GetAudioCodeRate(IVS_HANDLE h, IVS_UINT32* uAudioCodeRate, IVS_UINT32* uAvgAudioCodeRate)
{
    CHECK_HANDLE_NULL(h, "Reass_GetAudioCodeRate");

    CHECK_HANDLE_NULL(uAudioCodeRate, "Reass_GetAudioCodeRate");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    *uAudioCodeRate = packeReassemble->GetAudioCodeRate();

    *uAvgAudioCodeRate = packeReassemble->GetAvgAudioCodeRate();

    return IVS_SUCCEED;
}

/*****************************************************************************
 函数名称：Reass_GetFPS
 功能描述：获取视频帧率
 输入参数：
 IVS_HANDLE ： 渲染通道句柄
 uFps       :  码率
 输出参数   ：NA
 返 回 值   ：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int  Reass_GetFPS(IVS_HANDLE h, IVS_UINT32* uFps)
{
    CHECK_HANDLE_NULL(h, "Reass_GetFPS");

    CHECK_HANDLE_NULL(uFps, "Reass_GetFPS");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    *uFps = packeReassemble->GetFPS();

    return IVS_SUCCEED;
}//lint !e954

/*****************************************************************************
 函数名称：Reass_GetLostPacketRate
 功能描述：获取平均丢包率
 输入参数：
 @IVS_HANDLE     ： 渲染通道句柄
 @fLostPacketRate:  丢包率
 输出参数：NA
 返 回 值：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int  Reass_GetLostPacketRate(IVS_HANDLE h, IVS_FLOAT* pfLostPacketRate)
{
    CHECK_HANDLE_NULL(h, "Reass_GetLostPacketRate");

    CHECK_HANDLE_NULL(pfLostPacketRate, "Reass_GetLostPacketRate");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    *pfLostPacketRate = packeReassemble->GetLostPacketRate();
    return IVS_SUCCEED;
}

/*****************************************************************************
 函数名称：Reass_ReSetReassemble
 功能描述：发生拖动进度条等动作的时候，调用重设方法，清除残余数据
 输入参数：
 IVS_HANDLE： 渲染通道句柄
 输出参数：NA
 返 回 值：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int Reass_ReSetReassemble(IVS_HANDLE h)
{
    CHECK_HANDLE_NULL(h, "Reass_ReSetReassemble");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    return packeReassemble->ReSetReassemble();
}

void Reass_SetResetFlag(IVS_HANDLE h, bool bIsReset)
{
	if (NULL == h)
	{
		IVS_LOG(IVS_LOG_ERR, "Reass_SetResetFlag h is null.", "");
		return;
	}

	CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

	packeReassemble->SetResetFlag(bIsReset);
}
/*****************************************************************************
 函数名称：Reass_SetRawFrameLimit
 功能描述：当发生丢包的时候，可以调用该方法设置组帧缓存区大小
 输入参数：
 IVS_HANDLE： 渲染通道句柄
 uSize     ： 缓存大小（建议值在3~8）
 输出参数：NA
 返 回 值：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int  Reass_SetRawFrameLimit(IVS_HANDLE h, IVS_UINT32 uSize)
{
    CHECK_HANDLE_NULL(h, "Reass_SetRawFrameLimit");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    packeReassemble->SetFrameCacheLimit(uSize);

    return IVS_SUCCEED;
}

/*****************************************************************************
 函数名称：Reass_SetWaterMarkState
 功能描述：设置是否启动水印校验
 输入参数：
 IVS_HANDLE： 渲染通道句柄
 IVS_BOOL  ： 是否启动水印校验
 输出参数：NA
 返 回 值：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int   Reass_SetWaterMarkState(IVS_HANDLE h, IVS_BOOL bIsStart)
{
    CHECK_HANDLE_NULL(h, "Reass_SetWaterMarkState");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    return packeReassemble->SetWaterMarkState(bIsStart);
}

/*****************************************************************************
 函数名称：Reass_SetPlayBackMode
 功能描述：设置视频回放模式
 输入参数：
 IVS_HANDLE： 渲染通道句柄
 IVS_BOOL  ： 是否启动回放组帧模式 ture:启动；false:不启动
 输出参数：NA
 返 回 值：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int   Reass_SetPlayBackMode(IVS_HANDLE h, IVS_BOOL bIsPlayBackMode)
{
    CHECK_HANDLE_NULL(h, "Reass_SetPlayBackMode");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    packeReassemble->SetPlayBackMode(bIsPlayBackMode);

	return IVS_SUCCEED;
}

/*****************************************************************************
 函数名称：Reass_SetDecryptDataKeyAndMode
 功能描述：设置视频解密密钥
 输入参数：
 IVS_HANDLE  ： 渲染通道句柄
 pSecretKey  ： 解密密钥
 ulKeyLen    :  密钥长度
 iSecretKeyType ：密钥类型 (参考 DECRYP_TYPE )
 输出参数：NA
 返 回 值：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int  Reass_SetDecryptDataKeyAndMode(IVS_HANDLE h, const IVS_CHAR* pSecretKey, IVS_ULONG ulKeyLen,
                                         IVS_INT32 iSecretKeyType)
{
    CHECK_HANDLE_NULL(h, "Reass_SetVideoDecryptDataKeyAndMode h");
    //CHECK_HANDLE_NULL(pSecretKey, "Reass_SetVideoDecryptDataKeyAndMode pSecretKey")
    CPacketReassemble * packeReassemble = (CPacketReassemble *)h;
    return packeReassemble->SetDecryptDataKeyAndMode(pSecretKey, ulKeyLen, iSecretKeyType);
}

/*****************************************************************************
 函数名称：Reass_StopVideoDecryptData
 功能描述：停止视频解密
 输入参数：
 IVS_HANDLE  ： 渲染通道句柄
 输出参数：NA
 返 回 值：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int Reass_StopVideoDecryptData(IVS_HANDLE h)
{
    CHECK_HANDLE_NULL(h, "Reass_StopVideoDecryptData");
    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;
    packeReassemble->StopDecryptVideoData();
    return IVS_SUCCEED;
}

/*****************************************************************************
 函数名称：Reass_SetCompleteCallback
 功能描述：回调函数
 输入参数：
 IVS_HANDLE：     渲染通道句柄
 cbFunc：         回调函数指针
 pUser：          用户数据
 输出参数：NA
 返 回 值：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int  Reass_SetCompleteCallback(IVS_HANDLE h, REASSM_CALLBACK cbFunc, void *pUser)
{
    CHECK_HANDLE_NULL(h, "Reass_SetCompleteCallback");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    return packeReassemble->SetCallbackFun(cbFunc, pUser);
}

/*****************************************************************************
 函数名称：Reass_SetExceptionCallBack
 功能描述：设置异常回调函数
 输入参数：
 IVS_HANDLE：     渲染通道句柄
 pExceptionCallBack： 回调函数指针
 pUser：          用户数据
 输出参数：NA
 返 回 值：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int  Reass_SetExceptionCallBack(IVS_HANDLE h, PLAYER_EXCEPTION_CALLBACK pExceptionCallBack, void *pUser)
{
    CHECK_HANDLE_NULL(h, "Reass_SetExceptionCallBack");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    packeReassemble->SetExceptionCallback(pExceptionCallBack, pUser);

    return IVS_SUCCEED;
}

/*****************************************************************************
 函数名称：Reass_GetStatisInfo
 功能描述：获取码率信息
 输入参数：
 IVS_HANDLE：     渲染通道句柄
 CODE_RATE_INFO： 码率信息
 pUser：          用户数据
 输出参数：NA
 返 回 值：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int  Reass_GetStatisInfo(IVS_HANDLE h, CODE_RATE_INFO* pCodeRateInfo)
{
    CHECK_HANDLE_NULL(h, "Reass_ShowCodeRateInfo");
    CHECK_HANDLE_NULL(pCodeRateInfo, "Reass_ShowCodeRateInfo");
    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;
    pCodeRateInfo->uAudioCodeRate = packeReassemble->GetAudioCodeRate();
    pCodeRateInfo->uAvgAudioCodeRate = packeReassemble->GetAvgAudioCodeRate();
    pCodeRateInfo->uAvgVideoCodeRate = packeReassemble->GetAvgVideoCodeRate();
    pCodeRateInfo->uVideoCodeRate   = packeReassemble->GetVideoCodeRate();
    pCodeRateInfo->uUsedMemBlockNum = packeReassemble->GetUsedMemBlockNum();
    pCodeRateInfo->uFrameListSize = packeReassemble->GetFrameListSize();
    return IVS_SUCCEED;
}

/*
 * Reass_SetMediaAttr
 * 设置媒体属性
 */
int Reass_SetMediaAttr(IVS_HANDLE h, const MEDIA_ATTR *pVideoAttr, const MEDIA_ATTR *pAudioAttr)
{
    CHECK_HANDLE_NULL(h, "Reass_ShowCodeRateInfo");
	CPacketReassemble *packeReassemble = (CPacketReassemble *)h;
    packeReassemble->SetMediaAttr(pVideoAttr, pAudioAttr);
    return IVS_SUCCEED;
}


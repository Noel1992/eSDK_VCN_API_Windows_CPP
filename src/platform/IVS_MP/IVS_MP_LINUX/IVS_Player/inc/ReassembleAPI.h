/********************************************************************
filename    : ReassembleAPI.h
author      : c00206592
created     : 2012/11/17
description : 组帧方法API头文件
copyright   : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     : 2012/11/17 初始版本
 *********************************************************************/

#ifndef __IVS_PLATER_REASSEMBLE_API_H__
#define __IVS_PLATER_REASSEMBLE_API_H__

#include "IVSPlayerDataType.h"

/*****************************************************************************
 函数名称：Reass_GetHandle
 功能描述：获取渲染句柄
 输入参数：nDecoderType 编码类型(VIDEO_DEC_TYPE)
 输出参数：NA
 返 回 值：RENDER_HANDLE
*****************************************************************************/
IVS_HANDLE Reass_GetHandle(IVS_INT32 iDecoderType);

/*****************************************************************************
 函数名称：Render_FreeHandle
 功能描述：释放渲染句柄
 输入参数：IVS_HANDLE 句柄
 输出参数：NA
 返 回 值：void
*****************************************************************************/
void       Reass_FreeHandle(IVS_HANDLE h);

/*****************************************************************************
 函数名称：Reass_InsertPacket
 功能描述：插入RTP包
 输入参数：
 IVS_HANDLE：     渲染通道句柄
 pBuf：           数据流
 ulBufLen：       数据长度
 输出参数：NA
 返 回 值：
 IVS_SUCCEED   ：  成功
 IVS_FAIL      ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int        Reass_InsertPacket(IVS_HANDLE h, IVS_CHAR* pBuf, IVS_UINT32 ulBufLen);

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
int        Reass_GetVideoCodeRate(IVS_HANDLE h, IVS_UINT32* uVideoCodeRate, IVS_UINT32* uAvgVideoCodeRate);

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
int        Reass_GetAudioCodeRate(IVS_HANDLE h, IVS_UINT32* uAudioCodeRate, IVS_UINT32* uAvgAudioCodeRate);

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
int        Reass_GetFPS(IVS_HANDLE h, IVS_UINT32* uFps);

/*****************************************************************************
 函数名称：Reass_GetLostPacketRate
 功能描述：获取平均丢包率
 输入参数：
 IVS_HANDLE     ： 渲染通道句柄
 pfLostPacketRate:  丢包率
 输出参数：NA
 返 回 值：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int        Reass_GetLostPacketRate(IVS_HANDLE h, IVS_FLOAT* pfLostPacketRate);

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
int        Reass_ReSetReassemble(IVS_HANDLE h);

void       Reass_SetResetFlag(IVS_HANDLE h, bool bIsReset);
/*****************************************************************************
 函数名称：Reass_SetRawFrameLimit
 功能描述：当发生丢包的时候，可以调用该方法设置组帧缓存区大小
 输入参数：
 IVS_HANDLE： 渲染通道句柄
 uSize     ： 缓存大小（建议值在4~7）
 输出参数：NA
 返 回 值：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int        Reass_SetRawFrameLimit(IVS_HANDLE h, IVS_UINT32 uSize);

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
int        Reass_SetWaterMarkState(IVS_HANDLE h, IVS_BOOL bIsStart);


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
int        Reass_SetPlayBackMode(IVS_HANDLE h, IVS_BOOL bIsPlayBackMode);


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
int        Reass_SetDecryptDataKeyAndMode(IVS_HANDLE h,const IVS_CHAR* pSecretKey,IVS_ULONG ulKeyLen, IVS_INT32 iSecretKeyType);

///*****************************************************************************
// 函数名称：Reass_SetAudioDecryptDataKey
// 功能描述：设置音频频解密密钥
// 输入参数：
// IVS_HANDLE  ： 渲染通道句柄
// pSecretKey  ： 解密密钥
// ulKeyLen    :  密钥长度
// iSecretKeyType ：密钥类型 (参考 DECRYP_TYPE )
// 输出参数：NA
// 返 回 值：
// IVS_SUCCEED     ：  成功
// IVS_FAIL        ：  失败
// IVS_PARA_INVALID： 输入参数非法
//*****************************************************************************/
//int        Reass_SetAudioDecryptDataKey(IVS_HANDLE h,const IVS_CHAR* pSecretKey,IVS_ULONG ulKeyLen, IVS_INT32 iSecretKeyType);

/*****************************************************************************
 函数名称：Reass_StopAudioDecryptData
 功能描述：停止音频解密
 输入参数：
 IVS_HANDLE  ： 渲染通道句柄
 输出参数：NA
 返 回 值：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
//int        Reass_StopAudioDecryptData(IVS_HANDLE h);

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
int        Reass_StopVideoDecryptData(IVS_HANDLE h);


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
int        Reass_SetCompleteCallback(IVS_HANDLE h, REASSM_CALLBACK cbFunc, void *pUser);

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
int        Reass_SetExceptionCallBack(IVS_HANDLE h, PLAYER_EXCEPTION_CALLBACK pExceptionCallBack, void *pUser);


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
int        Reass_GetStatisInfo(IVS_HANDLE h, CODE_RATE_INFO* pCodeRateInfo);


/*****************************************************************************
 函数名称：Reass_SetMediaAttr
 功能描述：设置rtp媒体属性
 输入参数：
 IVS_HANDLE：     组帧通道句柄

 输出参数：NA
 返 回 值：
 IVS_SUCCEED     ：  成功
 IVS_FAIL        ：  失败
 IVS_PARA_INVALID： 输入参数非法
*****************************************************************************/
int        Reass_SetMediaAttr(IVS_HANDLE h, const MEDIA_ATTR *pVideoAttr, const MEDIA_ATTR *pAudioAttr);



#endif //__IVS_PLATER_REASSEMBLE_API_H__

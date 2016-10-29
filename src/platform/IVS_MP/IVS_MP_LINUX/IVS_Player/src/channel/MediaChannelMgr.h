/********************************************************************
filename    :    MediaChannelMgr.h
author      :    s00191067
created     :    2012/11/01
description :    媒体通道管理类;
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/01 初始版本
*********************************************************************/
#ifndef MEDIA_CHANNEL_MGR_H
#define MEDIA_CHANNEL_MGR_H

#include <map>
#include "IVSPlayerDataType.h"
#include "Lock.h"
#ifdef WIN32
#include "AudioChannel.h"
#endif

class CMediaChannel;
class CMediaChannelMgr
{
private:
    CMediaChannelMgr(void);   // 单实例，构造函数设为私有;
public:
    ~CMediaChannelMgr(void);
	static CMediaChannelMgr& instance()
    {
        static CMediaChannelMgr s_mediaChannelMgr;
        return s_mediaChannelMgr;
    }
    // 初始化
    void Init(void);

    // 清空
    void UnInit(void);

    // 获取可用句柄
    int GetChannel(unsigned long& ulChannel);

    // 释放指定句柄
    int FreeChannel(unsigned long ulChannel);

	// 设置异常回调;
    void SetExceptionCallBack(PLAYER_EXCEPTION_CALLBACK cbExceptionCallBack, void* pUser);

    // 设置事件回调;
    void SetEventCallBack(PLAYER_EVENT_CALLBACK cbExceptionCallBack, void* pUser);

    /***************************************************************************
     函 数 名 : DoExceptionCallBack
     功能描述 : 反馈异常，该函数由内部模块调用，将异常统一反馈到管理类中，再统一回调上层;
     输入参数 : iMsgType          异常错误码;
                pParam            参数信息，根据异常类型可扩展;
                pUser             用户数据;
     输出参数 : NA
     返 回 值 : 成功:IVS_SUCCEED 失败:IVS_FAIL;
    ***************************************************************************/
    void DoExceptionCallBack(int iMsgType, void* pParam, void* pUser);

    /***************************************************************************
     函 数 名 : DoEventCallBack
     功能描述 : 反馈事件，该函数由内部模块调用，将事件统一反馈到管理类中，再统一回调上层;
     输入参数 : iMsgType          事件类型;
                pParam            参数信息，根据事件类型可扩展;
                pUser             用户数据;
     输出参数 : NA
     返 回 值 : 成功:IVS_SUCCEED 失败:IVS_FAIL;
    ***************************************************************************/
    void DoEventCallBack(int iMsgType, void* pParam, void* pUser);
	
	// 根据通道号获取媒体通道对象;
	CMediaChannel* GetChannelbyID(unsigned long ulChannel);

#ifdef WIN32
    //创建语音对讲通道
    CAudioChannel * CreateVoiceChannel() const;

    //获取语音对讲通道对象
    CAudioChannel* GetVoiceChannel() const;


    //重置语音对讲通道
    int ResetVoiceChannel() const;

    //销毁语音对讲通道
    void DestroyVoiceChannel() const;

    //设置语音对讲传输协议类型
    void SetVoiceProtocolType(unsigned int uVoiceProtocolType) const;

    //获取语音对讲传输协议类型
    int GetVoiceProtocolType(unsigned int* pVoiceProtocolType) const;

    //设置语音对讲解码类型
    void SetVoiceDecoderType(unsigned int uVoiceDecoderType) const;

    //获取语音对讲解码类型
    int GetVoiceDecoderType(unsigned int* pVoiceDecoderType) const;

//     //获取语音对讲接收端地址
//     int GetRecvVoiceAddr(unsigned long*  pRecvVoiceRtpPort);
// 
//     //释放语音对讲接收端地址
//     void FreeRecvVoiceAddr() const;

#endif

    //设置接收码流IP
    void SetRecvStreamIp(char*  pRecvStreamIp);

    //获取接收码流IP
    int GetRecvStreamIp(char*  pRecvStreamIp) const;
    /***************************************************************************
     函 数 名 : SetMediaBufferLimit
     功能描述 : 设置媒体缓冲区的上下限百分比
     输入参数 : uiLowerLimit      下限百分比;
                uiUpperLimit      上限百分比;
     输出参数 : NA
     返 回 值 : void
    ***************************************************************************/
    void SetMediaBufferLimit(IVS_UINT32 uiLowerLimit, IVS_UINT32 uiUpperLimit)
    {
        m_uiLowerLimit = uiLowerLimit;
        m_uiUpperLimit = uiUpperLimit;
    }

    /***************************************************************************
     函 数 名 : GetMediaBufferLimit
     功能描述 : 获取媒体缓冲区的上下限百分比
     输入参数 : NA
     输出参数 : uiLowerLimit      下限百分比;
                uiUpperLimit      上限百分比;
     返 回 值 : void
    ***************************************************************************/
    void GetMediaBufferLimit(IVS_UINT32& uiLowerLimit, IVS_UINT32& uiUpperLimit) const
    {
        uiLowerLimit = m_uiLowerLimit;
        uiUpperLimit = m_uiUpperLimit;
    }
private:
    //通道号与通道资源的映射表;
	typedef std::map<unsigned long, CMediaChannel*> MediaChannelMap;
	typedef MediaChannelMap::iterator MediaChannelMapIter;
	MediaChannelMap             m_mapMediaChannel;	        

    PLAYER_EXCEPTION_CALLBACK   m_cbExceptionCallBack;  // 异常回调函数指针;
    void*                       m_pUserData;            // 异常用户数据;

    PLAYER_EVENT_CALLBACK       m_cbEventCallBack;      // 事件回调函数指针;
    void*                       m_pEventUserData;       // 事件用户数据;
    CIVSMutex                   m_mutexChannel;
    CIVSMutex                   m_mutexExceptionCallback;

    std::string m_strRecvStreamIp;

#ifdef WIN32
    static CAudioChannel* m_pAudioChannel; //语音对讲通道对象指针	
#endif

    CIVSMutex                   m_mutexEventCallback;
    IVS_UINT32                  m_uiLowerLimit;         // 下限百分比
    IVS_UINT32                  m_uiUpperLimit;         // 上限百分比
};
#endif //MEDIA_CHANNEL_MGR_H

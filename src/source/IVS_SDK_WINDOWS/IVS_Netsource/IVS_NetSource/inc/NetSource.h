/*Copyright 2015 Huawei Technologies Co., Ltd. All rights reserved.
eSDK is licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
		http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

/********************************************************************
filename    :    NetSource.h
author      :    s00191067
created     :    2012/11/05
description :    收流基类
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/05 初始版本
*********************************************************************/
#ifndef NET_SOURCE_H
#define NET_SOURCE_H

#include "NetSourceCommon.h"
#include "BaseJob.h"
#include "Lock.h"

typedef enum tagRecvStreamState
{
    START_RECV_STREAM = 0,  // 开始收流
    STOP_RECV_STREAM        // 停止收流
}RECV_STREAM_STATE;

class CChannelInfo;
class _BaseThread;
class CNetSource : public BaseJob
{
public:
    CNetSource(void);
    virtual ~CNetSource(void);

    /******************************************************************
     function   : Init
     description: 初始化;
     input      : CChannelInfo * pChannelInfo 通道信息仓库类指针;
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    virtual int Init(CChannelInfo* pChannelInfo);

    /******************************************************************
     function   : Release
     description: 释放数据;
     output     : NA
     return     : void
    *******************************************************************/
    virtual void Release();

    /******************************************************************
     function   : DoExceptionCallBack
     description: 处理异常回调;
     input      : int iMsgType  异常消息类型;
     input      : void * pParam 异常数据，根据消息类型对应不同的结构体;
     output     : NA
     return     : void
    *******************************************************************/
    void DoExceptionCallBack(int iMsgType, void*  pParam);

    /******************************************************************
     function   : MainLoop
     description: 主循环
     output     : NA
     return     : void
    *******************************************************************/
    virtual void MainLoop();

    /******************************************************************
     function   : kill
     description: 停止任务
     output     : NA
     return     : void
    *******************************************************************/
    virtual void kill();

    /******************************************************************
     function   : Start
     description: 启动收流
     output     : NA
     return     : int
    *******************************************************************/
    int Start();

    /******************************************************************
     function   : Stop
     description: 停止收流
     output     : NA
     return     : void
    *******************************************************************/
    void Stop();

protected:
    /******************************************************************
     function   : ProcessRecevData
     description: 收流
     output     : NA
     return     : void
    *******************************************************************/
    virtual void ProcessRecevData() = 0;

	/******************************************************************
     function   : CheckNatSend
     description: 检查是否满足NAT穿越条件,每隔30s向服务器发送nat消息;
     output     : NA
     return     : int IVS_FAIL表示不满足条件， IVS_SUCCEED表示满足条件;
    *******************************************************************/
    virtual int CheckNatSend();

	/******************************************************************
     function   : ProcMediaPacket
     description: 分发媒体报文
     output     : NA
     return     : void
    *******************************************************************/
    void ProcMediaPacket();

    /******************************************************************
     function   : CheckTimeOut
     description: 检测是否超时
     output     : NA
     return     : int
    *******************************************************************/
    virtual int CheckTimeOut();

protected:
    CChannelInfo*       m_pChannelInfo;				// 通道信息对象;
	_BaseThread*		m_pSourceThread;			// 收流线程;
    char*               m_pRecvBuf;                 // 音视频码流缓冲数据;
    unsigned long       m_ulRecvBufLen;             // 音视频码流缓冲数据长度;
    unsigned long       m_ulLastSendNatTick;        // 上次发送Nat穿越包的时间;
    unsigned long       m_ulLastCheckTimeoutTick;   // 上次开始计算超时的时间;
	bool                m_bRun;                     // 线程运行标志; 
	CIVSMutex			m_RunFlagMutex;				// 运行标志的锁;
};
#endif //NET_SOURCE_H

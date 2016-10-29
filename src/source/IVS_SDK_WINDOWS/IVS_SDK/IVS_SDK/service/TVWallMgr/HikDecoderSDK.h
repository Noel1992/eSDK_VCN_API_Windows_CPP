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
 filename			:    HikDecoderSDK.h 
 author				:    ywx313149
 created			:    2016/08/27
 description	:    海康解码器类
 copyright		:    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history				:    2016/08/27 初始版本
*********************************************************************/

#ifndef __IVS_SDK_TVWALL_HIK_DECODER_SDK_H__
#define __IVS_SDK_TVWALL_HIK_DECODER_SDK_H__

#include<map>
#include "DecoderSDK.h"
#include "IVS_SDKTVWall.h"
#include "LockGuard.h"
#include "sdktimer.h" 

typedef std::map<IVS_LONG, DECODER_DEV> DECODER_REG_INFO_MAP; 
typedef std::map<std::string, DECODER_DEV> DECODER_ID_INFO_MAP; 


class CHikDecoderSDK;
class CHikTrigger : public ITrigger
{
public:
    CHikTrigger(void);
    ~CHikTrigger(void);
    virtual void onTrigger(void * /*pArg*/, ULONGLONG /*ullScales*/, TriggerStyle /*enStyle*/);
    //设置电视墙管理对象
    void SetHikDecoderSDK(CHikDecoderSDK* pHikDecoderSDK);

private:
    CHikDecoderSDK* m_pHikDecoderSDK;		//电视墙管理对象
};


class CHikDecoderSDK : public CDecoderSDK
{
public:
    CHikDecoderSDK(void);
    virtual ~CHikDecoderSDK(void);

    virtual IVS_INT32 Login(IVS_LONG& ulSessionID,const DECODER_DEV* pDecoder,const IVS_CHAR* pPwd);
    virtual IVS_INT32 Logout(IVS_LONG ulSessionID);
    virtual IVS_INT32 GetChannels(IVS_LONG ulSessionID,DECODER_DEV* pDecoder);
    virtual IVS_INT32 StartPlayURL(IVS_LONG ulSessionID,const IVS_CHAR* pUrl,CUMW_DECODER_CHANNEL_INFO* pChannel);
    virtual IVS_INT32 StopPlayURL(IVS_LONG ulSessionID,CUMW_DECODER_CHANNEL_INFO* pChannel);

    //通过解码器ID获取用户的登录号
    virtual IVS_INT32 GetUserIDbyDecorderID(const IVS_CHAR * pszDecoderID, IVS_LONG& lUserID);
	//海康解码器是否在线
    virtual bool IsHikDecOnline(const char* pszDecoderID);

    //获取设备ID
    LONG GetIPID(const CUMW_DECODER_CHANNEL_INFO* pChannel, WORD& wIPID);
    //初始化海康库
    IVS_INT32 InitHik(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);
    //判断窗口解码状态是否正常
    bool IsWinDecodeStatusNatural(IVS_LONG lUserID, IVS_ULONG iChannelEx);
    //检查异常状态
    void CheckChannelExp();

    void NotifyDecDisconn(LONG lUserID);
    void NotifyDecResumeConn(LONG lUserID);
    void SetDecOnlineStatus(LONG lUserID, bool bLoginSuc);

    //检查解码器通道状态线程
    static void __cdecl CheckDecThread(LPVOID lpvoid);
    void StartCheckDecThread();
    //查看解码器或者通道是否能找到，0--能找到通道；1--解码器找不到；2--解码器存在，通道找不到
    virtual IVS_INT32 CheckDecoderChannel(const IVS_CHAR* pDecoderID,IVS_ULONG uiChannel);
	// create Hik decoderID
	IVS_INT32 CreateHikDecoderID(std::string& strDecoderID);
	// add decoder info
	IVS_INT32 AddDecoderInfo(IVS_LONG ulSessionID, const IVS_CHAR* pDecoderID, DECODER_DEV& stDecoder);

	 void GetDecInfo2Map(DECODER_REG_INFO_MAP& TempMap);
	 void DecoderRelease();
protected:
    void SetChannelStatus(IVS_LONG ulSessionID, unsigned long ulChannelID, bool bDecording);
   
    void NotifyChannelExp(const DECODER_DEV& stDevInfo, unsigned long ulChannelID);
    void SaveDecInfo(IVS_LONG lUserID, DECODER_DEV& stDecoder);

private:
    bool GetDecOnlineStatus(IVS_LONG ulSessionID);
    DECODER_REG_INFO_MAP m_DecodeRegInfoMap;   //解码器登录信息表，以userid为key，与m_DecoderInfoMap同步
    DECODER_ID_INFO_MAP m_DecoderInfoMap;      //解码器信息表,以解码器ID为key，与m_DecodeRegInfoMap同步
    VOS_Mutex* m_pDecodeRegInfoMapMutex;	   //解码器登录信息表锁

    bool m_bInitHikSdk;                        //是否已初始化海康SDK库
    CNVSTimer m_timer;                         //定时器
    CHikTrigger m_Tigger;                      //触发器

    PCUMW_CALLBACK_NOTIFY   m_pFunCallBack;    //回调函数指针
    void*                   m_pUserData;       //用户数据指针

    bool m_CheckDecThreadRun;
    VOS_Thread*  m_pCheckDecThread; 
public:
	bool m_bHikDecPlay;						//是否使用海康解码器播放,规避海康URL 240Bytes 的限制
};
#endif //__IVS_SDK_TVWALL_HIK_DECODER_SDK_H__


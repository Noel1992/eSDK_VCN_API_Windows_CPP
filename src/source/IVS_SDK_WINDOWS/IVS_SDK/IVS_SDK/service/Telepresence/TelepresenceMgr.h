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
 filename    :    TelepresenceMgr.h
 author      :    g00209332
 created     :    2013/3/6
 description :    智真对接管理类;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/3/6 初始版本
*********************************************************************/

#ifndef _TELEPRESENCE_MGR_H_
#define _TELEPRESENCE_MGR_H_

#include "hwsdk.h"
#include "SDKDef.h"
#include "vos.h"


class CUserMgr;
class CTPPlayChannel;
class CTelepresenceMgr
{
	typedef std::map<IVS_ULONG, CTPPlayChannel*> TP_PLAY_CHANNEL_MAP;
public:
	CTelepresenceMgr(void);
	~CTelepresenceMgr(void);
	//开始播放视频
	IVS_INT32 StartRealPlayByIP(const std::string& strCameraCode,
								const IVS_MEDIA_ADDR& stMediaAddrDst,
								IVS_MEDIA_ADDR& stMediaAddrSrc,
								IVS_ULONG& ulHandle);

	//开始播放视频
	IVS_INT32 StartRealPlayByIPEx(const std::string& strCameraCode,
								const IVS_REALPLAY_PARAM&  RealplayParam,
								const IVS_MEDIA_ADDR& stMediaAddrDst,
								IVS_MEDIA_ADDR& stMediaAddrSrc,
								IVS_ULONG& ulHandle);

	//开始平台录像回放
	IVS_INT32 StartPlatformPlayBackByIP(const std::string& strCameraCode,
								const IVS_PLAYBACK_PARAM& stPlaybackParam,
								const IVS_MEDIA_ADDR& stMediaAddrDst,
								IVS_MEDIA_ADDR& stMediaAddrSrc,
								IVS_ULONG& ulHandle);

	//停止播放
	IVS_INT32 StopPlayByIP(IVS_ULONG ulHandle);

	//停止所有播放
	void StopAllPlayByIP();

	//设置UserMgr
	void SetUserMgr(CUserMgr* pUserMgr) {m_pUserMgr = pUserMgr;}

	//从SCU获取媒体URL
	IVS_INT32 GetURL(const char *pszAuthHeader, ST_URL_MEDIA_INFO& stUrlMediaInfo, ST_MEDIA_RSP& stMediaRsp);

	//检验回放参数是否合法
    IVS_INT32 CheckParam(const IVS_TIME_SPAN* pTimeSpan,IVS_UINT32 uiProtocolType, IVS_FLOAT fSpeed, TIME_SPAN_INTERNAL& stTimeSpanInter);

	//获取播放句柄
	IVS_ULONG GetPlayHandlebyRtspHandle(IVS_ULONG iRtspHandle);

	//暂停平台录像回放
	IVS_INT32 PlatformPlayBackPauseByIP(IVS_ULONG ulHandle);

	//恢复平台录像回放
	IVS_INT32 PlatformPlayBackResumeByIP(IVS_ULONG ulHandle);

	//设置平台录像码流回放速度
	IVS_INT32 SetPlayBackSpeedByIP(IVS_ULONG ulHandle, IVS_FLOAT fSpeed);
	IVS_INT32 SetPlayBackSpeedByIPnTime(IVS_ULONG ulHandle, IVS_FLOAT fSpeed, IVS_UINT64 uiOffsetTime);

	//设置平台录像码流回放时间
	IVS_INT32 SetPlayBackTimeByIP(IVS_ULONG ulHandle, IVS_UINT32 uiTime);

	//获取摄像机在该时间段内所处的NVR信息
	IVS_INT32 GetRecordNVRInfo(const IVS_CHAR* pCameraCode,const IVS_INT32 iStarTime,const IVS_INT32 iEndTime, IVS_CHAR* pNVR);

public:
	// 根据ulHandle获取播放通道
	// 如果bCreate为TURE，没找到就创建一个新的
	CTPPlayChannel* GetPlayChannel(IVS_ULONG ulHandle, IVS_BOOL bCreate=FALSE);
	// 根据ulHandle移除一个播放通道
	IVS_VOID RemovePlayChannel(IVS_ULONG ulHandle);

private:
	TP_PLAY_CHANNEL_MAP m_channelMap;	// 播放通道MAP
	VOS_Mutex* m_pChannelMapMutex;		// 播放通道MAP锁
	CUserMgr* m_pUserMgr;				// 用户管理

};

#endif

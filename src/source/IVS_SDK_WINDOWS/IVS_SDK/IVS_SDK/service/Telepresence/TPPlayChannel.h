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
 filename    :    TPPlayChannel.h
 author      :    g00209332
 created     :    2013/3/6
 description :    智真对接播放类;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/3/6 初始版本
*********************************************************************/

#ifndef _TP_PLAY_CHANNEL_H_
#define _TP_PLAY_CHANNEL_H_

#include "hwsdk.h"
#include "SDKDef.h"
#include "UserMgr.h"

class CTelepresenceMgr;
class CTPPlayChannel
{
public:
	CTPPlayChannel(void);
	~CTPPlayChannel(void);
	//开始播放视频
	IVS_INT32 StartRealPlayByIP(const std::string& strCameraCode,
								const IVS_MEDIA_ADDR& stMediaAddrDst,
								IVS_MEDIA_ADDR& stMediaAddrSrc);

	//开始平台录像回放
	IVS_INT32 StartPlatformPlayBackByIP(const IVS_CHAR* pDomainCode,
								const IVS_CHAR* pNVRCode,
								const std::string& strCameraCode,
								const IVS_MEDIA_ADDR& stMediaAddrDst,
								IVS_MEDIA_ADDR& stMediaAddrSrc,
								const TIME_SPAN_INTERNAL& stTimeSpanInter,
								IVS_FLOAT fSpeed);

	//停止实况及录像回放
	IVS_INT32 StopPlayByIP();

	//设置智真管理智真
	IVS_VOID SetTPMgr(CTelepresenceMgr* pTPMgr)
	{
		m_pTPMgr = pTPMgr;
	}

	//设置媒体参数
	IVS_INT32 SetMediaParam(const IVS_MEDIA_PARA* pMediaPara);

	//获取RTSP句柄
	IVS_ULONG GetRtspHandle() const { return m_iRtspHandle; }

	//暂停平台录像回放
	IVS_INT32 PlatformPlayBackPauseByIP(IVS_VOID);

	//恢复平台录像回放
	IVS_INT32 PlatformPlayBackResumeByIP(IVS_VOID);

	//设置平台录像码流回放速度
	IVS_INT32 SetPlayBackSpeedByIP(IVS_FLOAT fSpeed);
	IVS_INT32 SetPlayBackSpeedByIPnTime(IVS_FLOAT fSpeed, IVS_UINT64 uiOffsetTime);

	//设置平台录像码流回放时间
	IVS_INT32 SetPlayBackTimeByIP(IVS_UINT32 uiTime);

	bool IsPaused() const { return m_bPaused; }

private:
	//获取实况URL
	IVS_INT32 GetRealPlayURL(const std::string& strCamera, 
		const IVS_MEDIA_ADDR& stMediaAddrDst, 
		ST_MEDIA_RSP& stMediaRsp);

	//获取录像回放URL
	IVS_INT32 GetPlayBackURL(const IVS_CHAR* pDomainCode,
		const IVS_CHAR* pNVRCode,
		const std::string& strCamera, 
		const TIME_SPAN_INTERNAL& stTimeSpanInter, 
		const IVS_MEDIA_ADDR& stMediaAddrDst, 
		ST_MEDIA_RSP& stMediaRsp);

	//获取播放流的开始结束时间
	IVS_INT32 GetPlayBackTimeSpan(const std::string& strURL,
		const TIME_SPAN_INTERNAL& stTimeSpan);

private:
	//IVS_INT32 m_iRtspHandle;				//RTSP句柄
	IVS_ULONG m_iRtspHandle;				//RTSP句柄
	std::string m_strURL;			//媒体URL
	IVS_MEDIA_PARA m_stuMediaParam;   // 媒体参数
	CTelepresenceMgr* m_pTPMgr;		//智真对接管理
	float m_fRTSPSpeed;									// RTSP播放速度;
	TIME_SPAN_INTERNAL m_stTimeSpanInter;                // 播放流的开始结束时间;
	bool m_bPaused;
	IVS_FLOAT m_fSpeed;		//入参速度
};

#endif

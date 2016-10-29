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
 description :    ����ԽӲ�����;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/3/6 ��ʼ�汾
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
	//��ʼ������Ƶ
	IVS_INT32 StartRealPlayByIP(const std::string& strCameraCode,
								const IVS_MEDIA_ADDR& stMediaAddrDst,
								IVS_MEDIA_ADDR& stMediaAddrSrc);

	//��ʼƽ̨¼��ط�
	IVS_INT32 StartPlatformPlayBackByIP(const IVS_CHAR* pDomainCode,
								const IVS_CHAR* pNVRCode,
								const std::string& strCameraCode,
								const IVS_MEDIA_ADDR& stMediaAddrDst,
								IVS_MEDIA_ADDR& stMediaAddrSrc,
								const TIME_SPAN_INTERNAL& stTimeSpanInter,
								IVS_FLOAT fSpeed);

	//ֹͣʵ����¼��ط�
	IVS_INT32 StopPlayByIP();

	//���������������
	IVS_VOID SetTPMgr(CTelepresenceMgr* pTPMgr)
	{
		m_pTPMgr = pTPMgr;
	}

	//����ý�����
	IVS_INT32 SetMediaParam(const IVS_MEDIA_PARA* pMediaPara);

	//��ȡRTSP���
	IVS_ULONG GetRtspHandle() const { return m_iRtspHandle; }

	//��ͣƽ̨¼��ط�
	IVS_INT32 PlatformPlayBackPauseByIP(IVS_VOID);

	//�ָ�ƽ̨¼��ط�
	IVS_INT32 PlatformPlayBackResumeByIP(IVS_VOID);

	//����ƽ̨¼�������ط��ٶ�
	IVS_INT32 SetPlayBackSpeedByIP(IVS_FLOAT fSpeed);
	IVS_INT32 SetPlayBackSpeedByIPnTime(IVS_FLOAT fSpeed, IVS_UINT64 uiOffsetTime);

	//����ƽ̨¼�������ط�ʱ��
	IVS_INT32 SetPlayBackTimeByIP(IVS_UINT32 uiTime);

	bool IsPaused() const { return m_bPaused; }

private:
	//��ȡʵ��URL
	IVS_INT32 GetRealPlayURL(const std::string& strCamera, 
		const IVS_MEDIA_ADDR& stMediaAddrDst, 
		ST_MEDIA_RSP& stMediaRsp);

	//��ȡ¼��ط�URL
	IVS_INT32 GetPlayBackURL(const IVS_CHAR* pDomainCode,
		const IVS_CHAR* pNVRCode,
		const std::string& strCamera, 
		const TIME_SPAN_INTERNAL& stTimeSpanInter, 
		const IVS_MEDIA_ADDR& stMediaAddrDst, 
		ST_MEDIA_RSP& stMediaRsp);

	//��ȡ�������Ŀ�ʼ����ʱ��
	IVS_INT32 GetPlayBackTimeSpan(const std::string& strURL,
		const TIME_SPAN_INTERNAL& stTimeSpan);

private:
	//IVS_INT32 m_iRtspHandle;				//RTSP���
	IVS_ULONG m_iRtspHandle;				//RTSP���
	std::string m_strURL;			//ý��URL
	IVS_MEDIA_PARA m_stuMediaParam;   // ý�����
	CTelepresenceMgr* m_pTPMgr;		//����Խӹ���
	float m_fRTSPSpeed;									// RTSP�����ٶ�;
	TIME_SPAN_INTERNAL m_stTimeSpanInter;                // �������Ŀ�ʼ����ʱ��;
	bool m_bPaused;
	IVS_FLOAT m_fSpeed;		//����ٶ�
};

#endif

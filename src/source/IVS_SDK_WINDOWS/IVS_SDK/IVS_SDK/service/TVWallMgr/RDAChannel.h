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
#ifndef _SDK_TVWALL_RDA_CHANNEL_H_
#define _SDK_TVWALL_RDA_CHANNEL_H_

#include "decoderchannel.h"

class CRDAChannel :
	public CDecoderChannel
{
public:
	CRDAChannel(IVS_ULONG ulAsyncSeq, 
				const IVS_CHAR* pDecoderID, 
				IVS_UINT32 uiChannel,
				CTVWallMgr* pTVWallMgr);
	~CRDAChannel(void);
protected:
	// ��ʼ��ͨ��
	virtual IVS_INT32 InitChannel(IVS_UINT32 uiProtocolType, MEDIA_ADDR& stDecoderAddr);
	// ��ʼ��ͨ��
	virtual IVS_INT32 ReInitChannel(IVS_UINT32 uiProtocolType, MEDIA_ADDR& stDecoderAddr);
	// ����ͨ����ʼ����
	virtual IVS_INT32 StartChannel(const std::string strURL, const MEDIA_ADDR stSourceMediaAddr);
	// ��������ͨ����ʼ����
	virtual IVS_INT32 ReStartChannel(const std::string strURL, const MEDIA_ADDR stSourceMediaAddr);
	// ����ͨ������
	virtual IVS_INT32 StopChannel();
	// ����ͨ������
	virtual IVS_INT32 StartChannelSound();
	// ֹͣͨ������
	virtual IVS_INT32 StopChannelSound();
	// ����ͨ����������
	virtual IVS_INT32 SetChannelVolume(IVS_UINT32 /*uiVolumeValue*/);
private:
	CRDAChannel();
	IVS_ULONG m_ulBusinessID;				// ������ҵ��ID
	IVS_UCHAR m_ucTransportProtocol;		// ������ʹ�õ�Э��
};

#endif	//_SDK_TVWALL_RDA_CHANNEL_H_

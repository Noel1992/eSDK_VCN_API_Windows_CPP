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
#ifndef _SDK_TVWALL_SDK_CHANNEL_H_
#define _SDK_TVWALL_SDK_CHANNEL_H_

#include "decoderchannel.h"

class CSDKChannel :
	public CDecoderChannel
{
public:
	CSDKChannel(IVS_ULONG ulAsyncSeq, 
				const IVS_CHAR* pDecoderID, 
				IVS_UINT32 uiChannel,
				CTVWallMgr* pTVWallMgr);
	~CSDKChannel(void);
protected:
	CSDKChannel();
	// ��ʼ��ͨ��
	virtual IVS_INT32 InitChannel(IVS_UINT32 /*uiProtocolType*/, MEDIA_ADDR& /*stDecoderAddr*/){return 1;}
	// ���³�ʼ��ͨ��
	virtual IVS_INT32 ReInitChannel(IVS_UINT32 /*uiProtocolType*/, MEDIA_ADDR& /*stDecoderAddr*/){return 1;}
	// ����ͨ����ʼ����
	virtual IVS_INT32 StartChannel(const std::string strURL, const MEDIA_ADDR stSourceMediaAddr){return 1;} //lint !e715 !e830
	// ����ͨ����ʼ����
	virtual IVS_INT32 ReStartChannel(const std::string strURL, const MEDIA_ADDR stSourceMediaAddr){return 1;} //lint !e715 !e830
	// ����ͨ������
	virtual IVS_INT32 StopChannel(){return 1;}
	// ����ͨ������
	virtual IVS_INT32 StartChannelSound(){return 1;}
	// ֹͣͨ������
	virtual IVS_INT32 StopChannelSound(){return 1;}
	// ����ͨ����������
	virtual IVS_INT32 SetChannelVolume(IVS_UINT32 /*uiVolumeValue*/){return 1;}
};

#endif	//_SDK_TVWALL_SDK_CHANNEL_H_

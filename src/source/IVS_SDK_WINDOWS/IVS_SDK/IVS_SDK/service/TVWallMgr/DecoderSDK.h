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
 filename			:    DecoderSDK.h 
 author				:    ywx313149
 created			:    2016/08/27
 description	:    ����������
 copyright		:    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history				:    2016/08/27 ��ʼ�汾
*********************************************************************/



#ifndef __IVS_SDK_TVWALL_DECODER_SDK_H__
#define __IVS_SDK_TVWALL_DECODER_SDK_H__

#include "IVS_TVWall.h"
#define IVS_DECODER_CHANNEL_FOUND 0
#define IVS_DECODER_NOT_FOUND 1
#define IVS_DECODER_CHANNEL_NOT_FOUND 2
class CDecoderSDK
{
public:
	CDecoderSDK(void);
	virtual ~CDecoderSDK(void);

	virtual IVS_INT32 Login(IVS_LONG& ulSessionID, const DECODER_DEV* pDecoder, const IVS_CHAR* pPwd);
	virtual IVS_INT32 Logout(IVS_LONG ulSessionID);
	virtual IVS_INT32 GetChannels(IVS_LONG ulSessionID, DECODER_DEV* pDecoder);
	virtual IVS_INT32 StartPlayURL(IVS_LONG ulSessionID, const IVS_CHAR* pUrl, CUMW_DECODER_CHANNEL_INFO* pChannel);
	virtual IVS_INT32 StopPlayURL(IVS_LONG ulSessionID, CUMW_DECODER_CHANNEL_INFO* pChannel);

	//ͨ��������ID��ȡ�û��ĵ�¼��
	virtual IVS_INT32 GetUserIDbyDecorderID(const IVS_CHAR * pszDecoderID, IVS_LONG& lUserID);

	//��ʼ��������
	virtual IVS_INT32 InitHik(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);

	virtual bool IsHikDecOnline(const char* pszDecoderID);

	//�鿴����������ͨ���Ƿ����ҵ���0--���ҵ�ͨ����1--�������Ҳ�����2--���������ڣ�ͨ���Ҳ���
	virtual IVS_INT32 CheckDecoderChannel(const IVS_CHAR* pDecoderID,IVS_ULONG uiChannel) = 0;//line !e1961

	/*virtual IVS_INT32 GetWindows(const IVS_CHAR* pDecoderID,DECODER_WINDOWS_INFO* pWinInfo);*/
};


#endif

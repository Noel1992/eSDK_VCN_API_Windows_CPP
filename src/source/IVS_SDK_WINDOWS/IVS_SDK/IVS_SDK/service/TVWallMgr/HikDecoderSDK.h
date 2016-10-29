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
 description	:    ������������
 copyright		:    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history				:    2016/08/27 ��ʼ�汾
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
    //���õ���ǽ�������
    void SetHikDecoderSDK(CHikDecoderSDK* pHikDecoderSDK);

private:
    CHikDecoderSDK* m_pHikDecoderSDK;		//����ǽ�������
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

    //ͨ��������ID��ȡ�û��ĵ�¼��
    virtual IVS_INT32 GetUserIDbyDecorderID(const IVS_CHAR * pszDecoderID, IVS_LONG& lUserID);
	//�����������Ƿ�����
    virtual bool IsHikDecOnline(const char* pszDecoderID);

    //��ȡ�豸ID
    LONG GetIPID(const CUMW_DECODER_CHANNEL_INFO* pChannel, WORD& wIPID);
    //��ʼ��������
    IVS_INT32 InitHik(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);
    //�жϴ��ڽ���״̬�Ƿ�����
    bool IsWinDecodeStatusNatural(IVS_LONG lUserID, IVS_ULONG iChannelEx);
    //����쳣״̬
    void CheckChannelExp();

    void NotifyDecDisconn(LONG lUserID);
    void NotifyDecResumeConn(LONG lUserID);
    void SetDecOnlineStatus(LONG lUserID, bool bLoginSuc);

    //��������ͨ��״̬�߳�
    static void __cdecl CheckDecThread(LPVOID lpvoid);
    void StartCheckDecThread();
    //�鿴����������ͨ���Ƿ����ҵ���0--���ҵ�ͨ����1--�������Ҳ�����2--���������ڣ�ͨ���Ҳ���
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
    DECODER_REG_INFO_MAP m_DecodeRegInfoMap;   //��������¼��Ϣ����useridΪkey����m_DecoderInfoMapͬ��
    DECODER_ID_INFO_MAP m_DecoderInfoMap;      //��������Ϣ��,�Խ�����IDΪkey����m_DecodeRegInfoMapͬ��
    VOS_Mutex* m_pDecodeRegInfoMapMutex;	   //��������¼��Ϣ����

    bool m_bInitHikSdk;                        //�Ƿ��ѳ�ʼ������SDK��
    CNVSTimer m_timer;                         //��ʱ��
    CHikTrigger m_Tigger;                      //������

    PCUMW_CALLBACK_NOTIFY   m_pFunCallBack;    //�ص�����ָ��
    void*                   m_pUserData;       //�û�����ָ��

    bool m_CheckDecThreadRun;
    VOS_Thread*  m_pCheckDecThread; 
public:
	bool m_bHikDecPlay;						//�Ƿ�ʹ�ú�������������,��ܺ���URL 240Bytes ������
};
#endif //__IVS_SDK_TVWALL_HIK_DECODER_SDK_H__


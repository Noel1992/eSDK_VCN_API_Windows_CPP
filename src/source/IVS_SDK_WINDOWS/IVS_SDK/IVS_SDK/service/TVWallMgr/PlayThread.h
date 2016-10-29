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
 filename    :    PlayThread.h
 author      :    g00209332
 created     :    2013/1/8
 description :    �����߳���;���󲥷�ʱ���������߳̽��д�����ֹ��������
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/1/8 ��ʼ�汾
*********************************************************************/
#ifndef _SDK_TVWALL_PLAY_THREAD_H_
#define _SDK_TVWALL_PLAY_THREAD_H_

#include "vos.h"
#include "hwsdk.h"
#include "DecoderChannel.h"
#include "TVWallMgr.h"

class CPlayThread
{
public:
    CPlayThread(void);
    virtual ~CPlayThread(void);
    IVS_INT32 run();
	// ���������Code
    void SetCameraCode(const IVS_CHAR* pCameraCode);
	// ���ý���������ͨ��
    void SetDecoderChannel(CDecoderChannel* pCh);
	// ���õ���ǽ�������
	void SetTVWallMgr(CTVWallMgr* pTVWallMgr);
	// ���õ���ǽ����
    void SetTVWallParam(const IVS_TVWALL_PARAM* pTVWallParam);
protected:
	//��ʼ������Ƶ��ʵ�����طţ�
    virtual IVS_INT32 StartPlay() = 0;
    IVS_CHAR m_szCameraCode[IVS_DEV_CODE_LEN+1];	//�����Code
    IVS_TVWALL_PARAM m_stTVWallParam;				//����ǽ����
    CDecoderChannel* m_pChannel;					//������ͨ������
	CTVWallMgr* m_pTVWallMgr;						//����ǽ�������
private:
	//�̵߳��ú���
    static void invoke(void *argc);					
    VOS_Thread* m_pVosThread;						//�̶߳���
};

class CRealPlayThread : public CPlayThread
{
public:
    CRealPlayThread(void);
    virtual ~CRealPlayThread(void);
	// ����ʵ������
    void SetRealPlayParam(const IVS_REALPLAY_PARAM* pRealPlayParam);
protected:
	//��ʼ����ʵ��
    virtual IVS_INT32 StartPlay();
private:
    IVS_REALPLAY_PARAM m_stRealPlayParam;			//ʵ������
};

class CPlayBackThread : public CPlayThread
{
public:
    CPlayBackThread(void);
    virtual ~CPlayBackThread(void);
protected:
	//��ʼ���Żط�
    virtual IVS_INT32 StartPlay();
private:
    IVS_PLAYBACK_PARAM m_stRealPlayParam;			//�طŲ���
};

#endif	//_SDK_TVWALL_PLAY_THREAD_H_

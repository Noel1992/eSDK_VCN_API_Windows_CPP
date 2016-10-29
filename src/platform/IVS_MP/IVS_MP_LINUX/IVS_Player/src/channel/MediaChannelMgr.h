/********************************************************************
filename    :    MediaChannelMgr.h
author      :    s00191067
created     :    2012/11/01
description :    ý��ͨ��������;
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/01 ��ʼ�汾
*********************************************************************/
#ifndef MEDIA_CHANNEL_MGR_H
#define MEDIA_CHANNEL_MGR_H

#include <map>
#include "IVSPlayerDataType.h"
#include "Lock.h"
#ifdef WIN32
#include "AudioChannel.h"
#endif

class CMediaChannel;
class CMediaChannelMgr
{
private:
    CMediaChannelMgr(void);   // ��ʵ�������캯����Ϊ˽��;
public:
    ~CMediaChannelMgr(void);
	static CMediaChannelMgr& instance()
    {
        static CMediaChannelMgr s_mediaChannelMgr;
        return s_mediaChannelMgr;
    }
    // ��ʼ��
    void Init(void);

    // ���
    void UnInit(void);

    // ��ȡ���þ��
    int GetChannel(unsigned long& ulChannel);

    // �ͷ�ָ�����
    int FreeChannel(unsigned long ulChannel);

	// �����쳣�ص�;
    void SetExceptionCallBack(PLAYER_EXCEPTION_CALLBACK cbExceptionCallBack, void* pUser);

    // �����¼��ص�;
    void SetEventCallBack(PLAYER_EVENT_CALLBACK cbExceptionCallBack, void* pUser);

    /***************************************************************************
     �� �� �� : DoExceptionCallBack
     �������� : �����쳣���ú������ڲ�ģ����ã����쳣ͳһ�������������У���ͳһ�ص��ϲ�;
     ������� : iMsgType          �쳣������;
                pParam            ������Ϣ�������쳣���Ϳ���չ;
                pUser             �û�����;
     ������� : NA
     �� �� ֵ : �ɹ�:IVS_SUCCEED ʧ��:IVS_FAIL;
    ***************************************************************************/
    void DoExceptionCallBack(int iMsgType, void* pParam, void* pUser);

    /***************************************************************************
     �� �� �� : DoEventCallBack
     �������� : �����¼����ú������ڲ�ģ����ã����¼�ͳһ�������������У���ͳһ�ص��ϲ�;
     ������� : iMsgType          �¼�����;
                pParam            ������Ϣ�������¼����Ϳ���չ;
                pUser             �û�����;
     ������� : NA
     �� �� ֵ : �ɹ�:IVS_SUCCEED ʧ��:IVS_FAIL;
    ***************************************************************************/
    void DoEventCallBack(int iMsgType, void* pParam, void* pUser);
	
	// ����ͨ���Ż�ȡý��ͨ������;
	CMediaChannel* GetChannelbyID(unsigned long ulChannel);

#ifdef WIN32
    //���������Խ�ͨ��
    CAudioChannel * CreateVoiceChannel() const;

    //��ȡ�����Խ�ͨ������
    CAudioChannel* GetVoiceChannel() const;


    //���������Խ�ͨ��
    int ResetVoiceChannel() const;

    //���������Խ�ͨ��
    void DestroyVoiceChannel() const;

    //���������Խ�����Э������
    void SetVoiceProtocolType(unsigned int uVoiceProtocolType) const;

    //��ȡ�����Խ�����Э������
    int GetVoiceProtocolType(unsigned int* pVoiceProtocolType) const;

    //���������Խ���������
    void SetVoiceDecoderType(unsigned int uVoiceDecoderType) const;

    //��ȡ�����Խ���������
    int GetVoiceDecoderType(unsigned int* pVoiceDecoderType) const;

//     //��ȡ�����Խ����ն˵�ַ
//     int GetRecvVoiceAddr(unsigned long*  pRecvVoiceRtpPort);
// 
//     //�ͷ������Խ����ն˵�ַ
//     void FreeRecvVoiceAddr() const;

#endif

    //���ý�������IP
    void SetRecvStreamIp(char*  pRecvStreamIp);

    //��ȡ��������IP
    int GetRecvStreamIp(char*  pRecvStreamIp) const;
    /***************************************************************************
     �� �� �� : SetMediaBufferLimit
     �������� : ����ý�建�����������ްٷֱ�
     ������� : uiLowerLimit      ���ްٷֱ�;
                uiUpperLimit      ���ްٷֱ�;
     ������� : NA
     �� �� ֵ : void
    ***************************************************************************/
    void SetMediaBufferLimit(IVS_UINT32 uiLowerLimit, IVS_UINT32 uiUpperLimit)
    {
        m_uiLowerLimit = uiLowerLimit;
        m_uiUpperLimit = uiUpperLimit;
    }

    /***************************************************************************
     �� �� �� : GetMediaBufferLimit
     �������� : ��ȡý�建�����������ްٷֱ�
     ������� : NA
     ������� : uiLowerLimit      ���ްٷֱ�;
                uiUpperLimit      ���ްٷֱ�;
     �� �� ֵ : void
    ***************************************************************************/
    void GetMediaBufferLimit(IVS_UINT32& uiLowerLimit, IVS_UINT32& uiUpperLimit) const
    {
        uiLowerLimit = m_uiLowerLimit;
        uiUpperLimit = m_uiUpperLimit;
    }
private:
    //ͨ������ͨ����Դ��ӳ���;
	typedef std::map<unsigned long, CMediaChannel*> MediaChannelMap;
	typedef MediaChannelMap::iterator MediaChannelMapIter;
	MediaChannelMap             m_mapMediaChannel;	        

    PLAYER_EXCEPTION_CALLBACK   m_cbExceptionCallBack;  // �쳣�ص�����ָ��;
    void*                       m_pUserData;            // �쳣�û�����;

    PLAYER_EVENT_CALLBACK       m_cbEventCallBack;      // �¼��ص�����ָ��;
    void*                       m_pEventUserData;       // �¼��û�����;
    CIVSMutex                   m_mutexChannel;
    CIVSMutex                   m_mutexExceptionCallback;

    std::string m_strRecvStreamIp;

#ifdef WIN32
    static CAudioChannel* m_pAudioChannel; //�����Խ�ͨ������ָ��	
#endif

    CIVSMutex                   m_mutexEventCallback;
    IVS_UINT32                  m_uiLowerLimit;         // ���ްٷֱ�
    IVS_UINT32                  m_uiUpperLimit;         // ���ްٷֱ�
};
#endif //MEDIA_CHANNEL_MGR_H

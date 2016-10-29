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
 filename    :    TVWallMgr.h
 author      :    g00209332
 created     :    2012/12/19
 description :    ����ǽ������;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/12/19 ��ʼ�汾
*********************************************************************/
#ifndef _SDK_TVWALL_MGR_H_
#define _SDK_TVWALL_MGR_H_

#include "hwsdk.h"
#include "SDKDef.h"
#include "ivs_xml.h"
#include "DecoderChannel.h"
#include "vos.h"
#include "SDKTimer.h"
#include "SDKDecoderMgr.h"
#include "AutoResumeTrigger.h"
#include "HikDecoderSDK.h"
#include "DecoderSDKFactory.h"

//����ͷID
#define TVWALL_CAMERA_CODE_LEN  128

//�ص���Ϣ
typedef struct 
{
	PCUMW_CALLBACK_NOTIFY pCallBackFunc;		//�ص�����
	void* pUserData;							//�ص�����
}TVWALL_CALLBACK_INFO;

//�Զ�������Ϣ
typedef struct 
{
    CTVWallMgr* pTVWallMgr;
    CDecoderChannel* pCh;
    CUMW_BREAKDOWN_REASON emReason;
}TVWALL_ADD_AUTO_RESUME_INFO;

//�Զ������������Ϣ
typedef struct 
{
    CTVWallMgr* pTVWallMgr;
    AUTO_RESUME_PARAM stAutoResumePara;
}AUTO_RESUME_TREAD_PARAM;

class CUserMgr;//lint !e763
class CTVWallMgr
{
	typedef std::map<std::string, CUMW_NOTIFY_REMOTE_DECODER_STATUS*> REMOTE_DECODER_MAP;	   //��������Ϣ����
	typedef std::map<IVS_ULONG, CUMW_NOTIFY_INIT_DECODER_INFO*> REMOTE_DECODER_INIT_INFO_MAP;//��������ʼ����ϢMAP
	typedef std::map<std::string, CDecoderChannel*> TVWALL_DEC_CH_MAP;	//������MAP
    typedef std::map<std::string, AUTO_RESUME_PARAM*> AUTO_RESUME_MAP;//�Զ��ָ�����MAP
	typedef std::map<IVS_UINT64, TVWALL_CALLBACK_INFO> TVWALL_CALLBACK_MAP;
    typedef std::map<time_t, CDecoderChannel*> TVWALL_IDLE_CH_MAP;
	typedef std::vector<std::string> TVWALL_CAPABILITY_VEC;	//�����ϱ�����
	typedef std::map<IVS_ULONG, CUMW_NOTIFY_START_STOP_DECODER_INFO*> REMOTE_DECODER_START_INFO_MAP; //����������Ϣ����
	typedef std::list<std::string>	TVWALL_DEC_NAME_LIST;	// ���������б�

public:
    // ��ʼ��������ǽģ��
    IVS_INT32 InitTVWall(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);
    // �ͷŵ�����ǽģ��
    IVS_INT32 CleanupTVWall();
	// ��ȡ�������б�
	IVS_INT32 GetDecoderList(const IVS_INDEX_RANGE* pIndexRange,
								CUMW_NOTIFY_REMOTE_DECODER_STATUS_LIST* pDecoderList,
								IVS_UINT32 uiBufferSize);
	// ��������ǽʵʱ���
	IVS_INT32 StartRealPlayTVWall(const IVS_CHAR* pCameraCode,
									const IVS_REALPLAY_PARAM* pRealplayParam,
									const IVS_TVWALL_PARAM* pTVWallParam,
									IVS_BOOL bAutoResume=FALSE);	
	// ��������ǽʵʱ���
	IVS_INT32 SwitchRealPlayTVWall(const IVS_CHAR* pCameraCode,
		const IVS_REALPLAY_PARAM* pRealplayParam,
		const IVS_TVWALL_PARAM* pTVWallParam,
		IVS_BOOL bAutoResume=FALSE);	
	// ��������ǽʵʱ���
	IVS_INT32 StopRealPlayTVWall(const IVS_TVWALL_PARAM* pTVWallParam);
	// ��������ǽ¼��ط�
    IVS_INT32 StartPlayBackTVWall(const IVS_CHAR* pCameraCode,
                                            const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                            const IVS_TVWALL_PARAM* pTVWallParam,
											REPLAY_TYPE iReplayType,
											const IVS_CHAR* pDomainCode=NULL,
											const IVS_CHAR* pNVRCode=NULL);

	// ֹͣ����ǽ¼��ط�
	IVS_INT32 StopPlayBackTVWall(const IVS_TVWALL_PARAM* pTVWallParam);
	// �ݶ�����ǽ¼��ط�
	IVS_INT32 PlayBackPauseTVWall(const IVS_TVWALL_PARAM& stTVWallParam);
	// �ָ�����ǽ¼��ط�
	IVS_INT32 PlayBackResumeTVWall(const IVS_TVWALL_PARAM& stTVWallParam);

	// ��֡���
	IVS_INT32 PlaybackFrameStepForwardTVWall(const IVS_TVWALL_PARAM& stTVWallParam);
	// ��������
	IVS_INT32 PlaySoundTVWall(const IVS_TVWALL_PARAM& stTVWallParam);
	// ֹͣ����
	IVS_INT32 StopSoundTVWall(const IVS_TVWALL_PARAM& stTVWallParam);
	// ��������
	IVS_INT32 SetVolumeTVWall(const IVS_TVWALL_PARAM& stTVWallParam, IVS_UINT32 uiVolumeValue);
	// ����һ���ص���Ϣ
	IVS_INT32 AddCallBack(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);
	// �Ƴ�һ���ص���Ϣ
	IVS_INT32 RemoveCallBack(void* pUserData);
    //ɾ������ͨ������
    void DelIdleCh();

	// init hikvision decoder sdk
	 IVS_INT32 InitHik();
	 IVS_INT32 DecoderLogin(const CUMW_DECODER_CONNECT_TYPE emConnectType, const SDK_DECODER_LOGIN_INFO *pSDKDecoderLoginInfo);
	 IVS_INT32 DecoderLogout(const CUMW_DECODER_CONNECT_TYPE emConnectType, const IVS_CHAR* pDecoderID);
	 IVS_INT32 GetInfoFromHikDecoder(REMOTE_DECODER_MAP& stRemoteDecInfoMap);
	 CDecoderSDKFactory& GetDecoderSDKHandle(){return m_DecoderSDKFactory;}	//lint !e1536
	 IVS_INT32 GetDecoderChannelInfo(const IVS_CHAR* pDecoderID, IVS_UINT32 uiChannel, IVS_LONG& lUserID, CUMW_DECODER_CHANNEL_INFO& stDecoderChannelInfo);
	 // create Hik decoderID
	 IVS_INT32 CreateHikDecoderID(std::string& strDecoderID);
	 // add/delete hikvision decoder
	 IVS_INT32 AddDecoder(DECODER_DEV *pDecoder);
	 IVS_INT32 ValidateDecoder(DECODER_DEV *pDecoder);
	 IVS_INT32 DelDecoder(const IVS_CHAR* pszDecoderID);
	 // release alll decoder info when sdk logout
	 void DecoderRelease();


public:
	CTVWallMgr(void);
	~CTVWallMgr(void);

	void SetUserMgr(CUserMgr* pUserMgr);
	//��SCU��ȡý��URL
	IVS_INT32 GetURL(const char *pszAuthHeader, ST_URL_MEDIA_INFO& stUrlMediaInfo, ST_MEDIA_RSP& stMediaRsp);
	//��ȡ������ͨ����ʼ����Ϣ
	IVS_INT32 GetDecoderInitInfo(IVS_ULONG ulWndID, CUMW_NOTIFY_INIT_DECODER_INFO* pInitDecoderInfo);
	//���ӽ�����ͨ����ʼ����Ϣ
	void AddDecoderInitInfo(IVS_ULONG ulWndID, const CUMW_NOTIFY_INIT_DECODER_INFO& initDecoderInfo);
	//ɾ��ָ������Ľ�����ͨ����ʼ����Ϣ
	void DeleteDecoderInitInfo(IVS_ULONG ulWndID);
    //�Զ��ָ�����,�ɴ�������������
    void HandleAutoResume();
    //�첽��������֪ͨ
    void NotifyAsyncReturn(const CUMW_NOTIFY_ASYNC_RET_EX& stAsyncRet);
	//�¼�֪ͨ�ϲ�
	void NotifyEvent(CUMW_NOTIFY_INFO* pstNotifyInfo);

	// ʶ��decoder��������
	IVS_INT32 CheckDecContType(const IVS_CHAR* pDecoderID, IVS_UINT32 uiChannel);
	//���������Ƿ�����
	IVS_INT32 CheckDecoderEx(const IVS_CHAR* pDecoderID, IVS_UINT32 uiChannel);

private:
	//��ȡָ��������ͨ������û�оʹ���һ��
    CDecoderChannel* GetDecoderChannel(const char* pDecoderID, IVS_UINT32 uiChannel);
    //��ȡָ��������ͨ������ֻ�ǻ�ȡ
    CDecoderChannel* GetDecoderChannel(const std::string& key);
    //��ȡָ��������ͨ������ֻ�ǻ�ȡ
	CDecoderChannel* GetDecoderChannel(IVS_ULONG ulAsyncSeq);
	//����MAP��Key
	std::string MakeKey(const char* pDecoderID, IVS_UINT32 uiChannel) const;
	//����һ����������Ϣ
	void AddRemoteDecoder(const CUMW_NOTIFY_REMOTE_DECODER_STATUS& remoteDecoder);
	//�Ƴ�һ��������
	void RemoveRemoteDecoder(const IVS_CHAR* remoteDecoderID);
    //����ǰ��¼���ļ����� 
    IVS_INT32 GetPUPlayBackFileName(const char* pCameraCode, const IVS_TIME_SPAN* pTimeSpan, IVS_RECORD_INFO &stRecordInfo);
    //�Զ��ָ�
    IVS_INT32 AutoResumeEx(const AUTO_RESUME_PARAM* pResumeParam);

public:
	//�����Զ��ָ�
	void AddAutoResume(CDecoderChannel* pCh, CUMW_BREAKDOWN_REASON iReason);

private:
	//����Ƿ����Զ���������.�У��ر�RTSP������true;��,����false.
	bool RemoveAutoResume(const char* szDecoderCode, unsigned int uiChannel);
	//����ǽ�ص�����
	static IVS_INT32 __stdcall HandleNotifyCallBack(CUMW_NOTIFY_INFO* pstNotifyInfo, void* pUserData);
	//�����ʼ��������ͨ���ķ�������
	void InitDecoderNotify(const CUMW_NOTIFY_INFO* pstNotifyInfo, const void* pUserData);
    //��ѯָ���Ľ������Ƿ�����
    bool IsOnline(const char* pDecoderID);
public:
	//��ѯָ���Ľ������Ƿ����ߣ��Ƿ���ָ����ͨ��
	IVS_INT32 CheckDecoder(const char* pDecoderID, IVS_UINT32 uiChannel);
private:
	//�����������쳣����
	IVS_INT32 DecoderStreamError(const char* pDecoderID, IVS_UINT32 uiChannel);
	//�ϱ��Զ��ָ��¼� bStartResume �Ƿ��������ָ� TRUE-�����ָ� FALSE-�ָ���ɣ������iErrCode
	void ReportAutoResumeEvnet(const AUTO_RESUME_PARAM* pResumeParam, IVS_INT32 iErrCode = IVS_SUCCEED, IVS_BOOL bStartResume=TRUE);
    //ȡ�õ�ǰϵͳʱ�䣬����Ϊ��λ
    time_t GetCurrentTime() const;
    //��ͨ������ָ��ŵ�����������
    void AddToIdleList(CDecoderChannel* pDecoderChannel);

    //��������������Ƿ񻹴���ָ���ļ�ֵ����������
    bool IsAutoResumeParaExist(const std::string& key);
    //ɾ�����������������
    void DelAutoResumePara(const std::string& key);
    //���������쳣�������Զ�����
    void DecoderStreamError(CDecoderChannel* pCh, CUMW_BREAKDOWN_REASON emReason);
    //�����Զ��������̺߳���
    static void __cdecl AddAutoResumeThread(LPVOID lpvoid);
    //��ʼӲ��ʵ���̺߳���
    static void __cdecl AutoResumeThreadFun(LPVOID lpvoid);
	//�����л�ǰCDecoderChannel�ı�ʶΪ���л�
	IVS_INT32 SetPrePlaySwitched(const char* pDecoderID, IVS_UINT32 uiChannel);
	//����޷����������������Ƿ����ָ���ļ�ֵ�Ľڵ�
	bool IsCapabilityVecExist(const std::string& key);
	//����޷����������������Ƿ����ָ���ļ�ֵ�Ľڵ�
	void PushBackCapabilityVec(const std::string& key);

	//SDK��ʽ����Ľ������Ļص�����
	static IVS_INT32 __stdcall HandleSdkCallBack(CUMW_NOTIFY_INFO* pstNotifyInfo, void* pUserData);
	// �ж�SDK��ʽ���ӵĺ����������Ƿ�����
	bool IsHikDecOnline(const char* pszDecoderID);

public:
		//��ȡ����������ͨ����Ϣ
		IVS_INT32 GetDecoderStartInfo(IVS_ULONG ulWndID, CUMW_NOTIFY_START_STOP_DECODER_INFO* pInfo);
		//��������������ͨ����Ϣ
		void AddDecoderStartInfo(IVS_ULONG ulWndID, const CUMW_NOTIFY_START_STOP_DECODER_INFO& StartDecoderInfo);
		//ɾ��ָ�����������������ͨ����Ϣ
		void DeleteDecoderStartInfo(IVS_ULONG ulWndID);
		//��������������ͨ���ķ�������
		void StartDecoderNotify(const CUMW_NOTIFY_INFO* pstNotifyInfo, const void* pUserData);
		// ��ȡ�˿�����
		IVS_INT32 GetTVWallPortConfig(const IVS_CHAR* pszDecoderCode);
		// ���ö˿�����
		IVS_INT32 SetTVWallPortConfig(IPI_SET_PORTS_CONFIG_REQ* pInfo);
		// ��ȡ�˿�����
		IVS_INT32 GetTVWallDevAbility(const IVS_CHAR* pszDecoderCode);
		//��ȡ������ͨ���������
		CDecoderChannel* GetChannelAndInfo(IVS_ULONG ulRtspHandle, 
			unsigned long& usChannelNo, IVS_ULONG& ulHandle, char* pDecoderID, IVS_ULONG ulBufLen);

private:
	//��ȡָ��������ͨ������û�оʹ���һ��
	CDecoderChannel* GetDecoderChannel(const char* pDecoderID, IVS_UINT32 uiChannel, IVS_BOOL bSDKConnect);
	//��ͨ���żӵ�������
	IVS_INT32 AddChannel(const PORTS_CONFIG& stPortCfgs, std::list<unsigned short>& listTotalChannel)const;
	//У��ͨ�����Ƿ��ظ�
	IVS_INT32 CheckChannel(const IPI_SET_PORTS_CONFIG_REQ* pInfo)const;

private:
	// ���һ����������
	IVS_INT32 AddDecoderNameToList(const IVS_CHAR* pDecoderName);
	// ɾ��һ����������
	IVS_INT32 DelDecoderNameFromList(const IVS_CHAR* pDecoderName);
	// ɾ�����Խ�������
	void DelAllDecoderNameFromList();
	// �жϸý��������Ƿ��Ѵ���
	IVS_BOOL IsDecoderNameExist(const IVS_CHAR* pDecoderName);

private:
	//////////////////////////////////////////////////////////////////////////
	//CUʹ�ö��OCX������initTVWallֻ��ע��һ��ocx��ȡ�¼�
	//Ϊ��ʹ����ǽ���ocx���ܻ�ȡ���¼���ʹ��Map��Ŷ����¼���OCX��Ϣ
	//PCUMW_CALLBACK_NOTIFY m_pFunCallBack;       //�ص�����ָ��
	//void* m_pUserData;                          //�ϲ���ô����Ĳ�����һ����thisָ��
	TVWALL_CALLBACK_MAP m_callbackMap;			//�ϲ�ص���ϢMAP
	VOS_Mutex* m_pCallBackMapMutex;					   //��������ʼ��Map��
	//////////////////////////////////////////////////////////////////////////

	CUserMgr* m_pUserMgr;

	REMOTE_DECODER_INIT_INFO_MAP m_remoteDecodeInitInfoMap;//Զ�̽�������ʼ����Ϣ��
	VOS_Mutex* m_pInitDecoderMapMutex;					   //��������ʼ��Map��

	TVWALL_DEC_CH_MAP m_tvwallDecChMap;			//����ǽ������ͨ��MAP
	VOS_Mutex* m_pDecChMapMutex;					//ͨ��MAP��;

	REMOTE_DECODER_MAP m_onlineDecoderMap;	    //���߽�������Ϣ
    VOS_Mutex* m_pOnlineDecoderMapMutex;		//���߽�������ϢMAP��;

    AUTO_RESUME_MAP m_autoResumeMap;            //�Զ��ָ�����MAP����Ҫ�Զ��ָ��Ĳ��Ų����������ڴ�MAP��
    VOS_Mutex* m_pAutoResumeMapMutex;           //�Զ��ָ�MAP��
    CNVSTimer m_timer;                          //�Զ��ָ���ʱ��
    CAutoResumeTrigger m_AutoResumeTigger;    //�Զ��ָ�������
    bool m_bTimerInit;                          //��ʱ���Ƿ��Ѿ���ʼ��

	IVS_ULONG m_ulAsyncSeq;						//�����ǽģ��ͨ���첽ҵ�����

	CDecoderSDKFactory m_DecoderSDKFactory;				//SDK�������������

	//��������ǽ�ɹ���ͨ���б�
	typedef std::map<IVS_ULONG,CDecoderChannel*> CHANNEL_MAP;
	typedef CHANNEL_MAP::iterator CHANNEL_MAP_ITER;
	CHANNEL_MAP m_mapPlayChannel;//ʵ���б�

    TVWALL_IDLE_CH_MAP m_IdleChMap;            //���н�����ͨ����������
    VOS_Mutex* m_pIdleChMapMutex;              //���н�����ͨ�������������

	TVWALL_CAPABILITY_VEC m_CapabilityVec;		//�޷���������������value��DecoderID
	VOS_Mutex* m_pCapabilityVecMutex;					//�޷���������������

	REMOTE_DECODER_START_INFO_MAP m_remoteDecodeStartInfoMap; //����������Ϣ��
	VOS_Mutex* m_pStartDecoderMapMutex;	                      //��������Map��

	TVWALL_DEC_NAME_LIST m_listDecoderName;		// ���������б������ж��Ƿ��Ѿ�����˸ý�������
	VOS_Mutex* m_pDecoderNameMutex;				// ���������б���

public:
	CDecoderChannel* GetChannel(IVS_ULONG ulHandle);
	IVS_ULONG GetTVWallHandlebyRtspHandle(IVS_ULONG ulHandle);
	void DeleteChannel(IVS_ULONG ulHandle);//ɾ��ĳͨ��

    VOS_Mutex* m_pHandleMutex;		          //�����;
};

#endif	//_SDK_TVWALL_MGR_H_

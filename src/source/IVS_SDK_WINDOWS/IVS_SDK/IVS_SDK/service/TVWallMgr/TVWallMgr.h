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
 description :    电视墙管理类;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/12/19 初始版本
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

//摄像头ID
#define TVWALL_CAMERA_CODE_LEN  128

//回调信息
typedef struct 
{
	PCUMW_CALLBACK_NOTIFY pCallBackFunc;		//回调函数
	void* pUserData;							//回调数据
}TVWALL_CALLBACK_INFO;

//自动重连信息
typedef struct 
{
    CTVWallMgr* pTVWallMgr;
    CDecoderChannel* pCh;
    CUMW_BREAKDOWN_REASON emReason;
}TVWALL_ADD_AUTO_RESUME_INFO;

//自动重连的相关信息
typedef struct 
{
    CTVWallMgr* pTVWallMgr;
    AUTO_RESUME_PARAM stAutoResumePara;
}AUTO_RESUME_TREAD_PARAM;

class CUserMgr;//lint !e763
class CTVWallMgr
{
	typedef std::map<std::string, CUMW_NOTIFY_REMOTE_DECODER_STATUS*> REMOTE_DECODER_MAP;	   //解码器信息链表
	typedef std::map<IVS_ULONG, CUMW_NOTIFY_INIT_DECODER_INFO*> REMOTE_DECODER_INIT_INFO_MAP;//解码器初始化信息MAP
	typedef std::map<std::string, CDecoderChannel*> TVWALL_DEC_CH_MAP;	//解码器MAP
    typedef std::map<std::string, AUTO_RESUME_PARAM*> AUTO_RESUME_MAP;//自动恢复参数MAP
	typedef std::map<IVS_UINT64, TVWALL_CALLBACK_INFO> TVWALL_CALLBACK_MAP;
    typedef std::map<time_t, CDecoderChannel*> TVWALL_IDLE_CH_MAP;
	typedef std::vector<std::string> TVWALL_CAPABILITY_VEC;	//能力上报向量
	typedef std::map<IVS_ULONG, CUMW_NOTIFY_START_STOP_DECODER_INFO*> REMOTE_DECODER_START_INFO_MAP; //启动解码信息链表
	typedef std::list<std::string>	TVWALL_DEC_NAME_LIST;	// 解码器名列表

public:
    // 初始化电视上墙模块
    IVS_INT32 InitTVWall(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);
    // 释放电视上墙模块
    IVS_INT32 CleanupTVWall();
	// 获取解码器列表
	IVS_INT32 GetDecoderList(const IVS_INDEX_RANGE* pIndexRange,
								CUMW_NOTIFY_REMOTE_DECODER_STATUS_LIST* pDecoderList,
								IVS_UINT32 uiBufferSize);
	// 启动电视墙实时浏览
	IVS_INT32 StartRealPlayTVWall(const IVS_CHAR* pCameraCode,
									const IVS_REALPLAY_PARAM* pRealplayParam,
									const IVS_TVWALL_PARAM* pTVWallParam,
									IVS_BOOL bAutoResume=FALSE);	
	// 启动电视墙实时浏览
	IVS_INT32 SwitchRealPlayTVWall(const IVS_CHAR* pCameraCode,
		const IVS_REALPLAY_PARAM* pRealplayParam,
		const IVS_TVWALL_PARAM* pTVWallParam,
		IVS_BOOL bAutoResume=FALSE);	
	// 启动电视墙实时浏览
	IVS_INT32 StopRealPlayTVWall(const IVS_TVWALL_PARAM* pTVWallParam);
	// 启动电视墙录像回放
    IVS_INT32 StartPlayBackTVWall(const IVS_CHAR* pCameraCode,
                                            const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                            const IVS_TVWALL_PARAM* pTVWallParam,
											REPLAY_TYPE iReplayType,
											const IVS_CHAR* pDomainCode=NULL,
											const IVS_CHAR* pNVRCode=NULL);

	// 停止电视墙录像回放
	IVS_INT32 StopPlayBackTVWall(const IVS_TVWALL_PARAM* pTVWallParam);
	// 暂定电视墙录像回放
	IVS_INT32 PlayBackPauseTVWall(const IVS_TVWALL_PARAM& stTVWallParam);
	// 恢复电视墙录像回放
	IVS_INT32 PlayBackResumeTVWall(const IVS_TVWALL_PARAM& stTVWallParam);

	// 单帧快进
	IVS_INT32 PlaybackFrameStepForwardTVWall(const IVS_TVWALL_PARAM& stTVWallParam);
	// 播放声音
	IVS_INT32 PlaySoundTVWall(const IVS_TVWALL_PARAM& stTVWallParam);
	// 停止声音
	IVS_INT32 StopSoundTVWall(const IVS_TVWALL_PARAM& stTVWallParam);
	// 设置音量
	IVS_INT32 SetVolumeTVWall(const IVS_TVWALL_PARAM& stTVWallParam, IVS_UINT32 uiVolumeValue);
	// 增加一个回调信息
	IVS_INT32 AddCallBack(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);
	// 移除一个回调信息
	IVS_INT32 RemoveCallBack(void* pUserData);
    //删除空闲通道对象
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
	//从SCU获取媒体URL
	IVS_INT32 GetURL(const char *pszAuthHeader, ST_URL_MEDIA_INFO& stUrlMediaInfo, ST_MEDIA_RSP& stMediaRsp);
	//获取解码器通道初始化信息
	IVS_INT32 GetDecoderInitInfo(IVS_ULONG ulWndID, CUMW_NOTIFY_INIT_DECODER_INFO* pInitDecoderInfo);
	//增加解码器通道初始化信息
	void AddDecoderInitInfo(IVS_ULONG ulWndID, const CUMW_NOTIFY_INIT_DECODER_INFO& initDecoderInfo);
	//删除指定窗格的解码器通道初始化信息
	void DeleteDecoderInitInfo(IVS_ULONG ulWndID);
    //自动恢复处理,由触发器触发调用
    void HandleAutoResume();
    //异步操作返回通知
    void NotifyAsyncReturn(const CUMW_NOTIFY_ASYNC_RET_EX& stAsyncRet);
	//事件通知上层
	void NotifyEvent(CUMW_NOTIFY_INFO* pstNotifyInfo);

	// 识别decoder连接类型
	IVS_INT32 CheckDecContType(const IVS_CHAR* pDecoderID, IVS_UINT32 uiChannel);
	//检查解码器是否在线
	IVS_INT32 CheckDecoderEx(const IVS_CHAR* pDecoderID, IVS_UINT32 uiChannel);

private:
	//获取指定解码器通道对象，没有就创建一个
    CDecoderChannel* GetDecoderChannel(const char* pDecoderID, IVS_UINT32 uiChannel);
    //获取指定解码器通道对象，只是获取
    CDecoderChannel* GetDecoderChannel(const std::string& key);
    //获取指定解码器通道对象，只是获取
	CDecoderChannel* GetDecoderChannel(IVS_ULONG ulAsyncSeq);
	//生成MAP的Key
	std::string MakeKey(const char* pDecoderID, IVS_UINT32 uiChannel) const;
	//增加一个解码器信息
	void AddRemoteDecoder(const CUMW_NOTIFY_REMOTE_DECODER_STATUS& remoteDecoder);
	//移除一个解码器
	void RemoveRemoteDecoder(const IVS_CHAR* remoteDecoderID);
    //查找前端录像文件名称 
    IVS_INT32 GetPUPlayBackFileName(const char* pCameraCode, const IVS_TIME_SPAN* pTimeSpan, IVS_RECORD_INFO &stRecordInfo);
    //自动恢复
    IVS_INT32 AutoResumeEx(const AUTO_RESUME_PARAM* pResumeParam);

public:
	//增加自动恢复
	void AddAutoResume(CDecoderChannel* pCh, CUMW_BREAKDOWN_REASON iReason);

private:
	//检查是否有自动重连数据.有，关闭RTSP，返回true;无,返回false.
	bool RemoveAutoResume(const char* szDecoderCode, unsigned int uiChannel);
	//电视墙回调函数
	static IVS_INT32 __stdcall HandleNotifyCallBack(CUMW_NOTIFY_INFO* pstNotifyInfo, void* pUserData);
	//处理初始化解码器通道的返回数据
	void InitDecoderNotify(const CUMW_NOTIFY_INFO* pstNotifyInfo, const void* pUserData);
    //查询指定的解码器是否在线
    bool IsOnline(const char* pDecoderID);
public:
	//查询指定的解码器是否在线，是否有指定的通道
	IVS_INT32 CheckDecoder(const char* pDecoderID, IVS_UINT32 uiChannel);
private:
	//解码器收流异常处理
	IVS_INT32 DecoderStreamError(const char* pDecoderID, IVS_UINT32 uiChannel);
	//上报自动恢复事件 bStartResume 是否是启动恢复 TRUE-启动恢复 FALSE-恢复完成，结果看iErrCode
	void ReportAutoResumeEvnet(const AUTO_RESUME_PARAM* pResumeParam, IVS_INT32 iErrCode = IVS_SUCCEED, IVS_BOOL bStartResume=TRUE);
    //取得当前系统时间，以秒为单位
    time_t GetCurrentTime() const;
    //把通道对象指针放到空闲链表中
    void AddToIdleList(CDecoderChannel* pDecoderChannel);

    //检查重连链表里是否还存在指定的键值的重连参数
    bool IsAutoResumeParaExist(const std::string& key);
    //删除重连链表里的数据
    void DelAutoResumePara(const std::string& key);
    //解码器流异常，增加自动重连
    void DecoderStreamError(CDecoderChannel* pCh, CUMW_BREAKDOWN_REASON emReason);
    //增加自动重连的线程函数
    static void __cdecl AddAutoResumeThread(LPVOID lpvoid);
    //开始硬解实况线程函数
    static void __cdecl AutoResumeThreadFun(LPVOID lpvoid);
	//设置切换前CDecoderChannel的标识为被切换
	IVS_INT32 SetPrePlaySwitched(const char* pDecoderID, IVS_UINT32 uiChannel);
	//检查无缝切屏能力向量中是否存在指定的键值的节点
	bool IsCapabilityVecExist(const std::string& key);
	//检查无缝切屏能力向量中是否存在指定的键值的节点
	void PushBackCapabilityVec(const std::string& key);

	//SDK方式接入的解码器的回调函数
	static IVS_INT32 __stdcall HandleSdkCallBack(CUMW_NOTIFY_INFO* pstNotifyInfo, void* pUserData);
	// 判断SDK方式连接的海康解码器是否在线
	bool IsHikDecOnline(const char* pszDecoderID);

public:
		//获取启动解码器通道信息
		IVS_INT32 GetDecoderStartInfo(IVS_ULONG ulWndID, CUMW_NOTIFY_START_STOP_DECODER_INFO* pInfo);
		//增加启动解码器通道信息
		void AddDecoderStartInfo(IVS_ULONG ulWndID, const CUMW_NOTIFY_START_STOP_DECODER_INFO& StartDecoderInfo);
		//删除指定窗格的启动解码器通道信息
		void DeleteDecoderStartInfo(IVS_ULONG ulWndID);
		//处理启动解码器通道的返回数据
		void StartDecoderNotify(const CUMW_NOTIFY_INFO* pstNotifyInfo, const void* pUserData);
		// 获取端口配置
		IVS_INT32 GetTVWallPortConfig(const IVS_CHAR* pszDecoderCode);
		// 设置端口配置
		IVS_INT32 SetTVWallPortConfig(IPI_SET_PORTS_CONFIG_REQ* pInfo);
		// 获取端口配置
		IVS_INT32 GetTVWallDevAbility(const IVS_CHAR* pszDecoderCode);
		//获取解码器通道对象参数
		CDecoderChannel* GetChannelAndInfo(IVS_ULONG ulRtspHandle, 
			unsigned long& usChannelNo, IVS_ULONG& ulHandle, char* pDecoderID, IVS_ULONG ulBufLen);

private:
	//获取指定解码器通道对象，没有就创建一个
	CDecoderChannel* GetDecoderChannel(const char* pDecoderID, IVS_UINT32 uiChannel, IVS_BOOL bSDKConnect);
	//把通道号加到链表中
	IVS_INT32 AddChannel(const PORTS_CONFIG& stPortCfgs, std::list<unsigned short>& listTotalChannel)const;
	//校验通道号是否重复
	IVS_INT32 CheckChannel(const IPI_SET_PORTS_CONFIG_REQ* pInfo)const;

private:
	// 添加一个解码器名
	IVS_INT32 AddDecoderNameToList(const IVS_CHAR* pDecoderName);
	// 删除一个解码器名
	IVS_INT32 DelDecoderNameFromList(const IVS_CHAR* pDecoderName);
	// 删除所以解码器名
	void DelAllDecoderNameFromList();
	// 判断该解码器名是否已存在
	IVS_BOOL IsDecoderNameExist(const IVS_CHAR* pDecoderName);

private:
	//////////////////////////////////////////////////////////////////////////
	//CU使用多个OCX，导致initTVWall只能注册一个ocx获取事件
	//为了使电视墙相关ocx都能获取到事件，使用Map存放订阅事件的OCX信息
	//PCUMW_CALLBACK_NOTIFY m_pFunCallBack;       //回调函数指针
	//void* m_pUserData;                          //上层调用传进的参数，一般是this指针
	TVWALL_CALLBACK_MAP m_callbackMap;			//上层回调信息MAP
	VOS_Mutex* m_pCallBackMapMutex;					   //解码器初始化Map锁
	//////////////////////////////////////////////////////////////////////////

	CUserMgr* m_pUserMgr;

	REMOTE_DECODER_INIT_INFO_MAP m_remoteDecodeInitInfoMap;//远程解码器初始化信息表
	VOS_Mutex* m_pInitDecoderMapMutex;					   //解码器初始化Map锁

	TVWALL_DEC_CH_MAP m_tvwallDecChMap;			//电视墙解码器通道MAP
	VOS_Mutex* m_pDecChMapMutex;					//通道MAP锁;

	REMOTE_DECODER_MAP m_onlineDecoderMap;	    //在线解码器信息
    VOS_Mutex* m_pOnlineDecoderMapMutex;		//在线解码器信息MAP锁;

    AUTO_RESUME_MAP m_autoResumeMap;            //自动恢复参数MAP，需要自动恢复的播放参数都保存在此MAP中
    VOS_Mutex* m_pAutoResumeMapMutex;           //自动恢复MAP锁
    CNVSTimer m_timer;                          //自动恢复定时器
    CAutoResumeTrigger m_AutoResumeTigger;    //自动恢复触发器
    bool m_bTimerInit;                          //定时器是否已经初始化

	IVS_ULONG m_ulAsyncSeq;						//与电视墙模块通信异步业务序号

	CDecoderSDKFactory m_DecoderSDKFactory;				//SDK解码器管理对象

	//解码器上墙成功的通道列表
	typedef std::map<IVS_ULONG,CDecoderChannel*> CHANNEL_MAP;
	typedef CHANNEL_MAP::iterator CHANNEL_MAP_ITER;
	CHANNEL_MAP m_mapPlayChannel;//实况列表

    TVWALL_IDLE_CH_MAP m_IdleChMap;            //空闲解码器通道对象链表
    VOS_Mutex* m_pIdleChMapMutex;              //空闲解码器通道对象链表的锁

	TVWALL_CAPABILITY_VEC m_CapabilityVec;		//无缝切屏能力向量，value是DecoderID
	VOS_Mutex* m_pCapabilityVecMutex;					//无缝切屏能力向量锁

	REMOTE_DECODER_START_INFO_MAP m_remoteDecodeStartInfoMap; //启动解码信息表
	VOS_Mutex* m_pStartDecoderMapMutex;	                      //启动解码Map锁

	TVWALL_DEC_NAME_LIST m_listDecoderName;		// 解码器名列表，用于判断是否已经添加了该解码器名
	VOS_Mutex* m_pDecoderNameMutex;				// 解码器名列表锁

public:
	CDecoderChannel* GetChannel(IVS_ULONG ulHandle);
	IVS_ULONG GetTVWallHandlebyRtspHandle(IVS_ULONG ulHandle);
	void DeleteChannel(IVS_ULONG ulHandle);//删除某通道

    VOS_Mutex* m_pHandleMutex;		          //句柄锁;
};

#endif	//_SDK_TVWALL_MGR_H_
